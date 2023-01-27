#ifndef __loaded_types_Rivet_3_1_5_hpp__
#define __loaded_types_Rivet_3_1_5_hpp__ 1

#ifndef EXCLUDE_YODA
#ifndef EXCLUDE_HEPMC

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "wrapper_AnalysisHandler.hh"
#include "identification.hpp"
#pragma GCC diagnostic pop

// Indicate which types are provided by this backend, and what the symbols of their factories are.
#define Rivet_3_1_5_all_data \
  (( /*class*/(Rivet)(AnalysisHandler),    /*constructors*/(("Factory_AnalysisHandler_0__BOSS_1",(const std::string&))) (("Factory_AnalysisHandler_1__BOSS_2",())) )) \

// If the default version has been loaded, set it as default.
#if ALREADY_LOADED(CAT_3(BACKENDNAME,_,CAT(Default_,BACKENDNAME)))
  SET_DEFAULT_VERSION_FOR_LOADING_TYPES(BACKENDNAME,SAFE_VERSION,CAT(Default_,BACKENDNAME))
#endif

// Undefine macros to avoid conflict with other backends.
#include "gambit/Backends/backend_undefs.hpp"
 
#endif
#endif

#endif /* __loaded_types_Rivet_3_1_5_hpp__ */
