#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/pbarlike_1_0.hpp"

#ifdef HAVE_PYBIND11
    #include "gambit/Utils/begin_ignore_warnings_pybind11.hpp"
    #include <pybind11/pybind11.h>
    #include <pybind11/stl.h>
    #include <pybind11/stl_bind.h>
    #include <pybind11/functional.h>
    #include <pybind11/numpy.h>
    #include "gambit/Utils/end_ignore_warnings.hpp"

    // BE_INI_FUNCTION
    // {
    //     static bool scan_level = true;
    //     if(scan_level)
    //     {
    //         namespace py = pybind11;
    //         const std::string prevent_extrapolation = runOptions->getValue<std::string>("PreventExtrapolation");
    //         const std::string propagation_model = runOptions->getValue<std::string>("PropagationModel");
    //         mat_dbl propagation_parameters = runOptions->getValue<mat_dbl>("PropagationParameters");
    //         py::object DRN = pbarlike.attr("DRN_initialization")(prevent_extrapolation,propagation_model,propagation_parameters);
    //     }
    //     scan_level = false;
    // }
    // END_BE_INI_FUNCTION


    BE_NAMESPACE
    {
        namespace py = pybind11;
                
        double c_pbar_logLike_DRN(double& DM_mass, map_str_dbl& channel_dict, double& sv)
        {
            pybind11::dict inputs = pybind11::cast(channel_dict);
            // py::object DRN;
            // double del_chi2 = pbarlike.attr("py_pbar_logLike_DRN")(DRN,DM_mass,inputs,sv).cast<double>();
            double del_chi2 = pbarlike.attr("py_pbar_logLike_DRN")(DM_mass,inputs,sv).cast<double>();
            return del_chi2;
        }
    }
    END_BE_NAMESPACE
#endif
