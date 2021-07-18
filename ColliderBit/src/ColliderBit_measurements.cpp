//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of ColliderBit measurments.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date   2019
///
///  \author Tomasz Procter
///          (t.procter.1@research.gla.ac.uk)
///  \date   June 2021
///
///  *********************************************

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/ColliderBit/ColliderBit_rollcall.hpp"
#include "gambit/ColliderBit/Utils.hpp"
#include "gambit/ColliderBit/ColliderBit_eventloop.hpp"

#ifndef EXCLUDE_HEPMC
  #include "HepMC3/ReaderAscii.h"
  #include "HepMC3/ReaderAsciiHepMC2.h"
#endif

#ifndef EXCLUDE_YODA
  #include "YODA/AnalysisObject.h"
  #include "YODA/IO.h"
#endif

//Small convenience function for supplying options to the contur argparser.
void convert_yaml_options_for_contur(std::vector<std::string> &yaml_options)
{
  for (size_t i{0}; i <yaml_options.size(); ++i){
    yaml_options[i] = ("--"+yaml_options[i]);
  }
}

namespace Gambit
{

  namespace ColliderBit
  {

    #ifndef EXCLUDE_HEPMC
      #ifndef EXCLUDE_YODA

        // Analyse HepMC events with Rivet's measurements 
        // Collect results in a stream of a YODA file
        void Rivet_measurements(std::shared_ptr<std::ostringstream> &result)
        {
          using namespace Pipes::Rivet_measurements;
          using namespace Rivet_default::Rivet;
          
          static std::shared_ptr<AnalysisHandler> ah;

          if (*Loop::iteration == BASE_INIT)
          {
            if (ah != nullptr) {
              ah->~AnalysisHandler();  
            }
            ah = std::make_shared<AnalysisHandler>();

            // Get analysis list from yaml file
            std::vector<str> analyses = runOptions->getValueOrDef<std::vector<str> >(std::vector<str>(), "analyses");

            if(not analyses.size())
              ColliderBit_warning().raise(LOCAL_INFO, "No analyses set for Rivet");
            // TODO: Add somewhere a check to make sure we only do LHC analyses
            else{
              for (size_t i = 0; i < analyses.size() ; ++i){
                //If the analysis is a special code referring to multiple analyses,
                //append these to the end of the vector, so they are dealt with
                //later in the loop
                if (analyses[i] == "13TeV" || analyses[i] == "8TeV"){
                  BEreq::Contur_GetAnalyses(analyses, analyses[i]);
                }
                //If its a normal analyis just add it.
                else {
                  // Rivet is reading from file here, so make it critical
                  # pragma omp critical
                  {
                    ah->addAnalysis(analyses[i]);
                  }
                }
              }
            }

            //If the yaml file wants to exclude analyses, remove them
            //This feature was inspired by ATLAS_2016_I1469071, which is effectively
            //invalid for most BSM cases and can cause crashes.
            std::vector<str> excluded_analyses = runOptions->getValueOrDef<std::vector<str> >(std::vector<str>(), "exclude_analyses");
            # pragma omp critical
            {
              ah->removeAnalyses(excluded_analyses);
            }
            //Write the utilised analyses to a file in yaml-like format
            //This will list only the analyses that RIVET has succesfully loaded.
            # pragma omp critical
            {
              //Only do this the first time contur is run.
              static bool analyses_written_to_file = false;
              if (!analyses_written_to_file){
                std::ofstream analyses_output_file;
                //TODO please feel free to change name/put in more appropriate location.
                analyses_output_file.open(GAMBIT_DIR+std::string("/GAMBIT_rivet_analyses.log"));
                analyses_output_file << "analyses:";

                for (std::string an_analysis_string : ah->analysisNames()) {
                  analyses_output_file << "\n - " << an_analysis_string;
                }
                analyses_output_file.close();
                analyses_written_to_file = true;
              }
            }
          }
          //If we're not in an init loop, seg faults will occur if the analysis handler is null.
          //This should never happen if the loops happen in the right order, but just in case:
          else if (ah == nullptr){
            ColliderBit_error().raise(LOCAL_INFO, "No Analysis handler set. Is the Loop going in the right order?");
            //TODO: Is it possible special loop iterations could break this?
          }

          if (*Loop::iteration == BASE_FINALIZE)
          {
          #ifdef COLLIDERBIT_DEBUG
            std::cout << "Summary data from rivet:\n\tAnalyses used: ";
            for (auto analysis : ah.analysisNames()){
                std::cout << analysis << ", ";
            }
            std::cout << "\n\tBeam IDs are " << ah.beamIds().first << ", " << ah.beamIds().second;
            std::cout << "\n\tXS: " << ah.nominalCrossSection();
            std::cout << "\n\tRunName: " << ah.runName();
            std::cout << "\n\tSqrtS: " << ah.sqrtS();
            std::cout << "\n\tList of available analyses: ";
            for (auto analysis : ah.stdAnalysisNames()){
                std::cout << analysis << ", ";
            }
            std::cout << std::flush;
          #endif

            //Initialise somewhere for the yoda file to be outputted.
            //This circuitous route is necesarry because ostringstream does not
            //support copy assignment or copy initialisation, and which is 
            //necesarry to access items via Gambit's backends system, so we need
            //to go via a pointer. 
            if (result == nullptr){
              result = std::make_shared<std::ostringstream>();
            }

            ah->finalize();

            // Get YODA object
            ah->writeData(*result, "yoda");

            // Drop YODA file if requested
            bool drop_YODA_file = runOptions->getValueOrDef<bool>(false, "drop_YODA_file");
            if(drop_YODA_file)
            {
              str filename = "GAMBIT_collider_measurements.yoda";
              
              //TODO: should this also be critical?
              try{ ah->writeData(filename); }
              catch (...)
              { ColliderBit_error().raise(LOCAL_INFO, "Unexpected error in writing YODA file"); }
            }

            ah->~AnalysisHandler();
          }

          // Don't do anything else during special iterations
          if (*Loop::iteration < 0) return;

          // Make sure this is single thread only (assuming Rivet is not thread-safe)
          # pragma omp critical
          {
            // Get the HepMC event
            HepMC3::GenEvent ge = *Dep::HardScatteringEvent;

            try { ah->analyze(ge); }
            catch(std::runtime_error &e)
            {
              ColliderBit_error().raise(LOCAL_INFO, e.what());
            }
          }
        }
      
