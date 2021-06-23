///
///  \author Are Raklev
///  \date 2021 June
///
///  Based on the search presented in ATLAS-CONF-2021-022
///  *********************************************

#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>

#include "gambit/ColliderBit/analyses/Analysis.hpp"
#include "gambit/ColliderBit/ATLASEfficiencies.hpp"
#include "gambit/ColliderBit/lester_mt2_bisect.h"

// #define CHECK_CUTFLOW

using namespace std;


namespace Gambit {
  namespace ColliderBit {


    class Analysis_ATLAS_13TeV_2BoostedBosons_139invfb : public Analysis {

    protected:
      // Signal region map
      std::map<string, EventCounter> _counters = {
        // Exclusion regions
        {"SR-4Q-WW", EventCounter("SR-4Q-WW")},
        {"SR-4Q-WZ", EventCounter("SR-4Q-WZ")},
        {"SR-4Q-ZZ", EventCounter("SR-4Q-ZZ")},
        {"SR-4Q-VV", EventCounter("SR-4Q-VV")},
        {"SR-2B2Q-WZ", EventCounter("SR-2B2Q-WZ")},
        {"SR-2B2Q-ZZ", EventCounter("SR-2B2Q-ZZ")},
        {"SR-2B2Q-Wh", EventCounter("SR-2B2Q-Wh")},
        {"SR-2B2Q-Zh", EventCounter("SR-2B2Q-Zh")},
        {"SR-2B2Q-VZ", EventCounter("SR-2B2Q-VZ")},
        {"SR-2B2Q-Vh", EventCounter("SR-2B2Q-Vh")},
        // Discovery regions
        {"Disc-SR-2B2Q", EventCounter("Disc-SR-2B2Q")},  // Union of SR-2B2Q-VZ and SR-2B2Q-Vh
        {"Disc-SR-Incl", EventCounter("Disc-SR-Incl")},  // Union of SR-4Q-VV and Disc-SR-2B2Q
      };

    private:

      // Cut-flows
      size_t NCUTS;
      vector<int> cutFlowVector;
      vector<string> cutFlowVector_str;
      vector<double> cutFlowVectorATLAS;

    public:

      // Required detector sim
      static constexpr const char* detector = "ATLAS";

      static bool sortByPT(const HEPUtils::Jet* jet1, const HEPUtils::Jet* jet2) { return (jet1->pT() > jet2->pT()); }

      Analysis_ATLAS_13TeV_2BoostedBosons_139invfb() {

        set_analysis_name("ATLAS_13TeV_2BoostedBosons_139invfb");
        set_luminosity(139.);

        NCUTS=14;

        for(size_t i=0;i<NCUTS;i++){
          cutFlowVector.push_back(0);
          cutFlowVectorATLAS.push_back(0);
          cutFlowVector_str.push_back("");
        }
      }

      // The following section copied from Analysis_ATLAS_1LEPStop_20invfb.cpp
      void JetLeptonOverlapRemoval(vector<const HEPUtils::Jet*> &jetvec, vector<const HEPUtils::Particle*> &lepvec, double DeltaRMax) {
        //Routine to do jet-lepton check
        //Discards jets if they are within DeltaRMax of a lepton

        vector<const HEPUtils::Jet*> Survivors;

        for(unsigned int itjet = 0; itjet < jetvec.size(); itjet++) {
          bool overlap = false;
          HEPUtils::P4 jetmom=jetvec.at(itjet)->mom();
          for(unsigned int itlep = 0; itlep < lepvec.size(); itlep++) {
            HEPUtils::P4 lepmom=lepvec.at(itlep)->mom();
            double dR;

            dR=jetmom.deltaR_eta(lepmom);

            if(fabs(dR) <= DeltaRMax) overlap=true;
          }
          if(overlap) continue;
          Survivors.push_back(jetvec.at(itjet));
        }
        jetvec=Survivors;

        return;
      }

