#ifndef __abstract_AnalysisLoader_Rivet_3_0_1_hh__
#define __abstract_AnalysisLoader_Rivet_3_0_1_hh__

#include "gambit/Backends/abstractbase.hpp"
#include "forward_decls_abstract_classes.hh"
#include "forward_decls_wrapper_classes.hh"
#include <vector>
#include <set>
#include <string>
#include <cstddef>
#include <iostream>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
   
   
   namespace Rivet
   {
      class Abstract_AnalysisLoader : public virtual AbstractBase
      {
         public:
   
            virtual ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > > analysisNames() =0;
   
            virtual ::std::set<std::basic_string<char>, std::less<std::basic_string<char> >, std::allocator<std::basic_string<char> > > getAllAnalysisNames() =0;
   
         public:
            virtual void pointer_assign__BOSS(Abstract_AnalysisLoader*) =0;
            virtual Abstract_AnalysisLoader* pointer_copy__BOSS() =0;
   
         private:
            AnalysisLoader* wptr;
            bool delete_wrapper;
         public:
            AnalysisLoader* get_wptr() { return wptr; }
            void set_wptr(AnalysisLoader* wptr_in) { wptr = wptr_in; }
            bool get_delete_wrapper() { return delete_wrapper; }
            void set_delete_wrapper(bool del_wrp_in) { delete_wrapper = del_wrp_in; }
   
         public:
            Abstract_AnalysisLoader()
            {
               wptr = 0;
               delete_wrapper = false;
            }
   
            Abstract_AnalysisLoader(const Abstract_AnalysisLoader&)
            {
               wptr = 0;
               delete_wrapper = false;
            }
   
            Abstract_AnalysisLoader& operator=(const Abstract_AnalysisLoader&) { return *this; }
   
            virtual void init_wrapper() =0;
   
            AnalysisLoader* get_init_wptr()
            {
               init_wrapper();
               return wptr;
            }
   
            AnalysisLoader& get_init_wref()
            {
               init_wrapper();
               return *wptr;
            }
   
            virtual ~Abstract_AnalysisLoader() =0;
      };
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"


#endif /* __abstract_AnalysisLoader_Rivet_3_0_1_hh__ */
