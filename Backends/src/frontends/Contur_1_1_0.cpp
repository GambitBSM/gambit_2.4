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
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/Contur_1_1_0.hpp"

BE_NAMESPACE
{

  // Convenience functions
  double Contur_LogLike(vector_shared_ptr<YODA::AnalysisObject> &aos)
  {

    //pybind11::object ao = yoda.attr("AnalysisObject");
 
    return 0.0;

  }

  double Contur_LogLike_from_file(str &YODA_filename)
  {

    pybind11::object yodaFactory = Contur.attr("yodaFactory")(YODA_filename);

    // Disable grid runs
    yodaFactory.attr("_GridRun") = false;

    // Sort the list of contur buckets
    yodaFactory.attr("sortBuckets")();

    // Build a final conturPoint combining all buckets into one test
    yodaFactory.attr("buildFinal")();

    double CLs = yodaFactory.attr("conturPoint").attr("CLs").cast<double>();

    return CLs;
  }

}
END_BE_NAMESPACE

BE_INI_FUNCTION
{}
END_BE_INI_FUNCTION
