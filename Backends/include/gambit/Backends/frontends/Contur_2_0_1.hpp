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
///  \date 2019 Oct, 2020 Mar
///  \date 2021 Mar
///
///  *********************************************

#define BACKENDNAME Contur
#define BACKENDLANG Python2
#define VERSION 2.0.1
#define SAFE_VERSION 2_0_1

LOAD_LIBRARY

BE_CONV_FUNCTION(Contur_LogLike, double, (vector_shared_ptr<YODA::AnalysisObject> &), "Contur_LogLike")
BE_CONV_FUNCTION(Contur_LogLike_from_file, double, (str &), "Contur_LogLike")
// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
