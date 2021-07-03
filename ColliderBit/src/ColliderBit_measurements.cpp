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

//#define COLLIDERBIT_DEBUG

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

            // Rivet is reading from file here, so make it critical
            # pragma omp critical
            {
              // Add the list to theAnalaysisHandler
              for (auto analysis : analyses)
                ah->addAnalysis(analysis);
            }

            //Write the utilised analyses to a file
            //This will list only the analyses that RIVET has succesfully loaded.
            //Only do this the first time.
            # pragma omp critical
            {
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

      // GAMBIT version
      void LHC_measurements_LogLike(double &result)
      {
        using namespace Pipes::LHC_measurements_LogLike;

        // Get YODA analysis objects from Rivet
        //vector_shared_ptr<YODA::AnalysisObject> aos = *Dep::Rivet_measurements;

        // Compute the likelihood
        // TODO: come up with a homebrew computation of the likelihood based on this
        result = 0.0;

      }

      // Contur version
      void Contur_LHC_measurements_LogLike(double &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike;

        // Get YODA analysis objects from Rivet
        vector_shared_ptr<YODA::AnalysisObject> aos = *Dep::Rivet_measurements;

        // Call Contur
        result = BEreq::Contur_LogLike(aos);

      }

      // Contur version, from YODA stream
      void Contur_LHC_measurements_LogLike_from_stream(double &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike_from_stream;
  
        std::shared_ptr<std::ostringstream> yodastream = *Dep::Rivet_measurements;
        
        //Note the yaml options should be in the DEST form, not the commandline form, e.g.:
        //"USESEARCHES", not "--usesearches" or "-s".
        std::vector<std::string> yaml_contur_options = runOptions->getValueOrDef<std::vector<str>>(std::vector<str>(), "contur_options");

        //TODO: Check on Rivet/Contur thread safety
        #pragma omp critical
        {
          ///Call contur
          result = BEreq::Contur_LogLike(std::move(yodastream), yaml_contur_options);
        } 

        std::cout << "Contur loglike = " << result << std::endl;
      }

      // Contur version, from YODA file
      void Contur_LHC_measurements_LogLike_from_file(double &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike_from_file;

        // This function only works if there is a file
        str YODA_filename = runOptions->getValueOrDef<str>("", "YODA_filename");

        if (YODA_filename == "" or not Utils::file_exists(YODA_filename))
          ColliderBit_error().raise(LOCAL_INFO, "YODA file "+YODA_filename+" not found.");

        //Note the yaml options should be in the DEST form, not the commandline form, e.g.:
        //"USESEARCHES", not "--usesearches" or "-s".
        std::vector<std::string> yaml_contur_options = runOptions->getValueOrDef<std::vector<str>>(std::vector<str>(), "contur_options");

        #pragma omp critical
        {
          // Call Contur
          result = BEreq::Contur_LogLike(YODA_filename, yaml_contur_options);
        } 

        std::cout << "Contur loglike = " << result << std::endl;

      }

    #endif //EXCLUDE_YODA

  }  // namespace ColliderBit
}  // namespace Gambit
