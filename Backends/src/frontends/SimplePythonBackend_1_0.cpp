#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/SimplePythonBackend_1_0.hpp"
#include <pybind11/pybind11.h>
namespace py = pybind11;

BE_INI_FUNCTION{}
END_BE_INI_FUNCTION

BE_NAMESPACE
{
    double c_add(double& x1, double& x2 )
    {
        double ans = SimplePythonBackend.attr("py_add")(x1,x2).cast<double>();
        return ans;
    }

    void c_print_str(std::string& stuff, double& v)
    {
        SimplePythonBackend.attr("py_print_str")(stuff);
    }

    void c_print_double(double& num, double& v)
    {
        SimplePythonBackend.attr("py_print_double")(num);
    }

}
END_BE_NAMESPACE
