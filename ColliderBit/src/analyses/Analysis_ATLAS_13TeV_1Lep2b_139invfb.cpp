///
///  \author Christopher Chang
///  \date 2021 March
///
///  *********************************************

// Based on [TODO: Put Arxiv Link]
// TODO: Debug + validate
// TODO: VRs aren't provided in the json, should I be using these regions?

#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <fstream>

#include "gambit/ColliderBit/analyses/Analysis.hpp"
#include "gambit/ColliderBit/analyses/AnalysisUtil.hpp"
#include "gambit/ColliderBit/ATLASEfficiencies.hpp"
#include "gambit/ColliderBit/mt2_bisect.h"

using namespace std;

namespace Gambit
{
  namespace ColliderBit
  {

    class Analysis_ATLAS_13TeV_1Lep2b_139invfb : public Analysis
    {

    protected:

      // Counters for the number of accepted events for each signal region
      std::map<string, EventCounter> _counters = {
        {"WREM_cuts_0", EventCounter("WREM_cuts_0")},
        {"STCREM_cuts_0", EventCounter("STCREM_cuts_0")},
        {"TRHMEM_cuts_0", EventCounter("TRHMEM_cuts_0")},
        {"TRMMEM_cuts_0", EventCounter("TRMMEM_cuts_0")},
        {"TRLMEM_cuts_0", EventCounter("TRLMEM_cuts_0")},
        {"SRHMEM_mct2_0", EventCounter("SRHMEM_mct2_0")},
        {"SRHMEM_mct2_1", EventCounter("SRHMEM_mct2_1")},
        {"SRHMEM_mct2_2", EventCounter("SRHMEM_mct2_2")},
        {"SRMMEM_mct2_0", EventCounter("SRMMEM_mct2_0")},
        {"SRMMEM_mct2_1", EventCounter("SRMMEM_mct2_1")},
        {"SRMMEM_mct2_2", EventCounter("SRMMEM_mct2_2")},
        {"SRLMEM_mct2_0", EventCounter("SRLMEM_mct2_0")},
        {"SRLMEM_mct2_1", EventCounter("SRLMEM_mct2_1")},
        {"SRLMEM_mct2_2", EventCounter("SRLMEM_mct2_2")},
      };

    public:

      // Required detector sim
      static constexpr const char* detector = "ATLAS";

      Analysis_ATLAS_13TeV_1Lep2b_139invfb()
      {

        analysis_specific_reset();
        set_analysis_name("ATLAS_13TeV_1Lep2b_139invfb");
        set_luminosity(139.);

      }

