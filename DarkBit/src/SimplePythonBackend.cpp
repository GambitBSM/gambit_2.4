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
        result = BEreq::python_add(mv,mv);
        }

        void printProcessCatalog(double& result)
        {
        using namespace Pipes::printProcessCatalog;
        std::string DM_ID = Dep::WIMP_properties->name;
        std::string DMbar_ID = Dep::WIMP_properties->conjugate;
        double dummy = 0;
        double DM_mass = Dep::WIMP_properties->mass;
        std::cout << "DM ID:" << std::endl;
        BEreq::python_print_string(DM_ID, dummy);
        std::cout << "DM mass:" << std::endl;
        BEreq::python_print_double(DM_mass, dummy);
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
    }
}
