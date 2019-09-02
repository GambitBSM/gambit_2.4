///
///  \author Yang Zhang
///  \date 2019 May 
///  *********************************************

// Based on http://cdsweb.cern.ch/record/2668387/files/ATLAS-CONF-2019-008.pdf
// Search for electroweak production of charginos and sleptons decaying in final states with two leptons and missing transverse momentum in √s = 13 TeV p p collisions using the ATLAS detector

// Note:
// 1. Not validated!!!!
//    The excluding abilities in low mass region are much weaker than experimental report.
// 2. Use event-based MET significance instead of object-based significance

#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <fstream>

#include "gambit/ColliderBit/analyses/Analysis.hpp"
#include "gambit/ColliderBit/ATLASEfficiencies.hpp"
#include "gambit/ColliderBit/mt2_bisect.h"
#include "gambit/ColliderBit/analyses/Cutflow.hpp"

// #define CHECK_CUTFLOW

using namespace std;

namespace Gambit
{
  namespace ColliderBit
  {

    // This analysis class is a base class for two SR-specific analysis classes
    // defined further down:
    // - ATLAS_13TeV_2OSLEP_chargino_binned_139invfb
    // - ATLAS_13TeV_2OSLEP_chargino_inclusive_139invfb
    class Analysis_ATLAS_13TeV_2OSLEP_chargino_139invfb : public Analysis
    {

    protected:

      // Counters for the number of accepted events for each signal region
      std::map<string,double> _numSR = {
        {"SR-DF-0J-100",     0},
        {"SR-DF-0J-160",     0},
        {"SR-DF-0J-100-120", 0},
        {"SR-DF-0J-120-160", 0},
        {"SR-DF-1J-100",     0},
        {"SR-DF-1J-160",     0},
        {"SR-DF-1J-100-120", 0},
        {"SR-DF-1J-120-160", 0},
        {"SR-SF-0J-100",     0},
        {"SR-SF-0J-160",     0},
        {"SR-SF-0J-100-120", 0},
        {"SR-SF-0J-120-160", 0},
        {"SR-SF-1J-100",     0},
        {"SR-SF-1J-160",     0},
        {"SR-SF-1J-100-120", 0},
        {"SR-SF-1J-120-160", 0}
      };

      std::map<string,double> _numSR_bin = {
        {"SR-DF-0J-100-105", 0},
        {"SR-DF-0J-105-110", 0},
        {"SR-DF-0J-110-120", 0},
        {"SR-DF-0J-120-140", 0},
        {"SR-DF-0J-140-160", 0},
        {"SR-DF-0J-160-180", 0},
        {"SR-DF-0J-180-220", 0},
        {"SR-DF-0J-220-260", 0},
        {"SR-DF-0J-260", 0},
        {"SR-DF-1J-100-105", 0},
        {"SR-DF-1J-105-110", 0},
        {"SR-DF-1J-110-120", 0},
        {"SR-DF-1J-120-140", 0},
        {"SR-DF-1J-140-160", 0},
        {"SR-DF-1J-160-180", 0},
        {"SR-DF-1J-180-220", 0},
        {"SR-DF-1J-220-260", 0},
        {"SR-DF-1J-260", 0},
        {"SR-SF-0J-100-105", 0},
        {"SR-SF-0J-105-110", 0},
        {"SR-SF-0J-110-120", 0},
        {"SR-SF-0J-120-140", 0},
        {"SR-SF-0J-140-160", 0},
        {"SR-SF-0J-160-180", 0},
        {"SR-SF-0J-180-220", 0},
        {"SR-SF-0J-220-260", 0},
        {"SR-SF-0J-260", 0},
        {"SR-SF-1J-100-105", 0},
        {"SR-SF-1J-105-110", 0},
        {"SR-SF-1J-110-120", 0},
        {"SR-SF-1J-120-140", 0},
        {"SR-SF-1J-140-160", 0},
        {"SR-SF-1J-160-180", 0},
        {"SR-SF-1J-180-220", 0},
        {"SR-SF-1J-220-260", 0},
        {"SR-SF-1J-260", 0}
      };

      Cutflow _cutflow;

    public:

      // Required detector sim
      static constexpr const char* detector = "ATLAS";

