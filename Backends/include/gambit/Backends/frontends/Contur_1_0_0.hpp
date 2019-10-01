//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Fronted header for the Contur backend
///
///  Compile-time registration of available
///  functions and variables from this backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2019 Oct
///
///  *********************************************

#define BACKENDNAME Contur
#define BACKENDLANG Python2
#define VERSION 1.0.0
#define SAFE_VERSION 1_0_0

LOAD_LIBRARY


// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
