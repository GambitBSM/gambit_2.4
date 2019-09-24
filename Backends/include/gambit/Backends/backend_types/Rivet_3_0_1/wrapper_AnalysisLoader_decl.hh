#ifndef __wrapper_AnalysisLoader_decl_Rivet_3_0_1_hh__
#define __wrapper_AnalysisLoader_decl_Rivet_3_0_1_hh__

#include <cstddef>
#include "forward_decls_wrapper_classes.hh"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_AnalysisLoader.hh"
#include <vector>
#include <set>
#include <string>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
   
   namespace Rivet
   {
      
      class AnalysisLoader : public WrapperBase
      {
            // Member variables: 
         public:
            // -- Static factory pointers: 
            static Rivet::Abstract_AnalysisLoader* (*__factory0)();
      
            // -- Other member variables: 
      
            // Member functions: 
         public:
            ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > > analysisNames();
      
            ::std::set<std::basic_string<char>, std::less<std::basic_string<char> >, std::allocator<std::basic_string<char> > > getAllAnalysisNames();
      
      
            // Wrappers for original constructors: 
         public:
            AnalysisLoader();
      
            // Special pointer-based constructor: 
            AnalysisLoader(Rivet::Abstract_AnalysisLoader* in);
      
            // Copy constructor: 
            AnalysisLoader(const AnalysisLoader& in);
      
            // Assignment operator: 
            AnalysisLoader& operator=(const AnalysisLoader& in);
      
            // Destructor: 
            ~AnalysisLoader();
      
            // Returns correctly casted pointer to Abstract class: 
            Rivet::Abstract_AnalysisLoader* get_BEptr() const;
      
      };
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_AnalysisLoader_decl_Rivet_3_0_1_hh__ */
