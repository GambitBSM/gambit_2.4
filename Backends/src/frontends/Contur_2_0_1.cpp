//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Fronted source for the Contur backend
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2019 Oct
///
/// \author Tomasz Procter
///          (t.procter.1@research.gla.ac.uk)
/// \date 2021 June
///
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/Contur_2_0_1.hpp"


using namespace pybind11::literals;

BE_NAMESPACE
{

  // Convenience functions
  double Contur_LogLike(vector_shared_ptr<YODA::AnalysisObject> &aos)
  {

    //pybind11::object ao = yoda.attr("AnalysisObject");
    return 0.0;

  }

  double Contur_LogLike_from_stream(std::shared_ptr<std::ostringstream> yodastream, std::vector<std::string> &contur_yaml_args)
  {
    //Convert C++ ostringstream to python StringIO
    pybind11::str InputString = pybind11::cast(yodastream->str());
    pybind11::object yoda_string_IO = Contur.attr("StringIO")(InputString);
    yoda_string_IO.attr("seek")(pybind11::int_(pybind11::cast(0)));


    //Get default settings for Contur run and add a couple of our own as defaults for GAMBIT
    // - these can be overwritten below.
    pybind11::dict args_dict = ((Contur.attr("run_analysis").attr("get_argparser")()).attr("parse_args")()).attr("__dict__");
    args_dict[pybind11::cast("YODASTREAM")] = yoda_string_IO;
    args_dict[pybind11::cast("QUIET")] = pybind11::bool_(true);
    args_dict[pybind11::cast("STAT_OUTPUT_TYPE")] = pybind11::str("LLR");


    //Apply any of the additional contur args from the yodafile
    //Note these use the DEST name, NOT the command line option: i.e. use 'THY', not '--th', '--theory', 'th', or 'theory'.
    for (std::string arg : contur_yaml_args){
      //The presence check is ugly, because we can't use python's "in", and the value is allowed to be False or None.
      // Let's just hope contur never uses a key that has the default val  "NOT_A_VALID_ARGUMENT_JUST_A_TEST"!
      // If anyone knows a better method that works, please feel free to change.
      pybind11::object is_arg = args_dict.attr("get")(pybind11::cast(arg), pybind11::cast("NOT_A_VALID_ARGUMENT_JUST_A_TEST"));
      if ((is_arg.attr("__str__")()).cast<std::string>() != "NOT_A_VALID_ARGUMENT_JUST_A_TEST"){
        args_dict[pybind11::cast(arg)] = pybind11::bool_(true);
      }
      else{
        //TODO: Sorry for not knowing this, is there a formal Gambit way of raising a warning?
        std::cout << "\n\nWARNING: argument \"" << arg << "\" is not recognised in CONTUR.\n"
          << "Have you remembered to provide it in DEST form - e.g. \"USESEARCHES\", not "
          << "\"--usesearches\" or \"-s\".\nAlternatively, check spellings are correct and"
          << "/or Contur version is as expected.\nContur will run ignoring this argument.\n\n";
      }
    }
    
    //Run contur, get a LLR and return it
    return Contur.attr("run_analysis").attr("main")(args_dict).cast<double>();
  }


  double Contur_LogLike_from_file(str &YODA_filename, std::vector<std::string>& contur_yaml_args)
  {
    //Get default settings for Contur run and add a couple of our own as defaults for GAMBIT
    // - these can be overwritten below.
    pybind11::dict args_dict = ((Contur.attr("run_analysis").attr("get_argparser")()).attr("parse_args")()).attr("__dict__");
    args_dict[pybind11::cast("YODASTREAM")];
    args_dict[pybind11::cast("QUIET")] = pybind11::bool_(true);
    args_dict[pybind11::cast("STAT_OUTPUT_TYPE")] = pybind11::str("LLR");
    

    //Apply any of the additional contur args from the yodafile
    //Note these use the DEST name, NOT the command line option: i.e. use 'THY', not '--th', '--theory', 'th', or 'theory'.
    for (std::string arg : contur_yaml_args){
      //The presence check is ugly, because we can't use python's "in", and the value is allowed to be False or None.
      // Let's just hope contur never uses a key that has the default val  "NOT_A_VALID_ARGUMENT_JUST_A_TEST"!
      // If anyone knows a better method that works, please feel free to change.
      pybind11::object is_arg = args_dict.attr("get")(pybind11::cast(arg), pybind11::cast("NOT_A_VALID_ARGUMENT_JUST_A_TEST"));
      if ((is_arg.attr("__str__")()).cast<std::string>() != "NOT_A_VALID_ARGUMENT_JUST_A_TEST"){
        args_dict[pybind11::cast(arg)] = pybind11::bool_(true);
      }
      else{
        //TODO: Sorry for not knowing this, is there a formal Gambit way of raising a warning?
        std::cout << "\n\nWARNING: argument \"" << arg << "\" is not recognised in CONTUR.\n"
          << "Have you remembered to provide it in DEST form - e.g. \"USESEARCHES\", not "
          << "\"--usesearches\" or \"-s\".\nAlternatively, check spellings are correct and"
          << "/or Contur version is as expected.\nContur will run ignoring this argument.\n\n";
      }
    }
    
    //Run contur, get a LLR and return it
    return Contur.attr("run_analysis").attr("main")(args_dict).cast<double>();
  }

}
END_BE_NAMESPACE

BE_INI_FUNCTION
{}
END_BE_INI_FUNCTION
