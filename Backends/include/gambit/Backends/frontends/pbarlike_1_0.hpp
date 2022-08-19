#define BACKENDNAME pbarlike
#define BACKENDLANG Python
#define VERSION 1.0
#define SAFE_VERSION 1_0
#define REFERENCE DarkRayNet:2021

LOAD_LIBRARY

#ifdef HAVE_PYBIND11
    // namespace Gambit::DarkBit { struct TH_ProcessCatalog; }
    BE_CONV_FUNCTION(c_pbarlike_initialization,pybind11::object,(const std::string& ,const std::string& ,mat_dbl& ), "pbarlike_initialization")
    BE_CONV_FUNCTION(c_pbar_logLikes,map_str_dbl,(double&,  map_str_dbl&, double& ), "drn_pbar_logLikes")
    // BE_CONV_FUNCTION(c_pbar_logLike_uncorr,double,(double&, double& ), "pbar_logLike_uncorr")
    // BE_CONV_FUNCTION(c_pbar_logLike_corr,double,(double&, double& ), "pbar_logLike_corr")
    // BE_INI_DEPENDENCY(WIMP_properties, WIMPprops)
    // BE_INI_DEPENDENCY(TH_ProcessCatalog, DarkBit::TH_ProcessCatalog)
    // BE_INI_DEPENDENCY(LocalHalo, LocalMaxwellianHalo)
    // BE_INI_DEPENDENCY(RD_fraction, double)
#endif

#include "gambit/Backends/backend_undefs.hpp"
