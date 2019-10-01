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
            const static str HepMC2_filename = runOptions->getValueOrDef<str>("", "HepMC2_filename");
            static bool HepMC2_ON = (HepMC2_filename != "");
            const static str HepMC3_filename = runOptions->getValueOrDef<str>("", "HepMC3_filename");
            static bool HepMC3_ON = (HepMC3_filename != "");

            if (HepMC2_ON and HepMC3_ON)
              ColliderBit_error().raise(LOCAL_INFO, "Cannot read simultaneously from HepMC2 and HepMC3 files.");
            if (HepMC2_ON)
            {
              if (not Utils::file_exists(HepMC2_filename))
                ColliderBit_error().raise(LOCAL_INFO, "HepMC2 event file "+HepMC2_filename+" not found.");
              static HepMC3::ReaderAsciiHepMC2 hepmcio(HepMC2_filename);
              HepMC3::GenEvent *ge = new HepMC3::GenEvent(HepMC3::Units::GEV, HepMC3::Units::MM);

              int i=0;
              while(hepmcio.read_event(*ge) and !hepmcio.failed())
              {
                try { ah.analyze(*ge); }
                catch(std::runtime_error &e)
                {
                  ColliderBit_error().raise(LOCAL_INFO, e.what());
                }
                ge = new HepMC3::GenEvent(HepMC3::Units::GEV, HepMC3::Units::MM);
                i++;
              }
              delete ge;
 
            }
            else if (HepMC3_ON)
            {
              if (not Utils::file_exists(HepMC3_filename))
                ColliderBit_error().raise(LOCAL_INFO, "HepMC3 event file "+HepMC3_filename+" not found.");
              static HepMC3::ReaderAscii hepmcio(HepMC3_filename);
              HepMC3::GenEvent *ge = new HepMC3::GenEvent(HepMC3::Units::GEV, HepMC3::Units::MM);
  
              int i=0;
              while(hepmcio.read_event(*ge) and !hepmcio.failed())
              { 
                try { ah.analyze(*ge); }
                catch(std::runtime_error &e)
                {
                  ColliderBit_error().raise(LOCAL_INFO, e.what());
                }
                ge = new HepMC3::GenEvent(HepMC3::Units::GEV, HepMC3::Units::MM);
                i++;
              }
              delete ge;
 
            }
            else
              ColliderBit_error().raise(LOCAL_INFO, "Neither HepMC2 nor HepMC3 event file found.");

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
      void calc_LHC_measurements_LogLike(double &result)
      {
        using namespace Pipes::calc_LHC_measurements_LogLike;

        // Get YODA analysis objects from Rivet
        vector_shared_ptr<YODA::AnalysisObject> aos = *Dep::Rivet_measurements;

        // Compute the likelihood
        // TODO: come up with a homebrew computation of the likelihood based on this
        result = 0.0;

        std::cout << "LHC_measurements_LogLike" << std::endl;

      }

      // Contur version
      void calc_Contur_LHC_measurements_LogLike(double &result)
      {
        using namespace Pipes::calc_Contur_LHC_measurements_LogLike;

        // Get YODA analysis objects from Rivet
        vector_shared_ptr<YODA::AnalysisObject> aos = *Dep::Rivet_measurements;

        // Call Contur
        result = BEreq::Contur_LogLike();

      }

      // Contur version, from YODA file
      void calc_Contur_LHC_measurements_LogLike_from_file(double &result)
      {
        using namespace Pipes::calc_Contur_LHC_measurements_LogLike;

        // This function only works if there is a file
        str YODA_filename = runOptions->getValueOrDef<str>("", "YODA_filename");

        if (YODA_filename == "" or not Utils::file_exists(YODA_filename))
          ColliderBit_error().raise(LOCAL_INFO, "YODA file "+YODA_filename+" not found.");

        // Call Contur
        BEreq::Contur_LogLike();

      }

    #endif //EXCLUDE_YODA

  }  // namespace ColliderBit
}  // namespace Gambit
