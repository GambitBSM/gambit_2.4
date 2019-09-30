#ifndef __wrapper_AnalysisHandler_def_Rivet_3_0_1_hh__
#define __wrapper_AnalysisHandler_def_Rivet_3_0_1_hh__

#include <string>
#include <vector>
#include "HepMC3/GenEvent.h"
#include <utility>
#include <map>

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
   
   namespace Rivet
   {
      
      // Member functions: 
      inline ::std::basic_string<char, std::char_traits<char>, std::allocator<char> > AnalysisHandler::runName() const
      {
         return get_BEptr()->runName();
      }
      
      inline long unsigned int AnalysisHandler::numEvents() const
      {
         return get_BEptr()->numEvents();
      }
      
      inline double AnalysisHandler::sumW() const
      {
         return get_BEptr()->sumW();
      }
      
      inline double AnalysisHandler::sumW2() const
      {
         return get_BEptr()->sumW2();
      }
      
      inline const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& AnalysisHandler::weightNames() const
      {
         return get_BEptr()->weightNames();
      }
      
      inline long unsigned int AnalysisHandler::numWeights() const
      {
         return get_BEptr()->numWeights();
      }
      
      inline bool AnalysisHandler::haveNamedWeights() const
      {
         return get_BEptr()->haveNamedWeights();
      }
      
      inline void AnalysisHandler::setWeightNames(const ::HepMC3::GenEvent& ge)
      {
         get_BEptr()->setWeightNames(ge);
      }
      
      inline long unsigned int AnalysisHandler::defaultWeightIndex() const
      {
         return get_BEptr()->defaultWeightIndex();
      }
      
      inline void AnalysisHandler::setCrossSection(::std::pair<double, double> xsec)
      {
         get_BEptr()->setCrossSection(xsec);
      }
      
      inline double AnalysisHandler::nominalCrossSection() const
      {
         return get_BEptr()->nominalCrossSection();
      }
      
      inline ::std::pair<int, int> AnalysisHandler::beamIds() const
      {
         return get_BEptr()->beamIds();
      }
      
      inline double AnalysisHandler::sqrtS() const
      {
         return get_BEptr()->sqrtS();
      }
      
      inline void AnalysisHandler::setIgnoreBeams(bool ignore)
      {
         get_BEptr()->setIgnoreBeams(ignore);
      }
      
      inline void AnalysisHandler::setIgnoreBeams()
      {
         get_BEptr()->setIgnoreBeams__BOSS();
      }
      
      inline void AnalysisHandler::skipMultiWeights(bool ignore)
      {
         get_BEptr()->skipMultiWeights(ignore);
      }
      
      inline void AnalysisHandler::skipMultiWeights()
      {
         get_BEptr()->skipMultiWeights__BOSS();
      }
      
      inline ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > > AnalysisHandler::analysisNames() const
      {
         return get_BEptr()->analysisNames();
      }
      
      inline Rivet::AnalysisHandler& AnalysisHandler::addAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname)
      {
         return get_BEptr()->addAnalysis__BOSS(analysisname).get_init_wref();
      }
      
      inline Rivet::AnalysisHandler& AnalysisHandler::addAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname, ::std::map<std::basic_string<char>, std::basic_string<char>, std::less<std::basic_string<char> >, std::allocator<std::pair<const std::basic_string<char>, std::basic_string<char> > > > pars)
      {
         return get_BEptr()->addAnalysis__BOSS(analysisname, pars).get_init_wref();
      }
      
      inline Rivet::AnalysisHandler& AnalysisHandler::addAnalyses(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& analysisnames)
      {
         return get_BEptr()->addAnalyses__BOSS(analysisnames).get_init_wref();
      }
      
      inline Rivet::AnalysisHandler& AnalysisHandler::removeAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname)
      {
         return get_BEptr()->removeAnalysis__BOSS(analysisname).get_init_wref();
      }
      
      inline Rivet::AnalysisHandler& AnalysisHandler::removeAnalyses(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& analysisnames)
      {
         return get_BEptr()->removeAnalyses__BOSS(analysisnames).get_init_wref();
      }
      
      inline void AnalysisHandler::init(const ::HepMC3::GenEvent& event)
      {
         get_BEptr()->init(event);
      }
      
      inline void AnalysisHandler::analyze(const ::HepMC3::GenEvent& event)
      {
         get_BEptr()->analyze(event);
      }
      
      inline void AnalysisHandler::analyze(const ::HepMC3::GenEvent* event)
      {
         get_BEptr()->analyze(event);
      }
      
      inline void AnalysisHandler::finalize()
      {
         get_BEptr()->finalize();
      }
      
      inline void AnalysisHandler::pushToPersistent()
      {
         get_BEptr()->pushToPersistent();
      }
      
      inline void AnalysisHandler::readData(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& filename)
      {
         get_BEptr()->readData(filename);
      }
      
      inline void AnalysisHandler::writeData(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& filename) const
      {
         get_BEptr()->writeData(filename);
      }
      
      inline void AnalysisHandler::dump(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > dumpfile, int period)
      {
         get_BEptr()->dump(dumpfile, period);
      }
      
      inline void AnalysisHandler::mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& delopts, bool equiv)
      {
         get_BEptr()->mergeYodas(aofiles, delopts, equiv);
      }
      
      inline void AnalysisHandler::mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& delopts)
      {
         get_BEptr()->mergeYodas__BOSS(aofiles, delopts);
      }
      
      inline void AnalysisHandler::mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles)
      {
         get_BEptr()->mergeYodas__BOSS(aofiles);
      }
      
      
      // Wrappers for original constructors: 
      inline Rivet::AnalysisHandler::AnalysisHandler(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& runname) :
         WrapperBase(__factory0(runname))
      {
         get_BEptr()->set_wptr(this);
         get_BEptr()->set_delete_wrapper(false);
      }
      
      inline Rivet::AnalysisHandler::AnalysisHandler() :
         WrapperBase(__factory1())
      {
         get_BEptr()->set_wptr(this);
         get_BEptr()->set_delete_wrapper(false);
      }
      
      // Special pointer-based constructor: 
      inline Rivet::AnalysisHandler::AnalysisHandler(Rivet::Abstract_AnalysisHandler* in) :
         WrapperBase(in)
      {
         get_BEptr()->set_wptr(this);
         get_BEptr()->set_delete_wrapper(false);
      }
      
      // Copy constructor: 
      inline Rivet::AnalysisHandler::AnalysisHandler(const AnalysisHandler& in) :
         WrapperBase(in.get_BEptr()->pointer_copy__BOSS())
      {
         get_BEptr()->set_wptr(this);
         get_BEptr()->set_delete_wrapper(false);
      }
      
      // Assignment operator: 
      inline Rivet::AnalysisHandler& AnalysisHandler::operator=(const AnalysisHandler& in)
      {
         if (this != &in)
         {
            get_BEptr()->pointer_assign__BOSS(in.get_BEptr());
         }
         return *this;
      }
      
      
      // Destructor: 
      inline Rivet::AnalysisHandler::~AnalysisHandler()
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
      inline Rivet::Abstract_AnalysisHandler* Rivet::AnalysisHandler::get_BEptr() const
      {
         return dynamic_cast<Rivet::Abstract_AnalysisHandler*>(BEptr);
      }
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_AnalysisHandler_def_Rivet_3_0_1_hh__ */
