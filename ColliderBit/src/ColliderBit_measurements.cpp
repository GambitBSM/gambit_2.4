//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of ColliderBit measurments.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date   2019
///
///  \author Tomasz Procter
///          (t.procter.1@research.gla.ac.uk)
///  \date   June 2021
///
///  *********************************************

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/ColliderBit/ColliderBit_rollcall.hpp"
#include "gambit/ColliderBit/Utils.hpp"
#include "gambit/ColliderBit/ColliderBit_eventloop.hpp"

#ifndef EXCLUDE_HEPMC
  #include "HepMC3/ReaderAscii.h"
  #include "HepMC3/ReaderAsciiHepMC2.h"
#endif  

#ifndef EXCLUDE_YODA
  #include "YODA/AnalysisObject.h"
  #include "YODA/IO.h"
#endif

namespace Gambit
{

  namespace ColliderBit
  {

    //Small convenience function for supplying options to the contur argparser.
    void convert_yaml_options_for_contur(std::vector<std::string> &yaml_options)
    {
      for (size_t i{0}; i <yaml_options.size(); ++i){
        yaml_options[i] = ("--"+yaml_options[i]);
      }
    }


    #ifndef EXCLUDE_HEPMC
      #ifndef EXCLUDE_YODA

        // Analyse HepMC events with Rivet's measurements 
        // Collect results in a stream of a YODA file
        void Rivet_measurements(std::shared_ptr<std::ostringstream> &result)
        {
          using namespace Pipes::Rivet_measurements;
          using namespace Rivet_default::Rivet;
          
          static std::shared_ptr<AnalysisHandler> ah;
          static bool studying_first_event;
          static int events_analysed = 0;

          if (*Loop::iteration == COLLIDER_INIT)
          {
            std::cout << "\n\nADDING RIVET ANALYSES\n\n";
            if (ah != nullptr) {
              ah->~AnalysisHandler();
              ah = nullptr;
            }
            ah = std::make_shared<AnalysisHandler>();
            studying_first_event = true;

            YAML::Node colNode = runOptions->getValue<YAML::Node>(Dep::RunMC->current_collider());
            Options colOptions(colNode);
            std::cout << "\n\n STARTING rivet for Collider: " << Dep::RunMC->current_collider() << "\n\n";
            // xsec_veto_fb = colOptions.getValueOrDef<double>(xsec_veto_default, "xsec_veto");
            // result.partonOnly = colOptions.getValueOrDef<bool>(partonOnly_default, "partonOnly");
            // result.antiktR = colOptions.getValueOrDef<double>(antiktR_default, "antiktR");
          
            // Get analysis list from yaml file
            std::vector<str> analyses = colOptions.getValueOrDef<std::vector<str> >(std::vector<str>(), "analyses");

            if(not analyses.size())
              ColliderBit_warning().raise(LOCAL_INFO, "No analyses set for Rivet. This means an empty yoda file will be passed to Contur");
            // TODO: Add somewhere a check to make sure we only do LHC analyses
            else{
              for (size_t i = 0; i < analyses.size() ; ++i){
                //If the analysis is a special code referring to multiple analyses,
                //append these to the end of the vector, so they are dealt with
                //later in the loop
                if (analyses[i] == "13TeV" || analyses[i] == "8TeV" || analyses[i] == "7TeV"){
                  BEreq::Contur_GetAnalyses(analyses, analyses[i]);
                }
                //If its a normal analyis just add it.
                else {
                  ah->addAnalysis(analyses[i]);                  
                }
              }
            }

            //If the yaml file wants to exclude analyses, remove them
            //This feature was inspired by ATLAS_2016_I1469071, which is effectively
            //invalid for most BSM cases and can cause crashes.
            std::vector<str> excluded_analyses = colOptions.getValueOrDef<std::vector<str> >(std::vector<str>(), "exclude_analyses");
            ah->removeAnalyses(excluded_analyses);
          
            //Write the utilised analyses to a file in yaml-like format
            //This will list only the analyses that RIVET has succesfully loaded -
            // can be used to debug any typos in the initial yaml file etc.
            //Only do this the first time contur is run.
            //TODO: This may need adapting so it deals well with multiple different collider energies in one run.
            //Second TODO: the analysishandler can choose to autoremove analyses e.g. if the energy is wrong LATER.
            //Should we account for this? E.g. By wiping the file in base_init.
            static bool analyses_written_to_file = false;
            if (!analyses_written_to_file){
              std::ofstream analyses_output_file;
              //TODO please feel free to change name/put in more appropriate location.
              analyses_output_file.open(GAMBIT_DIR+std::string("/GAMBIT_rivet_analyses.log"));
              analyses_output_file << "analyses:";

              for (std::string an_analysis_string : ah->analysisNames()) {
                analyses_output_file << "\n - " << an_analysis_string;
              }
              analyses_output_file.close();
              analyses_written_to_file = true;
            }
          }

          if (*Loop::iteration == COLLIDER_FINALIZE)
          {
          #ifdef COLLIDERBIT_DEBUG
            std::cout << "Summary data from rivet:\n\tAnalyses used: ";
            for (auto analysis : ah.analysisNames()){
                std::cout << analysis << ", ";
            }
            std::cout << "\n\tBeam IDs are " << ah.beamIds().first << ", " << ah.beamIds().second;
            std::cout << "\n\tXS: " << ah.nominalCrossSection();
            std::cout << "\n\tRunName: " << ah.runName();
            std::cout << "\n\tSqrtS: " << ah.sqrtS();
            std::cout << "\n\tList of available analyses: ";
            for (auto analysis : ah.stdAnalysisNames()){
                std::cout << analysis << ", ";
            }
            std::cout << std::flush;
          #endif

            std::cout << "\n\nFINALISING RIVET ANALYSES\n" << std::endl;

            //Initialise somewhere for the yoda file to be outputted.
            //This circuitous route is necesarry because ostringstream does not
            //support copy assignment or copy initialisation, and which is 
            //necesarry to access items via Gambit's backends system, so we need
            //to go via a pointer. 
            if (result == nullptr){
              result = std::make_shared<std::ostringstream>();
            }


            // Drop YODA file if requested

            std::cout << "\n\nAbout to output yoda" << std::endl;
            std::cout << "\tNumEvnts: " << ah->numEvents() << std::endl;
            std::cout << "\tbeamIDs: " << ah->beamIds().first << ", " << ah->beamIds().second << std::endl;
            std::cout << "\tXS: " << ah->nominalCrossSection() << std::endl;
            std::cout << "\tRunName: " << ah->runName() << std::endl;
            std::cout << "\tSqrtS: " << ah->sqrtS() << std::endl;

            bool drop_YODA_file = runOptions->getValueOrDef<bool>(false, "drop_YODA_file");
            if(drop_YODA_file)
            {
              str filename = "GAMBIT_collider_measurements.yoda";
              
              #pragma omp critical
              {
                try{ ah->writeData(filename); }
                catch (...)
                { ColliderBit_error().raise(LOCAL_INFO, "Unexpected error in writing YODA file"); }
              }
            }

            #pragma omp critical
            {
              ah->finalize();
              ah->writeData(*result, "yoda");
            }

            

            #pragma omp critical
            {
              ah->~AnalysisHandler();
              ah = nullptr;
            }
          }

          // Don't do anything else during special iterations
          if (*Loop::iteration < 0) return;

          if (studying_first_event){
            if (omp_get_thread_num() == 0)
            {
              // Get the HepMC event
              HepMC3::GenEvent ge = *Dep::HardScatteringEvent;
              try { ah->analyze(ge); }
              catch(std::runtime_error &e)
              {
                ColliderBit_error().raise(LOCAL_INFO, e.what());
              }
              studying_first_event = false;
            }
            #pragma omp barrier
            if (omp_get_thread_num() != 0)
            {
              #pragma omp critical
              {
                // Get the HepMC event
                HepMC3::GenEvent ge = *Dep::HardScatteringEvent;
                // Save the old event number in case other bits of Gambit need it.
                int old_events_analysed = ge.event_number();
                // Set the Event number to a stream independent total so Rivet can
                // make sense of things.
                ge.set_event_number(++events_analysed);
                try { ah->analyze(ge); }
                catch(std::runtime_error &e)
                {
                  ColliderBit_error().raise(LOCAL_INFO, e.what());
                }
                // Reset the old event number in case GAMBIT needs it elsewhere.
                ge.set_event_number(old_events_analysed);
              }
            }
          }
        else{
          # pragma omp critical
          {
            // Get the HepMC event
            HepMC3::GenEvent ge = *Dep::HardScatteringEvent;
            // Save the old event number in case other bits of Gambit need it.
            int old_events_analysed = ge.event_number();
            // Set the Event number to a stream independent total so Rivet can
            // make sense of things.
            ge.set_event_number(++events_analysed);
            try { ah->analyze(ge); }
            catch(std::runtime_error &e)
            {
              ColliderBit_error().raise(LOCAL_INFO, e.what());
            }
            // Reset the old event number in case GAMBIT needs it elsewhere.
            ge.set_event_number(old_events_analysed);
            std::cout << "\nRIVET ANALYSING" << std::endl;
          }
        }
      }
      
