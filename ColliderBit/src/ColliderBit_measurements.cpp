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
#include "gambit/ColliderBit/MCLoopInfo.hpp"
#include "gambit/ColliderBit/ColliderBit_measurements_rollcall.hpp"

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

        // Analysis handler
        Rivet_default::Rivet::AnalysisHandler ah;

        // Get analysis list from yaml file
        std::vector<str> analyses = runOptions->getValueOrDef<std::vector<str> >(std::vector<str>(), "analyses");

        for(auto analysis : analyses) std::cout << analysis <<std::endl;
 
        // Add the list to the AnalaysisHandler
        //ah.addAnalysis(analyses);

        // Run Rivet
        Rivet_default::Rivet::Run rivet(ah);
      }

    #endif

  }  // namespace ColliderBit
}  // namespace Gambit
