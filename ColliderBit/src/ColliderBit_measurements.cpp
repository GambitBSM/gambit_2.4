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
#include "gambit/ColliderBit/ColliderBit_measurements_rollcall.hpp"
#include "HepMC3/ReaderAsciiHepMC2.h"

//#define COLLIDERBIT_DEBUG

namespace Gambit
{

  namespace ColliderBit
  {

    #ifndef EXCLUDE_HEPMC

      // Get measurments from Rivet
      void Rivet_measurements(double &result)
      {
        using namespace Pipes::Rivet_measurements;
        using namespace Rivet_default::Rivet;

        // Make sure this is single thread only
        # pragma omp critical
        {

          // Analysis handler
          AnalysisHandler ah;

          // Get analysis list from yaml file
          std::vector<str> analyses = runOptions->getValueOrDef<std::vector<str> >(std::vector<str>(), "analyses");

          for(auto analysis : analyses) std::cout << analysis <<std::endl;
 
          // Add the list to the AnalaysisHandler
          for (auto analysis : analyses)
            ah.addAnalysis(analysis);

          // Read HepMC
          std::string hepmc_filename = runOptions->getValueOrDef<std::string>("", "hepmc_file");
          static HepMC3::ReaderAsciiHepMC2 hepmcio(hepmc_filename);
          HepMC3::GenEvent *ge = new HepMC3::GenEvent(HepMC3::Units::GEV, HepMC3::Units::MM);

          int i=0;
          while(hepmcio.read_event(*ge) and !hepmcio.failed())
          {
            ah.analyze(*ge);
            ge = new HepMC3::GenEvent(HepMC3::Units::GEV, HepMC3::Units::MM);
            i++;
          }
          delete ge;
          
        }
      }

    #endif

  }  // namespace ColliderBit
}  // namespace Gambit
