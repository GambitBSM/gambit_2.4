#!/bin/python
#
# GAMBIT: Global and Modular BSM Inference Tool
#*********************************************
# \file
#
#  Module and functor type harvesting script
#  Generates all_functor_types.hpp and 
#  module_rollcall.hpp.
#  
#  This script identifies then reads through 
#  all the module rollcall and frontend headers, 
#  and harvests the types of every functor that 
#  GAMBIT will try to compile. These are needed
#  to generate the default (virtual) 
#  'print' functions in the base printer class
#  (see Printers/include/baseprinter.hpp).
#  It also finds all the module type headers
#  and includes them in modult_types_rollcall.hpp.  
#
#*********************************************
#
#  Authors (add name and date if you modify):
#   
#  \author Ben Farmer 
#          (ben.farmer@gmail.com)
#    \date 2013 Sep, 2014 Jan 
#
#  \author Pat Scott 
#          (patscott@physics.mcgill.ca)
#    \date 2013 Oct, Nov
#    \date 2014 Jan, Nov
#
#*********************************************
from harvesting_tools import *

def main(argv):

    # Lists of modules to exclude; anything starting with one of these strings is excluded.
    exclude_modules=set([])

    # Handle command line options
    verbose = False
    try:
        opts, args = getopt.getopt(argv,"vx:",["verbose","exclude-modules="])
    except getopt.GetoptError:
        print 'Usage: module_harvestor.py [flags]'
        print ' flags:'
        print '        -v                   : More verbose output'  
        print '        -x model1,model2,... : Exclude model1, model2, etc.' 
        sys.exit(2)
    for opt, arg in opts:
      if opt in ('-v','--verbose'):
        verbose = True
        print 'module_harvester.py: verbose=True'
      elif opt in ('-x','--exclude-modules'):
        exclude_modules.update(neatsplit(",",arg))
    exclude_header = exclude_modules
    module_type_headers=set([])
    full_rollcall_headers=[]
    full_type_headers=[]

    # List of headers to search
    rollcall_headers = set(["backend_rollcall.hpp"])
    type_headers     = set(["types_rollcall.hpp"])

    # List of headers NOT to search (things we know are not module rollcall headers or module type headers, 
    # but are included in module_rollcall.hpp or types_rollcall.hpp)
    exclude_header.update(["shared_types.hpp", "backend_macros.hpp", "backend_undefs.hpp", "identification.hpp",
                           "yaml.h"])

    # List of types not to bother looking for the definitions of.
    intrinsic_types=set(["char", "bool", "short", "int", "long", "float", "double", "std::string"])

    # List of types NOT to return (things we know are not printable, but can appear in START_FUNCTION calls)
    exclude_types=set(["void"])

    # Get list of rollcall header files to search
    rollcall_headers.update(retrieve_rollcall_headers(verbose,".",exclude_header))
    # Get list of module type header files to search
    module_type_headers.update(retrieve_module_type_headers(verbose,".",exclude_header))

    print "Module rollcall headers identified:"
    for h in rollcall_headers:
        print ' ',h
    print "Module type headers identified:"
    for h in module_type_headers:
        print ' ',h
    if verbose: print

    # Generate a c++ header containing all the model type headers we have just harvested.
    towrite = "\
//   GAMBIT: Global and Modular BSM Inference Tool\n\
//   *********************************************\n\
///  \\file                                       \n\
///                                               \n\
///  Compile-time registration of GAMBIT module   \n\
///  types.                                       \n\
///                                               \n\
///  This file was automatically generated by     \n\
///  module_harvester.py. Do not modify.          \n\
///                                               \n\
///  Do not add to this if you want to add new    \n\
///  types associated with a new (or old) module. \n\
///  Just add your model type header as           \n\
///  XBit/include/XBit_types.hpp and rest assured \n\
///  that module_harvester.py will make sure it   \n\
///  ends up here.                                \n\
///                                               \n\
///  *********************************************\n\
///                                               \n\
///  Authors:                                     \n\
///                                               \n\
///  \\author The GAMBIT Collaboration            \n\
///  \date "+datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")+"\n\
///                                               \n\
///  *********************************************\n\
                                                  \n\
#ifndef __module_types_rollcall_hpp__             \n\
#define __module_types_rollcall_hpp__             \n\
                                                  \n\
// Automatically-generated list of module types.  \n"

    for h in module_type_headers:
        towrite+='#include \"{0}\"\n'.format(h)
    towrite+="\n#endif // defined __module_types_rollcall_hpp__\n"
    
    with open("./Utils/include/module_types_rollcall.hpp","w") as f:
        f.write(towrite)

    print "Harvesting types from headers..."

    # Recurse through chosen rollcall headers, locating all the included headers therein, and find them all 
    # in the gambit source tree so that we can parse them for types etc.   
    if verbose: print "  Searching rollcall headers..."
    find_and_harvest_headers(rollcall_headers,full_rollcall_headers,exclude_header,verbose=verbose)
    if verbose: print "  Searching type headers..."
    find_and_harvest_headers(type_headers,full_type_headers,exclude_header,verbose=verbose)
     
    # Search through rollcall headers and look for macro calls that create module_functors or safe pointers to them 
    types=set(["ModelParameters"]) #Manually add this one to avoid scanning through Models directory
    for header in full_rollcall_headers:
        with open(header) as f:
            if verbose: print "  Scanning header {0} for types used to instantiate functor class templates".format(header)
            module = ""
            for line in readlines_nocomments(f):
                # If this line defines the module name, update it.
                module = update_module(line,module)
                # Check for calls to functor creation macros, and harvest the types used.
                addiffunctormacro(line,module,types,full_type_headers,intrinsic_types,exclude_types,verbose=verbose)
        
    print "Found:"
    for t in types:
        print ' ',t
    
    # Generate a c++ header containing the preprocessor sequence needed by Utils/include/printers.hpp, containing all the types we have harvested.
    towrite = "\
//   GAMBIT: Global and Modular BSM Inference Tool\n\
//   *********************************************\n\
///  \\file                                       \n\
///                                               \n\
///  Automatically generated preprocessor sequence\n\
///  of functor types.                            \n\
///                                               \n\
///  This file was automatically generated by     \n\
///  module_harvester.py. Do not modify.          \n\
///  The content is harvested from the rollcall   \n\
///  headers registered in module_rollcall.hpp    \n\
///  and the types registered in                  \n\
///  types_rollcall.hpp.                          \n\
///                                               \n\
///  *********************************************\n\
///                                               \n\
///  Authors:                                     \n\
///                                               \n\
///  \\author The GAMBIT Collaboration            \n\
///  \date "+datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")+"\n\
///                                               \n\
///  *********************************************\n\
                                                  \n\
#ifndef __all_functor_types_hpp__                 \n\
#define __all_functor_types_hpp__                 \n\
                                                  \n\
#include \"types_rollcall.hpp\"                   \n\
                                                  \n\
// Automatically generated preprocessor sequence of types \n\
#define PRINTABLE_TYPES "
    for t in types:
        towrite+='({0})'.format(t)
    towrite+="\n\n#endif // defined __all_functor_types_hpp__\n"
    
    with open("./Utils/include/all_functor_types.hpp","w") as f:
        f.write(towrite)

    if verbose:
        print "\nGenerated module_rollcall.hpp." 
        print "Generated module_types_rollcall.hpp." 
        print "Generated all_functor_types.hpp.\n" 


# Handle command line arguments (verbosity)
if __name__ == "__main__":
   main(sys.argv[1:])

