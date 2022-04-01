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

    namespace py = pybind11;

    BE_INI_FUNCTION{}
    END_BE_INI_FUNCTION

    BE_NAMESPACE
    {
        // double c_chi2(std::vector<double>& phi_pred, double& v)
        // {
        //     double ans = pbarlike.attr("chi2")(phi_pred).cast<double>();
        //     return ans;
        // }

        std::vector<double> c_pbar_pred(double& DM_mass, double& sv, map_str_dbl& channel_dict)
        {
            pybind11::dict inputs = pybind11::cast(channel_dict);
            py::array_t<double> br_fr = pbarlike.attr("br_fr")(inputs);
            std::vector<double> pbar_pred = pbarlike.attr("pbar_flux")(DM_mass, sv, br_fr).cast<std::vector<double>>();
            return pbar_pred;
        }
    }
    END_BE_NAMESPACE
#endif
