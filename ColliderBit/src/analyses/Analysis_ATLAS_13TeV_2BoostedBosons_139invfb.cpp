///
///  \author Are Raklev
///  \date 2021 July
///
/// TODO: Fix Higgsino decays in test point?
///
///  Based on the search presented in ATLAS-CONF-2021-022.
///
///  WARNING: This implementation only predicts the non-b-jet signal regions due to problems reproducing
///  the b-tagging used based on small radius track jets. With no real cut-flows available the implementation
///  has been checked solely as performing satisfactorily agains the digitized content of Figs. 10 and 11 a) in
///  the conference note. Some further limitations are:
///
///  * Due to mis-tagging probabilities for W- and Z-jets lacking from the note, events will be missing from
///  signal regions not directly corresponding to the produced bosons, e.g. in chargino--neutralino
///  production with decays into W and Z, events would otherwise also be expected in the ZZ signal region
///  due to misidentification of jets, but this will bot be the case with this inplementation. As a result the
///  most reliable and most constraining signal region will typically be the VV signal region.
///
///  * For b-tagging we take the conservative approach of allowing no events with b-labeled jets and use
///  the mis-tagging probabilities for the small radius track jets for non-b-labeled jets.
///
///  * The separation of E_T^miss and jets in the analysis is done using small radius jets. Here we use the
///  large radius jets since they are the only ones available.
///
///  *********************************************

#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>

#include "gambit/ColliderBit/analyses/Analysis.hpp"
#include "gambit/ColliderBit/ATLASEfficiencies.hpp"
//#include "gambit/ColliderBit/lester_mt2_bisect.h"

