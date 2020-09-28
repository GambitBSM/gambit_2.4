///
///  \author Anders Kvellestad
///  \date 2020 Aug
///  *********************************************

// Based on https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2020-040/


#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <fstream>

#include "gambit/ColliderBit/analyses/Analysis.hpp"
#include "gambit/ColliderBit/ATLASEfficiencies.hpp"
#include "gambit/ColliderBit/mt2_bisect.h"

#define CHECK_CUTFLOW

using namespace std;

namespace Gambit
{
  namespace ColliderBit
  {

    class Analysis_ATLAS_13TeV_4LEP_139invfb : public Analysis
    {

    protected:

      // Counters for the number of accepted events for each signal region
      std::map<string, EventCounter> _counters = {
        {"SR0-ZZ-loose-bveto", EventCounter("SR0-ZZ-loose-bveto")},
        {"SR0-ZZ-tight-bveto", EventCounter("SR0-ZZ-tight-bveto")},
        {"SR0-ZZ-loose", EventCounter("SR0-ZZ-loose")},
        {"SR0-ZZ-tight", EventCounter("SR0-ZZ-tight")},
        {"SR0-loose-bveto", EventCounter("SR0-loose-bveto")},
        {"SR0-tight-bveto", EventCounter("SR0-tight-bveto")},
        {"SR0-breq", EventCounter("SR0-breq")}
      };

    private:

      #ifdef CHECK_CUTFLOW
        vector<int> cutFlowVector;
        vector<string> cutFlowVector_str;
        size_t NCUTS;
        vector<double> cutFlowVectorATLAS_200_50;
        vector<double> cutFlowVectorATLAS_300_100;
      #endif

      struct ptComparison
      {
        bool operator() (const HEPUtils::Particle* i,const HEPUtils::Particle* j) {return (i->pT()>j->pT());}
      } comparePt;

      struct ptJetComparison
      {
        bool operator() (const HEPUtils::Jet* i,const HEPUtils::Jet* j) {return (i->pT()>j->pT());}
      } compareJetPt;

      // Jet lepton overlap removal
      // Discards jets if they are within DeltaRMax of a lepton
      void JetLeptonOverlapRemoval(vector<const HEPUtils::Jet*>& jets, vector<const HEPUtils::Particle*>& leptons, double DeltaRMax)
      {
        vector<const HEPUtils::Jet*> survivors;
        for(const HEPUtils::Jet* jet : jets)
        {
          bool overlap = false;
          for(const HEPUtils::Particle* lepton : leptons)
          {
            double dR = jet->mom().deltaR_eta(lepton->mom());
            if(fabs(dR) <= DeltaRMax) overlap = true;
          }
          if(!overlap) survivors.push_back(jet);
        }
        jets = survivors;
        return;
      }

     void bTagger(vector<const HEPUtils::Jet*>& candJets) {
        vector<const HEPUtils::Jet*> bJets;

        // Find b-jets
        double btag = 0.85; double cmisstag = 1/6.; double misstag = 1./134.;
        for (const HEPUtils::Jet* jet : candJets) {
          if (jet->abseta() > 2.5) continue;
          // Tag
          if( jet->btag() && random_bool(btag) ) bJets.push_back(jet);
          // Misstag c-jet
          else if( jet->ctag() && random_bool(cmisstag) ) bJets.push_back(jet);
          // Misstag light jet
          else if( random_bool(misstag) ) bJets.push_back(jet);
        }
        candJets = bJets;
    }

      // Lepton jet overlap removal
      // Discards leptons if they are within DeltaRMax of a jet
      void LeptonJetOverlapRemoval(vector<const HEPUtils::Particle*>& leptons, vector<const HEPUtils::Jet*>& jets, double DeltaRMax)
      {
        vector<const HEPUtils::Particle*> survivors;
        for(const HEPUtils::Particle* lepton : leptons)
        {
          bool overlap = false;
          for(const HEPUtils::Jet* jet : jets)
          {
            double dR = jet->mom().deltaR_eta(lepton->mom());
            if(fabs(dR) <= DeltaRMax) overlap = true;
          }
          if(!overlap) survivors.push_back(lepton);
        }
        leptons = survivors;
        return;
      }

