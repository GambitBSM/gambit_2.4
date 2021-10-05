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
      pybind11::object newdepot;
      double new_LLR = 0;
      //Check the depots actually exist:

      if (output1.contur_depot.is_none() && !output2.contur_depot.is_none()){
        newdepot = output2.contur_depot;
        if (output2.LLR != 0.0){
          new_LLR = (newdepot.attr("inbox")[pybind11::int_(0)].attr("yoda_factory").attr("_full_likelihood").attr("ts_s_b").cast<double>() - 
                    newdepot.attr("inbox")[pybind11::int_(0)].attr("yoda_factory").attr("_full_likelihood").attr("ts_b").cast<double>());
        }
      }
      else if (!output1.contur_depot.is_none() && output2.contur_depot.is_none()){
        newdepot = output1.contur_depot;
        if (output1.LLR != 0.0){
          new_LLR = (newdepot.attr("inbox")[pybind11::int_(0)].attr("yoda_factory").attr("_full_likelihood").attr("ts_s_b").cast<double>() - 
                    newdepot.attr("inbox")[pybind11::int_(0)].attr("yoda_factory").attr("_full_likelihood").attr("ts_b").cast<double>());
        }
      }
      else if (!output1.contur_depot.is_none() && !output2.contur_depot.is_none()){
        newdepot = output1.contur_depot;
        newdepot.attr("merge")(output2.contur_depot);
        newdepot.attr("resort_points")();
        if (output1.LLR != 0.0 && output2.LLR != 0.0){ 
           new_LLR = (newdepot.attr("inbox")[pybind11::int_(0)].attr("yoda_factory").attr("_full_likelihood").attr("ts_s_b").cast<double>()
                     - newdepot.attr("inbox")[pybind11::int_()].attr("yoda_factory").attr("_full_likelihood").attr("ts_b").cast<double>());
        }
      }
      else {
        newdepot = pybind11::none();
      }

      map_str_str new_pool_tags {};
      map_str_dbl new_pool_LLR {};

      //Concatenate the maps
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
      return Contur_output(new_LLR, new_pool_LLR, new_pool_tags, newdepot);
    }
}