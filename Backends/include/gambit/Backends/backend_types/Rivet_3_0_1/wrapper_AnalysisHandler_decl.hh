#ifndef __wrapper_AnalysisHandler_decl_Rivet_3_0_1_hh__
#define __wrapper_AnalysisHandler_decl_Rivet_3_0_1_hh__

#include <cstddef>
#include "forward_decls_wrapper_classes.hh"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_AnalysisHandler.hh"
#include <string>
#include <vector>
#include "HepMC3/GenEvent.h"
#include <utility>
#include <map>
#include "YODA/AnalysisObject.h"

#include "identification.hpp"

namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
   
   namespace Rivet
   {
      
      class AnalysisHandler : public WrapperBase
      {
            // Member variables: 
         public:
            // -- Static factory pointers: 
            static Rivet::Abstract_AnalysisHandler* (*__factory0)(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >&);
            static Rivet::Abstract_AnalysisHandler* (*__factory1)();
      
            // -- Other member variables: 
      
            // Member functions: 
         public:
            ::std::basic_string<char, std::char_traits<char>, std::allocator<char> > runName() const;
      
            long unsigned int numEvents() const;
      
            double sumW() const;
      
            double sumW2() const;
      
            const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& weightNames() const;
      
            long unsigned int numWeights() const;
      
            bool haveNamedWeights() const;
      
            void setWeightNames(const ::HepMC3::GenEvent& ge);
      
            long unsigned int defaultWeightIndex() const;
      
            void setCrossSection(::std::pair<double, double> xsec);
      
            double nominalCrossSection() const;
      
            ::std::pair<int, int> beamIds() const;
      
            double sqrtS() const;
      
            void setIgnoreBeams(bool ignore);
      
            void setIgnoreBeams();
      
            void skipMultiWeights(bool ignore);
      
            void skipMultiWeights();
      
            ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > > analysisNames() const;
      
            Rivet::AnalysisHandler& addAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname);
      
            Rivet::AnalysisHandler& addAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname, ::std::map<std::basic_string<char>, std::basic_string<char>, std::less<std::basic_string<char> >, std::allocator<std::pair<const std::basic_string<char>, std::basic_string<char> > > > pars);
      
            Rivet::AnalysisHandler& addAnalyses(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& analysisnames);
      
            Rivet::AnalysisHandler& removeAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname);
      
            Rivet::AnalysisHandler& removeAnalyses(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& analysisnames);
      
            void init(const ::HepMC3::GenEvent& event);
      
            void analyze(const ::HepMC3::GenEvent& event);
      
            void analyze(const ::HepMC3::GenEvent* event);
      
            void finalize();
      
            void pushToPersistent();
      
            void readData(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& filename);
      
            void writeData(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& filename) const;
      
            void writeData(::std::vector<std::shared_ptr<YODA::AnalysisObject>, std::allocator<std::shared_ptr<YODA::AnalysisObject> > >& arg_1, ::YODA::AnalysisObject* arg_2) const;
      
            void writeData(::std::vector<std::shared_ptr<YODA::AnalysisObject>, std::allocator<std::shared_ptr<YODA::AnalysisObject> > >& arg_1) const;
      
            void dump(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > dumpfile, int period);
      
            void mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& delopts, bool equiv);
      
            void mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& delopts);
      
            void mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles);
      
      
            // Wrappers for original constructors: 
         public:
            AnalysisHandler(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& runname);
            AnalysisHandler();
      
            // Special pointer-based constructor: 
            AnalysisHandler(Rivet::Abstract_AnalysisHandler* in);
      
            // Copy constructor: 
            AnalysisHandler(const AnalysisHandler& in);
      
            // Assignment operator: 
            AnalysisHandler& operator=(const AnalysisHandler& in);
      
            // Destructor: 
            ~AnalysisHandler();
      
            // Returns correctly casted pointer to Abstract class: 
            Rivet::Abstract_AnalysisHandler* get_BEptr() const;
      
      };
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_AnalysisHandler_decl_Rivet_3_0_1_hh__ */
