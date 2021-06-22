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

  double Contur_LogLike_from_stream(std::shared_ptr<std::ostringstream> yodastream)
  {
    //Convert C++ ostringstream to python StringIO
    //pybind11::object yoda_string_IO = Contur.attr("StringIO")(pybind11::cast(yodastream->str()));
    //yoda_string_IO.attr("write")(((pybind11::cast(yodastream->str())).attr("encode")(pybind11::cast("utf-8"))).attr("decode")(pybind11::cast("utf-8")));
    //yoda_string_IO.attr("seek")(pybind11::int_(pybind11::cast(0)));
    pybind11::str InputString = pybind11::cast(yodastream->str());
    pybind11::object yoda_string_IO = Contur.attr("StringIO")(InputString);

    yoda_string_IO.attr("seek")(pybind11::int_(pybind11::cast(0)));

    //Get default settings for Contur run and add a couple of our own:
    pybind11::dict args_dict = ((Contur.attr("run_analysis").attr("get_argparser")()).attr("parse_args")()).attr("__dict__");
    args_dict[pybind11::cast("YODASTREAM")] = yoda_string_IO;
    args_dict[pybind11::cast("QUIET")] = pybind11::bool_(true);
    args_dict[pybind11::cast("STAT_OUTPUT_TYPE")] = pybind11::str("LLR");

    //Run contur, get a LLR and return it
    return Contur.attr("run_analysis").attr("main")(args_dict).cast<double>();
    std::cout << __FILE__ << "; " << __LINE__ << "\n" << std::flush;
  }

  double Contur_LogLike_from_file(str &YODA_filename)
  {

    pybind11::object contur = Contur.attr("conturDepot")(false);
    pybind11::dict param = {};
    //param["No parameters specified"] = 0.0;

    contur.attr("addParamPoint")(YODA_filename, param);

    pybind11::list list = contur.attr("conturDepotInbox");
    double CLs = list[0].attr("yodaFactory").attr("_conturPoint").attr("CLs").cast<double>();

    /*pybind11::object yodaFactory = Contur.attr("yodaFactory")(YODA_filename);

    // Disable grid runs
    yodaFactory.attr("_GridRun") = false;

    // Sort the list of contur buckets
    yodaFactory.attr("sortBuckets")();

    // Build a final conturPoint combining all buckets into one test
    yodaFactory.attr("buildFinal")();

    double CLs = yodaFactory.attr("conturPoint").attr("CLs").cast<double>();
    */
    return CLs;

    // TODO: We actually need a loglike value. Contur provides a chisq, check that it is what we want
    //double chisq = yodaFactory.attr("conturPoint").attr("__chisq")(1.0,0.0).cast<double>();

    //return chisq;

    
  }

}
END_BE_NAMESPACE

BE_INI_FUNCTION
{}
END_BE_INI_FUNCTION