      void LeptonJetOverlapRemoval(vector<const HEPUtils::Particle*> &lepvec, vector<const HEPUtils::Jet*> &jetvec) {
        //Routine to do lepton-jet check
        //Discards leptons if they are within dR of a jet as defined in analysis paper

        vector<const HEPUtils::Particle*> Survivors;

        for(unsigned int itlep = 0; itlep < lepvec.size(); itlep++) {
          bool overlap = false;
          HEPUtils::P4 lepmom=lepvec.at(itlep)->mom();
          for(unsigned int itjet= 0; itjet < jetvec.size(); itjet++) {
            HEPUtils::P4 jetmom=jetvec.at(itjet)->mom();
            double dR;
            double DeltaRMax = std::min(0.4, 0.04 + 10 / lepmom.pT());
            dR=jetmom.deltaR_eta(lepmom);

            if(fabs(dR) <= DeltaRMax) overlap=true;
          }
          if(overlap) continue;
          Survivors.push_back(lepvec.at(itlep));
        }
        lepvec=Survivors;

        return;
      }

      void run(const HEPUtils::Event* event) {

        // Get the missing energy in the event
        double met = event->met();
        HEPUtils::P4 metVec = event->missingmom();

        // Count large jets (minimum 2)
        vector<const HEPUtils::Jet*> fatJets;
        for (const HEPUtils::Jet* jet : event->jets()) {
          if (jet->pT() > 200. && fabs(jet->eta()) < 2.0 && jet->mass() > 200.)
            fatJets.push_back(jet);
        }

        // Now define vectors of baseline objects, including:
        // - retrieval of electron, muon and jets from the event
        // - application of basic pT and eta cuts

        // Electrons
        vector<const HEPUtils::Particle*> electrons;
        for (const HEPUtils::Particle* electron : event->electrons()) {
          if (electron->pT() > 5.
              && fabs(electron->eta()) < 2.47)
            electrons.push_back(electron);
        }

        // Apply electron efficiency
        ATLAS::applyElectronEff(electrons);

        // Muons
        vector<const HEPUtils::Particle*> muons;
        for (const HEPUtils::Particle* muon : event->muons()) {
          if (muon->pT() > 5.
              && fabs(muon->eta()) < 2.5)
            muons.push_back(muon);
        }

        // Apply muon efficiency
        ATLAS::applyMuonEff(muons);

//   	    // Jets
//        vector<const HEPUtils::Jet*> bJets;
//        vector<const HEPUtils::Jet*> nonbJets;
//
//        // Find b-jets
//        double btag = 0.77; double cmisstag = 1/6.; double misstag = 1./134.;
//        for (const HEPUtils::Jet* jet : candJets) {
//          // Tag
//          if( jet->btag() && random_bool(btag) ) bJets.push_back(jet);
//          // Misstag c-jet
//          else if( jet->ctag() && random_bool(cmisstag) ) bJets.push_back(jet);
//          // Misstag light jet
//          else if( random_bool(misstag) ) bJets.push_back(jet);
//          // Non b-jet
//          else nonbJets.push_back(jet);
//        }
//
//        // Overlap removal
//        JetLeptonOverlapRemoval(nonbJets,electrons,0.2);
//        LeptonJetOverlapRemoval(electrons,nonbJets);
//        JetLeptonOverlapRemoval(nonbJets,muons,0.2);
//        LeptonJetOverlapRemoval(muons,nonbJets);

        // Find veto leptons with pT > 20 GeV
        vector<const HEPUtils::Particle*> vetoElectrons;
        for (const HEPUtils::Particle* electron : electrons) {
          if (electron->pT() > 20.) vetoElectrons.push_back(electron);
        }
        vector<const HEPUtils::Particle*> vetoMuons;
        for (const HEPUtils::Particle* muon : muons) {
          if (muon->pT() > 20.) vetoMuons.push_back(muon);
        }

//        // Restrict jets to pT > 25 GeV after overlap removal
//        vector<const HEPUtils::Jet*> bJets_survivors;
//        for (const HEPUtils::Jet* jet : bJets) {
//          if(jet->pT() > 25.) bJets_survivors.push_back(jet);
//        }
//        vector<const HEPUtils::Jet*> nonbJets_survivors;
//        for (const HEPUtils::Jet* jet : nonbJets) {
//          if(jet->pT() > 25.) nonbJets_survivors.push_back(jet);
//        }
//        vector<const HEPUtils::Jet*> jet_survivors;
//        jet_survivors = nonbJets_survivors;
//        for (const HEPUtils::Jet* jet : bJets) {
//          jet_survivors.push_back(jet);
//        }
//        std::sort(jet_survivors.begin(), jet_survivors.end(), sortByPT);
//
//        // Number of objects
//        size_t nbJets = bJets_survivors.size();
//        size_t nnonbJets = nonbJets_survivors.size();
//        size_t nJets = nbJets + nnonbJets;
//        //size_t nJets = jet_survivors.size();
        size_t nMuons=vetoMuons.size();
        size_t nElectrons=vetoElectrons.size();
        size_t nLeptons = nElectrons+nMuons;

        // Effective mass (missing energy plus two leading fatjet pTs)
        double meff = met;
        meff += fatJets[0]->pT();
        meff += fatJets[1]->pT();

        // Stransverse mass (two leading fat jets as legs, assumes 100 GeV invisible mass)
//        double MT2 =  asymm_mt2_lester_bisect::get_mT2(
//fatJets[0]->mom()
//        fatJets[0]->mass(), fatJets[0]->px(), fatJets[0]->py(),
//        fatJets[1]->mass(), fatJets[1]->px(), fatJets[1]->py(),
//        pxMiss, pyMiss,
//        100., 100., 0);


        // Increment cutFlowVector elements
        // Cut flow strings
//        cutFlowVector_str[0]  = "No cuts ";
//        cutFlowVector_str[1]  = "Trigger, $E_T^{miss} > 200$ GeV";
//        cutFlowVector_str[2]  = "$\\Delta\\phi_{min}^{4j} > 0.4$";
//        cutFlowVector_str[3]  = "$N_{lep} = 0$";
//        cutFlowVector_str[4]  = "$N_{jet} \\ge 4$, $N_{jet} \\le 5$";
//        cutFlowVector_str[5]  = "$110 < m(h_1)< 150$ GeV";
//        cutFlowVector_str[6]  = "$90 < m(h_2)< 140$ GeV$";
//        cutFlowVector_str[7]  = "$m_{T,min}^{b-jets}> 130$ GeV";
//        cutFlowVector_str[8]  = "$m_{eff} > 1100$ GeV";
//        cutFlowVector_str[9]  = "$N_{b-jets} \\ge 3$";
//        cutFlowVector_str[10]  = "$0.4 \\le \\Delta R_{max}^{bb} \\le 1.4$";
//        cutFlowVector_str[11]  = "m_{eff} > 600 GeV";
//        cutFlowVector_str[12]  = "$N_{b-jet} \\ge 4$";
//        cutFlowVector_str[13]  = "$0.4 \\le \\Delta R_{max}^{bb} \\le 1.4$";

        // Cut flow from paper
        // Higgsino 300 GeV
//        cutFlowVectorATLAS[0] = 10276.0;
//        cutFlowVectorATLAS[1] =  1959.1;
//        cutFlowVectorATLAS[2] =  1533.0;
//        cutFlowVectorATLAS[3] =  1319.3;
//        cutFlowVectorATLAS[4] =   664.9;
//        cutFlowVectorATLAS[5] =   249.3;
//        cutFlowVectorATLAS[6] =   123.0;
//        cutFlowVectorATLAS[7] =    74.3;
//        cutFlowVectorATLAS[8] =     4.0;
//        cutFlowVectorATLAS[9] =     1.5;
//        cutFlowVectorATLAS[10] =    1.4;
//        cutFlowVectorATLAS[11] =   90.2;
//        cutFlowVectorATLAS[12] =   15.6;
//        cutFlowVectorATLAS[13] =    6.8;
        // Higgsino 500 GeV
//        cutFlowVectorATLAS[0] = 1220.7;
//        cutFlowVectorATLAS[1] =  739.0;
//        cutFlowVectorATLAS[2] =  647.1;
//        cutFlowVectorATLAS[3] =  548.2;
//        cutFlowVectorATLAS[4] =  291.9;
//        cutFlowVectorATLAS[5] =  133.5;
//        cutFlowVectorATLAS[6] =   78.0;
//        cutFlowVectorATLAS[7] =   64.1;
//        cutFlowVectorATLAS[8] =   12.0;
//        cutFlowVectorATLAS[9] =    5.7;
//        cutFlowVectorATLAS[10] =   4.8;
//        cutFlowVectorATLAS[11] =  74.3;
//        cutFlowVectorATLAS[12] =  15.0;
//        cutFlowVectorATLAS[13] =   9.7;
        // Higgsino 800 GeV
//        cutFlowVectorATLAS[0] = 124.9;
//        cutFlowVectorATLAS[1] = 101.9;
//        cutFlowVectorATLAS[2] =  89.5;
//        cutFlowVectorATLAS[3] =  73.7;
//        cutFlowVectorATLAS[4] =  39.4;
//        cutFlowVectorATLAS[5] =  19.0;
//        cutFlowVectorATLAS[6] =  13.4;
//        cutFlowVectorATLAS[7] =  11.8;
//        cutFlowVectorATLAS[8] =   8.1;
//        cutFlowVectorATLAS[9] =   3.8;
//        cutFlowVectorATLAS[10] =  3.6;
//        cutFlowVectorATLAS[11] = 13.3;
//        cutFlowVectorATLAS[12] =  2.3;
//        cutFlowVectorATLAS[13] =  2.0;

        // Apply cutflow
//        for(size_t j=0;j<NCUTS;j++){
//          if(
//             (j==0) ||
//
//             (j==1 && met > 200.) ||
//
//             (j==2 && met > 200 && phi4min > 0.4) ||
//
//             (j==3 && met > 200 && phi4min > 0.4 && nLeptons == 0) ||
//
//             (j==4 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5)) ||
//
//             (j==5 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5) && mlead > 110. && mlead < 150.) ||
//
//             (j==6 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5) && mlead > 110. && mlead < 150. && msubl > 90. && msubl < 140.) ||
//
//             (j==7 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5) && mlead > 110. && mlead < 150. && msubl > 90. && msubl < 140. && mTmin > 130.) ||
//
//             (j==8 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5)  && mlead > 110. && mlead < 150. && msubl > 90. && msubl < 140. && mTmin > 130. && meff > 1100.) ||
//
//             (j==9 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5)  && mlead > 110. && mlead < 150. && msubl > 90. && msubl < 140. && mTmin > 130. && meff > 1100. && nbJets >= 3) ||
//
//             (j==10 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5)  && mlead > 110. && mlead < 150. && msubl > 90. && msubl < 140. && mTmin > 130. && meff > 1100. && nbJets >= 3 && Rbbmax > 0.4 && Rbbmax < 1.4) ||
//
//             (j==11 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5) && mlead > 110. && mlead < 150. && msubl > 90. && msubl < 140. && meff > 600.) ||
//
//             (j==12 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5) && mlead > 110. && mlead < 150. && msubl > 90. && msubl < 140. && meff > 600. && nbJets >= 4) ||
//
//             (j==13 && met > 200 && phi4min > 0.4 && nLeptons == 0 && (nJets == 4 || nJets == 5) && mlead > 110. && mlead < 150. && msubl > 90. && msubl < 140. && meff > 600. && nbJets >= 4 && Rbbmax > 0.4 && Rbbmax < 1.4)
//
//             ) cutFlowVector[j]++;
//        }

        // Now increment signal region variables
        // First exclusion regions
        if(met > 200 && nLeptons == 0) _counters.at("SR-4Q-WW").add_event(event);
        // Discovery regions
        if(met > 200 && nLeptons == 0) _counters.at("Disc-SR-2B2Q").add_event(event);

        return;

      } // End of analyze

      
      /// Combine the variables of another copy of this analysis (typically on another thread) into this one.
      void combine(const Analysis* other)
      {
        const Analysis_ATLAS_13TeV_2BoostedBosons_139invfb* specificOther = dynamic_cast<const Analysis_ATLAS_13TeV_2BoostedBosons_139invfb*>(other);
        for (auto& pair : _counters) { pair.second += specificOther->_counters.at(pair.first); }
      }


