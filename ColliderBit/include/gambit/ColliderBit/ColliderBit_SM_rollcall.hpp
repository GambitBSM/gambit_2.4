//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Rollcall header for ColliderBit SM measurments.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2019 July
///
///  *********************************************

#pragma once

#define MODULE ColliderBit

  // Get SM measurements from Rivet
  #define CAPABILITY SM_measurements
  START_CAPABILITY
    #define FUNCTION get_SM_measurements
    START_FUNCTION(double)
    DEPENDENCY(RunMC, MCLoopInfo) // Change to one that stores HepMC
    BACKEND_REQ(Rivet_get_SM_measurments, (libRivet), double, ()) 
    BACKEND_OPTION((Rivet, 3.0.0), (libRivet))
    #undef FUNCTION
  #undef CAPABILITY
  
#undef MODULE
