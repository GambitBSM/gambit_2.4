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

    // Backend functions
    BE_NAMESPACE
    {
        pybind11::object static DRN;

        pybind11::object c_pbarlike_initialization(const std::string& prevent_extrapolation,const std::string& propagation_model,mat_dbl& propagation_parameters)
        {
            pybind11::object DRN = pbarlike.attr("DRN_initialization")(propagation_parameters,propagation_model,prevent_extrapolation);
            return DRN;
        }
                
        double c_pbar_logLike_DRN(double& DM_mass, map_str_dbl& channel_dict, double& sv)
        {
            pybind11::dict inputs = pybind11::cast(channel_dict);
            double del_chi2 = pbarlike.attr("py_pbar_logLike_DRN")(DRN,DM_mass,inputs,sv).cast<double>();
            return del_chi2;
        }
    }
    END_BE_NAMESPACE
#endif

BE_INI_FUNCTION
{
    #ifdef HAVE_PYBIND11
        static bool scan_level = true;
        if(scan_level)
        {
            const std::string prevent_extrapolation = runOptions->getValue<std::string>("PreventExtrapolation");
            const std::string propagation_model = runOptions->getValue<std::string>("PropagationModel");
            mat_dbl propagation_parameters = runOptions->getValue<mat_dbl>("PropagationParameters");
            DRN = c_pbarlike_initialization(prevent_extrapolation,propagation_model,propagation_parameters);
        }
        scan_level = false;
    #endif
}
END_BE_INI_FUNCTION

