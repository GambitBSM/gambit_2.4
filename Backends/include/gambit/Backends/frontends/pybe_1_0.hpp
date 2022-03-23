#define BACKENDNAME pybe
#define BACKENDLANG Python
#define VERSION 1.0
#define SAFE_VERSION 1_0

LOAD_LIBRARY

BE_CONV_FUNCTION(c_add,double,(double&, double&),"add_here")

#include "gambit/Backends/backend_undefs.hpp"
