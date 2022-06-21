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

    BE_INI_FUNCTION{}
    END_BE_INI_FUNCTION

    BE_NAMESPACE
    {
        namespace py = pybind11;
                
        double c_del_chi2(double& DM_mass, map_str_dbl& channel_dict, double& sv, std::string& propagation_model)
        {
            pybind11::dict inputs = pybind11::cast(channel_dict);
            double del_chi2 = pbarlike.attr("pbarlike")(DM_mass,inputs,sv,propagation_model).cast<double>();
            return del_chi2;
        }
    }
    END_BE_NAMESPACE
#endif
