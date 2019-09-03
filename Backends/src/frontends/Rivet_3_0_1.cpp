//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend for Rivet backend
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
/// \author Tomas Gonzalo
///         (tomas.gonzalo@monash.edu)
/// \date 2019 July
///
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/Rivet_3_0_1.hpp"

// Initialisation
BE_INI_FUNCTION
{

  std::cout << "Running Rivet" << std::endl;

}
END_BE_INI_FUNCTION

// Convenience functions (definitions)
BE_NAMESPACE
{

  double get_SM_measurements()
  {
    return 1.0;
  }

}
END_BE_NAMESPACE
