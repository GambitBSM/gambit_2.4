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

#ifndef EXCLUDE_HEPMC
  #include "HepMC3/ReaderAscii.h"
  #include "HepMC3/ReaderAsciiHepMC2.h"
#endif

#ifndef EXCLUDE_YODA
  #include "YODA/AnalysisObject.h"
#endif

//#define COLLIDERBIT_DEBUG

namespace Gambit
{

  namespace ColliderBit
  {

    #ifndef EXCLUDE_HEPMC
      #ifndef EXCLUDE_YODA

        // Get measurments from Rivet
        void Rivet_measurements(double &result)
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

            // Create YODA object
            std::vector<std::shared_ptr<YODA::AnalysisObject> > output;

            // First get all multiwight AOs
/*            vector<MultiweightAOPtr> raos = ah.getRivetAOs();
            output.reserve(raos.size()*2*ah.numWeights());

            // Fix the ordering so that default weight is written out first.
            vector<size_t> order;
            if ( ah.defaultWeightIndex() >= 0 && ah.defaultWeightIndex() < ah.numWeights() )
              order.push_back(ah.defaultWeightIndex());
            for ( size_t  i = 0; i < ah.numWeights(); ++i )
              if ( i != ah.defaultWeightIndex() ) order.push_back(i);

            // Then we go through all finalized AOs one weight at a time
            for (size_t iW : order )
            {
              for ( auto rao : raos )
              {
                rao.get()->ah.setActiveFinalWeightIdx(iW);
                if ( rao->path().find("/TMP/") != string::npos ) continue;
                output.push_back(rao.get()->activeYODAPtr());
              }
            }

            // Finally the RAW objects.
            for (size_t iW : order )
            {
              for ( auto rao : raos )
              {
                rao.get()->ah.setActiveWeightIdx(iW);
                output.push_back(rao.get()->activeYODAPtr());
              }
            }*/
          }
        }
      #endif
    #endif

  }  // namespace ColliderBit
}  // namespace Gambit
