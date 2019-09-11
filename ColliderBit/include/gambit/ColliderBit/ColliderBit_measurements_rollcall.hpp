//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Rollcall header for ColliderBit measurments.
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


  /// Only activate this capability if HepMC is activated
  #ifndef EXCLUDE_HEPMC

    // Get measurements from Rivet
    #define CAPABILITY Rivet_measurements
    START_CAPABILITY
      #define FUNCTION Rivet_measurements
      START_FUNCTION(double)
      NEEDS_CLASSES_FROM(Rivet, default)
      DEPENDENCY(RunMC, MCLoopInfo) // Change to one that stores HepMC
      //BACKEND_REQ(Rivet_get_SM_measurements, (libRivet), double, ()) 
      //BACKEND_OPTION((Rivet, 3.0.1), (libRivet))
      #undef FUNCTION
    #undef CAPABILITY

  #endif
  
#undef MODULE