      #endif //EXCLUDE_YODA
    #endif // EXCLUDE_HEPMC

    #ifndef EXCLUDE_YODA

      // Contur version, from YODA stream
      void Contur_LHC_measurements_from_stream(pybind11::object &result)
      {
        using namespace Pipes::Contur_LHC_measurements_from_stream;
  
        std::shared_ptr<std::ostringstream> yodastream = *Dep::Rivet_measurements;
        
        std::vector<std::string> yaml_contur_options = runOptions->getValueOrDef<std::vector<str>>(std::vector<str>(), "contur_options");
        convert_yaml_options_for_contur(yaml_contur_options);

        #pragma omp critical
        {
          ///Call contur
          result = BEreq::Contur_Measurements(std::move(yodastream), yaml_contur_options);
        } 
      }

      // Contur version, from YODA file
      void Contur_LHC_measurements_from_file(pybind11::object &result)
      {
        using namespace Pipes::Contur_LHC_measurements_from_file;

        // This function only works if there is a file
        str YODA_filename = runOptions->getValueOrDef<str>("", "YODA_filename");

        if (YODA_filename == "" or not Utils::file_exists(YODA_filename))
          ColliderBit_error().raise(LOCAL_INFO, "YODA file "+YODA_filename+" not found.");

        std::vector<std::string> yaml_contur_options = runOptions->getValueOrDef<std::vector<str>>(std::vector<str>(), "contur_options");
        convert_yaml_options_for_contur(yaml_contur_options);

        #pragma omp critical
        {
          // Call Contur
          result = BEreq::Contur_Measurements(YODA_filename, yaml_contur_options);
        } 
      }

      void Contur_LHC_measurements_LogLike(double &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike;
        pybind11::object contur_likelihood_object = *Dep::LHC_measurements;

        result = contur_likelihood_object.attr("_full_likelihood").attr("ts_s_b").cast<double>()
          - contur_likelihood_object.attr("_full_likelihood").attr("ts_b").cast<double>();
      }

      void Contur_LHC_measurements_LogLike_perPool(map_str_dbl &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike_perPool;
        std::stringstream summary_line;

        pybind11::list contur_sorted_likelihoods = (*Dep::LHC_measurements).attr("sorted_likelihood_blocks");
        const size_t number_of_pools = pybind11::len(contur_sorted_likelihoods);
        for (size_t i = 0; i < number_of_pools; i++){
          pybind11::object individual_likelihood = contur_sorted_likelihoods[i];
          result[individual_likelihood.attr("pools").cast<std::string>()] = 
            individual_likelihood.attr("ts_s_b").cast<double>() - individual_likelihood.attr("ts_b").cast<double>();
        }

        //result = (contur_sorted_likelihoods[0].attr("pools")).cast<map_str_dbl>();
        summary_line << "LHC Contur LogLikes per pool: ";

        for( auto const& entry : result){
          summary_line << entry.first << ":" << entry.second << ", ";
        }

        logger() << LogTags::debug << summary_line.str() << EOM;
      }
    #endif //EXCLUDE_YODA

  }  // namespace ColliderBit
}  // namespace Gambit