      #endif //EXCLUDE_YODA
    #endif // EXCLUDE_HEPMC

    #ifndef EXCLUDE_YODA

      // Contur version, from YODA stream
      void Contur_LHC_measurements_from_stream(Contur_output &result)
      {
        using namespace Pipes::Contur_LHC_measurements_from_stream;
        if (*Loop::iteration == COLLIDER_FINALIZE)
        {
          std::cout << "\n\nDOING CONTUR STUFF\n" << std::endl;
    
          std::shared_ptr<std::ostringstream> yodastream = *Dep::Rivet_measurements;
          
          std::vector<std::string> yaml_contur_options = runOptions->getValueOrDef<std::vector<str>>(std::vector<str>(), "contur_options");
          convert_yaml_options_for_contur(yaml_contur_options);

          #pragma omp critical
          {
            ///Call contur
            result = BEreq::Contur_Measurements(std::move(yodastream), yaml_contur_options);
          } 
          std::cout << "\n\nFINISHING CONTUR STUFF\n" << std::endl;
        }
      }

      // Contur version, from YODA file
      void Contur_LHC_measurements_from_file(Contur_output &result)
      {
        using namespace Pipes::Contur_LHC_measurements_from_file;

        // This function only works if there is a file
        str YODA_filename = runOptions->getValueOrDef<str>("", "YODA_filename");

        if (YODA_filename == "" or not Utils::file_exists(YODA_filename))
          ColliderBit_error().raise(LOCAL_INFO, "YODA file "+YODA_filename+" not found.");

        std::vector<std::string> yaml_contur_options = runOptions->getValueOrDef<std::vector<str>>(std::vector<str>(), "contur_options");
        convert_yaml_options_for_contur(yaml_contur_options);

        #pragma omp critical
        {
          // Call Contur
          result = BEreq::Contur_Measurements(YODA_filename, yaml_contur_options);
        } 
      }

