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

      void run(const HEPUtils::Event* event) {

        // Get the missing energy in the event
        double met = event->met();
        HEPUtils::P4 metVec = event->missingmom();
        
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

        // Overlap removal
        //        JetLeptonOverlapRemoval(nonbJets,electrons,0.2);
        //        LeptonJetOverlapRemoval(electrons,nonbJets);
        //        JetLeptonOverlapRemoval(nonbJets,muons,0.2);
        //        LeptonJetOverlapRemoval(muons,nonbJets);
        
        // Number of leptons
        size_t nMuons = muons.size();
        size_t nElectrons = electrons.size();
        size_t nLeptons = nElectrons+nMuons;

        // Look at two hardest jets to see if they fullfil criteria for fat jets
        vector<const HEPUtils::Jet*> fatJets;
        int njet = 0; int nfat = 0;
        for (const HEPUtils::Jet* jet : event->jets()) {
          njet++;
          cout << njet << " " << jet->pT() << " " << jet->mass() << " Z-tag " <<  jet->Ztag() << " W-tag " << jet->Wtag() << " " << endl;
          if (jet->pT() > 200. && fabs(jet->eta()) < 2.0 && jet->mass() > 40.){
            nfat++;
            fatJets.push_back(jet);
          }
          if (njet > 1) break;
        }

        // Tag the large jets
        int nW = 0; int nZ = 0;
        double Wtag = 0.5; double Wmiss = 1/30.;
        double Ztag = 0.5; double Zmiss = 1/30.;
        for (const HEPUtils::Jet* jet : fatJets) {
          // Tag W
          if( jet->Wtag() && random_bool(Wtag) ) nW++;
          // Misstag W
          if( !jet->Wtag() && random_bool(Wmiss) ) nW++;
          // Tag Z
          if( jet->Ztag() && random_bool(Ztag) ) nZ++;
          // Misstag Z
          if( !jet->Ztag() && random_bool(Zmiss) ) nZ++;
        }
        int nV = nZ + nW;
        cout << "nZ " << nZ << " nW " << nW << " nV " << nV << endl;
        

//   	    // b-jets
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
          if(met > 300. && meff > 1300. && nV == 2 && nW == 2) _counters.at("SR-4Q-WW").add_event(event);
          if(met > 300. && meff > 1300. && nV == 2  && nW > 0 && nZ > 0) _counters.at("SR-4Q-WZ").add_event(event);
          if(met > 300. && meff > 1300. && nV == 2  && nZ ==2) _counters.at("SR-4Q-ZZ").add_event(event);
          if(met > 300. && meff > 1300. && nV == 2) _counters.at("SR-4Q-VV").add_event(event);
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
