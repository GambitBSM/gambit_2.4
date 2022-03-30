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

    void c_print_str(std::string& stuff, double& v)
    {
        pybe.attr("be_print_str")(stuff);
    }

    void c_print_num(double& num, double& v)
    {
        pybe.attr("be_print_num")(num);
    }

    // void c_print_process(DarkBit::TH_Process& process, double& v)
    // {
    //     std::cout << process << std::endl;
    //     //pybe.attr("be_print_process")(process);
    // }

}
END_BE_NAMESPACE
