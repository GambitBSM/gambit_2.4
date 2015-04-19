//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Types used internally, returned and/or read
///  in by more than one backend, model or module.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott  
///          (patscott@physics.mcgill.ca)
///  \date 2013 Apr, Oct
///  \date 2014 Mar, Sep, Nov
///  \date 2015 Jan
///
///  \author Abram Krislock
///          (abram.krislock@fysik.su.se)
///  \date 2013 Dec
//
///  \author Christoph Weniger
///          (c.weniger@uva.nl)
///  \date 2014 Mar
///
///  \author Anders Kvellestad
///          (anderkve@fys.uio.no)
///  \date 2014 Oct
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.su)
///  \date 2015 Apr
///
///  *********************************************

#ifndef __shared_types_hpp__
#define __shared_types_hpp__

#include "gambit/Utils/funktions.hpp"         // Generalized functions
#include "gambit/Utils/SMInputs.hpp"          // Struct carrying SMINPUTS block (SLHA2) 
#include "gambit/Utils/SubSpectrum.hpp"       // Carries particle spectrum info for single model
#include "gambit/Utils/Spectrum.hpp"          // Carries BSM plus Standard Model spectrum info
#include "gambit/Utils/decay_table.hpp"       // Decay table class (carries particle decay info)
#include "gambit/Models/model_parameters.hpp" // Definitions required to understand model parameter objects

#include "SLHAea/slhaea.h"                    // SLHAea reader/writer class


// Other types that don't belong in any of the existing includes.  As the number of such types grows, they
// should be progressively organised into new headers, and those headers included from here.
namespace Gambit
{ 
  // Pointer to a function that takes an integer by reference and returns a double.
  // Just used for example purposes in ExampleBit_A and ExampleBit_B.
  typedef double(*fptr)(int&);

   /// Less confusing name for SLHAea container class
  typedef SLHAea::Coll SLHAstruct;

  /// DEPRECATED name for SLHAea container class. Please replace all instances of this with SLHAstruct (or SLHAea::Coll)
  typedef SLHAea::Coll eaSLHA;

}

#include "gambit/Backends/default_bossed_versions.hpp" // Default versions of backends to use when employing BOSSed types
#include "gambit/Backends/backend_types_rollcall.hpp"  // All backend types (header is auto-generated by backend harvester).

#endif //__shared_types_hpp__


