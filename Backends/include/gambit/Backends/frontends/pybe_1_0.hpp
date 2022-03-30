#define BACKENDNAME pybe
#define BACKENDLANG Python
#define VERSION 1.0
#define SAFE_VERSION 1_0

LOAD_LIBRARY

BE_CONV_FUNCTION(c_add,double,(double&, double&),"add_here")

BE_CONV_FUNCTION(c_print_str,void,(std::string&, double&),"print_string")

BE_CONV_FUNCTION(c_print_num,void,(double&, double&),"print_num")

// BE_CONV_FUNCTION(c_print_process,void,(DarkBit::TH_Process&, double&),"print_process")

#include "gambit/Backends/backend_undefs.hpp"
