#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/pybe_1_0.hpp"
#include <pybind11/pybind11.h>
namespace py = pybind11;

BE_INI_FUNCTION{}
END_BE_INI_FUNCTION

BE_NAMESPACE
{
    double c_add(double& x1, double& x2 )
    {
        double ans = pybe.attr("be_add")(x1,x2).cast<double>();
        return ans;
    }
}
END_BE_NAMESPACE
