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
//#include "gambit/ColliderBit/lester_mt2_bisect.h"

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

      
    public:

      // Required detector sim
      static constexpr const char* detector = "ATLAS";

      Analysis_ATLAS_13TeV_2BoostedBosons_139invfb() {

        set_analysis_name("ATLAS_13TeV_2BoostedBosons_139invfb");
        set_luminosity(139.);

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

      // Run main event analysis
      void run(const HEPUtils::Event* event) {

        // Get the missing energy in the event
        double met = event->met();
//        HEPUtils::P4 metVec = event->missingmom();
        
        // Define vectors of baseline leptons
        // Baseline electrons
        vector<const HEPUtils::Particle*> electrons;
        for (const HEPUtils::Particle* electron : event->electrons()) {
          if (electron->pT() > 4.5
              && fabs(electron->eta()) < 2.47)
            electrons.push_back(electron);
        }
        // TODO: Check this
        // Apply electron efficiency
        ATLAS::applyElectronEff(electrons);
        // Baseline muons
        vector<const HEPUtils::Particle*> muons;
        for (const HEPUtils::Particle* muon : event->muons()) {
          if (muon->pT() > 3.
              && fabs(muon->eta()) < 2.7)
            muons.push_back(muon);
        }
        // TODO: Check this
        // Apply muon efficiency
        ATLAS::applyMuonEff(muons);
        
        // Number of leptons
        size_t nMuons = muons.size();
        size_t nElectrons = electrons.size();
        size_t nLeptons = nElectrons+nMuons;

        // Overlap removal (remove fat jets within DR=1)
        vector<const HEPUtils::Jet*> candJets;
        for (const HEPUtils::Jet* jet : event->jets()) {
          if (jet->pT() > 20. && fabs(jet->eta()) < 2.0)
            candJets.push_back(jet);
        }
        JetLeptonOverlapRemoval(candJets, electrons, 1.0);
        
        // Look at two hardest jets to see if they fullfil criteria for fat jets
        vector<const HEPUtils::Jet*> fatJets;
        int njet = 0; int nfat = 0;
        for (const HEPUtils::Jet* jet : candJets) {
          njet++;
//          cout << njet << " " << jet->pT() << " " << jet->mass() << " Z-tag " <<  jet->Ztag() << " W-tag " << jet->Wtag() << " " << endl;
          if (jet->pT() > 200. && fabs(jet->eta()) < 2.0 && jet->mass() > 40.){
            nfat++;
            fatJets.push_back(jet);
          }
          if (njet > 1) break;
        }
        
        // TODO: Read off the mis-tag rates more accurately
        // TODO: This conservatively does not use jets that have been both W and Z tagged
        // Tag the large jets
        int nW = 0; int nZ = 0;
        const vector<double> bpT = {200., 300., 500., 700., 900., DBL_MAX}; // pT bin edges
        const vector<double> pW = {0.470, 0.475, 0.480, 0.495, 0.520}; // W tag prob
        const vector<double> pWmiss = {1/10., 1/20., 1/30., 1/30., 1/30.}; // W misstag prob
        const vector<double> pZ = {0.470, 0.490, 0.510, 0.515, 0.525}; // Z tag prob
        const vector<double> pZmiss = {1/10., 1/20., 1/30., 1/30., 1/30.}; // Z misstag prob
        const HEPUtils::BinnedFn1D<double> _eff1dW(bpT, pW);
        const HEPUtils::BinnedFn1D<double> _eff1dWmiss(bpT, pWmiss);
        const HEPUtils::BinnedFn1D<double> _eff1dZ(bpT, pZ);
        const HEPUtils::BinnedFn1D<double> _eff1dZmiss(bpT, pZmiss);
        for (const HEPUtils::Jet* jet : fatJets) {
          // Tag W
          if( jet->Wtag() && !jet->Ztag() && random_bool( _eff1dW.get_at( jet->pT() ) ) ) nW++;
          // Tag Z
          if( jet->Ztag() && !jet->Wtag() && random_bool( _eff1dZ.get_at( jet->pT() ) ) ) nZ++;
          // Double taged
          if( jet->Ztag() && jet->Wtag() ){
            // Conservative choice not to tag jets that have been labeled both as Z and W
//            cout << "DOUBLE TAG!!" << endl;
          }
          // Misstag as Z or W
          if( !jet->Wtag() && !jet->Ztag() ) {
            if( random_bool( _eff1dZmiss.get_at( jet->pT() ) )  ) nZ++;
            if( random_bool( _eff1dWmiss.get_at( jet->pT() ) )  ) nW++;
          }
        }
        int nV = nZ + nW;
//        cout << "nZ " << nZ << " nW " << nW << " nV " << nV << endl;
        
  	    // b-jet tagging
        /* This is the largest difference wrt the actual analysis where sliding
         radius track jets are used and the number of such b-jets are counted.
         This means that the rejection for b-jets in the 4Q SRs has to be
         changed. We use the tagging probability for a *single* track jet and
         reject on a single b-jet as a conservative choice.
        */
        double btag = 0.83; double cmisstag = 1/3.; double misstag = 1./33.;
        int nb = 0;
        for (const HEPUtils::Jet* jet : fatJets) {
          // Tag b-jet
          if( jet->btag() && random_bool(btag) ) nb++;
          // Misstag c-jet
          else if( jet->ctag() && random_bool(cmisstag) ) nb++;
          // Misstag light jet
          else if( random_bool(misstag) ) nb++;
        }

        // Effective mass (missing energy plus two leading fatjet pTs)
        double meff = met;
        if(fatJets.size() > 0) meff += fatJets[0]->pT();
        if(fatJets.size() > 1) meff += fatJets[1]->pT();

//        TODO: Implement stransverse mass for the b-jet SRs
        // Stransverse mass (two leading fat jets as legs, assumes 100 GeV invisible mass)
//        double MT2 =  asymm_mt2_lester_bisect::get_mT2(
//fatJets[0]->mom()
//        fatJets[0]->mass(), fatJets[0]->px(), fatJets[0]->py(),
//        fatJets[1]->mass(), fatJets[1]->px(), fatJets[1]->py(),
//        metVec.px(), metVec.py(),
//        100., 100., 0);


        // Now increment signal region variables
        // Preselection conditions
        if(nfat > 1 && nLeptons == 0){
          // First exclusion regions
          if(nb == 0){
            if(met > 300. && meff > 1300. && nV == 2 && nW == 2) _counters.at("SR-4Q-WW").add_event(event);
            if(met > 300. && meff > 1300. && nV == 2 && nW > 0 && nZ > 0) _counters.at("SR-4Q-WZ").add_event(event);
            if(met > 300. && meff > 1300. && nV == 2 && nZ ==2) _counters.at("SR-4Q-ZZ").add_event(event);
            if(met > 300. && meff > 1300. && nV == 2) _counters.at("SR-4Q-VV").add_event(event);
          }
          if(nb == 1){
          }
          // Discovery regions
          
        }

      } // End of analyze

      
      /// Combine the variables of another copy of this analysis (typically on another thread) into this one.
      void combine(const Analysis* other)
      {
        const Analysis_ATLAS_13TeV_2BoostedBosons_139invfb* specificOther = dynamic_cast<const Analysis_ATLAS_13TeV_2BoostedBosons_139invfb*>(other);
        for (auto& pair : _counters) { pair.second += specificOther->_counters.at(pair.first); }
      }


      void collect_results() {

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
      }

    };


    // Factory fn
    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_2BoostedBosons_139invfb)

  }
}



/* 4000

 WZ
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-VV__i3__signal: 5.9563702
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-VV__i3__signal_uncert: 0.4015784
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-WW__i0__signal: 0.05414882
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-WW__i0__signal_uncert: 0.038288998
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-WZ__i1__signal: 5.8209982
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-WZ__i1__signal_uncert: 0.39698878
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-ZZ__i2__signal: 0.08122323
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-ZZ__i2__signal_uncert: 0.046894254

 
 Wh
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-VV__i3__signal: 1.5718671
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-VV__i3__signal_uncert: 0.20639627
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-WW__i0__signal: 1.1924509
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-WW__i0__signal_uncert: 0.17976874
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-WZ__i1__signal: 0.37941621
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-WZ__i1__signal_uncert: 0.10140325
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-ZZ__i2__signal: 0
 0, 1: #LHC_signals @ColliderBit::calc_LHC_signals::ATLAS_13TeV_2BoostedBosons_139invfb__SR-4Q-ZZ__i2__signal_uncert: 0

 */
