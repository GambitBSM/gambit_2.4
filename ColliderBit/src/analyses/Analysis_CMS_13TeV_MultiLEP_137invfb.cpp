///
///  \author Tomas Gonzalo
///          (gonzalo@physik.rwth-aachen.de)
///  \date 2021 July
///  *********************************************


#include "gambit/ColliderBit/analyses/Analysis.hpp"
#include "gambit/ColliderBit/Utils.hpp"
//#include "gambit/ColliderBit/CMSEfficiencies.hpp"

// Based on http://cms-results.web.cern.ch/cms-results/public-results/publications/SUS-19-012/index.html

namespace Gambit
{
  namespace ColliderBit
  {

    class Analysis_CMS_13TeV_MultiLEP_137invfb : public Analysis
    {

      protected:
        // Counters for the number of accepted events for each signal region
        std::map<str, EventCounter> _counters;

      private:

        inline str SR(str type, size_t count)
        {
          return count < 10 ? type + "0" + std::to_string(count) : type + std::to_string(count);
        }

      public:
 
        // Required detector sim
        static constexpr const char* detector = "CMS";

        Analysis_CMS_13TeV_MultiLEP_137invfb()
        {

          // Fill counters map
          // 2SSLep (2lSS)
          for(size_t i=1; i<=20; ++i)
            _counters[SR("SS",i)] =  EventCounter(SR("SS",i));
          // 3Lep, OSSF pair (3lA)
          for(size_t i=1; i<=64; ++i)
            _counters[SR("A",i)] = EventCounter(SR("A",i));
          // 3Lep, no OSSF pair (3lB)
          for(size_t i=1; i<=3; ++i)
            _counters[SR("B",i)] = EventCounter(SR("B",i));
          // 3Lep, OSSF pair + tau (3lC)
          for(size_t i=1; i<=9; ++i)
            _counters[SR("C",i)] = EventCounter(SR("C",i));
          // 3Lep, no OSSF pair, 2 OS light leptons + tau (3lD)
          for(size_t i=1; i<=16; ++i)
            _counters[SR("D",i)] = EventCounter(SR("D",i));
          // 3Lep, no OSSF pair, 2 SS light leptons + tau (3lE)
          for(size_t i=1; i<=9; ++i)
            _counters[SR("E",i)] = EventCounter(SR("E",i));
          // 3Lep, 2 tau (3lF)
          for(size_t i=1; i<=12; ++i)
            _counters[SR("F",i)] = EventCounter(SR("F",i));
          // 4Lep, 2 OSSF pairs (4lG)
          for(size_t i=1; i<=5; ++i)
            _counters[SR("G",i)] = EventCounter(SR("G",i));
          // 4Lep, 1 or fewer OSSF pairs (4lH)
          for(size_t i=1; i<=3; ++i)
            _counters[SR("H",i)] = EventCounter(SR("H",i));
          // 4Lep, tau + 3 light leptons (4lI)
          for(size_t i=1; i<=3; ++i)
            _counters[SR("I",i)] = EventCounter(SR("I",i));
          // 4Lep, 2 tau + 2 light leptons, 2 OSSF pairs (4lJ)
          for(size_t i=1; i<=3; ++i)
            _counters[SR("J",i)] = EventCounter(SR("J",i));
          // 4Lep, 2 tau + 2 light leptons, 1 or fewer OSSF pairs (4lK)
          for(size_t i=1; i<=3; ++i)
            _counters[SR("K",i)] = EventCounter(SR("K",i));


          set_analysis_name("CMS_13TeV_MultiLEP_137invfb");
          set_luminosity(137.0);

        }

