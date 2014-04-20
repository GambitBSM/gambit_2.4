/* 
 * Placeholder "SoftSUSY" backend for testing
 * 
 * \author Ben Farmer
 * \date 2013-06-16  
 *
*/

/* Specify the path to the shared library along with a backend name. */

/* #define LIBPATH      "Backends/lib/softsusy-3.3.8/.libs/libsoft.so" */

#include <SLHA_types.hpp>
#include <shared_types.hpp>

#ifdef BACKENDRENAME
  #define BACKENDNAME BACKENDRENAME
#else
  #define BACKENDNAME FakeSoftSUSY
#endif
#define VERSION 1.0


/* The following macro loads the library (using dlmopen) in LIBPATH 
 * when this header file is included somewhere. */

/* LOAD_LIBRARY # Fake library so don't need this. However we still have to register a backend LogTag, which would have been done automatically by this macro, so below we run this 'secret' macro. Most backends should not use this macro, however. */
REGISTER_BACKEND_LOGTAG 

namespace Gambit
{
  namespace Backends
  {
    namespace BACKENDNAME
    {
      /* Convenience functions go here */
      
      /* In reality we need some sort of initialisation to occur, but I don't
      know how that is supposed to be achieved yet. So here we will just place
      function which "retrieve" the MSSM spectrum for specific models */
      
      // Get the CMSSM spectrum at scale Qout for a chosen model point
      // QGUT is the unification scale; make this optional?
      /// MSSMspecQ getCMSSMspectrum(SMpars &smpars,
      ///        double m0, double m12, double tanb, double a0, int sgnmu, double QGUT, double Qout)
      /// {
      ///   MSSMspecQ spectrum; // GAMBIT format spectrum object.
      ///   /* run SoftSUSY, stick results into spectrum object */
      ///   return spectrum;
      /// };
      /// 
      /// // Get the MSSM spectrum at scale Qout, from general MSSM soft masses at scale Qin
      /// MSSMspecQ getgenMSSMspectrum(SMpars &smpars, MSSMsoftmassesQ &softmasses, double Qin, double Qout)
      /// {
      ///   MSSMspecQ spectrum; // GAMBIT format spectrum object.
      ///   /* run SoftSUSY, stick results into spectrum object */
      ///   spectrum.MASS.stop1 = 1000;
      ///   spectrum.MASS.neut1 = 500;
    
      ///   return spectrum;
      /// };
      
    } /* end namespace BACKENDNAME */                                          
  } /* end namespace Backends */                                                
} /* end namespace Gambit */                                                   


/* Now register any convenience functions and wrap them in functors. 
 *
 * Syntax for BE_CONV_FUNCTION:
 * BE_CONV_FUNCTION([function name], type, "[choose capability name]")
 * 
 * As with BE_FUNCTION, the last argument is optional. */
 
//BE_CONV_FUNCTION(getCMSSMspectrum,   MSSMspecQ, (put some args here), "BECMSSMspectrum")
//BE_CONV_FUNCTION(getgenMSSMspectrum, MSSMspecQ, (put some args here), "BEgenMSSMspectrum")

// Undefine macros to avoid conflict with other backends
#undef LIBPATH 
#undef BACKENDNAME
#undef VERSION

