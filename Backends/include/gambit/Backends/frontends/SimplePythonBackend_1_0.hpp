#define BACKENDNAME SimplePythonBackend
#define BACKENDLANG Python
#define VERSION 1.0
#define SAFE_VERSION 1_0
#define REFERENCE DarkRayNet:2021


LOAD_LIBRARY

BE_CONV_FUNCTION(c_add,double,(double&, double&),"python_add")

BE_CONV_FUNCTION(c_print_str,void,(std::string&, double&),"python_print_string")

BE_CONV_FUNCTION(c_print_double,void,(double&, double&),"python_print_double")

#include "gambit/Backends/backend_undefs.hpp"
