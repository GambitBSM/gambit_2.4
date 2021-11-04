//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Example frontend header for the ATLAS_FullLikes 
///  backend
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Chris Chang
///          (christopher.chang@uqconnect.edu.au)
///  \date 2021
///
///  *********************************************


#define BACKENDNAME ATLAS_FullLikes
#define BACKENDLANG Python3
#define VERSION 1.0
#define SAFE_VERSION 1_0

LOAD_LIBRARY

#ifdef HAVE_PYBIND11

  BE_FUNCTION(FullLikeBackend, double, (PyDict&,const str&), "FullLikeBackend", "ATLAS_FullLikeBackend")
  BE_FUNCTION(FullLikeRead, int, (str&), "ReadJsonFiles", "ATLAS_ReadJsonFiles")


#endif

#include "gambit/Backends/backend_undefs.hpp"

