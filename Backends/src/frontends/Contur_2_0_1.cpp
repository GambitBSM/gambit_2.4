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

  double Contur_LogLike_from_stream(std::ostream& yodastream)
  {
    pybind11::object contur = Contur.attr("factories").attr("Depot")(false);
    pybind11::dict params = {};
    params["No parameters specified"] = 0.0;

    contur.attr("add_point")("param_dict"_a=params, "yodafile"_a=yodastream);

    pybind11::list inbox = contur.attr("_inbox");

    double LLR = inbox[0].attr("yoda_factory").attr("_full_likelihood").attr("._ts_s_b").cast<double>() 
     - inbox[0].attr("yoda_factory").attr("_full_likelihood").attr("_ts_b").cast<double>();
    return LLR;
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