      Analysis_ATLAS_13TeV_2OSLEP_chargino_139invfb():
      _cutflow("ATLAS 2-lep chargino-W 13 TeV", {"Two_OS_leptons", "mll_25", "b_jet_veto", "MET_110", "MET_significance_10", "n_j<=1", "m_ll_m_Z"})
      {

        set_analysis_name("ATLAS_13TeV_2OSLEP_chargino_139invfb");
        set_luminosity(139);

      }

      // The following section copied from Analysis_ATLAS_1LEPStop_20invfb.cpp
      void JetLeptonOverlapRemoval(vector<HEPUtils::Jet*> &jetvec, vector<HEPUtils::Particle*> &lepvec, double DeltaRMax) {
        //Routine to do jet-lepton check
        //Discards jets if they are within DeltaRMax of a lepton

        vector<HEPUtils::Jet*> Survivors;

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

      void LeptonJetOverlapRemoval(vector<HEPUtils::Particle*> &lepvec, vector<HEPUtils::Jet*> &jetvec) {
        //Routine to do lepton-jet check
        //Discards leptons if they are within dR of a jet as defined in analysis paper

        vector<HEPUtils::Particle*> Survivors;

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


      struct ptComparison {
        bool operator() (HEPUtils::Particle* i,HEPUtils::Particle* j) {return (i->pT()>j->pT());}
      } comparePt;


      void run(const HEPUtils::Event* event)
      {
        _cutflow.fillinit();

        // Baseline objects
        double met = event->met();

        // Electrons
        vector<HEPUtils::Particle*> electrons;
        for (HEPUtils::Particle* electron : event->electrons()) {
          if (electron->pT() > 10.
              && fabs(electron->eta()) < 2.47)
            electrons.push_back(electron);
        }

        // Apply electron efficiency
        ATLAS::applyElectronEff(electrons);

        // Muons
        vector<HEPUtils::Particle*> muons;
        for (HEPUtils::Particle* muon : event->muons()) {
          if (muon->pT() > 10.
              && fabs(muon->eta()) < 2.5)
            muons.push_back(muon);
        }

        // Apply muon efficiency
        ATLAS::applyMuonEff(muons);

        // Jets
        vector<HEPUtils::Jet*> candJets;
        for (HEPUtils::Jet* jet : event->jets()) {
          if (jet->pT() > 20. && fabs(jet->eta()) < 2.5)
            candJets.push_back(jet);
        }

        // Scalar sum of the transverse momenta from all the reconstructed hard objects
        double HT = 0.0;
        for (HEPUtils::Jet* j : candJets) HT += j->pT();
        for (HEPUtils::Particle* e : electrons) HT += e->pT();
        for (HEPUtils::Particle* mu : muons) HT += mu->pT();

        // Overlap removal
        JetLeptonOverlapRemoval(candJets,electrons,0.2);
        LeptonJetOverlapRemoval(electrons,candJets);
        JetLeptonOverlapRemoval(candJets,muons,0.4);
        LeptonJetOverlapRemoval(muons,candJets);

        // Jets
        vector<HEPUtils::Jet*> bJets;
        vector<HEPUtils::Jet*> nonbJets;

        // Find b-jets
        // Copied from ATLAS_13TeV_3b_24invfb
        double btag = 0.85; double cmisstag = 1/12.; double misstag = 1./381.;
        for (HEPUtils::Jet* jet : candJets) {
          // Tag
          if( jet->btag() && random_bool(btag) ) bJets.push_back(jet);
          // Misstag c-jet
          else if( jet->ctag() && random_bool(cmisstag) ) bJets.push_back(jet);
          // Misstag light jet
          else if( random_bool(misstag) ) bJets.push_back(jet);
          // Non b-jet
          else nonbJets.push_back(jet);
        }


        // Find signal leptons with pT > 20 GeV
        vector<HEPUtils::Particle*> signalElectrons;
        for (HEPUtils::Particle* electron : electrons) {
          if (electron->pT() > 25.) signalElectrons.push_back(electron);
        }
        vector<HEPUtils::Particle*> signalMuons;
        for (HEPUtils::Particle* muon : muons) {
          if (muon->pT() > 25.) signalMuons.push_back(muon);
        }

        // Signal leptons = electrons + muons
        vector<HEPUtils::Particle*> signalLeptons;
        signalLeptons=signalElectrons;
        signalLeptons.insert(signalLeptons.end(),signalMuons.begin(),signalMuons.end());
        sort(signalLeptons.begin(),signalLeptons.end(),comparePt);


        // Tow exactly opposite-sign lepton
        if (signalLeptons.size() != 2) return;
        if (signalLeptons[0]->pid()*signalLeptons[1]->pid()>0) return;
        _cutflow.fill(1);


        // m_{ll} > 100 GeV
        double mll=(signalLeptons[0]->mom()+signalLeptons[1]->mom()).m();
        if (mll<100) return;
        _cutflow.fill(2);

        // b-jet veto
        if (bJets.size()>0) return;
        _cutflow.fill(3);

        // MET>110 GeV
        if (met<110) return;
        _cutflow.fill(4);

        // The missing transverse momentum significance >10
        // TODO Use event-based MET significance instead of object-based significance
        // https://cds.cern.ch/record/2630948/files/ATLAS-CONF-2018-038.pdf
        double met_sig=met/sqrt(met);
        if (met_sig<10) return;
        _cutflow.fill(5);

        // n_non_b_tagged_jets <= 1
        if (nonbJets.size()>1) return;
        _cutflow.fill(6);

        // Same flavour
        bool flag_SF = signalLeptons[0]->pid() + signalLeptons[1]->pid() == 0;
        if (flag_SF) {
            if (mll<121.2) return ;
        }
        _cutflow.fill(7);

        // Mt2
        double pLep1[3] = {signalLeptons[0]->mass(), signalLeptons[0]->mom().px(), signalLeptons[0]->mom().py()};
        double pLep2[3] = {signalLeptons[1]->mass(), signalLeptons[1]->mom().px(), signalLeptons[1]->mom().py()};
        double pMiss[3] = {0., event->missingmom().px(), event->missingmom().py() };
        mt2_bisect::mt2 mt2_calc;
        mt2_calc.set_momenta(pLep1,pLep2,pMiss);
        mt2_calc.set_mn(0.0);
        double mT2 = mt2_calc.get_mt2();

        if (flag_SF) {
            if (nonbJets.size()==0){
                if (mT2>100)             _numSR["SR-SF-0J-100"]++;
                if (mT2>160)             _numSR["SR-SF-0J-160"]++;
                if (mT2>100 and mT2<120) _numSR["SR-SF-0J-100-120"]++;
                if (mT2>120 and mT2<160) _numSR["SR-SF-0J-120-160"]++;
                // binned SRs
                if (mT2>100 and mT2<105) _numSR_bin["SR-SF-0J-100-105"]++;
                if (mT2>105 and mT2<110) _numSR_bin["SR-SF-0J-105-110"]++;
                if (mT2>110 and mT2<120) _numSR_bin["SR-SF-0J-110-120"]++;
                if (mT2>120 and mT2<140) _numSR_bin["SR-SF-0J-120-140"]++;
                if (mT2>140 and mT2<160) _numSR_bin["SR-SF-0J-140-160"]++;
                if (mT2>160 and mT2<180) _numSR_bin["SR-SF-0J-160-180"]++;
                if (mT2>180 and mT2<220) _numSR_bin["SR-SF-0J-180-220"]++;
                if (mT2>220 and mT2<260) _numSR_bin["SR-SF-0J-220-260"]++;
                if (mT2>260            ) _numSR_bin["SR-SF-0J-260"]++;
            } else {
                if (mT2>100)             _numSR["SR-SF-1J-100"]++;
                if (mT2>160)             _numSR["SR-SF-1J-160"]++;
                if (mT2>100 and mT2<120) _numSR["SR-SF-1J-100-120"]++;
                if (mT2>120 and mT2<160) _numSR["SR-SF-1J-120-160"]++;
                // binned SRs
                if (mT2>100 and mT2<105) _numSR_bin["SR-SF-1J-100-105"]++;
                if (mT2>105 and mT2<110) _numSR_bin["SR-SF-1J-105-110"]++;
                if (mT2>110 and mT2<120) _numSR_bin["SR-SF-1J-110-120"]++;
                if (mT2>120 and mT2<140) _numSR_bin["SR-SF-1J-120-140"]++;
                if (mT2>140 and mT2<160) _numSR_bin["SR-SF-1J-140-160"]++;
                if (mT2>160 and mT2<180) _numSR_bin["SR-SF-1J-160-180"]++;
                if (mT2>180 and mT2<220) _numSR_bin["SR-SF-1J-180-220"]++;
                if (mT2>220 and mT2<260) _numSR_bin["SR-SF-1J-220-260"]++;
                if (mT2>260            ) _numSR_bin["SR-SF-1J-260"]++;
            }
        } else {
            if (nonbJets.size()==0){
                if (mT2>100)             _numSR["SR-DF-0J-100"]++;
                if (mT2>160)             _numSR["SR-DF-0J-160"]++;
                if (mT2>100 and mT2<120) _numSR["SR-DF-0J-100-120"]++;
                if (mT2>120 and mT2<160) _numSR["SR-DF-0J-120-160"]++;
                // binned SRs
                if (mT2>100 and mT2<105) _numSR_bin["SR-DF-0J-100-105"]++;
                if (mT2>105 and mT2<110) _numSR_bin["SR-DF-0J-105-110"]++;
                if (mT2>110 and mT2<120) _numSR_bin["SR-DF-0J-110-120"]++;
                if (mT2>120 and mT2<140) _numSR_bin["SR-DF-0J-120-140"]++;
                if (mT2>140 and mT2<160) _numSR_bin["SR-DF-0J-140-160"]++;
                if (mT2>160 and mT2<180) _numSR_bin["SR-DF-0J-160-180"]++;
                if (mT2>180 and mT2<220) _numSR_bin["SR-DF-0J-180-220"]++;
                if (mT2>220 and mT2<260) _numSR_bin["SR-DF-0J-220-260"]++;
                if (mT2>260            ) _numSR_bin["SR-DF-0J-260"]++;
            } else {
                if (mT2>100)             _numSR["SR-DF-1J-100"]++;
                if (mT2>160)             _numSR["SR-DF-1J-160"]++;
                if (mT2>100 and mT2<120) _numSR["SR-DF-1J-100-120"]++;
                if (mT2>120 and mT2<160) _numSR["SR-DF-1J-120-160"]++;
                // binned SRs
                if (mT2>100 and mT2<105) _numSR_bin["SR-DF-1J-100-105"]++;
                if (mT2>105 and mT2<110) _numSR_bin["SR-DF-1J-105-110"]++;
                if (mT2>110 and mT2<120) _numSR_bin["SR-DF-1J-110-120"]++;
                if (mT2>120 and mT2<140) _numSR_bin["SR-DF-1J-120-140"]++;
                if (mT2>140 and mT2<160) _numSR_bin["SR-DF-1J-140-160"]++;
                if (mT2>160 and mT2<180) _numSR_bin["SR-DF-1J-160-180"]++;
                if (mT2>180 and mT2<220) _numSR_bin["SR-DF-1J-180-220"]++;
                if (mT2>220 and mT2<260) _numSR_bin["SR-DF-1J-220-260"]++;
                if (mT2>260            ) _numSR_bin["SR-DF-1J-260"]++;
            }

        }

      }

      /// Combine the variables of another copy of this analysis (typically on another thread) into this one.
      void combine(const Analysis* other)
      {
        const Analysis_ATLAS_13TeV_2OSLEP_chargino_139invfb* specificOther
                = dynamic_cast<const Analysis_ATLAS_13TeV_2OSLEP_chargino_139invfb*>(other);

        for (auto& el : _numSR)
        {
          el.second += specificOther->_numSR.at(el.first);
        }

        for (auto& el : _numSR_bin)
        {
          el.second += specificOther->_numSR_bin.at(el.first);
        }

      }

      // This function can be overridden by the derived SR-specific classes
      virtual void collect_results() {

        #ifdef CHECK_CUTFLOW
        cout << _cutflow << endl;
        for (auto& el : _numSR) {
            cout << el.first << "\t" << _numSR[el.first] << endl;
        }
        for (auto& el : _numSR_bin) {
            cout << el.first << "\t" << _numSR_bin[el.first] << endl;
        }
        #endif

        // add_result(SignalRegionData("SR label", n_obs, {s, s_sys}, {b, b_sys}));
        add_result(SignalRegionData("SR-SF-0J-100"    , 147., {_numSR["SR-SF-0J-100"],     0.}, {145. , 12.}));
        add_result(SignalRegionData("SR-SF-0J-160"    ,  37., {_numSR["SR-SF-0J-160"],     0.}, {37.3 , 3.}));
        add_result(SignalRegionData("SR-SF-0J-100-120",  53., {_numSR["SR-SF-0J-100-120"], 0.}, {56.  , 6.}));
        add_result(SignalRegionData("SR-SF-0J-120-160",  57., {_numSR["SR-SF-0J-120-160"], 0.}, {51.  , 5.}));

        add_result(SignalRegionData("SR-SF-1J-100"    , 120., {_numSR["SR-SF-1J-100"],     0.}, {124. , 12.}));
        add_result(SignalRegionData("SR-SF-1J-160"    ,  29., {_numSR["SR-SF-1J-160"],     0.}, {36.  , 5. }));
        add_result(SignalRegionData("SR-SF-1J-100-120",  55., {_numSR["SR-SF-1J-100-120"], 0.}, {48.  , 8.}));
        add_result(SignalRegionData("SR-SF-1J-120-160",  36., {_numSR["SR-SF-1J-120-160"], 0.}, {40.  , 4. }));

        add_result(SignalRegionData("SR-DF-0J-100"    ,  95., {_numSR["SR-DF-0J-100"],     0.}, {97.  , 15.}));
        add_result(SignalRegionData("SR-DF-0J-160"    ,  21., {_numSR["SR-DF-0J-160"],     0.}, {18.8 , 2.4 }));
        add_result(SignalRegionData("SR-DF-0J-100-120",  47., {_numSR["SR-DF-0J-100-120"], 0.}, {45.  , 9.}));
        add_result(SignalRegionData("SR-DF-0J-120-160",  27., {_numSR["SR-DF-0J-120-160"], 0.}, {33.  , 5.}));

        add_result(SignalRegionData("SR-DF-1J-100"    ,  75., {_numSR["SR-DF-1J-100"],     0.}, {75.  , 9.}));
        add_result(SignalRegionData("SR-DF-1J-160"    ,  15., {_numSR["SR-DF-1J-160"],     0.}, {15.1 , 2.7 }));
        add_result(SignalRegionData("SR-DF-1J-100-120",  38., {_numSR["SR-DF-1J-100-120"], 0.}, {39.  , 6.}));
        add_result(SignalRegionData("SR-DF-1J-120-160",  22., {_numSR["SR-DF-1J-120-160"], 0.}, {21.3 , 2.8 }));
      }


    protected:
      void analysis_specific_reset() {
        for (auto& el : _numSR) { el.second = 0.;}
        for (auto& el : _numSR_bin) { el.second = 0.;}
      }

    };

    // Factory fn
    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_2OSLEP_chargino_139invfb)