      // Particle overlap removal
      // Discards particle (from "particles1") if it is within DeltaRMax of another particle
      void ParticleOverlapRemoval(vector<const HEPUtils::Particle*>& particles1, vector<const HEPUtils::Particle*>& particles2, double DeltaRMax)
      {
        vector<const HEPUtils::Particle*> survivors;
        for(const HEPUtils::Particle* p1 : particles1)
        {
          bool overlap = false;
          for(const HEPUtils::Particle* p2 : particles2)
          {
            double dR = p1->mom().deltaR_eta(p2->mom());
            if(fabs(dR) <= DeltaRMax) overlap = true;
          }
          if(!overlap) survivors.push_back(p1);
        }
        particles1 = survivors;
        return;
      }

      // Particle overlap removal
      // Discard particles within DeltaRMax of one another, IF pT of any < pTMax
      void ParticlePTOverlapPairsRemoval(vector<const HEPUtils::Particle*>& particles, double DeltaRMax, double pTMax)
      {
        if (particles.size() < 2)
            return;
        vector<const HEPUtils::Particle*> survivors;
        vector<const HEPUtils::Particle*> todrop;
        for(auto p1_it = particles.begin(); p1_it != particles.end()-1; ++p1_it)
        {
          auto p1 = *p1_it;
          bool overlap = false;
          bool lowpt = false;
          for(auto p2_it = p1_it+1; p2_it != particles.end(); ++p2_it)
          {
            auto p2 = *p2_it;
            double dR = p1->mom().deltaR_eta(p2->mom());
            if(fabs(dR) <= DeltaRMax) {
                overlap = true;
                lowpt = true;
                if (p1->pT() < pTMax || p2->pT() < pTMax) {
                    todrop.push_back(p2);
                    lowpt = true;
                    break;
                }
            }
          }
          if (!(overlap && lowpt)) {
            survivors.push_back(p1);
          }
        }
        std::sort(survivors.begin(), survivors.end());
        std::sort(todrop.begin(), todrop.end());
        vector<const HEPUtils::Particle*> result;
        std::set_difference(survivors.begin(), survivors.end(), todrop.begin(), todrop.end(), std::back_inserter(result));
        particles = result;
        return;
      }

      // Removes a lepton from the leptons1 vector if it forms an OS pair with a
      // lepton in leptons2 and the pair has a mass in the range (m_low, m_high).
      void removeOSPairsInMassRange(vector<const HEPUtils::Particle*>& leptons1, vector<const HEPUtils::Particle*>& leptons2, double m_low, double m_high)
      {
        vector<const HEPUtils::Particle*> l1_survivors;
        for(const HEPUtils::Particle* l1 : leptons1)
        {
          bool survived = true;
          for(const HEPUtils::Particle* l2 : leptons2)
          {
            if(l2 == l1) continue;
            if (l1->pid()*l2->pid() < 0.)
            {
              double m = (l1->mom() + l2->mom()).m();
              if ((m >= m_low) && (m <= m_high))
              {
                survived = false;
                break;
              }
            }
          }
          if(survived) l1_survivors.push_back(l1);
        }
        leptons1 = l1_survivors;
        return;
      }


    public:

      // Required detector sim
      static constexpr const char* detector = "ATLAS";

      Analysis_ATLAS_13TeV_4LEP_139invfb()
      {

        set_analysis_name("ATLAS_13TeV_4LEP_139invfb");
        set_luminosity(139.);

        #ifdef CHECK_CUTFLOW
          NCUTS = 11;
          for (size_t i=0;i<NCUTS;i++)
          {
            cutFlowVector.push_back(0);
            cutFlowVectorATLAS_200_50.push_back(0);
            cutFlowVectorATLAS_300_100.push_back(0);
            cutFlowVector_str.push_back("");
          }
        #endif

      }

