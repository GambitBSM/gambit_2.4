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
  double Contur_LogLike(vector_shared_ptr<YODA::AnalysisObject> &ao)
  {

    return 0.0;

  }

  double Contur_LogLike_from_file(str &YODA_filename)
  {

    pybind11::object yodaFactory = Contur.attr("yodaFactory")(YODA_filename);

    // Set the test method to be used
    // TODO: LL for now
    yodaFactory.attr("_testMethod") = "LL";
    yodaFactory.attr("_GridRun") = false;

    yodaFactory.attr("getConturPoints")();

    pybind11::object conturPoint  = yodaFactory.attr("_ctPt");
    double CLs = conturPoint.attr("CLs").cast<double>();
    
    // TODO: This is not a likelihood, need to figure out how to convert it to one
    return CLs;
  }

}
END_BE_NAMESPACE

BE_INI_FUNCTION
{}
END_BE_INI_FUNCTION
