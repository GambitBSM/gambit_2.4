//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Declaration of the Contur_output class that
///  converts the python dictionary output from
///  the contur API main into a simple class.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomasz Procter
///          (t.procter.1@research.gla.ac.uk)
///  \date 2021 July
///
///  *********************************************

#ifndef __contur_types_hpp__
#define __contur_types_hpp__

#include "gambit/cmake/cmake_variables.hpp"
#include "gambit/Utils/util_types.hpp"

#ifdef HAVE_PYBIND11

  #include <pybind11/stl.h>

  namespace Gambit
  {
    // Class that manages the input dictionary for Contur
    class Contur_output
    {
      friend Contur_output merge_contur_outputs(const Contur_output& output1, const Contur_output& output2);
    public:
      //Three member objects:
      double LLR;
      map_str_dbl pool_LLR;
      map_str_str pool_tags;
      pybind11::object contur_depot;

      //Default constructor - used if no events produced
      //TODO: now we have the depot, should we take care to merge these properly?
      Contur_output(){
          LLR = 0.0;
          pool_LLR = {};
          pool_tags = {};
          contur_depot = pybind11::none();
      }

      //Conctructor using the pybind11::dict we get from contur:
      Contur_output(pybind11::dict input_dict){
        LLR = input_dict.attr("get")("LLR").cast<double>();
        pool_LLR = input_dict.attr("get")("Pool_LLR").cast<map_str_dbl>();
        pool_tags = input_dict.attr("get")("Pool_tags").cast<map_str_str>();
        contur_depot = input_dict.attr("get")("depot");
      }

      //Constructor with all parts supplied: for the friend merge function
      Contur_output(const double newLLR, const map_str_dbl& newPool_LLR, const map_str_str& newPool_tags, const pybind11::object &newContur_depot){
        LLR = newLLR;
        pool_LLR = newPool_LLR;
        pool_tags = newPool_tags;
        contur_depot = newContur_depot;
      }

      //Automatic destructor should be fine, no need to define

      //Print the object and all its data in an easy to read format.
      void print_Contur_output_debug(std::ostream&outstream = std::cout) const;
      
    };

    //Helper function
    Contur_output merge_contur_outputs(const Contur_output& output1, const Contur_output& output2);

  }
#endif

#endif
