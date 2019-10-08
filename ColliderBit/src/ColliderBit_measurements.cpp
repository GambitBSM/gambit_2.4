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
///  \date   2019 July Feb
///
///  *********************************************

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/ColliderBit/ColliderBit_rollcall.hpp"
#include "gambit/ColliderBit/Utils.hpp"

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

        // Get measurments from Rivet
        void Rivet_measurements(vector_shared_ptr<YODA::AnalysisObject> &result)
        {
          using namespace Pipes::Rivet_measurements;
          using namespace Rivet_default::Rivet;

          // Make sure this is single thread only (assuming Rivet is not thread-safe)
          # pragma omp critical
          {

            // Analysis handler
            AnalysisHandler ah;

            // TODO: this is temporary cause it does not work without it
            ah.setIgnoreBeams(true);

            // Get analysis list from yaml file
            std::vector<str> analyses = runOptions->getValueOrDef<std::vector<str> >(std::vector<str>(), "analyses");

            if(not analyses.size())
              ColliderBit_warning().raise(LOCAL_INFO, "No analyses set for Rivet");
            // TODO: Add somewhere a check to make sure we only do LHC analyses

            // Add the list to the AnalaysisHandler
            for (auto analysis : analyses)
              ah.addAnalysis(analysis);

            // Get the filename and initialise the HepMC reader
            const static str HepMC_filename = runOptions->getValueOrDef<str>("", "hepmc_filename");
            static int HepMC_file_version = -1;

            static bool first = true;
            if (first)
            {
              if (not Utils::file_exists(HepMC_filename)) throw std::runtime_error("HepMC event file "+HepMC_filename+" not found. Quitting...");

              // Figure out if the file is HepMC2 or HepMC3
              std::ifstream infile(HepMC_filename);
              if (infile.good())
              {
                std::string line;
                while(std::getline(infile, line))
                {
                  // Skip blank lines
                  if(line == "") continue;

                  // We look for "HepMC::Version 2" or "HepMC::Version 3", 
                  // so we only need the first 16 characters of the line
                  std::string short_line = line.substr(0,16);

                  if (short_line == "HepMC::Version 2")
                  {
                    HepMC_file_version = 2;
                    break;
                  }
                  else if (short_line == "HepMC::Version 3")
                  {
                    HepMC_file_version = 3;
                   break;
                  }
                  else
                  {
                    throw std::runtime_error("Could not determine HepMC version from the string '"+short_line+"' extracted from the line '"+line+"'. Quitting...");
                  }
                }
              }
              first = false;
            }

            if(HepMC_file_version != 2 and HepMC_file_version != 3)
            {
              throw std::runtime_error("Failed to determine HepMC version for input file "+HepMC_filename+". Quitting...");
            }

            static HepMC3::Reader *HepMCio;

            if (HepMC_file_version == 2)
              HepMCio =  new HepMC3::ReaderAsciiHepMC2(HepMC_filename);
            else
              HepMCio =  new HepMC3::ReaderAscii(HepMC_filename);

            // Attempt to read the next HepMC event as a HEPUtils event. If there are no more events, wrap up the loop and skip the rest of this iteration.
            HepMC3::GenEvent ge(HepMC3::Units::GEV, HepMC3::Units::MM);
            while(HepMCio->read_event(ge) and !HepMCio->failed())
            {
              try { ah.analyze(ge); }
              catch(std::runtime_error &e)
              {
                ColliderBit_error().raise(LOCAL_INFO, e.what());
              }
            }

            delete HepMCio;

            ah.finalize();

            // Get YODA object
            ah.writeData(result);

            // Drop YODA file if requested
            bool drop_YODA_file = runOptions->getValueOrDef<bool>(false, "drop_YODA_file");
            if(drop_YODA_file)
            {
              str filename = "GAMBIT_collider_measurements.yoda";
              try{ YODA::write(filename, result.begin(), result.end()); }
              catch (...)
              { ColliderBit_error().raise(LOCAL_INFO, "Unexpected error in writing YODA file"); }
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
        vector_shared_ptr<YODA::AnalysisObject> aos = *Dep::Rivet_measurements;

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
        //result = BEreq::Contur_LogLike(aos);

      }

      // Contur version, from YODA file
      void Contur_LHC_measurements_LogLike_from_file(double &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike_from_file;

        // This function only works if there is a file
        str YODA_filename = runOptions->getValueOrDef<str>("", "YODA_filename");

        if (YODA_filename == "" or not Utils::file_exists(YODA_filename))
          ColliderBit_error().raise(LOCAL_INFO, "YODA file "+YODA_filename+" not found.");

        // Call Contur
        //result = BEreq::Contur_LogLike(YODA_filename);

      }

    #endif //EXCLUDE_YODA

  }  // namespace ColliderBit
}  // namespace Gambit
