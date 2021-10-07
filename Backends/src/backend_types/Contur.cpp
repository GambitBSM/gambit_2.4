//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  implementation of the Contur_output class (and helpers)
///  that converts the python dictionary output from
///  the contur API main into a simple class.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomasz Procter
///          (t.procter.1@research.gla.ac.uk)
///  \date 2021 October
///
///  *********************************************

#include "gambit/Backends/backend_types/Contur.hpp"

namespace Gambit{

    void Contur_output::print_Contur_output_debug(std::ostream&outstream) const
    {
      outstream << "\nContur Output Object.\n\tLLR is: " <<
        LLR << "\n\tPool LLR's:";

      for (auto pool : pool_LLR){
        outstream << "\n\t\t" << pool.first << ": " << pool.second;
      }
      outstream << "\n\tPool Histotags:";
      for (auto pool : pool_tags){
        outstream << "\n\t\t" << pool.first << ": " << pool.second;
      }
      outstream << std::endl;
    }
  
    Contur_output merge_contur_outputs(const Contur_output& output1, const Contur_output& output2)
    {
      map_str_str new_pool_tags {};
      map_str_dbl new_pool_LLR {};

      //Concatenate the maps
      //TODO I presume there's something better than this syntax
      for(auto const& pool : output1.pool_LLR){
        new_pool_LLR[pool.first] = pool.second;
      }
      for(auto const& pool : output1.pool_tags){
        new_pool_tags[pool.first] = pool.second;
      }
      for(auto const& pool : output2.pool_LLR){
        new_pool_LLR[pool.first] = pool.second;
      }
      for(auto const& pool : output2.pool_tags){
        new_pool_tags[pool.first] = pool.second;
      }

      //Return the object
      return Contur_output(output1.LLR + output2.LLR, new_pool_LLR, new_pool_tags);
    }
}