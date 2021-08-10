#ifndef __wrapper_AnalysisHandler_decl_Rivet_3_1_4_hh__
#define __wrapper_AnalysisHandler_decl_Rivet_3_1_4_hh__

#include <cstddef>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <ostream>
#include "forward_decls_wrapper_classes.hh"
#include "gambit/Backends/wrapperbase.hpp"
#include "abstract_AnalysisHandler.hh"
#include "HepMC3/GenEvent.h"
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
            static Abstract_AnalysisHandler* (*__factory0)(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >&);
            static Abstract_AnalysisHandler* (*__factory1)();
      
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
      
            void setWeightCap(const double maxWeight);
      
            void setNLOSmearing(double frac);
      
            void skipMultiWeights(bool ignore);
      
            void skipMultiWeights();
      
            void selectMultiWeights(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > patterns);
      
            void selectMultiWeights();
      
            void deselectMultiWeights(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > patterns);
      
            void deselectMultiWeights();
      
            void setNominalWeightName(::std::basic_string<char, std::char_traits<char>, std::allocator<char> > name);
      
            void setNominalWeightName();
      
            void setCrossSection(const ::std::pair<double, double>& xsec, bool isUserSupplied);
      
            void setCrossSection(const ::std::pair<double, double>& xsec);
      
            void setCrossSection(double xsec, double xsecerr, bool isUserSupplied);
      
            void setCrossSection(double xsec, double xsecerr);
      
            double nominalCrossSection() const;
      
            ::std::pair<int, int> beamIds() const;
      
            double sqrtS() const;
      
            void checkBeams(bool check);
      
            void checkBeams();
      
            void setIgnoreBeams(bool ignore);
      
            void setIgnoreBeams();
      
            ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > > analysisNames() const;
      
            ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > > stdAnalysisNames() const;
      
            Rivet::AnalysisHandler& addAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname);
      
            Rivet::AnalysisHandler& addAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname, ::std::map<std::basic_string<char>, std::basic_string<char>, std::less<std::basic_string<char> >, std::allocator<std::pair<const std::basic_string<char>, std::basic_string<char> > > > pars);
      
            Rivet::AnalysisHandler& addAnalyses(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& analysisnames);
      
            Rivet::AnalysisHandler& removeAnalysis(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& analysisname);
      
            Rivet::AnalysisHandler& removeAnalyses(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& analysisnames);
      
            void init(const ::HepMC3::GenEvent& event);
      
            void analyze(const ::HepMC3::GenEvent& event);
      
            void analyze(const ::HepMC3::GenEvent* event);
      
            void finalize();
      
            void readData(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& filename);
      
            ::std::vector<std::shared_ptr<YODA::AnalysisObject>, std::allocator<std::shared_ptr<YODA::AnalysisObject> > > getYodaAOs(bool includeraw) const;
      
            ::std::vector<std::shared_ptr<YODA::AnalysisObject>, std::allocator<std::shared_ptr<YODA::AnalysisObject> > > getYodaAOs() const;
      
            void writeData(::std::basic_ostream<char, std::char_traits<char> >& ostr, const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& fmt) const;
      
            void writeData(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& filename) const;
      
            void dummy(::YODA::AnalysisObject* arg_1) const;
      
            void setAODump(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& dumpfile, int period);
      
            void setNoAODump();
      
            void dump(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& dumpfile, int period);
      
            void mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& delopts, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& addopts, bool equiv);
      
            void mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& delopts, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& addopts);
      
            void mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles, const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& delopts);
      
            void mergeYodas(const ::std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& aofiles);
      
      
            // Wrappers for original constructors: 
         public:
            AnalysisHandler(const ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >& runname);
            AnalysisHandler();
      
            // Special pointer-based constructor: 
            AnalysisHandler(Abstract_AnalysisHandler* in);
      
            // Copy constructor: 
            AnalysisHandler(const AnalysisHandler& in);
      
            // Assignment operator: 
            AnalysisHandler& operator=(const AnalysisHandler& in);
      
            // Destructor: 
            ~AnalysisHandler();
      
            // Returns correctly casted pointer to Abstract class: 
            Abstract_AnalysisHandler* get_BEptr() const;
      
      };
   }
   
}


#include "gambit/Backends/backend_undefs.hpp"

#endif /* __wrapper_AnalysisHandler_decl_Rivet_3_1_4_hh__ */
