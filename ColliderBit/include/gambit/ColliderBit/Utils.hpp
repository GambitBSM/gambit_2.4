#pragma once
#include "HEPUtils/MathUtils.h"
#include "HEPUtils/BinnedFn.h"
#include "HEPUtils/Event.h"
#include "HEPUtils/FastJet.h"
#include <functional>

namespace Gambit {
  namespace ColliderBit {


    /// Unit conversions (multiply to construct in standard units, divide to decode to that unit)
    static const double GeV = 1, MeV = 1e-3, TeV = 1e3;


    /// Return a random true/false at a success rate given by a number
    bool random_bool(double eff);

    /// Return a random true/false at a success rate given by a 1D efficiency map
    inline bool random_bool(const HEPUtils::BinnedFn1D<double>& effmap, double x) {
      return random_bool( effmap.get_at(x) );
    }

    /// Return a random true/false at a success rate given by a 2D efficiency map
    inline bool random_bool(const HEPUtils::BinnedFn2D<double>& effmap, double x, double y) {
      return random_bool( effmap.get_at(x, y) );
    }


    /// Utility function for filtering a supplied particle vector by sampling wrt an efficiency scalar
    void filtereff(std::vector<HEPUtils::Particle*>& particles, double eff, bool do_delete=true);

    /// Utility function for filtering a supplied particle vector by sampling an efficiency returned by a provided function object
    void filtereff(std::vector<HEPUtils::Particle*>& particles, std::function<double(HEPUtils::Particle*)> eff_fn, bool do_delete=true);

    /// Utility function for filtering a supplied particle vector by sampling wrt a binned 1D efficiency map in pT
    void filtereff_pt(std::vector<HEPUtils::Particle*>& particles, const HEPUtils::BinnedFn1D<double>& eff_pt, bool do_delete=true);

    /// Utility function for filtering a supplied particle vector by sampling wrt a binned 2D efficiency map in |eta| and pT
    void filtereff_etapt(std::vector<HEPUtils::Particle*>& particles, const HEPUtils::BinnedFn2D<double>& eff_etapt, bool do_delete=true);


    /// Randomly get a tag result (can be anything) from a 2D |eta|-pT efficiency map
    /// @todo Also need 1D? Sampling in what variable?
    inline bool has_tag(const HEPUtils::BinnedFn2D<double>& effmap, double eta, double pt) {
      try {
        return random_bool(effmap, fabs(eta), pt);
      } catch (...) {
        return false; // No tag if outside lookup range... be careful!
      }
    }


    template <typename NUM1, typename NUM2>
    inline size_t binIndex(NUM1 val, const std::vector<NUM2>& binedges, bool allow_overflow=false) {
      if (val < binedges.front()) return -1; ///< Below/out of histo range
      if (val >= binedges.back()) return allow_overflow ? int(binedges.size())-1 : -1; ///< Above/out of histo range
      return std::distance(binedges.begin(), --std::upper_bound(binedges.begin(), binedges.end(), val));
    }


    /// Make a vector of central bin values from a vector of bin edge values using linear interpolation
    inline std::vector<double> mk_bin_values(const std::vector<double>& binEdgeValues) {
      std::vector<double> results;
      results.reserve(binEdgeValues.size()-1);
      for (size_t i = 0; i < binEdgeValues.size()-1; ++i)
        results.push_back( (binEdgeValues[i] + binEdgeValues[i+1])/2.0 );
      return results;
    }
    /// Alias
    inline std::vector<double> makeBinValues(const std::vector<double>& binEdgeValues) {
      return mk_bin_values(binEdgeValues);
    }


    /// Run jet clustering from any P4-compatible momentum type
    template <typename MOM>
    inline std::vector<HEPUtils::Jet*> get_jets(const std::vector<MOM*>& moms, double R,
                                                double ptmin=0*GeV, FJNS::JetAlgorithm alg=FJNS::antikt_algorithm) {
      // Make PseudoJets
      std::vector<FJNS::PseudoJet> constituents;
      for (const MOM* p : moms) constituents.push_back(HEPUtils::mk_pseudojet(*p));
      // Run clustering
      std::vector<FJNS::PseudoJet> jets = HEPUtils::get_jets(constituents, R, ptmin, alg);
      // Make newly-allocated Jets
      std::vector<HEPUtils::Jet*> rtn;
      for (const FJNS::PseudoJet* j : jets) rtn.push_back(new HEPUtils::Jet(HEPUtils::mk_p4(j)));
      return rtn;
    }


    /// Check if there's a physics object above ptmin in an annulus rmin..rmax around the given four-momentum p4
    inline bool object_in_cone(const HEPUtils::Event& e, const HEPUtils::P4& p4, double ptmin, double rmax, double rmin=0.05) {
      for (const HEPUtils::Particle* p : e.visible_particles())
        if (p->pT() > ptmin && HEPUtils::in_range(HEPUtils::deltaR_eta(p4, *p), rmin, rmax)) return true;
      for (const HEPUtils::Jet* j : e.jets())
        if (j->pT() > ptmin && HEPUtils::in_range(HEPUtils::deltaR_eta(p4, *j), rmin, rmax)) return true;
      return false;
    }


    /// Non-iterator version of std::all_of
    template <typename CONTAINER, typename FN>
    inline bool all_of(const CONTAINER& c, const FN& f) {
      return std::all_of(std::begin(c), std::end(c), f);
    }

    /// Non-iterator version of std::any_of
    template <typename CONTAINER, typename FN>
    inline bool any_of(const CONTAINER& c, const FN& f) {
      return std::any_of(std::begin(c), std::end(c), f);
    }

    /// Non-iterator version of std::none_of
    template <typename CONTAINER, typename FN>
    inline bool none_of(const CONTAINER& c, const FN& f) {
      return std::none_of(std::begin(c), std::end(c), f);
    }



  }
}