      void run(const HEPUtils::Event* event)
      {

        // Baseline objects
        vector<const HEPUtils::Particle*> baselineElectrons;
        vector<const HEPUtils::Particle*> baselineMuons;
        vector<const HEPUtils::Particle*> baselineTaus;
        vector<const HEPUtils::Jet*> baselineJets;
        double met = event->met();

        #ifdef  CHECK_CUTFLOW
          bool generator_filter = false;
          bool trigger = true;
        #endif


        #ifdef  CHECK_CUTFLOW
            int e_gen_filter_cnt = 0;
        #endif
        for (const HEPUtils::Particle* electron : event->electrons())
        {
          if (electron->pT()>4.5 && electron->abseta()<2.47) baselineElectrons.push_back(electron);
          #ifdef  CHECK_CUTFLOW
              if (!generator_filter && electron->pT() > 4 && electron->abseta()<2.8) {
                  ++e_gen_filter_cnt;
              }
          #endif
        }

        // Apply electron efficiency
        ATLAS::applyElectronEff(baselineElectrons);

        // Apply loose electron selection
        ATLAS::applyLooseIDElectronSelectionR2(baselineElectrons);

        for (const HEPUtils::Particle* muon : event->muons())
        {
          if (muon->pT()>3. && muon->abseta()<2.7) baselineMuons.push_back(muon);
          #ifdef  CHECK_CUTFLOW
            if (!generator_filter && muon->pT() > 4 && muon->abseta()<2.8) {
                //++e_gen_filter_cnt;
                generator_filter = true;
            }
          #endif
        }

        // Apply muon efficiency
        ATLAS::applyMuonEff(baselineMuons);

        // Missing: Apply "medium" muon ID criteria

        for (const HEPUtils::Particle* tau : event->taus())
        {
          if (tau->pT()>20. && (tau->abseta()>1.52 ? tau->abseta()<2.47 : tau->abseta()<1.37)) baselineTaus.push_back(tau);
          #ifdef  CHECK_CUTFLOW
            if (!generator_filter && tau->pT() > 15 && tau->abseta()<2.8) {
                //++e_gen_filter_cnt;
                generator_filter = true;
            }
          #endif
        }
        #ifdef  CHECK_CUTFLOW
            if (!generator_filter && e_gen_filter_cnt >= 4) {
                generator_filter = true;
            }
        #endif

        // Since tau efficiencies are not applied as part of the BuckFast ATLAS sim we apply it here
        ATLAS::applyTauEfficiencyR2(baselineTaus);

        for (const HEPUtils::Jet* jet : event->jets())
        {
          if (jet->pT()>20. && jet->abseta()<2.8) baselineJets.push_back(jet);
        }
        // Missing: Some additional requirements for jets with abseta < 2.5, originating from b-quarks (see paper)
        // Missing: some jets are originating from hadronically decayed taus pT > 10, abseta < 2.47 and more. (see paper)



        // Overlap removal
        // 1) Remove taus within DeltaR = 0.2 of an electron or muon
        ParticleOverlapRemoval(baselineTaus, baselineElectrons, 0.2);
        ParticleOverlapRemoval(baselineTaus, baselineMuons, 0.2);

        // 2) Missing: Remove electron sharing an ID track with a muon

        // 3) Remove jets within DeltaR = 0.2 of electron
        JetLeptonOverlapRemoval(baselineJets, baselineElectrons, 0.2);

        // 4) Remove electrons within DeltaR = 0.4 of a jet
        LeptonJetOverlapRemoval(baselineElectrons, baselineJets, 0.4);

        // 5) Missing: Remove jets with < 3 assocated tracks if a muon is
        //    within DeltaR = 0.2 *or* if the muon is a track in the jet.

        // 6) Remove muons within DeltaR = 0.4 of jet
        LeptonJetOverlapRemoval(baselineMuons, baselineJets, 0.4);

        // 7) Remove jets within DeltaR = 0.4 of a "medium" tau
        JetLeptonOverlapRemoval(baselineJets, baselineTaus, 0.4);


        // Suppress low-mass particle decays
        vector<const HEPUtils::Particle*> baselineLeptons;
        baselineLeptons = baselineElectrons;
        baselineLeptons.insert(baselineLeptons.end(), baselineMuons.begin(), baselineMuons.end());
        // - Remove low-mass OS pairs
        removeOSPairsInMassRange(baselineElectrons, baselineLeptons, 0.0, 4.0);
        removeOSPairsInMassRange(baselineMuons, baselineLeptons, 0.0, 4.0);
        // - Remove SFOS pairs in the mass range (8.4, 10.4) GeV
        removeOSPairsInMassRange(baselineElectrons, baselineElectrons, 8.4, 10.4);
        removeOSPairsInMassRange(baselineMuons, baselineMuons, 8.4, 10.4);
        ParticlePTOverlapPairsRemoval(baselineLeptons, 0.6, 30.);


        // Signal objects
        vector<const HEPUtils::Jet*> signalBJets = baselineJets;
        bTagger(signalBJets);  // Keep only B-tagged jets
        vector<const HEPUtils::Particle*> signalElectrons = baselineElectrons;
        vector<const HEPUtils::Particle*> signalMuons = baselineMuons;
        vector<const HEPUtils::Particle*> signalTaus = baselineTaus;
        vector<const HEPUtils::Particle*> signalLeptons;
        signalLeptons = signalElectrons;
        signalLeptons.insert(signalLeptons.end(), signalMuons.begin(), signalMuons.end());

        // Missing: pT-dependent isolation criteria for signal leptons (see paper)

        // Sort by pT
        sort(signalLeptons.begin(), signalLeptons.end(), comparePt);

        // Count signal leptons and jets
        size_t nSignalLeptons = signalLeptons.size();
        size_t NbJets = signalBJets.size();

        // Get OS and SFOS pairs
        vector<vector<const HEPUtils::Particle*>> SFOSpairs = getSFOSpairs(signalLeptons);
        vector<vector<const HEPUtils::Particle*>> OSpairs = getOSpairs(signalLeptons);

        // Z requirements
        vector<double> SFOSpair_masses;
        for (vector<const HEPUtils::Particle*> pair : SFOSpairs)
        {
          SFOSpair_masses.push_back( (pair.at(0)->mom() + pair.at(1)->mom()).m() );
        }
        std::sort(SFOSpair_masses.begin(), SFOSpair_masses.end(), std::greater<double>());

        bool Z1 = false;
        bool Z2 = false;
        bool Zlike = false;
        for(double m : SFOSpair_masses)
        {
          if (!Z1 && (m > 81.2) && (m < 101.2))
          {
            Z1 = true;
          }
          else if (Z1 && (m > 61.2) && (m < 101.2))
          {
            Z2 = true;
          }
        }
        if (Z1) Zlike = true;
        // Missing: Also check Z-like combinations of SFOS+L and SFOS+SFOS (see paper)


        // Missing soft term correction for Et_miss. Constructed from tracks matched to the primary vertex
        // but not associated with identified physics objects (see paper)


        // Signal Regions

        // --- 4L0T ---

        // SR0-ZZ-loose-bveto
        if (nSignalLeptons >= 4 && NbJets == 0 && Z1 && Z2 && met > 100.) _counters.at("SR0-ZZ-loose-bveto").add_event(event);
        //if (nSignalLeptons >= 4 && NbJets == 0 && Z1 && Z2 && met > 100.)
        // {
        //   cout << "DEBUG: " << "--- Got event for SR0-ZZ-loose-bveto ---" << endl;
        //   cout << "DEBUG: " << "  leptons: " << nSignalLeptons << ", electrons: " << nSignalElectrons << ", muons: " << nSignalMuons << endl;
        //   cout << "DEBUG: " << "  jets: " << nSignalJets << endl;
        //   cout << "DEBUG: " << "  met = " << met << endl;
        //   cout << "DEBUG: " << "  nSFOSpairs = " << SFOSpairs.size() << endl;
        //   for (double mass : SFOSpair_masses)
        //   {
        //     cout << "DEBUG: " << "  pair mass: " << mass << endl;
        //   }
        // }

        // SR0-ZZ-tight-bveto
        if (nSignalLeptons >= 4 && NbJets == 0 && Z1 && Z2 && met > 200.) _counters.at("SR0-ZZ-tight-bveto").add_event(event);
        //if (nSignalLeptons >= 4 && NbJets == 0 && Z1 && Z2 && met > 200.)
        // {
        //   cout << "DEBUG: " << "--- Got event for SR0-ZZ-tight-bveto ---" << endl;
        //   cout << "DEBUG: " << "  leptons: " << nSignalLeptons << ", electrons: " << nSignalElectrons << ", muons: " << nSignalMuons << endl;
        //   cout << "DEBUG: " << "  jets: " << nSignalJets << endl;
        //   cout << "DEBUG: " << "  met = " << met << endl;
        //   cout << "DEBUG: " << "  nSFOSpairs = " << SFOSpairs.size() << endl;
        //   for (double mass : SFOSpair_masses)
        //   {
        //     cout << "DEBUG: " << "  pair mass: " << mass << endl;
        //   }
        // }

        // SR0-ZZ-loose
        if (nSignalLeptons >= 4 && Z1 && Z2 && met > 50.) _counters.at("SR0-ZZ-loose").add_event(event);
        //if (nSignalLeptons >= 4 && Z1 && Z2 && met > 50.)
        // {
        //   cout << "DEBUG: " << "--- Got event for SR0-ZZ-loose ---" << endl;
        //   cout << "DEBUG: " << "  leptons: " << nSignalLeptons << ", electrons: " << nSignalElectrons << ", muons: " << nSignalMuons << endl;
        //   cout << "DEBUG: " << "  jets: " << nSignalJets << endl;
        //   cout << "DEBUG: " << "  met = " << met << endl;
        //   cout << "DEBUG: " << "  nSFOSpairs = " << SFOSpairs.size() << endl;
        //   for (double mass : SFOSpair_masses)
        //   {
        //     cout << "DEBUG: " << "  pair mass: " << mass << endl;
        //   }
        // }

        // SR0-ZZ-tight
        if (nSignalLeptons >= 4 && Z1 && Z2 && met > 100.) _counters.at("SR0-ZZ-tight").add_event(event);
        //if (nSignalLeptons >= 4 && Z1 && Z2 && met > 100.)
        // {
        //   cout << "DEBUG: " << "--- Got event for SR0-ZZ-tight-bveto ---" << endl;
        //   cout << "DEBUG: " << "  leptons: " << nSignalLeptons << ", electrons: " << nSignalElectrons << ", muons: " << nSignalMuons << endl;
        //   cout << "DEBUG: " << "  jets: " << nSignalJets << endl;
        //   cout << "DEBUG: " << "  met = " << met << endl;
        //   cout << "DEBUG: " << "  nSFOSpairs = " << SFOSpairs.size() << endl;
        //   for (double mass : SFOSpair_masses)
        //   {
        //     cout << "DEBUG: " << "  pair mass: " << mass << endl;
        //   }
        // }

        // SR0-loose-bveto
        if (nSignalLeptons >= 4 && NbJets == 0 && !Zlike && met > 600.) _counters.at("SR0-loose-bveto").add_event(event);
        //if (nSignalLeptons >= 4 && NbJets == 0 && !Zlike && met > 600.)
        // {
        //   cout << "DEBUG: " << "--- Got event for SR0-loose-bveto ---" << endl;
        //   cout << "DEBUG: " << "  leptons: " << nSignalLeptons << ", electrons: " << nSignalElectrons << ", muons: " << nSignalMuons << endl;
        //   cout << "DEBUG: " << "  jets: " << nSignalJets << endl;
        //   cout << "DEBUG: " << "  met = " << met << endl;
        //   cout << "DEBUG: " << "  nSFOSpairs = " << SFOSpairs.size() << endl;
        //   for (double mass : SFOSpair_masses)
        //   {
        //     cout << "DEBUG: " << "  pair mass: " << mass << endl;
        //   }
        // }

        // SR0-tight-bveto
        if (nSignalLeptons >= 4 && NbJets == 0 && !Zlike && met > 1000.) _counters.at("SR0-tight-bveto").add_event(event);
        //if (nSignalLeptons >= 4 && NbJets == 0 && !Zlike && met > 1000.)
        // {
        //   cout << "DEBUG: " << "--- Got event for SR0-ZZ-tight-bveto ---" << endl;
        //   cout << "DEBUG: " << "  leptons: " << nSignalLeptons << ", electrons: " << nSignalElectrons << ", muons: " << nSignalMuons << endl;
        //   cout << "DEBUG: " << "  jets: " << nSignalJets << endl;
        //   cout << "DEBUG: " << "  met = " << met << endl;
        //   cout << "DEBUG: " << "  nSFOSpairs = " << SFOSpairs.size() << endl;
        //   for (double mass : SFOSpair_masses)
        //   {
        //     cout << "DEBUG: " << "  pair mass: " << mass << endl;
        //   }
        // }

        // SR0-breq
        if (nSignalLeptons >= 4 && NbJets >= 1 && !Zlike && met > 1300.) _counters.at("SR0-breq").add_event(event);
        //if (nSignalLeptons >= 4 && NbJets >= 1 && !Zlike && met > 1300.)
        // {
        //   cout << "DEBUG: " << "--- Got event for SR0-ZZ-tight-bveto ---" << endl;
        //   cout << "DEBUG: " << "  leptons: " << nSignalLeptons << ", electrons: " << nSignalElectrons << ", muons: " << nSignalMuons << endl;
        //   cout << "DEBUG: " << "  jets: " << nSignalJets << endl;
        //   cout << "DEBUG: " << "  met = " << met << endl;
        //   cout << "DEBUG: " << "  nSFOSpairs = " << SFOSpairs.size() << endl;
        //   for (double mass : SFOSpair_masses)
        //   {
        //     cout << "DEBUG: " << "  pair mass: " << mass << endl;
        //   }
        // }


        #ifdef CHECK_CUTFLOW
          cutFlowVector_str[0] = "Initial";
          cutFlowVector_str[1] = "Good Event";
          cutFlowVector_str[2] = "N_e_mu >= 2";
          cutFlowVector_str[3] = "Trigger";
          cutFlowVector_str[4] = "N_e_mu >= 4";
          cutFlowVector_str[5] = "ZZ";
          cutFlowVector_str[6] = "met > 50 (SR0-ZZ-loose)";
          cutFlowVector_str[7] = "met > 100 (SR0-ZZ-tight)";
          cutFlowVector_str[8] = "b-veto";
          cutFlowVector_str[9] = "met > 100 (SR0-ZZ-loose-bveto)";
          cutFlowVector_str[10] = "met > 200 (SR0-ZZ-tight-bveto)";

          cutFlowVectorATLAS_200_50[0] = -1;
          cutFlowVectorATLAS_200_50[1] = 2716.37;
          cutFlowVectorATLAS_200_50[2] = 1041.64;
          cutFlowVectorATLAS_200_50[3] = 951.78;
          cutFlowVectorATLAS_200_50[4] = 116.87;
          cutFlowVectorATLAS_200_50[5] = 71.53;
          cutFlowVectorATLAS_200_50[6] = 55.88;
          cutFlowVectorATLAS_200_50[7] = 28.47;
          cutFlowVectorATLAS_200_50[8] = 66.21;
          cutFlowVectorATLAS_200_50[9] = 26.41;
          cutFlowVectorATLAS_200_50[10] = 2.96;

          cutFlowVectorATLAS_300_100[0] = -1;
          cutFlowVectorATLAS_300_100[1] = 493.16;
          cutFlowVectorATLAS_300_100[2] = 319.87;
          cutFlowVectorATLAS_300_100[3] = 308.22;
          cutFlowVectorATLAS_300_100[4] = 74.92;
          cutFlowVectorATLAS_300_100[5] = 61.14;
          cutFlowVectorATLAS_300_100[6] = 56.74;
          cutFlowVectorATLAS_300_100[7] = 46.76;
          cutFlowVectorATLAS_300_100[8] = 55.42;
          cutFlowVectorATLAS_300_100[9] = 42.77;
          cutFlowVectorATLAS_300_100[10] = 19.46;

          for (size_t j=0;j<NCUTS;j++)
          {
            if(
              (j==0) ||

              (j==1 && generator_filter) ||

              (j==2 && generator_filter && nSignalLeptons >= 2) ||

              (j==3 && generator_filter && nSignalLeptons >= 2 && trigger) ||

              (j==4 && generator_filter && nSignalLeptons >= 4 && trigger) ||

              (j==5 && generator_filter && nSignalLeptons >= 4 && trigger && Z1 && Z2) ||

              (j==6 && generator_filter && nSignalLeptons >= 4 && trigger && Z1 && Z2 && met > 50) ||

              (j==7 && generator_filter && nSignalLeptons >= 4 && trigger && Z1 && Z2 && met > 100) ||

              (j==8 && generator_filter && nSignalLeptons >= 4 && trigger && Z1 && Z2 && NbJets == 0) ||

              (j==9 && generator_filter && nSignalLeptons >= 4 && trigger && Z1 && Z2 && NbJets == 0 && met > 100) ||

              (j==10 && generator_filter && nSignalLeptons >= 4 && trigger && Z1 && Z2 && NbJets == 0 && met > 200)

              )

            cutFlowVector[j]++;
          }
        #endif
      }

