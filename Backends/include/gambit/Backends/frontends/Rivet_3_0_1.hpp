//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend for Rivet backend v3.0.1
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomas Gonzalo
///  \date   2019 July
///
///  *********************************************

#define BACKENDNAME Rivet
#define BACKENDLANG CC
#define VERSION 3.0.1
#define SAFE_VERSION 3_0_1

LOAD_LIBRARY

BE_CONV_FUNCTION(get_SM_measurements, double, (), "Rivet_get_SM_measurements")

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"

