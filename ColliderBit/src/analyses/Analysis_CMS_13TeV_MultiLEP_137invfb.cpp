///
///  \author Tomas Gonzalo
///          (gonzalo@physik.rwth-aachen.de)
///  \date 2021 July
///  *********************************************


#include "gambit/ColliderBit/analyses/Analysis.hpp"
#include "gambit/ColliderBit/Utils.hpp"
//#include "gambit/ColliderBit/CMSEfficiencies.hpp"
//#include "gambit/ColliderBit/mt2_bisect.h"

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
          // 2SSLep SRs
          for(size_t i=1; i<=20; ++i)
            _counters[SR("SS",i)] =  EventCounter(SR("SS",i));
          // 3Lep, OSSF pair
          for(size_t i=1; i<=64; ++i)
            _counters[SR("A",i)] = EventCounter(SR("A",i));
          // 3Lep, no OSSF pair
          for(size_t i=1; i<=3; ++i)
            _counters[SR("B",i)] = EventCounter(SR("B",i));
          // 3Lep, OSSF pair + tau
          for(size_t i=1; i<=9; ++i)
            _counters[SR("C",i)] = EventCounter(SR("C",i));
          // 3Lep, no OSSF pair + tau
          for(size_t i=1; i<=16; ++i)
            _counters[SR("D",i)] = EventCounter(SR("D",i));
          // 3Lep, SS + tau
          for(size_t i=1; i<=9; ++i)
            _counters[SR("E",i)] = EventCounter(SR("E",i));
          // 3Lep, 2 tau
          for(size_t i=1; i<=12; ++i)
            _counters[SR("F",i)] = EventCounter(SR("F",i));
          // 4Lep, 2 OSSF pairs
          for(size_t i=1; i<=5; ++i)
            _counters[SR("G",i)] = EventCounter(SR("G",i));
          // 4Lep, 1 or fewer OSSF pairs
          for(size_t i=1; i<=3; ++i)
            _counters[SR("H",i)] = EventCounter(SR("H",i));
          // 4Lep, tau + 3 light leptons
          for(size_t i=1; i<=3; ++i)
            _counters[SR("I",i)] = EventCounter(SR("I",i));
          // 4Lep, 2 tau + 2 light leptopns, 2 OSSF pairs
          for(size_t i=1; i<=3; ++i)
            _counters[SR("J",i)] = EventCounter(SR("J",i));
          // 4Lep, 2 tau + 2 light leptopns, 1 or fewer OSSF pairs
          for(size_t i=1; i<=3; ++i)
            _counters[SR("K",i)] = EventCounter(SR("K",i));


          set_analysis_name("CMS_13TeV_MultiLEP_137invfb");
          set_luminosity(137.0);

        }

        void run(const HEPUtils::Event* event)
        {

          // Missing ET
          double met = event->met();

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

          std::vector<const HEPUtils::Particle*> signalLightLeptons=signalElectrons;
          signalLightLeptons.insert(signalLightLeptons.end(), signalMuons.begin(), signalMuons.end());
          sortByPt(signalLightLeptons);

          std::vector<const HEPUtils::Particle*> signalLeptons=signalTaus;
          signalLeptons.insert(signalLeptons.end(), signalLightLeptons.begin(), signalLightLeptons.end());
          sortByPt(signalLeptons);

          std::vector<std::vector<const HEPUtils::Particle*> > SSpairs = getSSpairs(signalLightLeptons);
          std::vector<std::vector<const HEPUtils::Particle*> > OSSFpairs = getSFOSpairs(signalLightLeptons);

          std::vector<const HEPUtils::Jet*> signalJets = baselineJets;
          sortByPt(signalJets);
          std::vector<const HEPUtils::Jet*> signalBJets;
          for(const Jet* j: signalJets)
            if(j->btag()) signalBJets.push_back(j);
          

          ////////////////////////
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
            // TODO: missing
            double mt2 = 0.;

            // pT of dilepton system for SS leptons
            double pTll = 0.;
            if(nLeptons > 1)
              pTll = ( signalLeptons.at(0)->mom() + signalLeptons.at(1)->mom() ).pT();

            // Sign of final states
            bool positive = signalLeptons.at(0)->pid() * signalLeptons.at(1)->pid() > 0;
            bool negative = not positive;

            if(mt2 == 0. and pTll <  70.) _counters.at("SS01").add_event(event);
            if(mt2 == 0. and pTll >= 70. and met <  100.) _counters.at("SS02").add_event(event);
            if(mt2 == 0. and pTll >= 70. and met >= 100. and met < 200. and positive) _counters.at("SS03").add_event(event);
            if(mt2 == 0. and pTll >= 70. and met >= 100. and met < 200. and negative) _counters.at("SS04").add_event(event);
            if(mt2 == 0. and pTll >= 70. and met >= 200. and positive) _counters.at("SS05").add_event(event);
            if(mt2 == 0. and pTll >= 70. and met >= 200. and negative) _counters.at("SS06").add_event(event);
            if(mt2 >  0. and mt2 <= 80. and pTll <  30. and met <  200. and positive) _counters.at("SS07").add_event(event);
            if(mt2 >  0. and mt2 <= 80. and pTll <  30. and met <  200. and negative) _counters.at("SS08").add_event(event);
            if(mt2 >  0. and mt2 <= 80. and pTll <  30. and met >= 200.) _counters.at("SS09").add_event(event);
            if(mt2 >  0. and mt2 <= 80. and pTll >= 30.) _counters.at("SS10").add_event(event);
            if(mt2 > 80. and pTll <  200. and met <  100.) _counters.at("SS11").add_event(event);
            if(mt2 > 80. and pTll <  200. and met >= 100. and met < 200. and positive) _counters.at("SS12").add_event(event);
            if(mt2 > 80. and pTll <  200. and met >= 100. and met < 200. and negative) _counters.at("SS13").add_event(event);
            if(mt2 > 80. and pTll <  200. and met >= 200. and positive) _counters.at("SS14").add_event(event);
            if(mt2 > 80. and pTll <  200. and met >= 200. and negative) _counters.at("SS15").add_event(event);
            if(mt2 > 80. and pTll >= 200. and met <  100.) _counters.at("SS16").add_event(event);
            if(mt2 > 80. and pTll >= 200. and met >= 100. and met < 200. and positive) _counters.at("SS17").add_event(event);
            if(mt2 > 80. and pTll >= 200. and met >= 100. and met < 200. and negative) _counters.at("SS18").add_event(event);
            if(mt2 > 80. and pTll >= 200. and met >= 200. and positive) _counters.at("SS19").add_event(event);
            if(mt2 > 80. and pTll >= 200. and met >= 200. and negative) _counters.at("SS20").add_event(event);
          }

          // 3Lep, OSSF pair (3lA)
          if(nLightLeptons == 3 and nLeptons == 3 and nOSSFpairs > 0)
          {
            // Mll variable
            // TODO: Missing
            double mll = 0.;

            // MT variable
            // TODO: Missing
            double mT = 0.;

            //MT3l variable
            // TODO: Missing
            double mT3l = 0.;

            // HT variable
            // TODO: Missing
            double HT = 0.;

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
          if(nLeptons == 3 and nLightLeptons == 3 and nOSSFpairs == 0)
          {
            // Min DeltaR variable
            // TODO: Missing
            double minDeltaR = 0.;

            if(minDeltaR <  0.4) _counters.at("B01").add_event(event);
            if(minDeltaR >= 0.4 and minDeltaR < 1.0) _counters.at("B02").add_event(event);
            if(minDeltaR >= 1.0) _counters.at("B03").add_event(event);
          }

          // 3Lep, OSSF pair + tau (3lC)
          if(nLeptons == 3 and nTaus == 1 and nOSSFpairs > 0)
          {
            double mZ = 91.1876;

            // mT2 variable
            // TODO: Missing
            double mT2 = 0.;

            // mll variable
            // TODO: Missing
            double mll = 0.;

            // mT2l variable
            // TODO: Missing
            double mT2l = 0.;

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
          if(nLeptons == 3 and nTaus == 1 and nOSSFpairs == 0 and signalLightLeptons.at(0)->pid()*signalLightLeptons.at(1)->pid() < 0)
          {
            // mll variable
            // TODO: Missing
            double mll = 0.;

            // mT2 variable
            // TODO: Missing
            double mT2 = 0.;

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
          if(nLeptons == 3 and nTaus == 1 and nOSSFpairs == 0 and signalLightLeptons.at(0)->pid()*signalLightLeptons.at(1)->pid() > 0)
          {
            // mlth variable
            // TODO: Missing
            double mlth = 0.;

            // mT2 varaible
            // TODO: Missing
            double mT2 = 0.;

            // TODO: Do I need to remove events that have all the same sign for tau?

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
          if(nLeptons == 3 and nTaus == 2)
          {
            // mlth variable
            // TODO: Missing
            double mlth = 0.;

            // mT2 variable
            // TODO: Missing
            double mT2 = 0.;

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
            // mT2 variable
            // TODO: Missing
            double mT2 = 0.;

            // Z2 invariant mass
            // TODO: Missing
            double mZ2 = 0.;

            if(mT2 >=   0. and mT2 < 150.) _counters.at("G01").add_event(event);
            if(mT2 >= 150. and mT2 < 250. and mZ2 >= 60.) _counters.at("G02").add_event(event);
            if(mT2 >= 150. and mT2 < 250. and mZ2 <  60.) _counters.at("G03").add_event(event);
            if(mT2 >= 250. and mT2 < 400.) _counters.at("G04").add_event(event);
            if(mT2 >= 400) _counters.at("G05").add_event(event);
          }

          // 4Lep, 1 or fewer OSSF pairs (4lH)
          if(nLeptons == 4 and nLightLeptons == 4 and nOSSFpairs < 2)
          {
            // Z1 invariant mass
            // TODO: Missing
            double mZ1 = 0.;

            // deltaRH variable
            // TODO: Missing
            double deltaRH = 0.;

            if(deltaRH >= 0.8 and mZ1 > 60.) _counters.at("H01").add_event(event);
            if(deltaRH >= 0.8 and mZ1 >  0. and mZ1 <= 60.) _counters.at("H02").add_event(event);
            if(deltaRH < 0.8) _counters.at("H03").add_event(event);
          }

          // 4Lep, tau + 3 light leptons (4lI)
          if(nLeptons == 4 and nLightLeptons == 3 and nTaus == 1)
          {
            // Z1 invariant mass
            // TODO: Missing
            double mZ1 = 0.;

            // deltaRH variable
            // TODO: Missing
            double deltaRH = 0.;

            if(deltaRH >= 0.8 and mZ1 > 60.) _counters.at("I01").add_event(event);
            if(deltaRH >= 0.8 and mZ1 >  0. and mZ1 <= 60.) _counters.at("I02").add_event(event);
            if(deltaRH < 0.8) _counters.at("I03").add_event(event);
          }

          // 4Lep, 2 tau + 2 light leptopns, 2 OSSF pairs (4lJ)
          if(nLeptons == 4 and nLightLeptons == 2 and nTaus == 2 and nOSSFpairs == 2)
          {
            // Z1 invariant mass
            // TODO: Missing
            double mZ1 = 0.;

            // deltaRH variable
            // TODO: Missing
            double deltaRH = 0.;

            if(deltaRH >= 0.8 and mZ1 > 60.) _counters.at("J01").add_event(event);
            if(deltaRH >= 0.8 and mZ1 >  0. and mZ1 <= 60.) _counters.at("J02").add_event(event);
            if(deltaRH < 0.8) _counters.at("J03").add_event(event);
          }

          // 4Lep, 2 tau + 2 light leptopns, 1 or fewer OSSF pairs (4LK)
          if(nLeptons == 4 and nLightLeptons == 2 and nTaus == 2 and nOSSFpairs < 2)
          {
            // Z1 invariant mass
            // TODO: Missing
            double mZ1 = 0.;

            // deltaRH variable
            // TODO: Missing
            double deltaRH = 0.;

            if(deltaRH >= 0.8 and mZ1 > 60.) _counters.at("K01").add_event(event);
            if(deltaRH >= 0.8 and mZ1 >  0. and mZ1 <= 60.) _counters.at("K02").add_event(event);
            if(deltaRH < 0.8) _counters.at("K03").add_event(event);
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
