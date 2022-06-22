//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Likelihood for 7-year antiproton data from AMS-02
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Sowmiya Balan
///  \date 2022 June
///  
///
///  *********************************************

#include <iostream>
#include <fstream>
#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/DarkBit/DarkBit_rollcall.hpp"
#include "gambit/Utils/util_functions.hpp"

#include <boost/multi_array.hpp>

namespace Gambit
{
    namespace DarkBit
    {
        void doublingMass(double& result )
        {
        using namespace Pipes::doublingMass;
        double mv = runOptions->getValue<double>("mv");
        result = BEreq::add_here(mv,mv);
        }

        void printPC(double& result)
        {
        using namespace Pipes::printPC;
        std::string DM_ID = Dep::WIMP_properties->name;
        std::string DMbar_ID = Dep::WIMP_properties->conjugate;
        double dummy = 0;
        double DM_mass = Dep::WIMP_properties->mass;
        std::cout << "DM ID:" << std::endl;
        BEreq::print_string(DM_ID, dummy);
        std::cout << "DM mass:" << std::endl;
        BEreq::print_num(DM_mass, dummy);
        std::cout << "DM Annihilation:" << std::endl;
        TH_Process process = Dep::TH_ProcessCatalog->getProcess(DM_ID, DMbar_ID);   
        for (std::vector<TH_Channel>::iterator it = process.channelList.begin(); it!=process.channelList.end();it++)
        { 
            std::vector<std::string> fs = it->finalStateIDs;
            std::cout << "Annihilation Channel : " << fs << std::endl;
            double rate = it->genRate->bind("v")->eval(0.);
            std::cout << "Cross-section : " << rate << std::endl;

        }
        result = 0;
        }
        void DelChi2 (double& result)
        {
        using namespace Pipes::DelChi2;
        double dummy = 0;
        LocalMaxwellianHalo LocalHaloParameters = *Dep::LocalHalo;
        double rho0 = LocalHaloParameters.rho0;     
        double rho0_eff = (*Dep::RD_fraction)*rho0/0.43;
        std::string DM_ID = Dep::WIMP_properties->name;
        std::string DMbar_ID = Dep::WIMP_properties->conjugate;
        double DM_mass = Dep::WIMP_properties->mass;
        TH_Process process = Dep::TH_ProcessCatalog->getProcess(DM_ID, DMbar_ID);
        map_str_dbl input;
        double sv;
        std::vector<std::string> fs;
        std::string finalStates;
        double rate;
        for (std::vector<TH_Channel>::iterator it = process.channelList.begin(); it!=process.channelList.end();it++)
        { 
            fs = it->finalStateIDs;
            finalStates = fs[0] + " " + fs[1];
            rate = it->genRate->bind("v")->eval(0.);
            rate = rate * rho0_eff * rho0_eff;
            input.insert({finalStates, rate});
            sv += rate;
        }
        const std::string propagation_model = runOptions->getValue<std::string>("PropagationModel");
        double del_chi2 = BEreq::pbar_del_chi2(DM_mass, input, sv, propagation_model);
        std::cout << "Delta chi^2 (preference of DM signal over background only) for the above specified model and channel: " << std::endl;
        std::cout << del_chi2 << std::endl;
        result = del_chi2;
        }
    }
}
