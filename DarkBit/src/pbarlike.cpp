// //   GAMBIT: Global and Modular BSM Inference Tool
// //   *********************************************
// ///  \file
// ///
// ///  Likelihood for 7-year antiproton data from AMS-02
// ///
// ///  *********************************************
// ///
// ///  Authors (add name and date if you modify):
// ///
// ///  \author Sowmiya Balan
// ///  \date 2022 June
// ///  
// ///
// ///  *********************************************

// #include <iostream>
// #include <fstream>
// #include "gambit/Elements/gambit_module_headers.hpp"
// #include "gambit/DarkBit/DarkBit_rollcall.hpp"
// #include "gambit/Utils/util_functions.hpp"

// #include <boost/multi_array.hpp>

// using namespace DarkBit::Functown;     // Functors wrapping the module's actual module functions
// using namespace BackendIniBit::Functown;    // Functors wrapping the backend initialisation functions

// namespace Gambit
// {
//     namespace DarkBit
//     {
//         void delChi2 (double& result)
//         {
//         using namespace Pipes::delChi2;
//         double dummy = 0;
//         LocalMaxwellianHalo LocalHaloParameters = *Dep::LocalHalo;
//         double rho0 = LocalHaloParameters.rho0;     
//         double rho0_eff = (*Dep::RD_fraction)*rho0/0.43;
//         std::string DM_ID = Dep::WIMP_properties->name;
//         double DM_mass = Dep::WIMP_properties->mass;
//         TH_Process process = Dep::TH_ProcessCatalog->getProcess(DM_ID, DM_ID);
//         map_str_dbl input;
//         double sv;
//         std::vector<std::string> fs;
//         std::string finalStates;
//         double rate;
//         for (std::vector<TH_Channel>::iterator it = process.channelList.begin(); it!=process.channelList.end();it++)
//         { 
//             fs = it->finalStateIDs;
//             finalStates = fs[0] + " " + fs[1];
//             rate = it->genRate->bind("v")->eval(0.);
//             input.insert({finalStates, rate});
//             sv += rate;
//         }
//         sv = sv * rho0_eff * rho0_eff;
//         std::string propagation_model = runOptions->getValue<std::string>("propagation_model");
//         double del_chi2 = BEreq::pbar_del_chi2(DM_mass, input, sv, propagation_model);
//         std::cout << "Delta chi^2 (preference of DM signal over background only) for the above specified model and channel: " << std::endl;
//         std::cout << del_chi2 << std::endl;
//         result = del_chi2;
//         }
//     }
// }