      void collect_results() {

        // Cut-flow printout
        #ifdef CHECK_CUTFLOW
//        double L = 36.1;
////        double xsec = 284.65; // 300 GeV
////        double xsec = 33.81; // 500 GeV
//        double xsec = 3.460; // 800 GeV
//        cout << "DEBUG:" << endl;
//        for (size_t i=0; i<NCUTS; i++)
//        {
//          double ATLAS_abs = cutFlowVectorATLAS[i];
//
//          double eff = (double)cutFlowVector[i] / (double)cutFlowVector[0];
//
//          double GAMBIT_scaled = eff * xsec * L;
//
//          double ratio = GAMBIT_scaled/ATLAS_abs;
//          cout << "DEBUG 1: i: " << i << ":   " << setprecision(4) << ATLAS_abs << "\t" << GAMBIT_scaled << "\t" << "\t" << ratio << "\t\t" << cutFlowVector_str[i] << endl;
//        }
//        cout << "DEBUG:" << endl;
        #endif

        // Now fill a results object with the results for each SR
        add_result(SignalRegionData(_counters.at("SR-4Q-WW"),   2., {1.9, 0.4}));
        add_result(SignalRegionData(_counters.at("SR-4Q-WZ"),   3., {3.4, 0.7}));
        add_result(SignalRegionData(_counters.at("SR-4Q-ZZ"),   1., {1.9, 0.5}));
        add_result(SignalRegionData(_counters.at("SR-4Q-VV"),   3., {3.9, 0.8}));
        add_result(SignalRegionData(_counters.at("SR-2B2Q-WZ"), 2., {1.6, 0.4}));
        add_result(SignalRegionData(_counters.at("SR-2B2Q-ZZ"), 2., {1.7, 0.5}));
        add_result(SignalRegionData(_counters.at("SR-2B2Q-Wh"), 0., {1.9, 0.7}));
        add_result(SignalRegionData(_counters.at("SR-2B2Q-Zh"), 1., {1.6, 0.5}));
        add_result(SignalRegionData(_counters.at("SR-2B2Q-VZ"), 2., {2.2, 0.6}));
        add_result(SignalRegionData(_counters.at("SR-2B2Q-Vh"), 1., {2.5, 0.8}));
        add_result(SignalRegionData(_counters.at("Disc-SR-2B2Q"), 3., {4.7, 1.0})); // Union of SR-2B2Q-VZ and SR-2B2Q-Vh
        add_result(SignalRegionData(_counters.at("Disc-SR-Incl"), 6., {8.6, 1.3})); // Union of SR-4Q-VV and Disc-SR-2B2Q

      }

      void analysis_specific_reset() {
        // Clear signal regions
        for (auto& pair : _counters) { pair.second.reset(); }

        // Clear cut flow vector
        std::fill(cutFlowVector.begin(), cutFlowVector.end(), 0);
      }

    };


    // Factory fn
    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_2BoostedBosons_139invfb)


  }
}
