//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  AnalysisLogLikes struct.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2021 Jun
///
///  *********************************************

#pragma once

#include <string>
#include <vector>

// #define ANALYSISLOGLIKES_DEBUG

#ifdef ANALYSISLOGLIKES_DEBUG
#include <iostream>
#endif


namespace Gambit {
  namespace ColliderBit {

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
      double combination_expected_loglike;  // Might be interesting to include this in the future

      AnalysisLogLikes() :
        combination_sr_label("undefined"),
        combination_sr_index(-2),
        combination_loglike(0.0),
        combination_expected_loglike(0.0)
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
        combination_expected_loglike = 0.0;
      }

      void set_no_signal_result_all_SRs(std::string combination_sr_label_in, int combination_sr_index_in)
      {
        // Fill the variables for the combined result
        set_no_signal_result_combination(combination_sr_label_in, combination_sr_index_in);

        // Fill the vectors of per-SR results
        std::fill(sr_loglikes.begin(), sr_loglikes.end(), 0);
        std::fill(sr_expected_loglikes.begin(), sr_expected_loglikes.end(), 0);
      }

    };

  }
}
