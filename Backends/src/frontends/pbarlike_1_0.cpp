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
        std::string static DM_ID ;
        std::string static DMbar_ID ;
        pybind11::object static DRN;
        map_str_dbl static pbar_logLikelihoods;

        pybind11::object c_pbarlike_initialization(const std::string& prevent_extrapolation,const std::string& propagation_model,mat_dbl& propagation_parameters)
        {
            pybind11::object DRN = pbarlike.attr("DRN_initialization")(propagation_parameters,propagation_model,prevent_extrapolation);
            return DRN;
        }
                
        map_str_dbl c_pbar_logLikes(double& DM_mass, map_str_dbl& channel_dict, double& sv)
        {   
            pybind11::dict inputs = pybind11::cast(channel_dict);
            pybind11::dict del_chi2 = pbarlike.attr("py_pbar_logLikes")(DRN,DM_mass,inputs,sv);
            std::cout << "got dict" << std::endl;
            map_str_dbl c_del_chi2 = del_chi2.cast<map_str_dbl>();
            std::cout << "casted" << std::endl;
            return c_del_chi2;
        }

        // double c_pbar_logLike_uncorr(double& dummy1, double& dummy2)
        // {
        //     double result = pbar_logLikelihoods["uncorrelated"];
        //     return result;
        // }

        // double c_pbar_logLike_corr(double& dummy1, double& dummy2)
        // {
        //     double result = pbar_logLikelihoods["correlated"];
        //     return result;
        // }

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
            // std::string DM_ID = Dep::WIMP_properties->name;
            // std::string DMbar_ID = Dep::WIMP_properties->conjugate;
        }
        scan_level = false;
        
        // DarkBit::TH_Process process = Dep::TH_ProcessCatalog->getProcess(DM_ID, DMbar_ID);   
        // LocalMaxwellianHalo LocalHaloParameters = *Dep::LocalHalo;
        // double rho0 = LocalHaloParameters.rho0;     
        // double rho0_eff = (*Dep::RD_fraction)*rho0/0.43;
        // double DM_mass = Dep::WIMP_properties->mass;
        // map_str_dbl input;
        // double sv=0;
        // std::vector<std::string> fs;
        // std::string finalStates;
        // double rate=0;
        // for (std::vector<DarkBit::TH_Channel>::iterator it = process.channelList.begin(); it!=process.channelList.end();it++)
        // { 
        //     fs = it->finalStateIDs;
        //     finalStates = fs[0] + " " + fs[1];
        //     rate = it->genRate->bind("v")->eval(0.);
        //     rate = rate * rho0_eff * rho0_eff;
        //     input.insert({finalStates, rate});
        //     sv += rate;
        // }

        // pbar_logLikelihoods = c_pbar_logLikes(DM_mass, input, sv);
    #endif
}
END_BE_INI_FUNCTION

