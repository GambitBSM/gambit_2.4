//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Type definition header for module
///  ColliderBit.
///
///  Compile-time registration of type definitions
///  required for the rest of the code to
///  communicate with ColliderBit.
///
///  Add to this if you want to define a new type
///  for the functions in ColliderBit to return,
///  but you don't expect that type to be needed
///  by any other modules.
///
///  *********************************************
///
///  Authors (add name if you modify):
///
///  \author Abram Krislock
///          (a.m.b.krislock@fys.uio.no)
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2018 Jan
///  \date 2019 Jan, May
///
///  \author Tomas Gonzalo
///          (t.e.gonzalo@fys.uio.no)
///  \date 2018 Feb
///
///  *********************************************


#ifndef __ColliderBit_types_hpp__
#define __ColliderBit_types_hpp__

#include <vector>
#include <chrono>

#include "gambit/Utils/PID_pair.hpp"
#include "gambit/ColliderBit/MCLoopInfo.hpp"
#include "gambit/ColliderBit/MC_convergence.hpp"
#include "gambit/ColliderBit/xsec.hpp"
#include "gambit/ColliderBit/colliders/Pythia8/Py8Collider.hpp"
#include "gambit/ColliderBit/colliders/Pythia8/Py8Collider_typedefs.hpp"
#include "gambit/ColliderBit/detectors/BuckFast.hpp"
#include "gambit/ColliderBit/analyses/AnalysisContainer.hpp"
#include "gambit/ColliderBit/analyses/AnalysisData.hpp"

#include "gambit/ColliderBit/limits/ALEPHSleptonLimits.hpp"
#include "gambit/ColliderBit/limits/L3GauginoLimits.hpp"
#include "gambit/ColliderBit/limits/L3SleptonLimits.hpp"
#include "gambit/ColliderBit/limits/OPALGauginoLimits.hpp"
#include "gambit/ColliderBit/limits/OPALDegenerateCharginoLimits.hpp"
/// TODO: see if we can use this one:
//#include "gambit/ColliderBit/limits/L3SmallDeltaMGauginoLimits.hpp"

#include "HEPUtils/Event.h"

namespace Gambit
{

  namespace ColliderBit
  {

    /// @brief Container for data from multiple analyses and SRs
    typedef std::vector<AnalysisData> AnalysisNumbers;
    typedef std::vector<const AnalysisData*> AnalysisDataPointers;

    /// @brief Container for loglike information for an analysis
    struct AnalysisLogLikes
    {
      // Signal region labels
      std::vector<std::string> sr_labels;  
      // The "observed" log-likelihood
      std::vector<double> sr_loglikes;
      // The *expected* log-likelihoods, under the assumption that data (n) 
      // will equal background-only expectation (b). These are the log-likelihoods 
      // used to pick the most sensitive SR when we're forced to only use a single SR.
      std::vector<double> sr_expected_loglikes;  

      std::string combination_sr_label;
      int combination_sr_index;
      double combination_loglike;
      // double combination_expected_loglike;  // Might be interesting to include this in the future

      AnalysisLogLikes() :
        combination_sr_label("undefined"),
        combination_sr_index(-2),
        combination_loglike(0.0)
        // combination_expected_loglike(0.0)
        { }

      // Get the number of SRs and the correponding SR labels from 
      // an AnalysisData instance. Use this to initialize.
      void initialize(const AnalysisData& adata_in)
      {
        for (size_t sr_index = 0; sr_index < adata_in.size(); ++sr_index)
        {
          const std::string& sr_label = adata_in.srdata[sr_index].sr_label;
          sr_labels.push_back(sr_label);
          sr_loglikes.push_back(0.0);
          sr_expected_loglikes.push_back(0.0);
        }
      }

      void set_no_signal_result_combination(std::string combination_sr_label_in, int combination_sr_index_in)
      {
        // Fill the variables for the combined result
        combination_sr_index = combination_sr_index_in;
        combination_sr_label = combination_sr_label_in;
        combination_loglike = 0.0;
        // combination_expected_loglike = 0.0;
      }

      void set_no_signal_result_all_SRs(std::string combination_sr_label_in, int combination_sr_index_in) //, const std::vector<std::string>& sr_labels_in)
      {
        // Fill the variables for the combined result
        set_no_signal_result_combination(combination_sr_label_in, combination_sr_index_in);

        // Fill the vectors of per-SR results
        std::fill(sr_loglikes.begin(), sr_loglikes.end(), 0);
        std::fill(sr_expected_loglikes.begin(), sr_expected_loglikes.end(), 0);
      }

    };

    /// @brief Typedef for a string-to-AnalysisLogLikes map
    typedef std::map<std::string,AnalysisLogLikes> map_str_AnalysisLogLikes;

    /// @brief Container for multiple analysis containers
    typedef std::vector<AnalysisContainer> AnalysisContainers;

    typedef std::chrono::milliseconds ms;
    typedef std::chrono::steady_clock steady_clock;
    typedef std::chrono::steady_clock::time_point tp;
    typedef std::map<std::string,double> timer_map_type;

    /// @brief Typedef for a str-SLHAstruct pair, to pass around SLHA filenames + content
    typedef std::pair<std::string,SLHAstruct> pair_str_SLHAstruct;

    /// @brief Typedefs related to cross-sections 
    // Note: 
    // The 'iipair' name below refers to the typedef std::pair<int,int> iipair 
    // declared in Utils/include/gambit/Utils/util_types.hpp

    typedef std::vector<std::pair<int,int>> vec_iipair;
    typedef std::vector<PID_pair> vec_PID_pair;

    typedef std::multimap<int,std::pair<int,int>> multimap_int_iipair;
    typedef std::multimap<int,PID_pair> multimap_int_PID_pair;

    typedef std::multimap<PID_pair,int> multimap_PID_pair_int;

    typedef std::map<int,xsec_container> map_int_xsec;
    typedef std::map<int,process_xsec_container> map_int_process_xsec;

    typedef std::map<std::pair<int,int>,PID_pair_xsec_container> map_iipair_PID_pair_xsec;
    typedef std::map<PID_pair,PID_pair_xsec_container> map_PID_pair_PID_pair_xsec;

    /// @brief Typedef for a std::function that sets the weight for the input HEPUtils::Event
    typedef HEPUtils::Event HEPUtils_Event;  // Extra typedef to avoid macro problem with namespaces
    typedef std::function<void(HEPUtils_Event&, const BaseCollider*)> EventWeighterFunctionType;
  }
}



#endif /* defined __ColliderBit_types_hpp__ */
