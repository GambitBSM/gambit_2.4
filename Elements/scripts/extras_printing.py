#!/usr/bin/env python
#
# GAMBIT: Global and Modular BSM Inference Tool
#*********************************************
# \file
#
#  Script to edit files, usually to make
#  sure no printing is used for standalones.
#  Also runs in reverse when not running standalone
#  to rewrite the printing. Extra files can be edited
#  provided they are also added as a dependency in the
#  utilities.cmake function extras_printing().
#  Any files that need to be generated from scratch should first made with touch
#  in CMakeLists.txt and added to cleaning.cmake
#
#*********************************************
#
#  Authors (add name and date if you modify):
#
#  \author Christopher Chang
#          (christopher.chang@uqconnect.edu.au)
#    \date 2021 Feb
#
#*********************************************

import pickle,sys

def Suspicious_Points(isStandalone):
    # Write suspicious_points.cpp , ensuring it doesn't print in standalone mode.

    if isStandalone:
      contents = "\
//   GAMBIT: Global and Modular BSM Inference Tool\n\
//   *********************************************\n\
///  \\file\n\
///\n\
///  Suspicious point exception declarations.\n\
///\n\
///  *********************************************\n\
///\n\
///  Authors (add name and date if you modify):\n\
///\n\
///  \\author Chris Chang\n\
///          (christopher.chang@uqconnect.edu.au)\n\
///  \date 2020 Nov\n\
///\n\
///  *********************************************\n\
\n\
#include <map>\n\
#include <set>\n\
#include <string>\n\
#include <exception>\n\
#include <vector>\n\
#include <utility>\n\
#include <iostream>\n\
\n\
#include \"gambit/Utils/util_macros.hpp\"\n\
#include \"gambit/Logs/log_tags.hpp\"\n\
\n\
#include \"gambit/Elements/suspicious_points.hpp\"\n\
\n\
namespace Gambit\n\
{\n\
\n\
    /// Constructor\n\
    Suspicious_point_exception::Suspicious_point_exception() {}\n\
\n\
    /// Raise the new suspicious point exception, Print it with a message and a code.\n\
    void Suspicious_point_exception::raise(const std::string& msg,int, bool debug)\n\
    {\n\
      if (debug) std::cout << \"Point Suspicious: \" << msg << std::endl;\n\
    }\n\
\n\
}\n"

    else:
      contents = "\
//   GAMBIT: Global and Modular BSM Inference Tool\n\
//   *********************************************\n\
///  \\file\n\
///\n\
///  Suspicious point exception declarations.\n\
///\n\
///  *********************************************\n\
///\n\
///  Authors (add name and date if you modify):\n\
///\n\
///  \\author Chris Chang\n\
///          (christopher.chang@uqconnect.edu.au)\n\
///  \date 2020 Nov\n\
///\n\
///  *********************************************\n\
\n\
#include <map>\n\
#include <set>\n\
#include <string>\n\
#include <exception>\n\
#include <vector>\n\
#include <utility>\n\
#include <iostream>\n\
\n\
#include \"gambit/Utils/util_macros.hpp\"\n\
#include \"gambit/Logs/log_tags.hpp\"\n\
\n\
#include \"gambit/Elements/suspicious_points.hpp\"\n\
#include \"gambit/Printers/printermanager.hpp\"\n\
#include \"gambit/Printers/baseprinter.hpp\"\n\
\n\
namespace Gambit\n\
{\n\
\n\
    /// Constructor\n\
    Suspicious_point_exception::Suspicious_point_exception() {}\n\
\n\
    /// Raise the new suspicious point exception, Print it with a message and a code.\n\
    void Suspicious_point_exception::raise(const std::string& msg,int mycode, bool debug)\n\
    {\n\
      // get the printer pointer\n\
      Printers::BaseBasePrinter& printer = *(get_global_printer_manager()->printerptr);\n\
\n\
      printer.print(mycode, \"Suspicious Point Code\", Printers::get_main_param_id(\"Suspicious Point Code\"), printer.getRank(), Printers::get_point_id());\n\
\n\
      if (debug) std::cout << \"Point Suspicious: \" << msg << std::endl;\n\
    }\n\
\n\
}\n"

    filename = "./Elements/src/suspicious_points.cpp"
    with open(filename,"w") as f:
        f.write(contents)
    return()


def main(isStandalone):

    # Add in further functions for other required edits
    Suspicious_Points(isStandalone)

if __name__ == "__main__":
   main(int(sys.argv[1]))
