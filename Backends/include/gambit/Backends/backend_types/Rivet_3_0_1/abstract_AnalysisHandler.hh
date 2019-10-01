#ifndef __abstract_AnalysisHandler_Rivet_3_0_1_hh__
#define __abstract_AnalysisHandler_Rivet_3_0_1_hh__

#include "gambit/Backends/abstractbase.hpp"
#include "forward_decls_abstract_classes.hh"
#include "forward_decls_wrapper_classes.hh"
#include <string>
#include <vector>
#include "HepMC3/GenEvent.h"
#include <utility>
#include <map>
#include "YODA/AnalysisObject.h"
#include <cstddef>
#include <iostream>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
   
   
   namespace Rivet
   {
      class Abstract_AnalysisHandler : public virtual AbstractBase
      {
         public:
   
            virtual ::std::basic_string<char, std::char_traits<char>, std::allocator<char> > runName() const =0;
   
            virtual long unsigned int numEvents() const =0;
   
            virtual double sumW() const =0;
   
            virtual double sumW2() const =0;
   
            virtual const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& weightNames() const =0;
   
            virtual long unsigned int numWeights() const =0;
   
            virtual bool haveNamedWeights() const =0;
   
            virtual void setWeightNames(const ::HepMC3::GenEvent&) =0;
   
            virtual long unsigned int defaultWeightIndex() const =0;
   
            virtual void setCrossSection(::std::pair<double, double>) =0;
   
            virtual double nominalCrossSection() const =0;
   
            virtual ::std::pair<int, int> beamIds() const =0;
   
            virtual double sqrtS() const =0;
   
            virtual void setIgnoreBeams(bool) =0;
   
            virtual void setIgnoreBeams__BOSS() =0;
   
            virtual void skipMultiWeights(bool) =0;
   
            virtual void skipMultiWeights__BOSS() =0;
   
            virtual ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > > analysisNames() const =0;
   
            virtual Rivet::Abstract_AnalysisHandler& addAnalysis__BOSS(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >&) =0;
   
            virtual Rivet::Abstract_AnalysisHandler& addAnalysis__BOSS(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >&, ::std::map<std::basic_string<char>, std::basic_string<char>, std::less<std::basic_string<char> >, std::allocator<std::pair<const std::basic_string<char>, std::basic_string<char> > > >) =0;
   
            virtual Rivet::Abstract_AnalysisHandler& addAnalyses__BOSS(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >&) =0;
   
            virtual Rivet::Abstract_AnalysisHandler& removeAnalysis__BOSS(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >&) =0;
   
            virtual Rivet::Abstract_AnalysisHandler& removeAnalyses__BOSS(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >&) =0;
   
            virtual void init(const ::HepMC3::GenEvent&) =0;
   
            virtual void analyze(const ::HepMC3::GenEvent&) =0;
   
            virtual void analyze(const ::HepMC3::GenEvent*) =0;
   
            virtual void finalize() =0;
   
            virtual void pushToPersistent() =0;
   
            virtual void readData(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >&) =0;
   
            virtual void writeData(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >&) const =0;
   
            virtual void writeData(::std::vector<std::shared_ptr<YODA::AnalysisObject>, std::allocator<std::shared_ptr<YODA::AnalysisObject> > >&, ::YODA::AnalysisObject*) const =0;
   
            virtual void writeData__BOSS(::std::vector<std::shared_ptr<YODA::AnalysisObject>, std::allocator<std::shared_ptr<YODA::AnalysisObject> > >&) const =0;
   
            virtual void dump(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, int) =0;
   
            virtual void mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >&, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >&, bool) =0;
   
            virtual void mergeYodas__BOSS(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >&, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >&) =0;
   
            virtual void mergeYodas__BOSS(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >&) =0;
   
         public:
            virtual void pointer_assign__BOSS(Abstract_AnalysisHandler*) =0;
            virtual Abstract_AnalysisHandler* pointer_copy__BOSS() =0;
   
         private:
            AnalysisHandler* wptr;
            bool delete_wrapper;
         public:
            AnalysisHandler* get_wptr() { return wptr; }
            void set_wptr(AnalysisHandler* wptr_in) { wptr = wptr_in; }
            bool get_delete_wrapper() { return delete_wrapper; }
            void set_delete_wrapper(bool del_wrp_in) { delete_wrapper = del_wrp_in; }
   
         public:
            Abstract_AnalysisHandler()
            {
               wptr = 0;
               delete_wrapper = false;
            }
   
            Abstract_AnalysisHandler(const Abstract_AnalysisHandler&)
            {
               wptr = 0;
               delete_wrapper = false;
            }
   
            Abstract_AnalysisHandler& operator=(const Abstract_AnalysisHandler&) { return *this; }
   
            virtual void init_wrapper() =0;
   
            AnalysisHandler* get_init_wptr()
            {
               init_wrapper();
               return wptr;
            }
   
            AnalysisHandler& get_init_wref()
            {
               init_wrapper();
               return *wptr;
            }
   
            virtual ~Abstract_AnalysisHandler() =0;
      };
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"


#endif /* __abstract_AnalysisHandler_Rivet_3_0_1_hh__ */