    //
    // Derived analysis class for the 2Lep0Jets SRs
    //
    class Analysis_ATLAS_13TeV_2OSLEP_chargino_inclusive_139invfb : public Analysis_ATLAS_13TeV_2OSLEP_chargino_139invfb {

    public:
      Analysis_ATLAS_13TeV_2OSLEP_chargino_inclusive_139invfb() {
        set_analysis_name("ATLAS_13TeV_2OSLEP_chargino_inclusive_139invfb");
      }

      virtual void collect_results() {
        // add_result(SignalRegionData("SR label", n_obs, {s, s_sys}, {b, b_sys}));
        add_result(SignalRegionData("SR-SF-0J-100"    , 147., {_numSR["SR-SF-0J-100"],     0.}, {145. , 12.}));
        add_result(SignalRegionData("SR-SF-0J-160"    ,  37., {_numSR["SR-SF-0J-160"],     0.}, {37.3 , 3.}));
        add_result(SignalRegionData("SR-SF-0J-100-120",  53., {_numSR["SR-SF-0J-100-120"], 0.}, {56.  , 6.}));
        add_result(SignalRegionData("SR-SF-0J-120-160",  57., {_numSR["SR-SF-0J-120-160"], 0.}, {51.  , 5.}));

        add_result(SignalRegionData("SR-SF-1J-100"    , 120., {_numSR["SR-SF-1J-100"],     0.}, {124. , 12.}));
        add_result(SignalRegionData("SR-SF-1J-160"    ,  29., {_numSR["SR-SF-1J-160"],     0.}, {36.  , 5. }));
        add_result(SignalRegionData("SR-SF-1J-100-120",  55., {_numSR["SR-SF-1J-100-120"], 0.}, {48.  , 8.}));
        add_result(SignalRegionData("SR-SF-1J-120-160",  36., {_numSR["SR-SF-1J-120-160"], 0.}, {40.  , 4. }));

        add_result(SignalRegionData("SR-DF-0J-100"    ,  95., {_numSR["SR-DF-0J-100"],     0.}, {97.  , 15.}));
        add_result(SignalRegionData("SR-DF-0J-160"    ,  21., {_numSR["SR-DF-0J-160"],     0.}, {18.8 , 2.4 }));
        add_result(SignalRegionData("SR-DF-0J-100-120",  47., {_numSR["SR-DF-0J-100-120"], 0.}, {45.  , 9.}));
        add_result(SignalRegionData("SR-DF-0J-120-160",  27., {_numSR["SR-DF-0J-120-160"], 0.}, {33.  , 5.}));

        add_result(SignalRegionData("SR-DF-1J-100"    ,  75., {_numSR["SR-DF-1J-100"],     0.}, {75.  , 9.}));
        add_result(SignalRegionData("SR-DF-1J-160"    ,  15., {_numSR["SR-DF-1J-160"],     0.}, {15.1 , 2.7 }));
        add_result(SignalRegionData("SR-DF-1J-100-120",  38., {_numSR["SR-DF-1J-100-120"], 0.}, {39.  , 6.}));
        add_result(SignalRegionData("SR-DF-1J-120-160",  22., {_numSR["SR-DF-1J-120-160"], 0.}, {21.3 , 2.8 }));
      }

    };