      void Contur_LHC_measurements_LogLike(double &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike;
        if (*Loop::iteration == COLLIDER_FINALIZE)
        {
          Contur_output contur_likelihood_object = *Dep::LHC_measurements;
          result = contur_likelihood_object.LLR;
        }
      }

      void Contur_LHC_measurements_LogLike_perPool(map_str_dbl &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike_perPool;
        if (*Loop::iteration == COLLIDER_FINALIZE)
        {
          std::stringstream summary_line;
          summary_line << "LHC Contur LogLikes per pool: ";
          result = (*Dep::LHC_measurements).pool_LLR;

          for( auto const& entry : result){
            summary_line << entry.first << ":" << entry.second << ", ";
          }
          logger() << LogTags::debug << summary_line.str() << EOM;
        }
      }

      void Contur_LHC_measurements_histotags_perPool(map_str_str &result)
      {
        using namespace Pipes::Contur_LHC_measurements_LogLike_perPool;
        if (*Loop::iteration == COLLIDER_FINALIZE)
        {
          std::stringstream summary_line;
          summary_line << "LHC Contur LogLikes per pool: ";

          result = (*Dep::LHC_measurements).pool_tags;

          for( auto const& entry : result){
            summary_line << entry.first << ":" << entry.second << ", ";
          }

          logger() << LogTags::debug << summary_line.str() << EOM;
        }
      }


    #endif //EXCLUDE_YODA

  }  // namespace ColliderBit
}  // namespace Gambit