      // Lepton jet overlap removal
      // Discards leptons if they are within DeltaRMax of a jet
      // This uses a deltaR specific to this analysis. Be careful copying without thought.
      void LeptonJetOverlapRemoval(vector<const HEPUtils::Particle*>& leptons, vector<const HEPUtils::Jet*>& jets)
      {
        vector<const HEPUtils::Particle*> survivors;
        for(const HEPUtils::Particle* lepton : leptons)
        {
          bool overlap = false;
          double DeltaRMax = min(0.4,0.04 + 10 / (lepton->pT()));
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

      // Jet lepton overlap removal
      // Discards jets if they are within DeltaRMax of a lepton
      void JetLeptonOverlapRemoval(vector<const HEPUtils::Jet*>& jets, vector<const HEPUtils::Particle*>& leptons, double DeltaRMax)
      {
        vector<const HEPUtils::Jet*> survivors;
        for(const HEPUtils::Jet* jet : jets)
        {
          bool overlap = false;
          for(const HEPUtils::Particle* lepton: leptons)
          {
            double dR = lepton->mom().deltaR_eta(jet->mom());
            if(fabs(dR) <= DeltaRMax) overlap = true;
          }
          if(!overlap) survivors.push_back(jet);
        }
        jets = survivors;
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

      bool sortJetsByPt(const HEPUtils::Jet* jet1, const HEPUtils::Jet* jet2)
      {
        return (jet1->pT() > jet2->pT());
      }

      void run(const HEPUtils::Event* event)
      {

        // Baseline objects
        vector<const HEPUtils::Particle*> baselineElectrons = event->electrons();
        ATLAS::applyElectronEff(baselineElectrons);

        vector<const HEPUtils::Particle*> baselineMuons = event->muons();
        ATLAS::applyMuonEff(baselineMuons);

        vector<const HEPUtils::Particle*> baselineLepton;
        vector<const HEPUtils::Jet*> baselineJets;

        const P4 pmiss = event->missingmom();
        const double met = pmiss.pT();
        // MET Cut
        if (met < 240) return; //< VETO

        // Count the lepton s.
        for (const Particle* e : baselineElectrons) {
          if (e->pT() < 7 && e->abseta() > 2.47) return; //< VETO
          baselineLepton.push_back(e);
        }

        for (const Particle* m : baselineMuons) {
          if (m->pT() < 6 && m->abseta() > 2.7) return; //< VETO
          baselineLepton.push_back(m);
        }

        // Get jets (including b-tagged)
        vector<const Jet*> nonBJets;
        vector<const HEPUtils::Jet*> bJets;
        int Njet = 0;
        int Nbjet = 0;
        for (const Jet* jet : event->jets()) {
          if (jet->pT() > 30 && jet->abseta() < 2.8 ) {

            Njet = Njet + 1;

            // Trying to implement a b-tagging effeciency of 77%
            const double btag_rate = jet->btag() ? 0.77 : 0.0; 

            if (jet->abseta() < 2.5 && Random::draw() < btag_rate) {
              bJets.push_back(jet);
              Nbjet = Nbjet + 1;
            } else {
              nonBJets.push_back(jet);
            }
          }
        }

        // Remove Overlapped electrons, muons and jets
        // 1. Jets are rejected if they lie within deltaR=0.2 of a muon
        JetLeptonOverlapRemoval(nonBJets, baselineMuons, 0.2);
        JetLeptonOverlapRemoval(bJets, baselineMuons, 0.2);
        // 2. Electrons are removed if theu lie in a cone (see function for size) around a jet
        LeptonJetOverlapRemoval(baselineElectrons, nonBJets);
        LeptonJetOverlapRemoval(baselineElectrons, bJets);
        // 3. Muons are removed if theu lie in a cone (see function for size) around a jet
        LeptonJetOverlapRemoval(baselineMuons, nonBJets);
        LeptonJetOverlapRemoval(baselineMuons, bJets);

        // Count the leptons after removal.
        int Nlep = 0;
        for (const Particle* e : baselineElectrons) {
          Nlep = Nlep+1; 
        }

        for (const Particle* m : baselineMuons) {
          Nlep = Nlep+1;
        }

        // Cut on a single lepton
        if (Nlep != 1) return; //< VETO

        // Njet and Nbjet Cuts (2-3 jets, 2 of which must be b-tagged)
        if (Nbjet != 2 || (Njet !=2 && Njet !=3)) return; //< VETO

        // Order the b-jets by pT
        std::sort(bJets.begin(), bJets.end(), AnalysisUtil::sortJetsByPt);

        // Calculate mbb
        double mbb = (bJets[0]->mom() + bJets[1]->mom()).m();

        // Calculate invariant mass of lepton and leading jet
        double mlb = (bJets[0]->mom() + baselineLepton[0]->mom()).m();

        // Calculate transverse mass
        double mT = sqrt( 2* baselineLepton[0]->pT() * met * (1. - cos( deltaPhi(baselineLepton[0]->mom(), pmiss) ) )  );

        // Calculate contransverse mass of the two b-jets
        double mCT = sqrt(2 * (bJets[0]->pT()) * (bJets[0]->pT()) * (1. + cos(deltaPhi(bJets[0]->mom(), bJets[1]->mom()))) );
        const static vector<double> mCTedges = {180, 230, 280};
        const int i_sr = binIndex(mCT, mCTedges, true);

        // Assign to signal regions
        if ((mbb > 100 && mbb < 140) && (mT > 100 && mT < 160)) {
          std::stringstream sr_key; sr_key << "SRLMEM_mct2_" << i_sr;
          _counters.at(sr_key.str()).add_event(event->weight(), event->weight_err());
        } else if ((mbb > 100 && mbb < 140) && (mT > 160 && mT < 240)) {
          std::stringstream sr_key; sr_key << "SRMMEM_mct2_" << i_sr;
          _counters.at(sr_key.str()).add_event(event->weight(), event->weight_err());
        } else if ((mbb > 100 && mbb < 140) && (mT > 240) && (mlb > 120)) {
          std::stringstream sr_key; sr_key << "SRHMEM_mct2_" << i_sr;
          _counters.at(sr_key.str()).add_event(event->weight(), event->weight_err());
        } else if ((mbb < 100 || mbb > 140) && (mT > 100 && mT < 160) && (mCT < 180)) {
          _counters.at("TRLMEM_cuts_0").add_event(event->weight(), event->weight_err());
        } else if ((mbb < 100 || mbb > 140) && (mT > 160 && mT < 240) && (mCT < 180)) {
          _counters.at("TRMMEM_cuts_0").add_event(event->weight(), event->weight_err());
        } else if ((mbb < 100 || mbb > 140) && (mT > 240) && (mCT < 180)) {
          _counters.at("TRHMEM_cuts_0").add_event(event->weight(), event->weight_err());
        } else if ((mbb > 50 && mbb < 80) && (mT > 50 && mT < 100) && (mCT > 180)) {
          _counters.at("WREM_cuts_0").add_event(event->weight(), event->weight_err());
        } else if ((mbb > 195) && (mT > 100) && (mCT > 180)) {
          _counters.at("STCREM_cuts_0").add_event(event->weight(), event->weight_err());
        }

      }

      /// Combine the variables of another copy of this analysis (typically on another thread) into this one.
      void combine(const Analysis* other)
      {
        const Analysis_ATLAS_13TeV_1Lep2b_139invfb* specificOther = dynamic_cast<const Analysis_ATLAS_13TeV_1Lep2b_139invfb*>(other);
        for (auto& pair : _counters) { pair.second += specificOther->_counters.at(pair.first); }
      }

      void collect_results() { //TODO: This is if not running ATLAS_FullLikes. 
        add_result(SignalRegionData(_counters.at("WREM_cuts_0"), 144, {144.0,0.0 })); //TODO: Hard-setting equal obs and pred
        add_result(SignalRegionData(_counters.at("STCREM_cuts_0"), 155, {155.0, 0.0})); //TODO: Hard-setting equal obs and pred
        add_result(SignalRegionData(_counters.at("TRHMEM_cuts_0"), 641, {641.0,0.0 })); //TODO: Hard-setting equal obs and pred
        add_result(SignalRegionData(_counters.at("TRMMEM_cuts_0"), 491, {491.0,0.0 })); //TODO: Hard-setting equal obs and pred
        add_result(SignalRegionData(_counters.at("TRLMEM_cuts_0"), 657, {657.0,0.0 })); //TODO: Hard-setting equal obs and pred
        add_result(SignalRegionData(_counters.at("SRHMEM_mct2_0"), 6, {4.1, 1.9}));
        add_result(SignalRegionData(_counters.at("SRHMEM_mct2_1"), 5, {2.9, 1.3}));
        add_result(SignalRegionData(_counters.at("SRHMEM_mct2_2"), 3, {1.1, 0.5}));
        add_result(SignalRegionData(_counters.at("SRMMEM_mct2_0"), 4, {4.6, 1.7}));
        add_result(SignalRegionData(_counters.at("SRMMEM_mct2_1"), 7, {2.6, 1.3}));
        add_result(SignalRegionData(_counters.at("SRMMEM_mct2_2"), 2, {1.4, 0.6}));
        add_result(SignalRegionData(_counters.at("SRLMEM_mct2_0"), 16 , {8.8, 2.8}));
        add_result(SignalRegionData(_counters.at("SRLMEM_mct2_1"), 11 , {11.3, 3.1}));
        add_result(SignalRegionData(_counters.at("SRLMEM_mct2_2"), 7 , {7.3, 1.5}));


      }


    protected:
      void analysis_specific_reset() {
        for (auto& pair : _counters) { pair.second.reset(); }
      }

    };

    // Factory fn
    DEFINE_ANALYSIS_FACTORY(ATLAS_13TeV_1Lep2b_139invfb)

  }
}