    // Factory fn
    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_2OSLEP_chargino_inclusive_139invfb)

    //
    // Derived analysis class for the 3Lep SRs
    //
    class Analysis_ATLAS_13TeV_2OSLEP_chargino_binned_139invfb : public Analysis_ATLAS_13TeV_2OSLEP_chargino_139invfb {

    public:
      Analysis_ATLAS_13TeV_2OSLEP_chargino_binned_139invfb() {
        set_analysis_name("ATLAS_13TeV_2OSLEP_chargino_binned_139invfb");
      }

      virtual void collect_results() {
        // add_result(SignalRegionData("SR label", n_obs, {s, s_sys}, {b, b_sys}));
        add_result(SignalRegionData("SR-DF-0J-100-105", 14 , {_numSR_bin["SR-DF-0J-100-105"], 0.}, { 14.198132 , 3.946449 }));
        add_result(SignalRegionData("SR-DF-0J-105-110", 14 , {_numSR_bin["SR-DF-0J-105-110"], 0.}, { 11.369926 , 2.994202 }));
        add_result(SignalRegionData("SR-DF-0J-110-120", 19 , {_numSR_bin["SR-DF-0J-110-120"], 0.}, { 20.222225 , 3.756363 }));
        add_result(SignalRegionData("SR-DF-0J-120-140", 16 , {_numSR_bin["SR-DF-0J-120-140"], 0.}, { 21.771538 , 3.120926 }));
        add_result(SignalRegionData("SR-DF-0J-140-160", 11 , {_numSR_bin["SR-DF-0J-140-160"], 0.}, { 11.023659 , 1.883087 }));
        add_result(SignalRegionData("SR-DF-0J-160-180", 8 , {_numSR_bin["SR-DF-0J-160-180"], 0.}, { 6.449802 , 0.780903 }));
        add_result(SignalRegionData("SR-DF-0J-180-220", 9 , {_numSR_bin["SR-DF-0J-180-220"], 0.}, { 6.608662 , 1.129852 }));
        add_result(SignalRegionData("SR-DF-0J-220-260", 0 , {_numSR_bin["SR-DF-0J-220-260"], 0.}, { 3.374393 , 0.473004 }));
        add_result(SignalRegionData("SR-DF-0J-260", 4 , {_numSR_bin["SR-DF-0J-260"], 0.}, { 2.987064 , 0.473004 }));
        add_result(SignalRegionData("SR-DF-1J-100-105", 12 , {_numSR_bin["SR-DF-1J-100-105"], 0.}, { 14.82642 , 2.800548 }));
        add_result(SignalRegionData("SR-DF-1J-105-110", 12 , {_numSR_bin["SR-DF-1J-105-110"], 0.}, { 10.109783 , 1.940197 }));
        add_result(SignalRegionData("SR-DF-1J-110-120", 14 , {_numSR_bin["SR-DF-1J-110-120"], 0.}, { 14.487286 , 2.28648 }));
        add_result(SignalRegionData("SR-DF-1J-120-140", 15 , {_numSR_bin["SR-DF-1J-120-140"], 0.}, { 14.883545 , 2.118694 }));
        add_result(SignalRegionData("SR-DF-1J-140-160", 7 , {_numSR_bin["SR-DF-1J-140-160"], 0.}, { 6.688084 , 0.978134 }));
        add_result(SignalRegionData("SR-DF-1J-160-180", 4 , {_numSR_bin["SR-DF-1J-160-180"], 0.}, { 4.414993 , 1.095948 }));
        add_result(SignalRegionData("SR-DF-1J-180-220", 5 , {_numSR_bin["SR-DF-1J-180-220"], 0.}, { 5.726025 , 0.966533 }));
        add_result(SignalRegionData("SR-DF-1J-220-260", 3 , {_numSR_bin["SR-DF-1J-220-260"], 0.}, { 2.412327 , 0.576526 }));
        add_result(SignalRegionData("SR-DF-1J-260", 3 , {_numSR_bin["SR-DF-1J-260"], 0.}, { 2.888004 , 0.786255 }));
        add_result(SignalRegionData("SR-SF-0J-100-105", 14 , {_numSR_bin["SR-SF-0J-100-105"], 0.}, { 15.886662 , 2.382862 }));
        add_result(SignalRegionData("SR-SF-0J-105-110", 15 , {_numSR_bin["SR-SF-0J-105-110"], 0.}, { 13.941113 , 2.036582 }));
        add_result(SignalRegionData("SR-SF-0J-110-120", 24 , {_numSR_bin["SR-SF-0J-110-120"], 0.}, { 27.057575 , 3.057556 }));
        add_result(SignalRegionData("SR-SF-0J-120-140", 37 , {_numSR_bin["SR-SF-0J-120-140"], 0.}, { 33.259266 , 3.644798 }));
        add_result(SignalRegionData("SR-SF-0J-140-160", 20 , {_numSR_bin["SR-SF-0J-140-160"], 0.}, { 17.562698 , 2.331993 }));
        add_result(SignalRegionData("SR-SF-0J-160-180", 12 , {_numSR_bin["SR-SF-0J-160-180"], 0.}, { 10.329323 , 0.921909 }));
        add_result(SignalRegionData("SR-SF-0J-180-220", 12 , {_numSR_bin["SR-SF-0J-180-220"], 0.}, { 13.464527 , 1.776886 }));
        add_result(SignalRegionData("SR-SF-0J-220-260", 5 , {_numSR_bin["SR-SF-0J-220-260"], 0.}, { 6.697906 , 1.073632 }));
        add_result(SignalRegionData("SR-SF-0J-260", 8 , {_numSR_bin["SR-SF-0J-260"], 0.}, { 6.935303 , 0.995094 }));
        add_result(SignalRegionData("SR-SF-1J-100-105", 12 , {_numSR_bin["SR-SF-1J-100-105"], 0.}, { 17.521645 , 3.881305 }));
        add_result(SignalRegionData("SR-SF-1J-105-110", 13 , {_numSR_bin["SR-SF-1J-105-110"], 0.}, { 13.770641 , 2.521199 }));
        add_result(SignalRegionData("SR-SF-1J-110-120", 30 , {_numSR_bin["SR-SF-1J-110-120"], 0.}, { 17.372608 , 3.613556 }));
        add_result(SignalRegionData("SR-SF-1J-120-140", 21 , {_numSR_bin["SR-SF-1J-120-140"], 0.}, { 23.406528 , 2.84158 }));
        add_result(SignalRegionData("SR-SF-1J-140-160", 15 , {_numSR_bin["SR-SF-1J-140-160"], 0.}, { 17.055782 , 2.300755 }));
        add_result(SignalRegionData("SR-SF-1J-160-180", 11 , {_numSR_bin["SR-SF-1J-160-180"], 0.}, { 9.367249 , 1.860782 }));
        add_result(SignalRegionData("SR-SF-1J-180-220", 8 , {_numSR_bin["SR-SF-1J-180-220"], 0.}, { 12.414104 , 1.543061 }));
        add_result(SignalRegionData("SR-SF-1J-220-260", 5 , {_numSR_bin["SR-SF-1J-220-260"], 0.}, { 6.488174 , 1.576985 }));
        add_result(SignalRegionData("SR-SF-1J-260", 5 , {_numSR_bin["SR-SF-1J-260"], 0.}, { 7.986618 , 2.808563 }));

      }

    };

    // Factory fn
    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_2OSLEP_chargino_binned_139invfb)


  }
}
