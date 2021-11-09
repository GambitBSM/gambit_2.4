//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend source for the rivet backend.
///
///  Actual implementation of rivet ini function.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomek Procter
///  \date 2021 July
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/Rivet_3_1_4.hpp"
#include "gambit/Utils/util_functions.hpp"


BE_INI_FUNCTION
{
  // Scan-level initialisation
  static bool scan_level = true;
  if (scan_level)
  {
    std::string rivet_analyses_dir = std::string(GAMBIT_DIR)+
        "/Backends/installed/rivet/3.1.4/analyses";

    //Check if the rivet_analyses_dir if it actually exists: if it 
    //doesn't then installation probably hasn't installed properly.
    if (Utils::file_exists(rivet_analyses_dir)){
      addAnalysisLibPath(rivet_analyses_dir);
    }
    else{
      std::cout << "\nRivet analysis directory does not exist." << std::endl;
      backend_error();
    }
  }
  scan_level = false;
}
END_BE_INI_FUNCTION