#define CHECK_CUTFLOW

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
      
      #ifdef CHECK_CUTFLOW
      string scenario = "HG";
      // Cut-flows
      // Yields in SRs {4Q-WW, 4Q-WZ, 4Q-ZZ, 4Q-VV, 2B2Q-WZ, 2B2Q-Wh, 2B2Q-ZZ, 2B2Q-Zh, 2B2Q-VZ, 2B2Q-Vh}
      vector<double> _yield_WZ = {3.5967, 6.3812, 4.1271, 6.7624, 2.0387, 0.3481, 1.8895, 0.2818, 2.3702, 0.3812};
      vector<double> _yield_Wh = {0.3812, 0.6961, 0.4475, 0.7459, 1.1934, 5.2044, 0.8287, 3.6961, 1.2597, 5.5359};
      vector<double> _yield_HG = {0.8122, 1.5746, 1.2597, 1.8398, 1.5083, 2.0552, 1.9558, 3.0000, 2.1878, 3.2818};
      
      vector<double> _meff_4QVV_WW = {0.15911, 1.1519, 2.1652, 2.3533, 2.302, 1.3157, 0.70599, 0.39097, 0.22347, 0.12507, 0.0, 0.0};
      vector<double> _meff_4QVV_WZ = {0.0, 0.30543, 0.62458, 1.264, 1.4178, 1.493, 1.2082, 0.9881, 0.65451, 0.35119, 0.29642, 0.0};
      vector<double> _meff_4QVV_HG = {0.0, 0.1461, 0.56213, 0.43614, 0.4357, 0.66339, 0.40393, 0.1648, 0.0, 0.0, 0.0, 0.0};

      const vector<double> _meff_bins = {700., 850., 1000., 1150., 1300., 1450., 1600., 1750., 1900., 2050., 2200., 2350., 2500,};
      vector<double> _meff_4QVV = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      #endif
      
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
        HEPUtils::P4 metVec = event->missingmom();
        
        // Define vectors of baseline leptons
        // Baseline electrons
        vector<const HEPUtils::Particle*> electrons;
        for (const HEPUtils::Particle* electron : event->electrons()) {
          if (electron->pT() > 4.5
              && fabs(electron->eta()) < 2.47)
            electrons.push_back(electron);
        }
        // Apply electron efficiency from "Loose" criteria in 1902.04655
        ATLAS::applyElectronIDEfficiency2019(electrons, "Loose");
        // Baseline muons
        vector<const HEPUtils::Particle*> muons;
        for (const HEPUtils::Particle* muon : event->muons()) {
          if (muon->pT() > 3.
              && fabs(muon->eta()) < 2.7)
            muons.push_back(muon);
        }
        // Apply muon efficiency
        ATLAS::applyMuonEffR2(muons);
        
        // Number of leptons
        size_t nMuons = muons.size();
        size_t nElectrons = electrons.size();
        size_t nLeptons = nElectrons+nMuons;

        // Look at jets to see if they fulfil criteria for fat jets
        vector<const HEPUtils::Jet*> fatJets;
        for (const HEPUtils::Jet* jet : event->jets()) {
          //  cout  << jet->pT() << " " << jet->mass() << " Z-tag " <<  jet->Ztag() << " W-tag " << jet->Wtag() << " " << endl;
          if (jet->pT() > 200. && fabs(jet->eta()) < 2.0 && jet->mass() > 40.){
            fatJets.push_back(jet);
          }
        }
        // Overlap removal (remove fat jets within DR=1 of electrons)
        JetLeptonOverlapRemoval(fatJets, electrons, 1.0);
        size_t nfat = fatJets.size();
        
        // Tag the large jets (only look at two hardest jets)
        int nW = 0; int nZ = 0; int ntest = 0;
        const vector<double> bpT = {200., 300., 500., 700., 900., DBL_MAX}; // pT bin edges
        const vector<double> pW = {0.469, 0.475, 0.481, 0.496, 0.522}; // W tag prob
        const vector<double> pWmiss = {1/10.2574, 1/20.2997, 1/33.4745, 1/36.0622, 1/29.1341}; // W misstag prob
        const vector<double> pZ = {0.469, 0.488, 0.513, 0.516, 0.525}; // Z tag prob
        const vector<double> pZmiss = {1/11.5847, 1/18.5291, 1/27.7596, 1/38.4142, 1/26.0997}; // Z misstag prob
        const HEPUtils::BinnedFn1D<double> _eff1dW(bpT, pW);
        const HEPUtils::BinnedFn1D<double> _eff1dWmiss(bpT, pWmiss);
        const HEPUtils::BinnedFn1D<double> _eff1dZ(bpT, pZ);
        const HEPUtils::BinnedFn1D<double> _eff1dZmiss(bpT, pZmiss);
        for (const HEPUtils::Jet* jet : fatJets) {
          // Tag W
          if( jet->Wtag() && random_bool( _eff1dW.get_at( jet->pT() ) ) ) nW++;
          // Tag Z
          if( jet->Ztag()  && random_bool( _eff1dZ.get_at( jet->pT() ) ) ) nZ++;
          // Misstag as Z or W
          if( !jet->Wtag() && !jet->Ztag() ) {
            if( random_bool( _eff1dZmiss.get_at( jet->pT() ) )  ) nZ++;
            if( random_bool( _eff1dWmiss.get_at( jet->pT() ) )  ) nW++;
          }
          ntest++;
          if(ntest > 1) break;
        }
        int nV = nZ + nW;
        // cout << "nZ " << nZ << " nW " << nW << " nV " << nV << endl;
        
  	    // b-jet tagging
        /* There is a difference here wrt the actual analysis where small
         sliding radius track jets are used, and the number of such b-jets are
         counted. This means that the rejection for b-jets in the 4Q SRs has to
         be changed. We use the conservative choice of rejecting all events with
         a b-labeled large radius jet and mis-tagging large radius non-b-jets
         according to the mis-tag probabilities of the small radius track jets.
        */
        double btag = 0.83; double cmisstag = 1/3.; double misstag = 1./33.;
        int nb = 0;
        for ( const HEPUtils::Jet* jet : event->jets() ) {
          // Tag b-jet
          if( jet->btag() ) nb++;
          // Misstag c-jet
          else if( !jet->btag() && jet->ctag() && random_bool(cmisstag) ) nb++;
          // Misstag light jet
          else if( !jet->btag() && !jet->ctag() && random_bool(misstag) ) nb++;
        }
        
        // Check separation of jets and ETmiss
        bool delphi = true;
        for ( const HEPUtils::Jet* jet : fatJets ) {
          double phi = jet->mom().deltaPhi(metVec);
          if (phi < 1.0) delphi = false;
        }

        // Effective mass (missing energy plus two leading fatjet pTs)
        double meff = met;
        if(fatJets.size() > 0) meff += fatJets[0]->pT();
        if(fatJets.size() > 1) meff += fatJets[1]->pT();

        
        //
        // Count signal region events
        //
        
        // Preselection conditions
        if(nfat > 1 && nLeptons == 0){
          // First exclusion regions
          if(nb == 0 && delphi){
            if(met > 300. && meff > 1300. && nV == 2 && nW == 2) _counters.at("SR-4Q-WW").add_event(event);
            if(met > 300. && meff > 1300. && nV == 2 && nW > 0 && nZ > 0) _counters.at("SR-4Q-WZ").add_event(event);
            if(met > 300. && meff > 1300. && nV == 2 && nZ ==2) _counters.at("SR-4Q-ZZ").add_event(event);
            if(met > 300. && meff > 1300. && nV == 2) _counters.at("SR-4Q-VV").add_event(event);
          }
          if(nb == 1){
          }
          // Then discovery regions
          
        }
        
        #ifdef CHECK_CUTFLOW
        if(nfat > 1 && nLeptons == 0 && nb == 0 && delphi && met > 300. && nV == 2){
          size_t i = floor((meff-_meff_bins[0])/150);
          if(i < _meff_4QVV.size() ) _meff_4QVV[i]++;
        }
        #endif
        

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

        #ifdef CHECK_CUTFLOW
        cout << "Cut-flow output" << endl;
        
        double _xsec_model;
        double lumi = luminosity();

        vector<double> _yield_model(10);
        vector<double> _meff_4QVV_model(12);
        if(scenario == "WZ"){
          _yield_model = _yield_WZ;
          _meff_4QVV_model = _meff_4QVV_WZ;
          _xsec_model = 2.52;
        }
        else if(scenario == "WW"){
          _meff_4QVV_model = _meff_4QVV_WW;
          _xsec_model = 4.42;
        }
        else if(scenario == "Wh"){
          _yield_model = _yield_Wh;
          _xsec_model = 2.52;
        }
        else if(scenario == "HG"){
          _yield_model = _yield_HG;
          _meff_4QVV_model = _meff_4QVV_HG;
          _xsec_model = 2.34;
        }
        double weight = _xsec_model*lumi/50000; // Weights of less than 0.01
        
        // Compare final event yield per SR for model
        cout << "SR\t\t" << "GAMBIT\t" << "ATLAS" << endl;
        cout << "SR-4Q-WW\t" << _counters.at("SR-4Q-WW").sum()*weight << "\t" << _yield_model[0] << endl;
        cout << "SR-4Q-WZ\t" << _counters.at("SR-4Q-WZ").sum()*weight << "\t" << _yield_model[1] << endl;
        cout << "SR-4Q-ZZ\t" << _counters.at("SR-4Q-ZZ").sum()*weight << "\t" << _yield_model[2] << endl;
        cout << "SR-4Q-VV\t" << _counters.at("SR-4Q-VV").sum()*weight << "\t" << _yield_model[3] << endl;
        cout << endl;

        // Compare meff spectrum
        cout << "Meff SR-4Q-VV\t" << "GAMBIT\t" << "ATLAS " << endl;
        for( size_t j = 0; j < _meff_4QVV.size(); j++){
          cout << "[" << _meff_bins[j] << ", " << _meff_bins[j+1] << "]\t" << _meff_4QVV[j]*weight << "\t" << _meff_4QVV_model[j] << endl;
        }

        #endif
        
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