      /// Combine the variables of another copy of this analysis (typically on another thread) into this one.
      void combine(const Analysis* other)
      {
        const Analysis_ATLAS_13TeV_4LEP_139invfb* specificOther
                = dynamic_cast<const Analysis_ATLAS_13TeV_4LEP_139invfb*>(other);

        for (auto& pair : _counters) { pair.second += specificOther->_counters.at(pair.first); }

        #ifdef CHECK_CUTFLOW
          // if (NCUTS != specificOther->NCUTS) NCUTS = specificOther->NCUTS;
          for (size_t j = 0; j < NCUTS; j++) {
            cutFlowVector[j] += specificOther->cutFlowVector[j];
            cutFlowVector_str[j] = specificOther->cutFlowVector_str[j];
          }
        #endif
      }

      // This function can be overridden by the derived SR-specific classes
      virtual void collect_results() {
        add_result(SignalRegionData(_counters.at("SR0-ZZ-loose"), 157., {159., 42.}));
        add_result(SignalRegionData(_counters.at("SR0-ZZ-tight"), 17., {17.4, 3.3}));
        add_result(SignalRegionData(_counters.at("SR0-ZZ-loose-bveto"), 5., {7.2, 2.0}));
        add_result(SignalRegionData(_counters.at("SR0-ZZ-tight-bveto"), 1., {1.1, 0.4}));
        add_result(SignalRegionData(_counters.at("SR0-loose-bveto"), 11., {11.4, 2.4}));
        add_result(SignalRegionData(_counters.at("SR0-tight-bveto"), 1., {3.5, 2.0}));
        add_result(SignalRegionData(_counters.at("SR0-breq"), 3., {1.16, 0.26}));


        #ifdef CHECK_CUTFLOW
          size_t scale_to_row = 4;
          vector<double> cutFlowVector_scaled;
          double scale_factor = cutFlowVectorATLAS_200_50[scale_to_row]/cutFlowVector[scale_to_row];
          for (size_t i=0 ; i < cutFlowVector.size() ; i++)
          {
            cutFlowVector_scaled.push_back(cutFlowVector[i] * scale_factor);
          }
          cout << "DEBUG CUTFLOW:   Working point 200, 50%" << endl;
          cout << "DEBUG CUTFLOW:   ATLAS    GAMBIT(raw)    GAMBIT(scaled) " << endl;
          cout << "DEBUG CUTFLOW:   -------------------------------------" << endl;

          for (size_t j = 0; j < NCUTS; j++) {
            string scaled_prefix = j == scale_to_row ? "*" : "";
            cout << setprecision(4) << "DEBUG CUTFLOW:   " << cutFlowVectorATLAS_200_50[j] << "\t\t"
                                        << scaled_prefix << cutFlowVector[j] << "\t\t"
                                        << cutFlowVector_scaled[j] << "\t\t"
                                        << scaled_prefix << cutFlowVector_str[j]
                                        << endl;
          }
        #endif

        #ifdef CHECK_CUTFLOW
          scale_to_row = 4;
          cutFlowVector_scaled = vector<double>();
          scale_factor = cutFlowVectorATLAS_300_100[scale_to_row]/cutFlowVector[scale_to_row];
          for (size_t i=0 ; i < cutFlowVector.size() ; i++)
          {
            cutFlowVector_scaled.push_back(cutFlowVector[i] * scale_factor);
          }
          cout << "DEBUG CUTFLOW:   Working point 300, 100%" << endl;
          cout << "DEBUG CUTFLOW:   ATLAS    GAMBIT(raw)    GAMBIT(scaled) " << endl;
          cout << "DEBUG CUTFLOW:   -------------------------------------" << endl;

          for (size_t j = 0; j < NCUTS; j++) {
            string scaled_prefix = j == scale_to_row ? "*" : "";
            cout << setprecision(4) << "DEBUG CUTFLOW:   " << cutFlowVectorATLAS_300_100[j] << "\t\t"
                                        << scaled_prefix << cutFlowVector[j] << "\t\t"
                                        << cutFlowVector_scaled[j] << "\t\t"
                                        << scaled_prefix << cutFlowVector_str[j]
                                        << endl;
          }
        #endif
      }


    protected:
      void analysis_specific_reset() {
        for (auto& pair : _counters) { pair.second.reset(); }
        #ifdef CHECK_CUTFLOW
          std::fill(cutFlowVector.begin(), cutFlowVector.end(), 0);
        #endif
      }

    };

    // Factory fn
    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_4LEP_139invfb)


  }
}
