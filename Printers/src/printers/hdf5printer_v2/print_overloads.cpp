//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  HDF5Printer print function
///  overloads.  Add a new overload of the _print
///  function in this file if you want to be able
///  to print a new type.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Ben Farmer
///          (b.farmer@imperial.ac.uk)
///  \date 2015 May, 2019 Jan
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2017 March
///
///  *********************************************

#include "gambit/Printers/printers/hdf5printer_v2.hpp"
#include "gambit/Printers/printers/common_print_overloads.hpp"

namespace Gambit
{
  namespace Printers
  {

    /// @{ PRINT FUNCTIONS
    /// Need to define one of these for every type we want to print!

    /// Simple print functions
    #define PRINT(TYPE) _print(TYPE const& value, const std::string& label, const int /*vID*/, const uint rank, const ulong pID) \
       { basic_print(value,label,rank,pID); }
    void HDF5Printer2::PRINT(int)
    void HDF5Printer2::PRINT(uint)
    void HDF5Printer2::PRINT(long)
    void HDF5Printer2::PRINT(ulong)
    void HDF5Printer2::PRINT(float)
    void HDF5Printer2::PRINT(double)
    #undef PRINT

    // longlongs can lead to ambiguity problems matching C++ to HDF5 types, since they are sometimes the same as longs. So just stick
    // with longs in the printer, they are long enough
    #define PRINTAS(INTYPE,OUTTYPE) _print(INTYPE const& value, const std::string& label, const int vID, const uint rank, const ulong pID) \
    { _print((OUTTYPE)value,label,vID,rank,pID); }
    void HDF5Printer2::PRINTAS(longlong, long)
    void HDF5Printer2::PRINTAS(ulonglong, ulong)
    #undef PRINTAS

    /// Bools can't quite use the template print function directly, since there
    /// are some issues with bools and MPI/HDF5 types. Easier to just convert
    /// the bool to an int first.
    void HDF5Printer2::_print(bool const& value, const std::string& label, const int vID, const unsigned int mpirank, const unsigned long pointID)
    {
      unsigned int val_as_uint = value;
      _print(val_as_uint,label,vID,mpirank,pointID);
    }

<<<<<<< HEAD
    void HDF5Printer2::_print(const map_str_dbl& map, const std::string& label, const int /*vID*/, const unsigned int mpirank, const unsigned long pointID)
    {
      print_map_str_dbl(map, label, mpirank, pointID);
    }

    void HDF5Printer2::_print(const map_const_str_dbl& map, const std::string& label, const int /*vID*/, const unsigned int mpirank, const unsigned long pointID)
    {
      print_map_str_dbl(map, label, mpirank, pointID);
    }

    void HDF5Printer2::_print(const map_str_map_str_dbl& map, const std::string& label, const int vID, const unsigned int mpirank, const unsigned long pointID)
    {
      print_map_str_map_str_dbl(map, label, vID, mpirank, pointID);
    }

    void HDF5Printer2::_print(const map_const_str_map_const_str_dbl& map, const std::string& label, const int vID, const unsigned int mpirank, const unsigned long pointID)
    {
      print_map_str_map_str_dbl(map, label, vID, mpirank, pointID);
    }

    void HDF5Printer2::_print(ModelParameters const& value, const std::string& label, const int vID, const unsigned int mpirank, const unsigned long pointID)
    {
      std::map<std::string, double> parameter_map = value.getValues();
      _print(parameter_map, label, vID, mpirank, pointID);
    }

    void HDF5Printer2::_print(triplet<double> const& value, const std::string& label, const int vID, const unsigned int mpirank, const unsigned long pointID)
    {
      std::map<std::string, double> m;
      m["central"] = value.central;
      m["lower"] = value.lower;
      m["upper"] = value.upper;
      _print(m, label, vID, mpirank, pointID);
    }

    void HDF5Printer2::_print(map_intpair_dbl const& map, const std::string& label, const int /*vID*/, const unsigned int mpirank, const unsigned long pointID)
    {
      for (std::map<std::pair<int,int>, double>::const_iterator it = map.begin(); it != map.end(); it++)
      {
        std::stringstream ss;
        ss<<label<<"::"<<it->first;
        basic_print(it->second,ss.str(),mpirank,pointID);
      }
    }

    #ifndef SCANNER_STANDALONE // All the types inside HDF5_MODULE_BACKEND_TYPES need to go inside this def guard.

      void HDF5Printer2::_print(DM_nucleon_couplings const& value, const std::string& label, const int vID, const unsigned int mpirank, const unsigned long pointID)
      {
        std::map<std::string, double> m;
        m["Gp_SI"] = value.gps;
        m["Gn_SI"] = value.gns;
        m["Gp_SD"] = value.gpa;
        m["Gn_SD"] = value.gna;
        _print(m, label, vID, mpirank, pointID);
      }

      void HDF5Printer2::_print(Flav_KstarMuMu_obs const& value, const std::string& label, const int vID, const unsigned int mpirank, const unsigned long pointID)
      {
        std::map<std::string, double> m;
        std::ostringstream bins;
        bins << value.q2_min << "_" << value.q2_max;
        m["BR_"+bins.str()] = value.BR;
        m["AFB_"+bins.str()] = value.AFB;
        m["FL_"+bins.str()] = value.FL;
        m["S3_"+bins.str()] = value.S3;
        m["S4_"+bins.str()] = value.S4;
        m["S5_"+bins.str()] = value.S5;
        m["S7_"+bins.str()] = value.S7;
        m["S8_"+bins.str()] = value.S8;
        m["S9_"+bins.str()] = value.S9;
        _print(m, label, vID, mpirank, pointID);
      }

      void HDF5Printer2::_print(FlavBit::flav_prediction const& value, const std::string& label, const int vID, const unsigned int mpirank, const unsigned long pointID)
      {
        _print(value.central_values, label, vID, mpirank, pointID);
        _print(value.covariance, label, vID, mpirank, pointID);
      }

=======
    // Piggyback off existing print functions to build standard overloads
    USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, std::vector<double>)
    USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, map_str_dbl)
    USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, map_intpair_dbl)
    USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, ModelParameters)
    USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, triplet<double>)
    #ifndef SCANNER_STANDALONE
      USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, DM_nucleon_couplings)
      USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, DM_nucleon_couplings_fermionic_HP)
      USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, Flav_KstarMuMu_obs)
      USE_COMMON_PRINT_OVERLOAD(HDF5Printer2, BBN_container)
>>>>>>> master
    #endif

    /// @}

  }
}