/*
 
 
 ****
 WW (WW final states)

 SR        GAMBIT  ATLAS
 SR-4Q-VV  5.332    5.065
  
 Meff SR-4Q-VV  GAMBIT  ATLAS
 [700, 850]  0.233  0.159
 [850, 1e+03]  1.14  1.15
 [1e+03, 1.15e+03]  1.71  2.17
 [1.15e+03, 1.3e+03]  2.58  2.35
 [1.3e+03, 1.45e+03]  2.22  2.3
 [1.45e+03, 1.6e+03]  1.13  1.32
 [1.6e+03, 1.75e+03]  0.872  0.706
 [1.75e+03, 1.9e+03]  0.553  0.391
 [1.9e+03, 2.05e+03]  0.258  0.223
 [2.05e+03, 2.2e+03]  0.147  0.125
 [2.2e+03, 2.35e+03]  0.0492  0
 [2.35e+03, 2.5e+03]  0.0246  0


 ****
 HG (ZZ final states)

 Cut-flow output
 SR    GAMBIT  ATLAS
 SR-4Q-WW  0.039  0.812
 SR-4Q-WZ  0.325  1.57
 SR-4Q-ZZ  0.943  1.26
 SR-4Q-VV  1.31  1.84

 Meff SR-4Q-VV  GAMBIT  ATLAS
 [700, 850]  0.013  0
 [850, 1e+03]  0.124  0.146
 [1e+03, 1.15e+03]  0.234  0.562
 [1.15e+03, 1.3e+03]  0.377  0.436
 [1.3e+03, 1.45e+03]  0.358  0.436
 [1.45e+03, 1.6e+03]  0.293  0.663
 [1.6e+03, 1.75e+03]  0.202  0.404
 [1.75e+03, 1.9e+03]  0.215  0.165
 [1.9e+03, 2.05e+03]  0.111  0
 [2.05e+03, 2.2e+03]  0.0325  0
 [2.2e+03, 2.35e+03]  0.0325  0
 [2.35e+03, 2.5e+03]  0.026  0


 ****
 WZ (WZ final states)
 
 Cut-flow output
 SR    GAMBIT  ATLAS
 SR-4Q-WW  0.049  3.6
 SR-4Q-WZ  6.47  6.38
 SR-4Q-ZZ  0.056  4.13
 SR-4Q-VV  6.57  6.76

 Meff SR-4Q-VV  GAMBIT  ATLAS
 [700, 850]  0.00701  0
 [850, 1e+03]  0.231  0.305
 [1e+03, 1.15e+03]  0.673  0.625
 [1.15e+03, 1.3e+03]  0.911  1.26
 [1.3e+03, 1.45e+03]  1.43  1.42
 [1.45e+03, 1.6e+03]  1.36  1.49
 [1.6e+03, 1.75e+03]  1.11  1.21
 [1.75e+03, 1.9e+03]  0.974  0.988
 [1.9e+03, 2.05e+03]  0.595  0.655
 [2.05e+03, 2.2e+03]  0.364  0.351
 [2.2e+03, 2.35e+03]  0.273  0.296
 [2.35e+03, 2.5e+03]  0.182  0


 ***
 Wh (Wbb, WWW^*, WZZ^* final states)

 Cut-flow output
 SR    GAMBIT  ATLAS
 SR-4Q-WW  1.1  0.381
 SR-4Q-WZ  0.252  0.696
 SR-4Q-ZZ  0  0.448
 SR-4Q-VV  1.35  0.746

 */