        void run(const HEPUtils::Event* event)
        {

          // Useful constants
          double mZ = 91.1876;

          // Missing ET and momentum
          double met = event->met();
          P4 mmom = event->missingmom();

          // Baseline electrons
          std::vector<const HEPUtils::Particle*> baselineElectrons;
          for (const HEPUtils::Particle* electron : event->electrons())
          {
            // TODO: Missing efficiency in electron reconstruction
            bool isEl=1;//has_tag(_eff2dEl, fabs(electron->eta()), electron->pT());
            if (electron->pT()>10. && fabs(electron->eta())<2.5 && isEl)
              baselineElectrons.push_back(electron);
          }

          // Baseline muons
          std::vector<const HEPUtils::Particle*> baselineMuons;
          for (const HEPUtils::Particle* muon : event->muons())
          {
            // TODO: Missing efficiency in muon reconstruction
            bool isMu=1;//has_tag(_eff2dEl, fabs(muon->eta()), muon->pT());
            if (muon->pT()>10. && fabs(muon->eta())<2.4 && isMu)
              baselineMuons.push_back(muon);
          }

          // TODO: Some vertex and overlap stuff

          // TODO: Prompt stuff

          // Baseline taus
          std::vector<const HEPUtils::Particle*> baselineTaus;
          for (const HEPUtils::Particle* tau : event->taus())
          {
            // TODO: Missing efficiency in tau reconstruction
            bool isTau=1;//has_tag(_eff2dTau, fabs(tau->eta()), tau->pT());
            if (tau->pT()>20. &&fabs(tau->eta())<2.3 && isTau)
              baselineTaus.push_back(tau);
          }

          // TODO: Tight vs loose

          // Baseline jets
          std::vector<const HEPUtils::Jet*> baselineJets;
          for (const HEPUtils::Jet* jet : event->jets())
          {
            if (jet->pT()>25. &&fabs(jet->eta())<2.4)
              baselineJets.push_back(jet);
          }
          // TODO: Separation of R > 0.4 from leptons

          // TODO: Veto events with b-tagged jets

          // TODO: Veto mll < 12 GeV


          ////////////////////
          // Signal objects //
          std::vector<const HEPUtils::Particle*> signalElectrons=baselineElectrons;
          std::vector<const HEPUtils::Particle*> signalMuons=baselineMuons;
          std::vector<const HEPUtils::Particle*> signalTaus=baselineTaus;
          sortByPt(signalTaus);

          std::vector<const HEPUtils::Particle*> signalLightLeptons=signalElectrons;
          signalLightLeptons.insert(signalLightLeptons.end(), signalMuons.begin(), signalMuons.end());
          sortByPt(signalLightLeptons);

          std::vector<std::vector<const HEPUtils::Particle*> > SSpairs = getSSpairs(signalLightLeptons);
          std::vector<std::vector<const HEPUtils::Particle*> > OSSFpairs = getSFOSpairs(signalLightLeptons);
          sortByParentMass(OSSFpairs, mZ);


          std::vector<const HEPUtils::Particle*> signalLeptons=signalTaus;
          signalLeptons.insert(signalLeptons.end(), signalLightLeptons.begin(), signalLightLeptons.end());
          sortByPt(signalLeptons);

          std::vector<std::vector<const HEPUtils::Particle*> > OSpairs = getOSpairs(signalLeptons);
          sortByParentMass(OSpairs, mZ);         

          std::vector<const HEPUtils::Jet*> signalJets = baselineJets;
          sortByPt(signalJets);
          std::vector<const HEPUtils::Jet*> signalBJets;
          for(const Jet* j: signalJets)
            if(j->btag()) signalBJets.push_back(j);
          

          ///////////////////////////////
          // Common variables and cuts //
          const size_t nLeptons = signalLeptons.size();
          const size_t nLightLeptons = signalLightLeptons.size();
          const size_t nTaus = nLeptons - nLightLeptons;
          const size_t nSSpairs = SSpairs.size();
          const size_t nOSSFpairs = OSSFpairs.size();

          const size_t nJets = signalJets.size();
          const size_t nBJets = signalBJets.size();

          // Di-lepton invariant mass for OSSF pairs of light leptons
          std::vector<double> mossf;
          for(auto pair: OSSFpairs)
            mossf.push_back( (pair.at(0)->mom() + pair.at(1)->mom()).m() );
          std::sort(mossf.begin(), mossf.end());

          // Invariant mass of third lepton in the event with the rest of leptons
          // TODO: Unused for now
          std::vector<double> m3l;
          if(nLeptons > 2)
            for(auto lep: signalLeptons)
              m3l.push_back( (signalLeptons.at(2)->mom() + lep->mom() ).m());
          

           // Flags for lepton pairs
          bool muonPair = nLightLeptons == 2 and amIaMuon(signalLeptons.at(0)) and amIaMuon(signalLeptons.at(1));
          bool electronPair = nLightLeptons == 2 and amIanElectron(signalLeptons.at(0)) and amIanElectron(signalLeptons.at(1));
          bool mixedPair = nLightLeptons == 2 and ( ( amIaMuon(signalLeptons.at(0)) and amIanElectron(signalLeptons.at(1)) ) or
                                                  ( amIanElectron(signalLeptons.at(0)) and amIaMuon(signalLeptons.at(1)) ) );

          //////////////////
          // Preselection //
          if(nLeptons < 3 and (nLightLeptons < 2 or nSSpairs == 0) ) return;
          if(nBJets > 0) return;
          if(nOSSFpairs > 0 and mossf[0] < 12.0) return;

          ////////////////////
          // Signal regions //

          // 2SSLep, (2lSS)
          // TODO: Missing implementing the case of a third loose lepton
          if(nLightLeptons == 2 and nLeptons == 2 and
             ( (muonPair and signalLeptons.at(0)->pT() > 20.) or ((electronPair or mixedPair) and signalLeptons.at(0)->pT() > 25.) ) and
             ( (amIanElectron(signalLeptons.at(1)) and signalLeptons.at(1)->pT() >  15.) or (amIaMuon(signalLeptons.at(1)) and signalLeptons.at(1)->pT() > 10.) ) and
             ( nJets < 2 or signalJets.at(1)->pT() < 40) and
             met > 60. )
          {

            // Stransverse mass
            double mT2 = get_mT2(signalLeptons.at(0), signalLeptons.at(1), mmom, 0);

            // pT of dilepton system for SS leptons
            double pTll = ( signalLeptons.at(0)->mom() + signalLeptons.at(1)->mom() ).pT();

            // Sign of final states
            bool positive = signalLeptons.at(0)->pid() * signalLeptons.at(1)->pid() > 0;
            bool negative = not positive;

            if(mT2 == 0. and pTll <  70.) _counters.at("SS01").add_event(event);
            if(mT2 == 0. and pTll >= 70. and met <  100.) _counters.at("SS02").add_event(event);
            if(mT2 == 0. and pTll >= 70. and met >= 100. and met < 200. and positive) _counters.at("SS03").add_event(event);
            if(mT2 == 0. and pTll >= 70. and met >= 100. and met < 200. and negative) _counters.at("SS04").add_event(event);
            if(mT2 == 0. and pTll >= 70. and met >= 200. and positive) _counters.at("SS05").add_event(event);
            if(mT2 == 0. and pTll >= 70. and met >= 200. and negative) _counters.at("SS06").add_event(event);
            if(mT2 >  0. and mT2 <= 80. and pTll <  30. and met <  200. and positive) _counters.at("SS07").add_event(event);
            if(mT2 >  0. and mT2 <= 80. and pTll <  30. and met <  200. and negative) _counters.at("SS08").add_event(event);
            if(mT2 >  0. and mT2 <= 80. and pTll <  30. and met >= 200.) _counters.at("SS09").add_event(event);
            if(mT2 >  0. and mT2 <= 80. and pTll >= 30.) _counters.at("SS10").add_event(event);
            if(mT2 > 80. and pTll <  200. and met <  100.) _counters.at("SS11").add_event(event);
            if(mT2 > 80. and pTll <  200. and met >= 100. and met < 200. and positive) _counters.at("SS12").add_event(event);
            if(mT2 > 80. and pTll <  200. and met >= 100. and met < 200. and negative) _counters.at("SS13").add_event(event);
            if(mT2 > 80. and pTll <  200. and met >= 200. and positive) _counters.at("SS14").add_event(event);
            if(mT2 > 80. and pTll <  200. and met >= 200. and negative) _counters.at("SS15").add_event(event);
            if(mT2 > 80. and pTll >= 200. and met <  100.) _counters.at("SS16").add_event(event);
            if(mT2 > 80. and pTll >= 200. and met >= 100. and met < 200. and positive) _counters.at("SS17").add_event(event);
            if(mT2 > 80. and pTll >= 200. and met >= 100. and met < 200. and negative) _counters.at("SS18").add_event(event);
            if(mT2 > 80. and pTll >= 200. and met >= 200. and positive) _counters.at("SS19").add_event(event);
            if(mT2 > 80. and pTll >= 200. and met >= 200. and negative) _counters.at("SS20").add_event(event);
          }

          // Selection conditon for 3 lepton events
          bool _3Lep = nLeptons == 3 and 
                      nLightLeptons > 0 and 
                      ( (amIanElectron(signalLightLeptons.at(0)) and signalLightLeptons.at(0)->pT() > 25.) or 
                        (amIaMuon(signalLightLeptons.at(0)) and signalLightLeptons.at(0)->pT() > 20.) ) and
                      ( nLightLeptons < 2 or 
                        (amIanElectron(signalLightLeptons.at(1)) and signalLightLeptons.at(1)->pT() > 15.) or 
                        (amIaMuon(signalLightLeptons.at(1)) and signalLightLeptons.at(1)->pT() > 10.) ) and
                      met > 50;

          // 3Lep, OSSF pair (3lA)
          if(_3Lep and nLightLeptons == 3 and nOSSFpairs > 0)
          {
            // Mll variable, OSSF pairs are already ordered by how close they are to mZ
            double mll = (OSSFpairs.at(0).at(0)->mom() + OSSFpairs.at(0).at(1)->mom()).m();

            // MT variable
            double mT = 0.;
            for(auto lepton : signalLeptons)
              if(lepton != OSSFpairs.at(0).at(0) and lepton != OSSFpairs.at(0).at(1))
                mT = sqrt( 2*mmom.pT()*lepton->pT()*(1 - cos(lepton->phi() - mmom.phi())) );

            //MT3l variable
            double mT3l = get_mT(signalLeptons.at(0), signalLeptons.at(1), signalLeptons.at(2), mmom);

            // HT variable
            double HT = scalarSumPt(signalJets);

            // Signal regions
            if(mll < 50. and mT >= 0.   and mT < 100. and mT3l >=  0. and mT3l <  50.) _counters.at("A01").add_event(event);
            if(mll < 50. and mT >= 0.   and mT < 100. and mT3l >= 50. and mT3l < 100.) _counters.at("A02").add_event(event);
            if(mll < 50. and mT >= 0.   and mT < 100. and mT3l >= 100.) _counters.at("A03").add_event(event);
            if(mll < 50. and mT >= 100. and mT < 200.) _counters.at("A04").add_event(event);
            if(mll < 50. and mT >= 200.) _counters.at("A05").add_event(event);

            if(mll >= 50. and mll < 75. and mT >=   0. and mT < 100. and mT3l >=   0. and mT3l < 100.) _counters.at("A06").add_event(event);
            if(mll >= 50. and mll < 75. and mT >=   0. and mT < 100. and mT3l >= 100. and mT3l < 400.) _counters.at("A07").add_event(event);
            if(mll >= 50. and mll < 75. and mT >=   0. and mT < 100. and mT3l >= 400.) _counters.at("A08").add_event(event);
            if(mll >= 50. and mll < 75. and mT >= 100. and mT < 200. and mT3l >=   0. and mT3l < 200.) _counters.at("A09").add_event(event);
            if(mll >= 50. and mll < 75. and mT >= 100. and mT < 200. and mT3l >= 200.) _counters.at("A10").add_event(event);
            if(mll >= 50. and mll < 75. and mT >= 200. and mT3l >=   0. and mT3l < 400.) _counters.at("A11").add_event(event);
            if(mll >= 50. and mll < 75. and mT >= 200. and mT3l >= 400.) _counters.at("A12").add_event(event);

            if(mll >= 105. and mll < 250. and mT >=   0. and mT < 100. and mT3l >=   0. and mT3l < 400.) _counters.at("A13").add_event(event);
            if(mll >= 105. and mll < 250. and mT >=   0. and mT < 100. and mT3l >= 400.) _counters.at("A14").add_event(event);
            if(mll >= 105. and mll < 250. and mT >= 100. and mT < 200. and mT3l >=   0. and mT3l < 200.) _counters.at("A15").add_event(event);
            if(mll >= 105. and mll < 250. and mT >= 100. and mT < 200. and mT3l >= 200.) _counters.at("A16").add_event(event);
            if(mll >= 105. and mll < 250. and mT >= 200. and mT3l >=   0. and mT3l < 400.) _counters.at("A17").add_event(event);
            if(mll >= 105. and mll < 250. and mT >= 200. and mT3l >= 400.) _counters.at("A18").add_event(event);

            if(mll >= 250. and mT >=   0. and mT < 100. and mT3l >=   0. and mT3l < 400.) _counters.at("A19").add_event(event);
            if(mll >= 250. and mT >=   0. and mT < 100. and mT3l >= 400.) _counters.at("A20").add_event(event);
            if(mll >= 250. and mT >= 100. and mT < 200.) _counters.at("A21").add_event(event);
            if(mll >= 250. and mT >= 200.) _counters.at("A22").add_event(event);

            if(mll >= 75. and mll < 105. and HT < 100 and mT >=   0. and mT < 100. and met >=  50. and met < 100.) _counters.at("A23").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >=   0. and mT < 100. and met >= 100. and met < 150.) _counters.at("A24").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >=   0. and mT < 100. and met >= 150. and met < 200.) _counters.at("A25").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >=   0. and mT < 100. and met >= 200. and met < 250.) _counters.at("A26").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >=   0. and mT < 100. and met >= 250.) _counters.at("A27").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >= 100. and mT < 160. and met >=  50. and met < 100.) _counters.at("A28").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >= 100. and mT < 160. and met >= 100. and met < 150.) _counters.at("A29").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >= 100. and mT < 160. and met >= 150. and met < 200.) _counters.at("A30").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >= 100. and mT < 160. and met >= 200.) _counters.at("A31").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >= 160. and met >=  50. and met < 100.) _counters.at("A32").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >= 160. and met >= 100. and met < 150.) _counters.at("A33").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >= 160. and met >= 150. and met < 200.) _counters.at("A34").add_event(event);
            if(mll >= 75. and mll < 105. and HT < 100 and mT >= 160. and met >= 200.) _counters.at("A35").add_event(event);

            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >=   0. and mT < 100. and met >=  50. and met < 100.) _counters.at("A36").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >=   0. and mT < 100. and met >= 100. and met < 150.) _counters.at("A37").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >=   0. and mT < 100. and met >= 150. and met < 200.) _counters.at("A38").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >=   0. and mT < 100. and met >= 200. and met < 250.) _counters.at("A39").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >=   0. and mT < 100. and met >= 250.) _counters.at("A40").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >= 100. and mT < 160. and met >=  50. and met < 100.) _counters.at("A41").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >= 100. and mT < 160. and met >= 100. and met < 150.) _counters.at("A42").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >= 100. and mT < 160. and met >= 150. and met < 200.) _counters.at("A43").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >= 100. and mT < 160. and met >= 200.) _counters.at("A44").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >= 160. and met >=  50. and met < 100.) _counters.at("A45").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >= 160. and met >= 100. and met < 150.) _counters.at("A46").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >= 160. and met >= 150. and met < 200.) _counters.at("A47").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 100. and HT < 200 and mT >= 160. and met >= 200.) _counters.at("A48").add_event(event);

            if(mll >= 75. and mll < 105. and HT >= 200. and mT >=   0. and mT < 100. and met >=  50. and met < 150.) _counters.at("A49").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >=   0. and mT < 100. and met >= 150. and met < 250.) _counters.at("A50").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >=   0. and mT < 100. and met >= 250. and met < 350.) _counters.at("A51").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >=   0. and mT < 100. and met >= 350.) _counters.at("A52").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 100. and mT < 160. and met >=  50. and met < 100.) _counters.at("A53").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 100. and mT < 160. and met >= 100. and met < 150.) _counters.at("A54").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 100. and mT < 160. and met >= 150. and met < 200.) _counters.at("A55").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 100. and mT < 160. and met >= 200. and met < 250.) _counters.at("A56").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 100. and mT < 160. and met >= 250. and met < 300.) _counters.at("A57").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 100. and mT < 160. and met >= 300.) _counters.at("A58").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 160. and met >=  50. and met < 100.) _counters.at("A59").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 160. and met >= 100. and met < 150.) _counters.at("A60").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 160. and met >= 150. and met < 200.) _counters.at("A61").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 160. and met >= 200. and met < 250.) _counters.at("A61").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 160. and met >= 250. and met < 300.) _counters.at("A63").add_event(event);
            if(mll >= 75. and mll < 105. and HT >= 200. and mT >= 160. and met >= 300.) _counters.at("A64").add_event(event);
          }

          // 3Lep, no OSSF pair (3lB)
          if(_3Lep and nLightLeptons == 3 and nOSSFpairs == 0)
          {
            // Min DeltaR variable
            double minDeltaR = 0.;
            for(auto lep1 : signalLeptons)
              for(auto lep2 : signalLeptons)
                if(lep1 != lep2 and (minDeltaR > deltaR_eta(lep1->mom(),lep2->mom()) or minDeltaR == 0.) )
                  minDeltaR = deltaR_eta(lep1->mom(),lep2->mom());

            if(minDeltaR <  0.4) _counters.at("B01").add_event(event);
            if(minDeltaR >= 0.4 and minDeltaR < 1.0) _counters.at("B02").add_event(event);
            if(minDeltaR >= 1.0) _counters.at("B03").add_event(event);
          }

          // 3Lep, OSSF pair + tau (3lC)
          if(_3Lep and nTaus == 1 and nOSSFpairs > 0)
          {

            // mT2 variable
            // TODO: Is this the ll MT2?
            // TODO: Use Lester's
            double mT2 = get_mT2(signalLightLeptons.at(0), signalLightLeptons.at(1), mmom, 0);

            // mll variable
            double mll = (signalLightLeptons.at(0)->mom() + signalLightLeptons.at(1)->mom()).m();

            // mT2l variable
            double mT2l = get_mT(signalLightLeptons.at(0), signalLightLeptons.at(1), mmom);

            if(abs(mll - mZ) > 15. and met >=  50. and met < 200. and mT2l >= 0. and mT2 <   80.) _counters.at("C01").add_event(event);
            if(abs(mll - mZ) > 15. and met >=  50. and met < 200. and mT2l >= 0. and mT2 >=  80. and mT2 < 120.) _counters.at("C02").add_event(event);
            if(abs(mll - mZ) > 15. and met >=  50. and met < 200. and mT2l >= 0. and mT2 >= 120.) _counters.at("C03").add_event(event);
            if(abs(mll - mZ) > 15. and met >= 200. and met < 300. and mT2l >= 0. and mT2 <   80.) _counters.at("C04").add_event(event);
            if(abs(mll - mZ) > 15. and met >= 200. and met < 300. and mT2l >= 0. and mT2 >=  80. and mT2 < 120.) _counters.at("C05").add_event(event);
            if(abs(mll - mZ) > 15. and met >= 200. and met < 300. and mT2l >= 0. and mT2 >= 120.) _counters.at("C06").add_event(event);
            if(abs(mll - mZ) > 15. and met >= 300. and mT2l >=   0. and mT2l < 250.) _counters.at("C07").add_event(event);
            if(abs(mll - mZ) > 15. and met >= 300. and mT2l >= 250. and mT2l < 500.) _counters.at("C08").add_event(event);
            if(abs(mll - mZ) > 15. and met >= 300. and mT2l >= 500.) _counters.at("C09").add_event(event);
          }

          // 3Lep, no OSSF pair, 2 OS light leptons + tau (3lD)
          if(_3Lep and nTaus == 1 and nOSSFpairs == 0 and oppositeSign(signalLightLeptons.at(0), signalLightLeptons.at(1)))
          {
            // mll variable, OSpairs are alread ordered by how close they are to mZ
            double mll = (OSpairs.at(0).at(0)->mom() + OSpairs.at(0).at(1)->mom()).m();

            // mT2 variable
            // TODO: Lester's?
            double mT2 = get_mT2(signalLightLeptons.at(0), signalLightLeptons.at(1), mmom, 0.);

            if(mT2 >= 0. and mT2 < 100. and mll < 60. and met >=  50. and met < 100.) _counters.at("D01").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll < 60. and met >= 100. and met < 150.) _counters.at("D02").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll < 60. and met >= 150. and met < 200.) _counters.at("D03").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll < 60. and met >= 200. and met < 250.) _counters.at("D04").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll < 60. and met >= 250.) _counters.at("D05").add_event(event);

            if(mT2 >= 0. and mT2 < 100. and mll >= 60. and mll < 100. and met >=  50. and met < 100.) _counters.at("D06").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll >= 60. and mll < 100. and met >= 100. and met < 150.) _counters.at("D07").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll >= 60. and mll < 100. and met >= 150. and met < 200.) _counters.at("D08").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll >= 60. and mll < 100. and met >= 200. and met < 250.) _counters.at("D09").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll >= 60. and mll < 100. and met >= 250.) _counters.at("D10").add_event(event);

            if(mT2 >= 0. and mT2 < 100. and mll >= 100. and met >=  50. and met < 100.) _counters.at("D11").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll >= 100. and met >= 100. and met < 150.) _counters.at("D12").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll >= 100. and met >= 150. and met < 200.) _counters.at("D13").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mll >= 100. and met >= 200.) _counters.at("D14").add_event(event);

            if(mT2 >= 100. and met >=  50. and met < 200.) _counters.at("D15").add_event(event);
            if(mT2 >= 100. and met >= 200.) _counters.at("D16").add_event(event);

          }

          // 3Lep, no OSSF pair, 2 SS light leptons + tau (3lE)
          if(_3Lep and nTaus == 1 and nOSSFpairs == 0 and sameSign(signalLightLeptons.at(0), signalLightLeptons.at(1)))
          {
            // mlth variable
            double mlth1 = (signalTaus.at(0)->mom() + signalLightLeptons.at(0)->mom()).m();
            double mlth2 = (signalTaus.at(0)->mom() + signalLightLeptons.at(1)->mom()).m();
            double mlth = abs(mlth1 - mZ) < abs(mlth2 -mZ) ? mlth1 : mlth2;

            // Set mlth to zero is the tau has the same sign as the pair of light leptons
            if(sameSign(signalTaus.at(0), signalLightLeptons.at(0))) mlth = 0.;

            // mT2 varaible
            // TODO:  Lester's
            double mT2 = get_mT2(signalLightLeptons.at(0), signalTaus.at(0), mmom, 0.);

            if(mT2 >= 0. and mT2 < 80. and mlth <= 50. and met >=  50. and met < 100.) _counters.at("E01").add_event(event);
            if(mT2 >= 0. and mT2 < 80. and mlth <= 50. and met >= 100. and met < 250.) _counters.at("E02").add_event(event);
            if(mT2 >= 0. and mT2 < 80. and mlth <= 50. and met >= 250.) _counters.at("E03").add_event(event);
            if(mT2 >= 0. and mT2 < 80. and mlth >  50. and met >=  50. and met < 100.) _counters.at("E04").add_event(event);
            if(mT2 >= 0. and mT2 < 80. and mlth >  50. and met >= 100.) _counters.at("E05").add_event(event); 

            if(mT2 >= 80. and mlth <= 100. and met >=  50. and met < 150.) _counters.at("E06").add_event(event);
            if(mT2 >= 80. and mlth <= 100. and met >= 150.) _counters.at("E07").add_event(event);
            if(mT2 >= 80. and mlth >  100. and met >=  50. and met < 200.) _counters.at("E08").add_event(event);
            if(mT2 >= 80. and mlth >  100. and met >= 200.) _counters.at("E09").add_event(event);
          }

          // 3Lep, 2 tau (3lF)
          if(_3Lep and nTaus == 2)
          {
            // mlth variable
            double mlth = (signalTaus.at(0)->mom() + signalLightLeptons.at(0)->mom()).m();

            // mT2 variable
            // TODO: Lester's
            double mT2 = get_mT2(signalTaus.at(0), signalLightLeptons.at(0), mmom, 0);

            if(mT2 >= 0. and mT2 < 100. and mlth < 100. and met >=  50. and met < 100.) _counters.at("F01").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mlth < 100. and met >= 100. and met < 150.) _counters.at("F02").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mlth < 100. and met >= 150. and met < 200.) _counters.at("F03").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mlth < 100. and met >= 200. and met < 250.) _counters.at("F04").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mlth < 100. and met >= 250. and met < 300.) _counters.at("F05").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mlth < 100. and met >= 300.) _counters.at("F06").add_event(event);

            if(mT2 >= 0. and mT2 < 100. and mlth >= 100. and met >=  50. and met < 100.) _counters.at("F07").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mlth >= 100. and met >= 100. and met < 150.) _counters.at("F08").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mlth >= 100. and met >= 150. and met < 200.) _counters.at("F09").add_event(event);
            if(mT2 >= 0. and mT2 < 100. and mlth >= 100. and met >= 200.) _counters.at("F10").add_event(event);

            if(mT2 >= 100. and met >=  50. and met < 200.) _counters.at("F11").add_event(event);
            if(mT2 >= 100. and met >= 200.) _counters.at("F12").add_event(event);
          }

          // 4Lep, 2 OSSF pairs (4lG)
          if(nLeptons == 4 and nLightLeptons == 4 and nOSSFpairs > 1)
          {
            // mT2 variable, using Z1 and Z2
            // TODO: Lester's
            double mT2 = get_mT2(OSSFpairs.at(0).at(0)->mom() + OSSFpairs.at(0).at(1)->mom(), OSSFpairs.at(1).at(0)->mom() + OSSFpairs.at(1).at(1)->mom(), mmom, 0.);

            // Z2 invariant mass
            double mZ2 = (OSSFpairs.at(1).at(0)->mom() + OSSFpairs.at(1).at(1)->mom()).m();

            if(mT2 >=   0. and mT2 < 150.) _counters.at("G01").add_event(event);
            if(mT2 >= 150. and mT2 < 250. and mZ2 >= 60.) _counters.at("G02").add_event(event);
            if(mT2 >= 150. and mT2 < 250. and mZ2 <  60.) _counters.at("G03").add_event(event);
            if(mT2 >= 250. and mT2 < 400.) _counters.at("G04").add_event(event);
            if(mT2 >= 400) _counters.at("G05").add_event(event);
          }

          // These variables are common to all remaining SRs
          if(nLeptons == 4)
          {
            // Z1 invariant mass
            double mZ1 = 0.;
            if(nOSSFpairs > 0)
              mZ1 = (OSSFpairs.at(0).at(0)->mom() + OSSFpairs.at(0).at(1)->mom()).m();
            else
              mZ1 = (OSpairs.at(0).at(0)->mom() + OSpairs.at(0).at(1)->mom()).m();

            // deltaRH variable
            double deltaRH = 0.;
            for(auto lep1: signalLeptons)
            {
              if( (nOSSFpairs >  0 and lep1 != OSSFpairs.at(0).at(0) and lep1 != OSSFpairs.at(0).at(1)) or 
                  (nOSSFpairs == 0 and lep1 != OSpairs.at(0).at(0)   and lep1 != OSpairs.at(0).at(1)) )
              {
                for(auto lep2: signalLeptons)
                {
                  if( (nOSSFpairs >  0 and lep2 != OSSFpairs.at(0).at(0) and lep2 != OSSFpairs.at(0).at(1)) or 
                      (nOSSFpairs == 0 and lep2 != OSpairs.at(0).at(0)   and lep2 != OSpairs.at(0).at(1)) )
                  {
                    deltaRH = deltaR_eta(lep1->mom(),lep2->mom());
                  }
                }
              }
            }

            // 4Lep, 1 or fewer OSSF pairs (4lH)
            if(nLightLeptons == 4 and nOSSFpairs < 2)
            {
              if(deltaRH >= 0.8 and mZ1 > 60.) _counters.at("H01").add_event(event);
              if(deltaRH >= 0.8 and mZ1 >  0. and mZ1 <= 60.) _counters.at("H02").add_event(event);
              if(deltaRH < 0.8) _counters.at("H03").add_event(event);
            }

            // 4Lep, tau + 3 light leptons (4lI)
            if(nLightLeptons == 3 and nTaus == 1)
            {
              if(deltaRH >= 0.8 and mZ1 > 60.) _counters.at("I01").add_event(event);
              if(deltaRH >= 0.8 and mZ1 >  0. and mZ1 <= 60.) _counters.at("I02").add_event(event);
              if(deltaRH < 0.8) _counters.at("I03").add_event(event);
            }

            // 4Lep, 2 tau + 2 light leptons, 2 OSSF pairs (4lJ)
            if(nLightLeptons == 2 and nTaus == 2 and nOSSFpairs == 2)
            {
              if(deltaRH >= 0.8 and mZ1 > 60.) _counters.at("J01").add_event(event);
              if(deltaRH >= 0.8 and mZ1 >  0. and mZ1 <= 60.) _counters.at("J02").add_event(event);
              if(deltaRH < 0.8) _counters.at("J03").add_event(event);
            }

            // 4Lep, 2 tau + 2 light leptons, 1 or fewer OSSF pairs (4lK)
            if(nLightLeptons == 2 and nTaus == 2 and nOSSFpairs < 2)
            {
              if(deltaRH >= 0.8 and mZ1 > 60.) _counters.at("K01").add_event(event);
              if(deltaRH >= 0.8 and mZ1 >  0. and mZ1 <= 60.) _counters.at("K02").add_event(event);
              if(deltaRH < 0.8) _counters.at("K03").add_event(event);
            }
          }

        }

        /// Combine the variables of another copy of this analysis (typically on another thread) into this one.
        void combine(const Analysis* other)
        {
          const Analysis_CMS_13TeV_MultiLEP_137invfb* specificOther = dynamic_cast<const Analysis_CMS_13TeV_MultiLEP_137invfb*>(other);
          for (auto& pair : _counters) { pair.second += specificOther->_counters.at(pair.first); }
        }

        void collect_results()
        {

          //Now fill a results object with the results for each SR
          // TODO: This is just approximated from the plots, as there is no public data available

          // 2SSLep (2lSS)
          add_result(SignalRegionData(_counters.at("SS01"), 680., {680., 80.}));
          add_result(SignalRegionData(_counters.at("SS02"), 400., {360., 80.}));
          add_result(SignalRegionData(_counters.at("SS03"), 26., {23., 5.}));
          add_result(SignalRegionData(_counters.at("SS04"), 19., {12., 5.}));
          add_result(SignalRegionData(_counters.at("SS05"), 9.5, {6., 2.}));
          add_result(SignalRegionData(_counters.at("SS06"), 1.9, {2., 1.75}));
          add_result(SignalRegionData(_counters.at("SS07"), 700., {650., 80.}));
          add_result(SignalRegionData(_counters.at("SS08"), 600., {550., 80.}));
          add_result(SignalRegionData(_counters.at("SS09"), 1.9, {1.7, 0.8}));
          add_result(SignalRegionData(_counters.at("SS10"), 4100., {4300., 400.}));
          add_result(SignalRegionData(_counters.at("SS11"), 97., {100., 11.}));
          add_result(SignalRegionData(_counters.at("SS12"), 93., {80., 10.}));
          add_result(SignalRegionData(_counters.at("SS13"), 63., {50., 7.5}));
          add_result(SignalRegionData(_counters.at("SS14"), 2.8, {2.3, 1.3}));
          add_result(SignalRegionData(_counters.at("SS15"), 2.9, {0.7, 0.56}));
          add_result(SignalRegionData(_counters.at("SS16"), 10., {13., 3.5}));
          add_result(SignalRegionData(_counters.at("SS17"), 9.5, {3.9, 2.3}));
          add_result(SignalRegionData(_counters.at("SS18"), 5.8, {4., 1.6}));
          add_result(SignalRegionData(_counters.at("SS19"), 5.8, {3.7, 1.2}));
          add_result(SignalRegionData(_counters.at("SS20"), 5.8, {3.9, 1.6}));

          // 3Lep, OSSF pair (3lA)
          add_result(SignalRegionData(_counters.at("A01"), 242., {250., 25.}));
          add_result(SignalRegionData(_counters.at("A02"), 250., {235., 35.}));
          add_result(SignalRegionData(_counters.at("A03"), 280., {260., 35.}));
          add_result(SignalRegionData(_counters.at("A04"), 215., {195., 35.}));
          add_result(SignalRegionData(_counters.at("A05"), 35., {33., 5.5}));
          add_result(SignalRegionData(_counters.at("A06"), 960., {900., 100.}));
          add_result(SignalRegionData(_counters.at("A07"), 390., {360., 55.}));
          add_result(SignalRegionData(_counters.at("A08"), 7., {5.8, 1.6}));
          add_result(SignalRegionData(_counters.at("A09"), 207., {215., 40.}));
          add_result(SignalRegionData(_counters.at("A10"), 27., {20., 4.}));
          add_result(SignalRegionData(_counters.at("A11"), 37., {27., 6.}));
          add_result(SignalRegionData(_counters.at("A12"), 1., {3., 1.}));
          add_result(SignalRegionData(_counters.at("A13"), 620., {580., 80.}));
          add_result(SignalRegionData(_counters.at("A14"), 9., {13., 3.}));
          add_result(SignalRegionData(_counters.at("A15"), 160., {150., 20.}));
          add_result(SignalRegionData(_counters.at("A16"), 27., {24., 5.}));
          add_result(SignalRegionData(_counters.at("A17"), 29., {33., 5.5}));
          add_result(SignalRegionData(_counters.at("A18"), 1., {4.8, 1.2}));
          add_result(SignalRegionData(_counters.at("A19"), 60., {53., 9.}));
          add_result(SignalRegionData(_counters.at("A20"), 2.9, {2.9, 0.9}));
          add_result(SignalRegionData(_counters.at("A21"), 27., {18., 4.}));
          add_result(SignalRegionData(_counters.at("A22"), 9., {6.5, 1.5}));
          add_result(SignalRegionData(_counters.at("A23"), 7300., {7000., 1200.}));
          add_result(SignalRegionData(_counters.at("A24"), 1000., {1000., 200.}));
          add_result(SignalRegionData(_counters.at("A25"), 200., {200., 40.}));
          add_result(SignalRegionData(_counters.at("A26"), 62., {63., 11.}));
          add_result(SignalRegionData(_counters.at("A27"), 35., {43., 9.}));
          add_result(SignalRegionData(_counters.at("A28"), 1250., {1210., 200.}));
          add_result(SignalRegionData(_counters.at("A29"), 270., {230., 50.}));
          add_result(SignalRegionData(_counters.at("A30"), 25., {20.5, 4.5}));
          add_result(SignalRegionData(_counters.at("A31"), 6., {5.4, 1.7}));
          add_result(SignalRegionData(_counters.at("A32"), 112., {77., 16.}));
          add_result(SignalRegionData(_counters.at("A33"), 49., {56, 12.}));
          add_result(SignalRegionData(_counters.at("A34"), 18., {15.5, 3.5}));
          add_result(SignalRegionData(_counters.at("A35"), 15., {12., 2.5}));
          add_result(SignalRegionData(_counters.at("A36"), 1150., {1150., 200.}));
          add_result(SignalRegionData(_counters.at("A37"), 350., {350., 60.}));
          add_result(SignalRegionData(_counters.at("A38"), 100., {98., 17.}));
          add_result(SignalRegionData(_counters.at("A39"), 29., {30., 6.}));
          add_result(SignalRegionData(_counters.at("A40"), 15., {21., 4.}));
          add_result(SignalRegionData(_counters.at("A41"), 230., {215., 40.}));
          add_result(SignalRegionData(_counters.at("A42"), 70., {54., 13.}));
          add_result(SignalRegionData(_counters.at("A43"), 12., {11., 2.}));
          add_result(SignalRegionData(_counters.at("A44"), 4., {2.7, 1.1}));
          add_result(SignalRegionData(_counters.at("A45"), 35., {32., 7.}));
          add_result(SignalRegionData(_counters.at("A46"), 9., {10.5, 4.}));
          add_result(SignalRegionData(_counters.at("A47"), 4., {5.8, 1.6}));
          add_result(SignalRegionData(_counters.at("A48"), 10., {3.4, 0.9}));
          add_result(SignalRegionData(_counters.at("A49"), 1250., {1130., 170.}));
          add_result(SignalRegionData(_counters.at("A50"), 300., {290., 50.}));
          add_result(SignalRegionData(_counters.at("A51"), 76., {82.5, 17.5}));
          add_result(SignalRegionData(_counters.at("A52"), 36., {43., 9.}));
          add_result(SignalRegionData(_counters.at("A53"), 199., {155., 30.}));
          add_result(SignalRegionData(_counters.at("A54"), 63., {55., 11.}));
          add_result(SignalRegionData(_counters.at("A55"), 26., {18., 3.}));
          add_result(SignalRegionData(_counters.at("A56"), 12., {8., 2.2}));
          add_result(SignalRegionData(_counters.at("A57"), 1., {2.5, 1.}));
          add_result(SignalRegionData(_counters.at("A58"), 3., {4.2, 1.4}));
          add_result(SignalRegionData(_counters.at("A59"), 52., {38., 7.}));
          add_result(SignalRegionData(_counters.at("A60"), 20, {16.5, 4.}));
          add_result(SignalRegionData(_counters.at("A61"), 7., {7., 1.5}));
          add_result(SignalRegionData(_counters.at("A62"), 2., {4.1, 1.3}));
          add_result(SignalRegionData(_counters.at("A63"), 2., {1.7, 0.45}));
          add_result(SignalRegionData(_counters.at("A64"), 3., {5.5, 1.8}));

          // 3Lep, no OSSF pair (3lB)
          add_result(SignalRegionData(_counters.at("B01"), 14., {8.2, 1.6}));
          add_result(SignalRegionData(_counters.at("B02"), 110., {110., 25.}));
          add_result(SignalRegionData(_counters.at("B03"), 327, {322., 83.}));

          // 3Lep, OSSF pair + tau (3lC)
          add_result(SignalRegionData(_counters.at("C01"), 3850., {4180., 1640.}));
          add_result(SignalRegionData(_counters.at("C02"), 167., {190., 70.}));
          add_result(SignalRegionData(_counters.at("C03"), 20., {23., 6.}));
          add_result(SignalRegionData(_counters.at("C04"), 42., {34., 13.}));
          add_result(SignalRegionData(_counters.at("C05"), 3., {4.4, 1.7}));
          add_result(SignalRegionData(_counters.at("C06"), 5., {5.3, 1.4}));
          add_result(SignalRegionData(_counters.at("C07"), 7., {9., 3.5}));
          add_result(SignalRegionData(_counters.at("C08"), 1., {2.1, 1.}));
          add_result(SignalRegionData(_counters.at("C09"), 4., {3.4, 1.1}));

          // 3Lep, no OSSF pair, 2 OS light leptons + tau (3lD)
          add_result(SignalRegionData(_counters.at("D01"), 680., {666., 254.}));
          add_result(SignalRegionData(_counters.at("D02"), 280., {230., 90.}));
          add_result(SignalRegionData(_counters.at("D03"), 72., {49., 19.}));
          add_result(SignalRegionData(_counters.at("D04"), 13., {17., 6.}));
          add_result(SignalRegionData(_counters.at("D05"), 11., {13., 5.}));
          add_result(SignalRegionData(_counters.at("D06"), 600., {570., 230.}));
          add_result(SignalRegionData(_counters.at("D07"), 225., {200., 90.}));
          add_result(SignalRegionData(_counters.at("D08"), 47., {55., 21.}));
          add_result(SignalRegionData(_counters.at("D09"), 8., {13., 5.}));
          add_result(SignalRegionData(_counters.at("D10"), 6., {6.8, 2.5}));
          add_result(SignalRegionData(_counters.at("D11"), 200., {190., 70.}));
          add_result(SignalRegionData(_counters.at("D12"), 63., {70., 28.}));
          add_result(SignalRegionData(_counters.at("D13"), 20., {24., 9.}));
          add_result(SignalRegionData(_counters.at("D14"), 7., {13., 4.}));
          add_result(SignalRegionData(_counters.at("D15"), 20., {17., 6.}));
          add_result(SignalRegionData(_counters.at("D16"), 1., {1.13, 0.47}));

          // 3Lep, no OSSF pair, 2 SS light leptons + tau (3lE)
          add_result(SignalRegionData(_counters.at("E01"), 60., {70., 17.}));
          add_result(SignalRegionData(_counters.at("E02"), 28., {22., 3.}));
          add_result(SignalRegionData(_counters.at("E03"), 1., {2.2, 0.8}));
          add_result(SignalRegionData(_counters.at("E04"), 362., {345., 75.}));
          add_result(SignalRegionData(_counters.at("E05"), 108., {95., 18.}));
          add_result(SignalRegionData(_counters.at("E06"), 25., {19., 4.}));
          add_result(SignalRegionData(_counters.at("E07"), 5., {6., 1.6}));
          add_result(SignalRegionData(_counters.at("E08"), 15., {14., 4.}));
          add_result(SignalRegionData(_counters.at("E09"), 1., {1., 0.5}));

          // 3Lep, 2 tau (3lF)
          add_result(SignalRegionData(_counters.at("F01"), 1060., {1170., 400.}));
          add_result(SignalRegionData(_counters.at("F02"), 177., {190., 70.}));
          add_result(SignalRegionData(_counters.at("F03"), 40., {36., 13.}));
          add_result(SignalRegionData(_counters.at("F04"), 9., {9.5, 3.5}));
          add_result(SignalRegionData(_counters.at("F05"), 3., {2., 1.}));
          add_result(SignalRegionData(_counters.at("F06"), 4., {2.6, 1.2}));
          add_result(SignalRegionData(_counters.at("F07"), 585., {600., 200.}));
          add_result(SignalRegionData(_counters.at("F08"), 125., {130., 58.}));
          add_result(SignalRegionData(_counters.at("F09"), 33., {37., 14.}));
          add_result(SignalRegionData(_counters.at("F10"), 19., {17., 6.}));
          add_result(SignalRegionData(_counters.at("F11"), 18., {23., 6.}));
          add_result(SignalRegionData(_counters.at("F12"), 2., {3.5, 1.4}));

          // 4Lep, 2 OSSF pairs (4lG)
          add_result(SignalRegionData(_counters.at("G01"), 370., {365., 30.}));
          add_result(SignalRegionData(_counters.at("G02"), 15., {18.5, 2.}));
          add_result(SignalRegionData(_counters.at("G03"), 6., {6.7, 1.2}));
          add_result(SignalRegionData(_counters.at("G04"), 0., {1.7, 0.8}));
          add_result(SignalRegionData(_counters.at("G05"), 0., {0., 0.}));

          // 4Lep, 1 or fewer OSSF pairs (4lH)
          add_result(SignalRegionData(_counters.at("H01"), 49., {41.7, 5.}));
          add_result(SignalRegionData(_counters.at("H02"), 4., {3., 1.}));
          add_result(SignalRegionData(_counters.at("H03"), 3., {4.3, 0.9}));

          // 4Lep, tau + 3 light leptons (4lI)
          add_result(SignalRegionData(_counters.at("I01"), 92., {96., 17.}));
          add_result(SignalRegionData(_counters.at("I02"), 14., {10., 3.}));
          add_result(SignalRegionData(_counters.at("I03"), 7., {9.4, 1.6}));

          // 4Lep, 2 tau + 2 light leptons, 2 OSSF pairs (4lJ)
          add_result(SignalRegionData(_counters.at("J01"), 34., {35.5, 8.5}));
          add_result(SignalRegionData(_counters.at("J02"), 6., {10.7, 4.3}));
          add_result(SignalRegionData(_counters.at("J03"), 4., {4.3, 1.2}));

          // 4Lep, 2 tau + 2 light leptons, 1 or fewer OSSF pairs (4lK)
          add_result(SignalRegionData(_counters.at("K01"), 8., {16., 6.5}));
          add_result(SignalRegionData(_counters.at("K02"), 5., {5.2, 2.45}));
          add_result(SignalRegionData(_counters.at("K03"), 1., {0.61, 0.61}));

        }

      protected:

        void analysis_specific_reset()
        {
          for (auto& pair : _counters) { pair.second.reset(); }
        }


    };

    // Factory fn
    DEFINE_ANALYSIS_FACTORY(CMS_13TeV_MultiLEP_137invfb)


  }
}
