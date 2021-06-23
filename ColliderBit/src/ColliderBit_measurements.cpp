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

#define COLLIDERBIT_DEBUG

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
          
          static AnalysisHandler ah;

          if (*Loop::iteration == BASE_INIT)
          {

            // TODO: this is temporary cause it does not work without it
            //ah.setIgnoreBeams(true);

            // TODO: Cross section?

            // Get analysis list from yaml file
            std::vector<str> analyses = runOptions->getValueOrDef<std::vector<str> >(std::vector<str>(), "analyses");
            #ifdef COLLIDERBIT_DEBUG
              std::cout<< "Rivet analyses: ";
              for(auto analysis: analyses) std::cout << analysis << ",";
              std::cout << endl;
            #endif

            if(not analyses.size())
              ColliderBit_warning().raise(LOCAL_INFO, "No analyses set for Rivet");
            // TODO: Add somewhere a check to make sure we only do LHC analyses

            // Rivet is reading from file here, so make it critical
            # pragma omp critical
            {
              // Add the list to theAnalaysisHandler
              for (auto analysis : analyses)
                ah.addAnalysis(analysis);
            }
          }

          if (*Loop::iteration == BASE_FINALIZE)
          {
            ah.finalize();

            if (result == nullptr){
              result = std::make_shared<std::ostringstream>();
            }

            // Get YODA object
            ah.writeData(*result, "yoda");

            // Drop YODA file if requested
            bool drop_YODA_file = runOptions->getValueOrDef<bool>(false, "drop_YODA_file");

            if(drop_YODA_file)
            {
              str filename = "GAMBIT_collider_measurements.yoda";
              
              try{ ah.writeData(filename); }
              catch (...)
              { ColliderBit_error().raise(LOCAL_INFO, "Unexpected error in writing YODA file"); }
            }
          }

          // Don't do anything else during special iterations
          if (*Loop::iteration < 0) return;

          // Make sure this is single thread only (assuming Rivet is not thread-safe)
          # pragma omp critical
          {
            // Get the HepMC event
            HepMC3::GenEvent ge = *Dep::HardScatteringEvent;

            try { ah.analyze(ge); }
            catch(std::runtime_error &e)
            {
              ColliderBit_error().raise(LOCAL_INFO, e.what());
            }
          }
        }

        // Analyse HepMC events with Rivet's measurements 
        // Collect results in a vector of YODA Analysis objects
        // void Rivet_measurements(vector_shared_ptr<YODA::AnalysisObject> &result)
        // {
        //   using namespace Pipes::Rivet_measurements;
        //   using namespace Rivet_default::Rivet;

        //   static AnalysisHandler ah;

        //   if (*Loop::iteration == BASE_INIT)
        //   {

        //     // TODO: this is temporary cause it does not work without it
        //     ah.setIgnoreBeams(true);

        //     // TODO: Cross section?

        //     // Get analysis list from yaml file
        //     std::vector<str> analyses = runOptions->getValueOrDef<std::vector<str> >(std::vector<str>(), "analyses");
        //     #ifdef COLLIDERBIT_DEBUG
        //       std::cout<< "Rivet analyses: ";
        //       for(auto analysis: analyses) std::cout << analysis << ",";
        //       std::cout << endl;
        //     #endif

        //     if(not analyses.size())
        //       ColliderBit_warning().raise(LOCAL_INFO, "No analyses set for Rivet");
        //     // TODO: Add somewhere a check to make sure we only do LHC analyses

        //     // Rivet is reading from file here, so make it critical
        //     # pragma omp critical
        //     {
        //       // Add the list to the AnalaysisHandler
        //       for (auto analysis : analyses)
        //         ah.addAnalysis(analysis);
        //     }

        //   }

        //   if (*Loop::iteration == BASE_FINALIZE)
        //   {
        //     ah.finalize();

        //     //Write the yoda file to the result stream
        //     ah.writeData(result);

        //     // Drop YODA file if requested
        //     bool drop_YODA_file = runOptions->getValueOrDef<bool>(false, "drop_YODA_file");
        //     if(drop_YODA_file)
        //     {
        //       str filename = "GAMBIT_collider_measurements.yoda";

        //       try{ YODA::write(filename, result.begin(), result.end()); }
        //       catch (...)
        //       { ColliderBit_error().raise(LOCAL_INFO, "Unexpected error in writing YODA file"); }
        //     }
        //   }

        //   // Don't do anything else during special iterations
        //   if (*Loop::iteration < 0) return;

        //   // Make sure this is single thread only (assuming Rivet is not thread-safe)
        //   # pragma omp critical
        //   {

        //     // Get the HepMC event
        //     HepMC3::GenEvent ge = *Dep::HardScatteringEvent;

        //     try { ah.analyze(ge); }
        //     catch(std::runtime_error &e)
        //     {
        //       ColliderBit_error().raise(LOCAL_INFO, e.what());
        //     }
        //   }

        // }
      
      
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
        std::cout << __FILE__ << "; " << __LINE__ << "\n" << std::flush;

        
        std::shared_ptr<std::ostringstream> yodastream = *Dep::Rivet_measurements;


        std::cout << "Yodastream is pointing to: " << yodastream << "\n" << std::flush;

        //TODO: Check on Rivet/Contur thread safety
        #pragma omp critical
        {
          ///Call contur
          result = BEreq::Contur_LogLike(std::move(yodastream));
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

        #pragma omp critical
        {
          // Call Contur
          result = BEreq::Contur_LogLike(YODA_filename);
        } 

        std::cout << "Contur loglike = " << result << std::endl;

      }

    #endif //EXCLUDE_YODA

  }  // namespace ColliderBit
}  // namespace Gambit
