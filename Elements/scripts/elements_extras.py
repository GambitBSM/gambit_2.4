#!/usr/bin/env python
#
# GAMBIT: Global and Modular BSM Inference Tool
#*********************************************
# \file
#
#  Script to create files containing extra code
#  that should be generated at build time.
#  Usually to make sure the Printers are not used 
#  in the standalones.
#
#  Generated files:
#   Elements/elements_extras.hpp
#
#*********************************************
#
#  Authors (add name and date if you modify):
#
#  \author Christopher Chang
#          (christopher.chang@uqconnect.edu.au)
#  \date 2021 Feb
#
#  \author Tomas Gonzalo
#          (gonzalo@physik.rwth-aachen.de)
#  \date 2021 Mar
#
#*********************************************

import pickle,sys

toolsfile="./Utils/scripts/harvesting_tools.py"
exec(compile(open(toolsfile, "rb").read(), toolsfile, 'exec')) # Python 2/3 compatible version of 'execfile'

# Suspicious point exception, only using printers outside of standalones
def Suspicious_Points(isStandalone):

    contents = "\
  /// Gambit suspicious point exception class.\n\
  class Suspicious_point_exception\n\
  {\n\
\n\
    public:\n\
\n\
      /// Constructor\n\
      Suspicious_point_exception() {}\n\
\n\
      /// Raise the suspicious point exception. Print it with a message and a code. The default code is 1.\n\
      void raise(const std::string &msg, int code=1, bool debug=false)\n\
      {\n"

    if not isStandalone:
      contents += "\
        // get the printer pointer\n\
        Printers::BaseBasePrinter& printer = *(get_global_printer_manager()->printerptr);\n\
\n\
        printer.print(code, \"Suspicious Point Code\", Printers::get_main_param_id(\"Suspicious Point Code\"), printer.getRank(), Printers::get_point_id());\n\
\n"
    contents += "\
        if (debug) std::cout << \"Point Suspicious (\" << code << \"): \" << msg << std::endl;\n\
      }\n\
\n\
  };\n\
\n"

    return(contents)

# Main function
def main(argv):

    # Handle command line options
    verbose = False
    try:
        opts, args = getopt.getopt(argv,"v:",["verbose"])

        # Extract flag for standalone
        target = args[0]
        isStandalone = 1 if "standalone" in target or "CBS" in target else 0

    except :
        print('Usage: elements_extras.py [flags] <isStandalone>')
        print(' flags:')
        print('        -v                     : More verbose output')
        sys.exit(2)

    for opt, arg in opts:
      if opt in ('-v','--verbose'):
        verbose = True
        print('elements_extras.py: verbose=True')

    contents = "\
//   GAMBIT: Global and Modular BSM Inference Tool\n\
//   *********************************************\n\
///  \\file\n\
///\n\
///  Compile-time generation of extra code in Elements\n\
///  that uses the Printers. Required to avoid the use\n\
///  of printers in the standalones.\n\
///\n\
///  Classes added:\n\
///  - Suspicious point exception class.\n\
///\n\
///  This file was automatically generated by     \n\
///  elements_extras.py. Do not modify.           \n\
///\n\
///  *********************************************\n\
///\n\
///  Authors:\n\
///\n\
///  \\author The GAMBIT Collaboration            \n\
///  \date "+datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y")+"\n\
///\n\
///  *********************************************\n\
\n\
#ifndef __elements_extras_hpp__\n\
#define __elements_extras_hpp__\n\
\n\
#include <map>\n\
#include <set>\n\
#include <string>\n\
#include <exception>\n\
#include <vector>\n\
#include <utility>\n\
\n\
#include \"gambit/Utils/util_macros.hpp\"\n\
#include \"gambit/Logs/log_tags.hpp\"\n\
\n"

    if not isStandalone:
      contents += "\
#include \"gambit/Printers/printermanager.hpp\"\n\
#include \"gambit/Printers/baseprinter.hpp\"\n\
\n"

    contents += "\
namespace Gambit\n\
{\n\
\n"
 
    # Here add calls for all functions that generate code #
    ########################################################

    # Suspicious points, avoid using the printers when building standalones
    if verbose: print("Writing suspicious points code")
    contents += Suspicious_Points(isStandalone)

    contents += "\
}\n\
\n\
#endif\n"


    # Don't touch any existing file unless it is actually different from what we will create
    header = "./Elements/include/gambit/Elements/elements_extras.hpp"
    candidate = "./scratch/build_time/" + target + ".hpp.candidate"
    with open(candidate,"w") as f: f.write(contents)
    update_only_if_different(header, candidate, verbose=False)


if __name__ == "__main__":
   main(sys.argv[1:])
