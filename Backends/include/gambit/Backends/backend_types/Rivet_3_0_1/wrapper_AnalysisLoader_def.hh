#ifndef __wrapper_AnalysisLoader_def_Rivet_3_0_1_hh__
#define __wrapper_AnalysisLoader_def_Rivet_3_0_1_hh__

#include <vector>
#include <set>
#include <string>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
   
   namespace Rivet
   {
      
      // Member functions: 
      inline ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > > AnalysisLoader::analysisNames()
      {
         return get_BEptr()->analysisNames();
      }
      
      inline ::std::set<std::basic_string<char>, std::less<std::basic_string<char> >, std::allocator<std::basic_string<char> > > AnalysisLoader::getAllAnalysisNames()
      {
         return get_BEptr()->getAllAnalysisNames();
      }
      
      
      // Wrappers for original constructors: 
      inline Rivet::AnalysisLoader::AnalysisLoader() :
         WrapperBase(__factory0())
      {
         get_BEptr()->set_wptr(this);
         get_BEptr()->set_delete_wrapper(false);
      }
      
      // Special pointer-based constructor: 
      inline Rivet::AnalysisLoader::AnalysisLoader(Rivet::Abstract_AnalysisLoader* in) :
         WrapperBase(in)
      {
         get_BEptr()->set_wptr(this);
         get_BEptr()->set_delete_wrapper(false);
      }
      
      // Copy constructor: 
      inline Rivet::AnalysisLoader::AnalysisLoader(const AnalysisLoader& in) :
         WrapperBase(in.get_BEptr()->pointer_copy__BOSS())
      {
         get_BEptr()->set_wptr(this);
         get_BEptr()->set_delete_wrapper(false);
      }
      
      // Assignment operator: 
      inline Rivet::AnalysisLoader& AnalysisLoader::operator=(const AnalysisLoader& in)
      {
         if (this != &in)
         {
            get_BEptr()->pointer_assign__BOSS(in.get_BEptr());
         }
         return *this;
      }
      
      
      // Destructor: 
      inline Rivet::AnalysisLoader::~AnalysisLoader()
      {
         if (get_BEptr() != 0)
         {
            get_BEptr()->set_delete_wrapper(false);
            if (can_delete_BEptr())
            {
               delete BEptr;
               BEptr = 0;
            }
         }
         set_delete_BEptr(false);
      }
      
      // Returns correctly casted pointer to Abstract class: 
      inline Rivet::Abstract_AnalysisLoader* Rivet::AnalysisLoader::get_BEptr() const
      {
         return dynamic_cast<Rivet::Abstract_AnalysisLoader*>(BEptr);
      }
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_AnalysisLoader_def_Rivet_3_0_1_hh__ */
