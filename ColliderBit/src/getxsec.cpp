//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Module functions for computing cross-sections
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2019 Feb, May
///
///  \author Anders Kvellestad
///          (a.kvellestad@imperial.ac.uk)
///  \date 2019 Sep
///
///  *********************************************

#include "gambit/ColliderBit/ColliderBit_eventloop.hpp"
#include "gambit/ColliderBit/complete_process_PID_pair_multimaps.hpp"

#define COLLIDERBIT_DEBUG
#define DEBUG_PREFIX "DEBUG: OMP thread " << omp_get_thread_num() << ":  "

namespace Gambit
{

  namespace ColliderBit
  {

    // ======= Utility functions =======


    /// Helper function that takes a cross-section value in fb or pb, 
    /// along with an absolute or relative uncertainty, and returns the 
    /// xsec and absolute uncertainty in fb.
    std::pair<double,double> convert_xsecs_to_fb(double input_xsec, double input_xsec_uncert, str input_unit, bool input_fractional_uncert)
    {
      double xsec_fb;
      double xsec_uncert_fb;

      if (input_unit == "fb" && !input_fractional_uncert)
      {
        xsec_fb = input_xsec;
        xsec_uncert_fb = input_xsec_uncert;
      }
      else if (input_unit == "fb" && input_fractional_uncert)
      {
        xsec_fb = input_xsec;
        xsec_uncert_fb = input_xsec_uncert * xsec_fb;
      }
      else if (input_unit == "pb" && !input_fractional_uncert)
      {
        xsec_fb = input_xsec * 1000.;
        xsec_uncert_fb = input_xsec_uncert * 1000.;
      }
      else if (input_unit == "pb" && input_fractional_uncert)
      {
        xsec_fb = input_xsec * 1000.;
        xsec_uncert_fb = input_xsec_uncert * xsec_fb;
      }
      else
      {
        ColliderBit_error().raise(LOCAL_INFO, "Unknown combination of options for function convert_xsecs_to_fb.");
      }      

      return std::make_pair(xsec_fb, xsec_uncert_fb);
    }



    // ======= Module functions =======


    /// Dummy function for testing out how to return a PIDPairCrossSectionFunc
    xsec_container PIDPairCrossSection_dummy(const PID_pair& pids, const Spectrum& MSSM_spectrum)
    {
      xsec_container xs_result;

      // Get an SLHA1 object
      const SLHAstruct& slha = MSSM_spectrum.getSLHAea(1);

      // "Calculate" cross section
      // (only stop-stopbar production)
      double xs_fb = 0.0;
      if (pids.pid1() == -1000002 && pids.pid2() == 1000002)
      {
        xs_fb = 3.09816e-11 + 9.08223e-13;
      }
      else
      {
        xs_fb = 1.0;
      }
      double xs_rel_err = 0.01;
      // double xs_err = xs_fb * xs_rel_err;

      // Save result in xs_result
      xs_result.set_xsec(xs_fb, xs_rel_err);

      // Construct info string of the form "PID1:<PID1>, PID2:<PID2>"
      std::stringstream info_ss;
      info_ss << "PID1:" << pids.pid1() << ", " << "PID2:" << pids.pid2();
      xs_result.set_info_string(info_ss.str());

      return xs_result;
    }





    /// Test functions for provding PIDPairCrossSectionsMap (cross-sections in fb)
    PID_pair_xsec_container silly_pid_xsec_constructor(PID_pair pid_pair, double xsec_val)
    {
      PID_pair_xsec_container result;

      result.reset();
      result.set_pid_pair(pid_pair);
      result.set_xsec(xsec_val, xsec_val * 0.01);

      return result;
    }

    void getPIDPairCrossSectionsMap_testing(map_PID_pair_PID_pair_xsec& result)
    {
      using namespace Pipes::getPIDPairCrossSectionsMap_testing;

      static bool first = true;
      static map_PID_pair_PID_pair_xsec all_my_pid_pair_xsecs;
      if (first)
      {
        all_my_pid_pair_xsecs[PID_pair(1000021,1000021)] = silly_pid_xsec_constructor( PID_pair(1000021,1000021), 5.54434e+01);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000021)] = silly_pid_xsec_constructor( PID_pair(1000001,1000021), 2.49311e+01);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000021)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000021), 2.49311e+01);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000021)] = silly_pid_xsec_constructor( PID_pair(1000002,1000021), 6.40216e+01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000021)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000021), 6.40216e+01);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000021)] = silly_pid_xsec_constructor( PID_pair(1000003,1000021), 1.66806e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000021)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000021), 1.66806e+00);
        all_my_pid_pair_xsecs[PID_pair(1000004,1000021)] = silly_pid_xsec_constructor( PID_pair(1000004,1000021), 2.27272e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000021)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000021), 2.27272e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000021)] = silly_pid_xsec_constructor( PID_pair(1000005,1000021), 7.31109e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000021)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000021), 7.31109e-01);
        all_my_pid_pair_xsecs[PID_pair(1000021,2000001)] = silly_pid_xsec_constructor( PID_pair(1000021,2000001), 2.77095e+01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000021)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000021), 2.77095e+01);
        all_my_pid_pair_xsecs[PID_pair(1000021,2000002)] = silly_pid_xsec_constructor( PID_pair(1000021,2000002), 7.10295e+01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000021)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000021), 7.10295e+01);
        all_my_pid_pair_xsecs[PID_pair(1000021,2000003)] = silly_pid_xsec_constructor( PID_pair(1000021,2000003), 1.76121e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000021)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000021), 1.76121e+00);
        all_my_pid_pair_xsecs[PID_pair(1000021,2000004)] = silly_pid_xsec_constructor( PID_pair(1000021,2000004), 2.06936e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000021)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000021), 2.06936e+00);
        all_my_pid_pair_xsecs[PID_pair(1000021,2000005)] = silly_pid_xsec_constructor( PID_pair(1000021,2000005), 8.42707e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000021)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000021), 8.42707e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000001)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000001), 4.01371e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000002)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000002), 4.92314e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000003)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000003), 3.19973e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000004)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000004), 2.65451e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000005)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000005), 6.69967e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000006,1000006)] = silly_pid_xsec_constructor( PID_pair(-1000006,1000006), 2.07301e+01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,2000001)] = silly_pid_xsec_constructor( PID_pair(-2000001,2000001), 5.09805e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,2000002)] = silly_pid_xsec_constructor( PID_pair(-2000002,2000002), 6.79808e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,2000003)] = silly_pid_xsec_constructor( PID_pair(-2000003,2000003), 4.46973e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,2000004)] = silly_pid_xsec_constructor( PID_pair(-2000004,2000004), 3.34107e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,2000005)] = silly_pid_xsec_constructor( PID_pair(-2000005,2000005), 4.38953e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,2000006)] = silly_pid_xsec_constructor( PID_pair(-2000006,2000006), 4.36120e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000001)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000001), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000003)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000001)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000001), 3.17622e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000005)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000005), 3.17622e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000001)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000001), 5.87421e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000001,2000001)] = silly_pid_xsec_constructor( PID_pair(-1000001,2000001), 5.87421e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000001)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000001), 1.64119e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000001,2000003)] = silly_pid_xsec_constructor( PID_pair(-1000001,2000003), 1.64119e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000001)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000001), 6.56259e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000001,2000005)] = silly_pid_xsec_constructor( PID_pair(-1000001,2000005), 6.56259e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000002)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000002), 9.73674e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000004)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000004), 9.73674e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000002)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000002), 1.18265e+01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,2000002)] = silly_pid_xsec_constructor( PID_pair(-1000002,2000002), 1.18265e+01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000002)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000002), 3.32792e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,2000004)] = silly_pid_xsec_constructor( PID_pair(-1000002,2000004), 3.32792e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000002)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000002), 1.78791e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000001)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000001), 1.78791e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000002)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000002), 1.13208e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000003)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000003), 1.13208e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000002)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000002), 6.76058e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000005)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000005), 6.76058e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000002)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000002), 1.02447e+01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,2000001)] = silly_pid_xsec_constructor( PID_pair(-1000002,2000001), 1.02447e+01);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000002)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000002), 3.79778e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,2000003)] = silly_pid_xsec_constructor( PID_pair(-1000002,2000003), 3.79778e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000002)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000002), 1.90689e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,2000005)] = silly_pid_xsec_constructor( PID_pair(-1000002,2000005), 1.90689e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000003)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000005)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000003)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000003), 2.24316e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,2000001)] = silly_pid_xsec_constructor( PID_pair(-1000003,2000001), 2.24316e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000003)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000003), 9.96403e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000003,2000003)] = silly_pid_xsec_constructor( PID_pair(-1000003,2000003), 9.96403e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000003)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000003), 1.12844e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000003,2000005)] = silly_pid_xsec_constructor( PID_pair(-1000003,2000005), 1.12844e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000004)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000004), 3.42360e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,2000002)] = silly_pid_xsec_constructor( PID_pair(-1000004,2000002), 3.42360e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000004)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,2000004)] = silly_pid_xsec_constructor( PID_pair(-1000004,2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000004)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000004), 1.92106e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000001)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000001), 1.92106e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000004)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000004), 1.02749e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000003)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000003), 1.02749e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000004)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000004), 4.43727e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000005)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000005), 4.43727e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000004)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000004), 1.45607e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,2000001)] = silly_pid_xsec_constructor( PID_pair(-1000004,2000001), 1.45607e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000004)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000004), 3.30510e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000004,2000003)] = silly_pid_xsec_constructor( PID_pair(-1000004,2000003), 3.30510e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000004)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,2000005)] = silly_pid_xsec_constructor( PID_pair(-1000004,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000005)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000005), 1.26973e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,2000001)] = silly_pid_xsec_constructor( PID_pair(-1000005,2000001), 1.26973e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000005)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000005), 1.03690e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000005,2000003)] = silly_pid_xsec_constructor( PID_pair(-1000005,2000003), 1.03690e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000005)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000005), 8.55325e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000005,2000005)] = silly_pid_xsec_constructor( PID_pair(-1000005,2000005), 8.55325e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000006,1000006)] = silly_pid_xsec_constructor( PID_pair(-2000006,1000006), 9.14170e-03);
        all_my_pid_pair_xsecs[PID_pair(-1000006,2000006)] = silly_pid_xsec_constructor( PID_pair(-1000006,2000006), 9.14170e-03);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000006)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000006,1000001)] = silly_pid_xsec_constructor( PID_pair(-1000006,1000001), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000006)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000006,1000003)] = silly_pid_xsec_constructor( PID_pair(-1000006,1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000006)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000006,1000005)] = silly_pid_xsec_constructor( PID_pair(-1000006,1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000006)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000006,2000005)] = silly_pid_xsec_constructor( PID_pair(-1000006,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,2000001)] = silly_pid_xsec_constructor( PID_pair(-2000003,2000001), 5.36949e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,2000003)] = silly_pid_xsec_constructor( PID_pair(-2000001,2000003), 5.36949e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,2000001)] = silly_pid_xsec_constructor( PID_pair(-2000005,2000001), 2.34895e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,2000005)] = silly_pid_xsec_constructor( PID_pair(-2000001,2000005), 2.34895e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,2000002)] = silly_pid_xsec_constructor( PID_pair(-2000004,2000002), 8.45901e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,2000004)] = silly_pid_xsec_constructor( PID_pair(-2000002,2000004), 8.45901e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000001,2000002)] = silly_pid_xsec_constructor( PID_pair(-1000001,2000002), 9.37927e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000001)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000001), 9.37927e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,2000002)] = silly_pid_xsec_constructor( PID_pair(-1000003,2000002), 4.54595e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000003)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000003), 4.54595e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,2000002)] = silly_pid_xsec_constructor( PID_pair(-1000005,2000002), 2.15512e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000005)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000005), 2.15512e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,2000002)] = silly_pid_xsec_constructor( PID_pair(-2000001,2000002), 2.38149e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,2000001)] = silly_pid_xsec_constructor( PID_pair(-2000002,2000001), 2.38149e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,2000002)] = silly_pid_xsec_constructor( PID_pair(-2000003,2000002), 9.53774e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,2000003)] = silly_pid_xsec_constructor( PID_pair(-2000002,2000003), 9.53774e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,2000002)] = silly_pid_xsec_constructor( PID_pair(-2000005,2000002), 6.39304e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,2000005)] = silly_pid_xsec_constructor( PID_pair(-2000002,2000005), 6.39304e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,2000003)] = silly_pid_xsec_constructor( PID_pair(-2000005,2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,2000005)] = silly_pid_xsec_constructor( PID_pair(-2000003,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000001,2000004)] = silly_pid_xsec_constructor( PID_pair(-1000001,2000004), 1.70272e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000001)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000001), 1.70272e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,2000004)] = silly_pid_xsec_constructor( PID_pair(-1000003,2000004), 6.04042e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000003)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000003), 6.04042e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000005,2000004)] = silly_pid_xsec_constructor( PID_pair(-1000005,2000004), 1.47515e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000005)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000005), 1.47515e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,2000004)] = silly_pid_xsec_constructor( PID_pair(-2000001,2000004), 3.01283e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,2000001)] = silly_pid_xsec_constructor( PID_pair(-2000004,2000001), 3.01283e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000003,2000004)] = silly_pid_xsec_constructor( PID_pair(-2000003,2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,2000003)] = silly_pid_xsec_constructor( PID_pair(-2000004,2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,2000004)] = silly_pid_xsec_constructor( PID_pair(-2000005,2000004), 3.26311e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000004,2000005)] = silly_pid_xsec_constructor( PID_pair(-2000004,2000005), 3.26311e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000001,2000006)] = silly_pid_xsec_constructor( PID_pair(-1000001,2000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,1000001)] = silly_pid_xsec_constructor( PID_pair(-2000006,1000001), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,2000006)] = silly_pid_xsec_constructor( PID_pair(-1000003,2000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,1000003)] = silly_pid_xsec_constructor( PID_pair(-2000006,1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,2000006)] = silly_pid_xsec_constructor( PID_pair(-1000005,2000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,1000005)] = silly_pid_xsec_constructor( PID_pair(-2000006,1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,2000006)] = silly_pid_xsec_constructor( PID_pair(-2000005,2000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,2000005)] = silly_pid_xsec_constructor( PID_pair(-2000006,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000001)] = silly_pid_xsec_constructor( PID_pair(1000001,1000001), 3.99936e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000001,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000001,-1000001), 3.99936e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000003)] = silly_pid_xsec_constructor( PID_pair(1000001,1000003), 7.19642e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000003,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000003,-1000001), 7.19642e-01);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000005)] = silly_pid_xsec_constructor( PID_pair(1000001,1000005), 5.71617e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000005,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000005,-1000001), 5.71617e-01);
        all_my_pid_pair_xsecs[PID_pair(1000001,2000001)] = silly_pid_xsec_constructor( PID_pair(1000001,2000001), 3.04093e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,-1000001)] = silly_pid_xsec_constructor( PID_pair(-2000001,-1000001), 3.04093e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,2000003)] = silly_pid_xsec_constructor( PID_pair(1000001,2000003), 2.31864e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000003,-1000001)] = silly_pid_xsec_constructor( PID_pair(-2000003,-1000001), 2.31864e-01);
        all_my_pid_pair_xsecs[PID_pair(1000001,2000005)] = silly_pid_xsec_constructor( PID_pair(1000001,2000005), 8.98993e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-1000001)] = silly_pid_xsec_constructor( PID_pair(-2000005,-1000001), 8.98993e-02);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000002)] = silly_pid_xsec_constructor( PID_pair(1000002,1000002), 2.04501e+01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,-1000002)] = silly_pid_xsec_constructor( PID_pair(-1000002,-1000002), 2.04501e+01);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000004)] = silly_pid_xsec_constructor( PID_pair(1000002,1000004), 1.54296e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,-1000002)] = silly_pid_xsec_constructor( PID_pair(-1000004,-1000002), 1.54296e+00);
        all_my_pid_pair_xsecs[PID_pair(1000002,2000002)] = silly_pid_xsec_constructor( PID_pair(1000002,2000002), 1.49920e+01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,-1000002)] = silly_pid_xsec_constructor( PID_pair(-2000002,-1000002), 1.49920e+01);
        all_my_pid_pair_xsecs[PID_pair(1000002,2000004)] = silly_pid_xsec_constructor( PID_pair(1000002,2000004), 3.73234e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-1000002)] = silly_pid_xsec_constructor( PID_pair(-2000004,-1000002), 3.73234e-01);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000002)] = silly_pid_xsec_constructor( PID_pair(1000001,1000002), 2.96892e+01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000002,-1000001), 2.96892e+01);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000003)] = silly_pid_xsec_constructor( PID_pair(1000002,1000003), 2.13849e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,-1000002)] = silly_pid_xsec_constructor( PID_pair(-1000003,-1000002), 2.13849e+00);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000005)] = silly_pid_xsec_constructor( PID_pair(1000002,1000005), 3.74546e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000005,-1000002)] = silly_pid_xsec_constructor( PID_pair(-1000005,-1000002), 3.74546e-01);
        all_my_pid_pair_xsecs[PID_pair(1000002,2000001)] = silly_pid_xsec_constructor( PID_pair(1000002,2000001), 6.66739e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,-1000002)] = silly_pid_xsec_constructor( PID_pair(-2000001,-1000002), 6.66739e+00);
        all_my_pid_pair_xsecs[PID_pair(1000002,2000003)] = silly_pid_xsec_constructor( PID_pair(1000002,2000003), 5.66380e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000003,-1000002)] = silly_pid_xsec_constructor( PID_pair(-2000003,-1000002), 5.66380e-01);
        all_my_pid_pair_xsecs[PID_pair(1000002,2000005)] = silly_pid_xsec_constructor( PID_pair(1000002,2000005), 9.83711e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-1000002)] = silly_pid_xsec_constructor( PID_pair(-2000005,-1000002), 9.83711e-01);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000003)] = silly_pid_xsec_constructor( PID_pair(1000003,1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,-1000003)] = silly_pid_xsec_constructor( PID_pair(-1000003,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000005)] = silly_pid_xsec_constructor( PID_pair(1000003,1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,-1000003)] = silly_pid_xsec_constructor( PID_pair(-1000005,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,2000001)] = silly_pid_xsec_constructor( PID_pair(1000003,2000001), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,-1000003)] = silly_pid_xsec_constructor( PID_pair(-2000001,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,2000003)] = silly_pid_xsec_constructor( PID_pair(1000003,2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,-1000003)] = silly_pid_xsec_constructor( PID_pair(-2000003,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,2000005)] = silly_pid_xsec_constructor( PID_pair(1000003,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-1000003)] = silly_pid_xsec_constructor( PID_pair(-2000005,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000004,1000004)] = silly_pid_xsec_constructor( PID_pair(1000004,1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,-1000004)] = silly_pid_xsec_constructor( PID_pair(-1000004,-1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000004,2000002)] = silly_pid_xsec_constructor( PID_pair(1000004,2000002), 4.36102e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,-1000004)] = silly_pid_xsec_constructor( PID_pair(-2000002,-1000004), 4.36102e-01);
        all_my_pid_pair_xsecs[PID_pair(1000004,2000004)] = silly_pid_xsec_constructor( PID_pair(1000004,2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-1000004)] = silly_pid_xsec_constructor( PID_pair(-2000004,-1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000004)] = silly_pid_xsec_constructor( PID_pair(1000001,1000004), 9.81683e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000004,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000004,-1000001), 9.81683e-01);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000004)] = silly_pid_xsec_constructor( PID_pair(1000003,1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,-1000003)] = silly_pid_xsec_constructor( PID_pair(-1000004,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000004,1000005)] = silly_pid_xsec_constructor( PID_pair(1000004,1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,-1000004)] = silly_pid_xsec_constructor( PID_pair(-1000005,-1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000004,2000001)] = silly_pid_xsec_constructor( PID_pair(1000004,2000001), 2.01767e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,-1000004)] = silly_pid_xsec_constructor( PID_pair(-2000001,-1000004), 2.01767e-01);
        all_my_pid_pair_xsecs[PID_pair(1000004,2000003)] = silly_pid_xsec_constructor( PID_pair(1000004,2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,-1000004)] = silly_pid_xsec_constructor( PID_pair(-2000003,-1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000004,2000005)] = silly_pid_xsec_constructor( PID_pair(1000004,2000005), 5.71613e-03);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-1000004)] = silly_pid_xsec_constructor( PID_pair(-2000005,-1000004), 5.71613e-03);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000005)] = silly_pid_xsec_constructor( PID_pair(1000005,1000005), 2.75871e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000005,-1000005)] = silly_pid_xsec_constructor( PID_pair(-1000005,-1000005), 2.75871e-02);
        all_my_pid_pair_xsecs[PID_pair(1000005,2000001)] = silly_pid_xsec_constructor( PID_pair(1000005,2000001), 1.44240e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,-1000005)] = silly_pid_xsec_constructor( PID_pair(-2000001,-1000005), 1.44240e-01);
        all_my_pid_pair_xsecs[PID_pair(1000005,2000003)] = silly_pid_xsec_constructor( PID_pair(1000005,2000003), 4.47358e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000003,-1000005)] = silly_pid_xsec_constructor( PID_pair(-2000003,-1000005), 4.47358e-02);
        all_my_pid_pair_xsecs[PID_pair(1000005,2000005)] = silly_pid_xsec_constructor( PID_pair(1000005,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-1000005)] = silly_pid_xsec_constructor( PID_pair(-2000005,-1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000006)] = silly_pid_xsec_constructor( PID_pair(1000001,1000006), 1.32323e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000006,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000006,-1000001), 1.32323e-01);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000006)] = silly_pid_xsec_constructor( PID_pair(1000003,1000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000006,-1000003)] = silly_pid_xsec_constructor( PID_pair(-1000006,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000006)] = silly_pid_xsec_constructor( PID_pair(1000005,1000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000006,-1000005)] = silly_pid_xsec_constructor( PID_pair(-1000006,-1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000006,2000005)] = silly_pid_xsec_constructor( PID_pair(1000006,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-1000006)] = silly_pid_xsec_constructor( PID_pair(-2000005,-1000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(2000001,2000001)] = silly_pid_xsec_constructor( PID_pair(2000001,2000001), 4.31470e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,-2000001)] = silly_pid_xsec_constructor( PID_pair(-2000001,-2000001), 4.31470e+00);
        all_my_pid_pair_xsecs[PID_pair(2000001,2000003)] = silly_pid_xsec_constructor( PID_pair(2000001,2000003), 1.26186e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,-2000001)] = silly_pid_xsec_constructor( PID_pair(-2000003,-2000001), 1.26186e+00);
        all_my_pid_pair_xsecs[PID_pair(2000001,2000005)] = silly_pid_xsec_constructor( PID_pair(2000001,2000005), 3.77398e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-2000001)] = silly_pid_xsec_constructor( PID_pair(-2000005,-2000001), 3.77398e-01);
        all_my_pid_pair_xsecs[PID_pair(2000002,2000002)] = silly_pid_xsec_constructor( PID_pair(2000002,2000002), 2.18019e+01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,-2000002)] = silly_pid_xsec_constructor( PID_pair(-2000002,-2000002), 2.18019e+01);
        all_my_pid_pair_xsecs[PID_pair(2000002,2000004)] = silly_pid_xsec_constructor( PID_pair(2000002,2000004), 1.85184e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-2000002)] = silly_pid_xsec_constructor( PID_pair(-2000004,-2000002), 1.85184e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,2000002)] = silly_pid_xsec_constructor( PID_pair(1000001,2000002), 6.34197e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,-1000001)] = silly_pid_xsec_constructor( PID_pair(-2000002,-1000001), 6.34197e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,2000002)] = silly_pid_xsec_constructor( PID_pair(1000003,2000002), 4.73805e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,-1000003)] = silly_pid_xsec_constructor( PID_pair(-2000002,-1000003), 4.73805e-01);
        all_my_pid_pair_xsecs[PID_pair(1000005,2000002)] = silly_pid_xsec_constructor( PID_pair(1000005,2000002), 8.74804e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,-1000005)] = silly_pid_xsec_constructor( PID_pair(-2000002,-1000005), 8.74804e-01);
        all_my_pid_pair_xsecs[PID_pair(2000001,2000002)] = silly_pid_xsec_constructor( PID_pair(2000001,2000002), 2.38671e+01);
        all_my_pid_pair_xsecs[PID_pair(-2000002,-2000001)] = silly_pid_xsec_constructor( PID_pair(-2000002,-2000001), 2.38671e+01);
        all_my_pid_pair_xsecs[PID_pair(2000002,2000003)] = silly_pid_xsec_constructor( PID_pair(2000002,2000003), 2.21841e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,-2000002)] = silly_pid_xsec_constructor( PID_pair(-2000003,-2000002), 2.21841e+00);
        all_my_pid_pair_xsecs[PID_pair(2000002,2000005)] = silly_pid_xsec_constructor( PID_pair(2000002,2000005), 3.17342e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-2000002)] = silly_pid_xsec_constructor( PID_pair(-2000005,-2000002), 3.17342e-01);
        all_my_pid_pair_xsecs[PID_pair(2000003,2000003)] = silly_pid_xsec_constructor( PID_pair(2000003,2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,-2000003)] = silly_pid_xsec_constructor( PID_pair(-2000003,-2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(2000003,2000005)] = silly_pid_xsec_constructor( PID_pair(2000003,2000005), 6.33432e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-2000003)] = silly_pid_xsec_constructor( PID_pair(-2000005,-2000003), 6.33432e-02);
        all_my_pid_pair_xsecs[PID_pair(2000004,2000004)] = silly_pid_xsec_constructor( PID_pair(2000004,2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-2000004)] = silly_pid_xsec_constructor( PID_pair(-2000004,-2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,2000004)] = silly_pid_xsec_constructor( PID_pair(1000001,2000004), 1.92684e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-1000001)] = silly_pid_xsec_constructor( PID_pair(-2000004,-1000001), 1.92684e-01);
        all_my_pid_pair_xsecs[PID_pair(1000003,2000004)] = silly_pid_xsec_constructor( PID_pair(1000003,2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-1000003)] = silly_pid_xsec_constructor( PID_pair(-2000004,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,2000004)] = silly_pid_xsec_constructor( PID_pair(1000005,2000004), 2.41334e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-1000005)] = silly_pid_xsec_constructor( PID_pair(-2000004,-1000005), 2.41334e-02);
        all_my_pid_pair_xsecs[PID_pair(2000001,2000004)] = silly_pid_xsec_constructor( PID_pair(2000001,2000004), 8.91270e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-2000001)] = silly_pid_xsec_constructor( PID_pair(-2000004,-2000001), 8.91270e-01);
        all_my_pid_pair_xsecs[PID_pair(2000003,2000004)] = silly_pid_xsec_constructor( PID_pair(2000003,2000004), 1.24964e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,-2000003)] = silly_pid_xsec_constructor( PID_pair(-2000004,-2000003), 1.24964e-01);
        all_my_pid_pair_xsecs[PID_pair(2000004,2000005)] = silly_pid_xsec_constructor( PID_pair(2000004,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-2000004)] = silly_pid_xsec_constructor( PID_pair(-2000005,-2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(2000005,2000005)] = silly_pid_xsec_constructor( PID_pair(2000005,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-2000005)] = silly_pid_xsec_constructor( PID_pair(-2000005,-2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,2000006)] = silly_pid_xsec_constructor( PID_pair(1000001,2000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,-1000001)] = silly_pid_xsec_constructor( PID_pair(-2000006,-1000001), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,2000006)] = silly_pid_xsec_constructor( PID_pair(1000003,2000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,-1000003)] = silly_pid_xsec_constructor( PID_pair(-2000006,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,2000006)] = silly_pid_xsec_constructor( PID_pair(1000005,2000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,-1000005)] = silly_pid_xsec_constructor( PID_pair(-2000006,-1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(2000005,2000006)] = silly_pid_xsec_constructor( PID_pair(2000005,2000006), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000006,-2000005)] = silly_pid_xsec_constructor( PID_pair(-2000006,-2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000022)] = silly_pid_xsec_constructor( PID_pair(1000001,1000022), 5.01451e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000022)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000022), 5.01451e-02);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000022)] = silly_pid_xsec_constructor( PID_pair(1000002,1000022), 1.75806e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000022)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000022), 1.75806e-01);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000022)] = silly_pid_xsec_constructor( PID_pair(1000003,1000022), 1.18709e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000022)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000022), 1.18709e-02);
        all_my_pid_pair_xsecs[PID_pair(1000004,1000022)] = silly_pid_xsec_constructor( PID_pair(1000004,1000022), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000022)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000022), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000022)] = silly_pid_xsec_constructor( PID_pair(1000005,1000022), 1.65569e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000022)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000022), 1.65569e-02);
        all_my_pid_pair_xsecs[PID_pair(1000022,2000001)] = silly_pid_xsec_constructor( PID_pair(1000022,2000001), 5.55745e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000022)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000022), 5.55745e-01);
        all_my_pid_pair_xsecs[PID_pair(1000022,2000002)] = silly_pid_xsec_constructor( PID_pair(1000022,2000002), 4.30190e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000022)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000022), 4.30190e+00);
        all_my_pid_pair_xsecs[PID_pair(1000022,2000003)] = silly_pid_xsec_constructor( PID_pair(1000022,2000003), 9.10268e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000022)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000022), 9.10268e-02);
        all_my_pid_pair_xsecs[PID_pair(1000022,2000004)] = silly_pid_xsec_constructor( PID_pair(1000022,2000004), 1.52409e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000022)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000022), 1.52409e-01);
        all_my_pid_pair_xsecs[PID_pair(1000022,2000005)] = silly_pid_xsec_constructor( PID_pair(1000022,2000005), 9.19629e-02);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000022)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000022), 9.19629e-02);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000023)] = silly_pid_xsec_constructor( PID_pair(1000001,1000023), 1.70233e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000023)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000023), 1.70233e+00);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000023)] = silly_pid_xsec_constructor( PID_pair(1000002,1000023), 4.29508e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000023)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000023), 4.29508e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000023)] = silly_pid_xsec_constructor( PID_pair(1000003,1000023), 2.64129e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000023)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000023), 2.64129e-01);
        all_my_pid_pair_xsecs[PID_pair(1000004,1000023)] = silly_pid_xsec_constructor( PID_pair(1000004,1000023), 2.55986e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000023)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000023), 2.55986e-01);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000023)] = silly_pid_xsec_constructor( PID_pair(1000005,1000023), 2.37119e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000023)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000023), 2.37119e-01);
        all_my_pid_pair_xsecs[PID_pair(1000023,2000001)] = silly_pid_xsec_constructor( PID_pair(1000023,2000001), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000023)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000023), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000023,2000002)] = silly_pid_xsec_constructor( PID_pair(1000023,2000002), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000023)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000023), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000023,2000003)] = silly_pid_xsec_constructor( PID_pair(1000023,2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000023)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000023), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000023,2000004)] = silly_pid_xsec_constructor( PID_pair(1000023,2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000023)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000023), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000023,2000005)] = silly_pid_xsec_constructor( PID_pair(1000023,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000023)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000023), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000025)] = silly_pid_xsec_constructor( PID_pair(1000001,1000025), 2.35775e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000025)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000025), 2.35775e-02);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000025)] = silly_pid_xsec_constructor( PID_pair(1000002,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000025)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000025)] = silly_pid_xsec_constructor( PID_pair(1000003,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000025)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000004,1000025)] = silly_pid_xsec_constructor( PID_pair(1000004,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000025)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000025)] = silly_pid_xsec_constructor( PID_pair(1000005,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000025)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000025,2000001)] = silly_pid_xsec_constructor( PID_pair(1000025,2000001), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000025)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000025,2000002)] = silly_pid_xsec_constructor( PID_pair(1000025,2000002), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000025)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000025,2000003)] = silly_pid_xsec_constructor( PID_pair(1000025,2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000025)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000025,2000004)] = silly_pid_xsec_constructor( PID_pair(1000025,2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000025)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000025,2000005)] = silly_pid_xsec_constructor( PID_pair(1000025,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000025)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000035)] = silly_pid_xsec_constructor( PID_pair(1000001,1000035), 6.38136e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000001,1000035)] = silly_pid_xsec_constructor( PID_pair(-1000001,1000035), 6.38136e-02);
        all_my_pid_pair_xsecs[PID_pair(1000002,1000035)] = silly_pid_xsec_constructor( PID_pair(1000002,1000035), 8.33878e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000035)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000035), 8.33878e-02);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000035)] = silly_pid_xsec_constructor( PID_pair(1000003,1000035), 1.81470e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000003,1000035)] = silly_pid_xsec_constructor( PID_pair(-1000003,1000035), 1.81470e-02);
        all_my_pid_pair_xsecs[PID_pair(1000004,1000035)] = silly_pid_xsec_constructor( PID_pair(1000004,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000035)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000035)] = silly_pid_xsec_constructor( PID_pair(1000005,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000005,1000035)] = silly_pid_xsec_constructor( PID_pair(-1000005,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000035,2000001)] = silly_pid_xsec_constructor( PID_pair(1000035,2000001), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000001,1000035)] = silly_pid_xsec_constructor( PID_pair(-2000001,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000035,2000002)] = silly_pid_xsec_constructor( PID_pair(1000035,2000002), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000002,1000035)] = silly_pid_xsec_constructor( PID_pair(-2000002,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000035,2000003)] = silly_pid_xsec_constructor( PID_pair(1000035,2000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000003,1000035)] = silly_pid_xsec_constructor( PID_pair(-2000003,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000035,2000004)] = silly_pid_xsec_constructor( PID_pair(1000035,2000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000004,1000035)] = silly_pid_xsec_constructor( PID_pair(-2000004,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000035,2000005)] = silly_pid_xsec_constructor( PID_pair(1000035,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,1000035)] = silly_pid_xsec_constructor( PID_pair(-2000005,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000024)] = silly_pid_xsec_constructor( PID_pair(1000001,1000024), 7.31154e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000024,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000024,-1000001), 7.31154e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000002)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000002), 7.75207e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000024)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000024), 7.75207e+00);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000024)] = silly_pid_xsec_constructor( PID_pair(1000003,1000024), 7.33177e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000024,-1000003)] = silly_pid_xsec_constructor( PID_pair(-1000024,-1000003), 7.33177e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000004)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000004), 1.28496e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000024)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000024), 1.28496e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000024)] = silly_pid_xsec_constructor( PID_pair(1000005,1000024), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000024,-1000005)] = silly_pid_xsec_constructor( PID_pair(-1000024,-1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000006)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000006), 8.73812e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000006,1000024)] = silly_pid_xsec_constructor( PID_pair(-1000006,1000024), 8.73812e-01);
        all_my_pid_pair_xsecs[PID_pair(1000024,2000005)] = silly_pid_xsec_constructor( PID_pair(1000024,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-1000024)] = silly_pid_xsec_constructor( PID_pair(-2000005,-1000024), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000024,2000006)] = silly_pid_xsec_constructor( PID_pair(-1000024,2000006), 4.59134e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000006,1000024)] = silly_pid_xsec_constructor( PID_pair(-2000006,1000024), 4.59134e-01);
        all_my_pid_pair_xsecs[PID_pair(1000001,1000037)] = silly_pid_xsec_constructor( PID_pair(1000001,1000037), 2.18123e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000037,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000037,-1000001), 2.18123e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000002)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000002), 2.00248e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000002,1000037)] = silly_pid_xsec_constructor( PID_pair(-1000002,1000037), 2.00248e-01);
        all_my_pid_pair_xsecs[PID_pair(1000003,1000037)] = silly_pid_xsec_constructor( PID_pair(1000003,1000037), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,-1000003)] = silly_pid_xsec_constructor( PID_pair(-1000037,-1000003), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000004)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000004), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000004,1000037)] = silly_pid_xsec_constructor( PID_pair(-1000004,1000037), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000005,1000037)] = silly_pid_xsec_constructor( PID_pair(1000005,1000037), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,-1000005)] = silly_pid_xsec_constructor( PID_pair(-1000037,-1000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000006)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000006), 1.27278e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000006,1000037)] = silly_pid_xsec_constructor( PID_pair(-1000006,1000037), 1.27278e+00);
        all_my_pid_pair_xsecs[PID_pair(1000037,2000005)] = silly_pid_xsec_constructor( PID_pair(1000037,2000005), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000005,-1000037)] = silly_pid_xsec_constructor( PID_pair(-2000005,-1000037), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,2000006)] = silly_pid_xsec_constructor( PID_pair(-1000037,2000006), 1.19550e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000006,1000037)] = silly_pid_xsec_constructor( PID_pair(-2000006,1000037), 1.19550e-01);
        all_my_pid_pair_xsecs[PID_pair(1000022,1000022)] = silly_pid_xsec_constructor( PID_pair(1000022,1000022), 5.10676e-01);
        all_my_pid_pair_xsecs[PID_pair(1000022,1000023)] = silly_pid_xsec_constructor( PID_pair(1000022,1000023), 3.91765e-02);
        all_my_pid_pair_xsecs[PID_pair(1000023,1000023)] = silly_pid_xsec_constructor( PID_pair(1000023,1000023), 1.02251e+00);
        all_my_pid_pair_xsecs[PID_pair(1000022,1000025)] = silly_pid_xsec_constructor( PID_pair(1000022,1000025), 6.34525e-02);
        all_my_pid_pair_xsecs[PID_pair(1000023,1000025)] = silly_pid_xsec_constructor( PID_pair(1000023,1000025), 2.04170e-01);
        all_my_pid_pair_xsecs[PID_pair(1000025,1000025)] = silly_pid_xsec_constructor( PID_pair(1000025,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000022,1000035)] = silly_pid_xsec_constructor( PID_pair(1000022,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000023,1000035)] = silly_pid_xsec_constructor( PID_pair(1000023,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000025,1000035)] = silly_pid_xsec_constructor( PID_pair(1000025,1000035), 9.93281e-01);
        all_my_pid_pair_xsecs[PID_pair(1000035,1000035)] = silly_pid_xsec_constructor( PID_pair(1000035,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000022,1000024)] = silly_pid_xsec_constructor( PID_pair(1000022,1000024), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000022)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000022), 1.19689e-01);
        all_my_pid_pair_xsecs[PID_pair(1000022,1000037)] = silly_pid_xsec_constructor( PID_pair(1000022,1000037), 1.31157e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000022)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000022), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000023,1000024)] = silly_pid_xsec_constructor( PID_pair(1000023,1000024), 2.75428e+01);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000023)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000023), 1.26069e+01);
        all_my_pid_pair_xsecs[PID_pair(1000023,1000037)] = silly_pid_xsec_constructor( PID_pair(1000023,1000037), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000023)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000023), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000024,1000025)] = silly_pid_xsec_constructor( PID_pair(1000024,1000025), 3.25508e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000025)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000025), 1.65222e-01);
        all_my_pid_pair_xsecs[PID_pair(1000025,1000037)] = silly_pid_xsec_constructor( PID_pair(1000025,1000037), 1.75627e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000025)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000025), 8.81303e-01);
        all_my_pid_pair_xsecs[PID_pair(1000024,1000035)] = silly_pid_xsec_constructor( PID_pair(1000024,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000035)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000035), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000035,1000037)] = silly_pid_xsec_constructor( PID_pair(1000035,1000037), 1.92906e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000035)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000035), 6.36864e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000024)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000024), 2.27271e+01);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000024)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000024), 2.86751e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000037)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000037), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000037)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000037), 1.51063e+00);
        all_my_pid_pair_xsecs[PID_pair(1000021,1000022)] = silly_pid_xsec_constructor( PID_pair(1000021,1000022), 3.42889e+00);
        all_my_pid_pair_xsecs[PID_pair(1000021,1000023)] = silly_pid_xsec_constructor( PID_pair(1000021,1000023), 2.98531e+00);
        all_my_pid_pair_xsecs[PID_pair(1000021,1000025)] = silly_pid_xsec_constructor( PID_pair(1000021,1000025), 0.00000e+00);
        all_my_pid_pair_xsecs[PID_pair(1000021,1000035)] = silly_pid_xsec_constructor( PID_pair(1000021,1000035), 1.57247e-01);
        all_my_pid_pair_xsecs[PID_pair(1000021,1000024)] = silly_pid_xsec_constructor( PID_pair(1000021,1000024), 3.95918e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000024,1000021)] = silly_pid_xsec_constructor( PID_pair(-1000024,1000021), 3.95918e+00);
        all_my_pid_pair_xsecs[PID_pair(1000021,1000037)] = silly_pid_xsec_constructor( PID_pair(1000021,1000037), 8.28733e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000037,1000021)] = silly_pid_xsec_constructor( PID_pair(-1000037,1000021), 8.28733e-02);
        all_my_pid_pair_xsecs[PID_pair(-1000011,1000011)] = silly_pid_xsec_constructor( PID_pair(-1000011,1000011), 1.44094e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000012,1000012)] = silly_pid_xsec_constructor( PID_pair(-1000012,1000012), 1.68413e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000011,1000012)] = silly_pid_xsec_constructor( PID_pair(-1000011,1000012), 5.97540e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000012,1000011)] = silly_pid_xsec_constructor( PID_pair(-1000012,1000011), 5.97540e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000013,1000013)] = silly_pid_xsec_constructor( PID_pair(-1000013,1000013), 1.60394e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000014,1000014)] = silly_pid_xsec_constructor( PID_pair(-1000014,1000014), 1.24109e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000013,1000014)] = silly_pid_xsec_constructor( PID_pair(-1000013,1000014), 6.12722e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000014,1000013)] = silly_pid_xsec_constructor( PID_pair(-1000014,1000013), 6.12722e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000015,1000015)] = silly_pid_xsec_constructor( PID_pair(-1000015,1000015), 4.65549e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000015,1000015)] = silly_pid_xsec_constructor( PID_pair(-2000015,1000015), 3.61599e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000015,2000015)] = silly_pid_xsec_constructor( PID_pair(-1000015,2000015), 3.69232e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000016,1000016)] = silly_pid_xsec_constructor( PID_pair(-1000016,1000016), 1.91513e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000015,1000016)] = silly_pid_xsec_constructor( PID_pair(-1000015,1000016), 2.24581e-01);
        all_my_pid_pair_xsecs[PID_pair(-1000016,1000015)] = silly_pid_xsec_constructor( PID_pair(-1000016,1000015), 2.24581e-01);
        all_my_pid_pair_xsecs[PID_pair(-2000015,1000016)] = silly_pid_xsec_constructor( PID_pair(-2000015,1000016), 4.89826e+00);
        all_my_pid_pair_xsecs[PID_pair(-1000016,2000015)] = silly_pid_xsec_constructor( PID_pair(-1000016,2000015), 4.89826e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000011,2000011)] = silly_pid_xsec_constructor( PID_pair(-2000011,2000011), 4.18273e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000013,2000013)] = silly_pid_xsec_constructor( PID_pair(-2000013,2000013), 4.08130e+00);
        all_my_pid_pair_xsecs[PID_pair(-2000015,2000015)] = silly_pid_xsec_constructor( PID_pair(-2000015,2000015), 1.19703e+00);


        // Now multiply all cross-sections by 0.5 for all pairs of c.c. final states
        // in the all_my_pid_pair_xsecs map.

        // // _Anders DEBUG
        // // 1215, 1231
        // all_my_pid_pair_xsecs[PID_pair(1000001,-1000001)] = silly_pid_xsec_constructor( PID_pair(1000001,-1000001), 3.8695555984);

        // // 1351
        // all_my_pid_pair_xsecs[PID_pair(1000001,1000001)] = silly_pid_xsec_constructor( PID_pair(1000001,1000001), 0.5 * 8.3286182374);
        // all_my_pid_pair_xsecs[PID_pair(-1000001,-1000001)] = silly_pid_xsec_constructor( PID_pair(-1000001,-1000001), 0.5 * 8.3286182374);

      }

      if(*Loop::iteration == COLLIDER_INIT)
      {
        result.clear();
      }

      if(*Loop::iteration == XSEC_CALCULATION)
      {
        for (const PID_pair& pid_pair : *Dep::ActivePIDPairs)
        {
          cout << DEBUG_PREFIX << "getPIDPairCrossSectionsMap_testing: " << "Looking up xsec for [" << pid_pair.pid1() << "," << pid_pair.pid2() << "]." << endl;

          result[pid_pair] = all_my_pid_pair_xsecs.at(pid_pair);

          // try
          // {
          //   result[pid_pair] = all_my_pid_pair_xsecs.at(pid_pair);
          // }
          // catch (const std::out_of_range& err)
          // {
          //   PID_pair cc_pid_pair = pid_pair.cc_pid_pair();
          //   cout << DEBUG_PREFIX << "getPIDPairCrossSectionsMap_testing: " << "--> Not found! Trying again with [" << cc_pid_pair.pid1() << "," << cc_pid_pair.pid2() << "]." << endl;            
          //   // result[pid_pair] = all_my_pid_pair_xsecs.at(cc_pid_pair);
          //   try
          //   {
          //     result[pid_pair] = all_my_pid_pair_xsecs.at(cc_pid_pair);
          //   }
          //   catch (const std::out_of_range& err)
          //   {
          //     cout << DEBUG_PREFIX << "getPIDPairCrossSectionsMap_testing: " << "--> --> Also not found! Trying again with [" << cc_pid_pair.pid1() << "," << cc_pid_pair.pid2() << "]." << endl;                        
          //   }
          // }
        }
      } // end iteration

    }



    /// Get a PIDPairCrossSectionFunc pointing to PIDPairCrossSection_dummy,
    /// with the second argument already filled by *Dep::MSSM_spectrum
    void getPIDPairCrossSectionFunc_dummy(PIDPairCrossSectionFuncType& result)
    {
      using namespace Pipes::getPIDPairCrossSectionFunc_dummy;
      result = std::bind(PIDPairCrossSection_dummy, std::placeholders::_1, *Dep::MSSM_spectrum);
    }





    /// Get the cross-section from the xsec_example backend
    xsec_container PIDPairCrossSection_xsec_example(PID_pair pids, 
                                          const Spectrum& MSSM_spectrum)
                                          // double (*xsec_fb_fptr)(iipair&, map_str_dbl&, map_str_bool&))
    {
      xsec_container xs_result;

      // Get an SLHA1 object
      const SLHAstruct& slha = MSSM_spectrum.getSLHAea(1);

      // Call cross-section calculation 
      map_str_dbl proc_params;
      map_str_bool proc_flags;

      double xs_fb = 3.1415;

      // double xs_fb = 1.0e-3 * xsec_fb_fptr(pids, proc_params, proc_flags);

      double xs_rel_err = 0.01;

      // Save result in xs_result
      xs_result.set_xsec(xs_fb, xs_rel_err);

      // Construct info string of the form "PID1:<PID1>, PID2:<PID2>"
      std::stringstream info_ss;
      info_ss << "PID1:" << pids.pid1() << ", " << "PID2:" << pids.pid2();
      xs_result.set_info_string(info_ss.str());

      return xs_result;
    }


    /// Get a PIDPairCrossSectionFunc pointing to the xsec_example backend
    void getPIDPairCrossSectionFunc_xsec_example(PIDPairCrossSectionFuncType& result)
    {
      using namespace Pipes::getPIDPairCrossSectionFunc_xsec_example;

      // result = std::bind(PIDPairCrossSection_xsec_example, std::placeholders::_1, *Dep::MSSM_spectrum, BEreq::xsec_example_xsec_fb.pointer());
      result = std::bind(PIDPairCrossSection_xsec_example, std::placeholders::_1, *Dep::MSSM_spectrum);

      // Bind to the backend function xsec_fb with the two first arguments fixed

      // // Get an SLHA1 object
      // const SLHAstruct& slha = MSSM_spectrum.getSLHAea(1);

      // _Anders
      // BACKEND_REQ(xsec_example_xsec_fb, (), double, (std::vector<double>&, map_str_dbl&, map_str_bool&))

      // map_str_dbl proc_params;
      // map_str_bool proc_flags;

      // result = std::bind(BEreq::xsec_example_xsec_fb.pointer(), std::placeholders::_1, proc_params, proc_flags);
    }





    /// Get a map between Pythia process codes and cross-sections
    void getProcessCrossSectionsMap(map_int_process_xsec& result)
    {
      using namespace Pipes::getProcessCrossSectionsMap;

      // Use a static variable to communicate the result calculated on thread 0 during 
      // iteration XSEC_CALCULATION to all threads during iteration START_SUBPROCESS
      static map_int_process_xsec shared_result;

      const static bool set_missing_xsecs_to_zero = runOptions->getValueOrDef<bool>(false, "set_missing_xsecs_to_zero");

      // Only thread 0
      if(*Loop::iteration == COLLIDER_INIT)
      {
        shared_result.clear();
      }

      // All threads
      if(*Loop::iteration == COLLIDER_INIT_OMP)
      {
        result.clear();
      }

      // Only thread 0
      if(*Loop::iteration == XSEC_CALCULATION)
      {
        cout << DEBUG_PREFIX << "getProcessCrossSectionsMap: it = XSEC_CALCULATION, ActiveProcessCodes.size() = " << Dep::ActiveProcessCodes->size() << endl;          
        cout << DEBUG_PREFIX << "getProcessCrossSectionsMap: it = XSEC_CALCULATION, ActiveProcessCodeToPIDPairsMap.size() = " << Dep::ActiveProcessCodeToPIDPairsMap->size() << endl;          

        // Loop over all active processes and construct the cross-section map (shared_result)
        for (size_t i = 0; i != Dep::ActiveProcessCodes->size(); ++i)
        {
          // Get process code
          int proc_code = Dep::ActiveProcessCodes->at(i);

          // Construct a process_xsec_container instance to be stored in the shared_result map
          process_xsec_container proc_xs;
          proc_xs.set_process_code(proc_code);

          // Get iterator bounds (as a pair) over the multimap entries that match the key proc_code
          auto mm_proc2pid_range = Dep::ActiveProcessCodeToPIDPairsMap->equal_range(proc_code);

          // Loop over these elements in the multimap
          for (auto mm_it = mm_proc2pid_range.first; mm_it != mm_proc2pid_range.second; ++mm_it)
          {
            const PID_pair& pids = mm_it->second;

            // Obtain the cross-section from the PID pair via the PIDPairCrossSectionsMap (map_PID_pair_PID_pair_xsec) dependency
            cout << DEBUG_PREFIX << "Looking up PID pair: " << pids.pid1() << "," << pids.pid2() << endl;

            PID_pair_xsec_container pids_xs;
            map_PID_pair_PID_pair_xsec::const_iterator iter = Dep::PIDPairCrossSectionsMap->find(pids);
            if (iter != Dep::PIDPairCrossSectionsMap->end())
            {
              pids_xs = iter->second;
              cout << DEBUG_PREFIX << "--> Got it! Cross-section is: " << pids_xs.xsec() << " fb" << endl;
            }
            else
            {
              if(set_missing_xsecs_to_zero)
              {
                cout << DEBUG_PREFIX << "--> Not found! Creating 0-valued xsec_container " << endl;
                pids_xs.set_xsec(0.0, 0.0);
              }
              else
              {
                std::stringstream errmsg_ss;
                errmsg_ss << "No cross-section provided for PID pair [" << pids.pid1() << "," << pids.pid2() <<"]. ";
                ColliderBit_error().raise(LOCAL_INFO, errmsg_ss.str());
              }
            }

            // Accumulate result in the process_xsec_container proc_xs
            proc_xs.sum_xsecs(pids_xs.xsec(), pids_xs.xsec_err());
            proc_xs.add_related_pid_pair(pids);


            // Check if the current PID pair is related to any other process_codes,
            // using the multimap returned by all_PID_pairs_to_process_codes().
            // If yes, check that these processes are among the active processes
            // and register them in the proc_xs instance as processes sharing the cross-section

            // Get iterator bounds (as a pair) over the multimap entries that match the key pids
            auto mm_pid2proc_range = all_PID_pairs_to_process_codes().equal_range(pids);

            // Loop over these elements in the multimap
            for (auto mm_it = mm_pid2proc_range.first; mm_it != mm_pid2proc_range.second; ++mm_it)
            {
              // Get other process code
              int other_proc_code = mm_it->second;

              // Don't run around in circles...
              if(other_proc_code == proc_code) continue;

              // Check that other_proc_code is itself in one of the active processes, i.e. listed in Dep::ActiveProcessCodes
              if(std::find(Dep::ActiveProcessCodes->begin(), Dep::ActiveProcessCodes->end(), other_proc_code) != Dep::ActiveProcessCodes->end())  
              {
                // Add other_proc_code to the list of processes that share cross-section with proc_code
                proc_xs.add_process_sharing_xsec(other_proc_code);
              }
              else
              {
                std::stringstream errmsg_ss;
                errmsg_ss << "For correct cross-section scaling of collider process " << proc_code;
                errmsg_ss << ", process " << other_proc_code << " must also be activated. Please check your collider settings.";
                ColliderBit_error().raise(LOCAL_INFO, errmsg_ss.str());
              }
            }
          }

          // Construct info string of the form "ProcessCode:<proc_code>"
          std::stringstream info_ss;
          info_ss << "ProcessCode:" << proc_code;
          proc_xs.set_info_string(info_ss.str());

          // Store proc_xs in the shared_result map
          shared_result[proc_code] = proc_xs;
        }

        // Let thread 0 return the correct result already after iteration XSEC_CALCULATION
        result = shared_result;
      }


      // All threads
      if (*Loop::iteration == START_SUBPROCESS)
      {
        // All threads read the result from shared_result
        result = shared_result;
      }

    }


    /// Compute a cross-section from Monte Carlo
    void getMCxsec(MC_xsec_container& result)
    {
      using namespace Pipes::getMCxsec;

      // Don't bother if there are no analyses that will use this.
      if (Dep::RunMC->analyses.empty()) return;

      // Reset the xsec objects on all threads
      if (*Loop::iteration == COLLIDER_INIT_OMP)
      {
        result.reset();
      }

      // If we are in the main event loop, count the event towards cross-section normalisation on this thread
      if (*Loop::iteration >= 0)
      {
        result.log_event();
      }

      // Extract the xsecs from the MC on each thread
      if (*Loop::iteration == END_SUBPROCESS && Dep::RunMC->event_generation_began)
      {
        if (not Dep::RunMC->exceeded_maxFailedEvents)
        {
          const double xs_fb = (*Dep::HardScatteringSim)->xsec_fb();
          const double xserr_fb = (*Dep::HardScatteringSim)->xsecErr_fb();
          result.set_xsec(xs_fb, xserr_fb);
          #ifdef COLLIDERBIT_DEBUG
            cout << DEBUG_PREFIX << "xs_fb = " << xs_fb << " +/- " << xserr_fb << endl;
          #endif


          // _Anders DEBUG:
          std::vector<PID_pair> all_pid_pairs;
          std::map<int,int> pcode_counter;

          for (const std::pair<int,PID_pair>& elem : all_process_codes_to_PID_pairs)
          {
            int pcode = elem.first;
            const PID_pair& pids = elem.second;

            double LO_proc_xsec = (*Dep::HardScatteringSim)->xsec_fb(pcode);

            cout << std::fixed << std::setprecision(7);
            cout << DEBUG_PREFIX << "All xsecs:  " << pcode << ", [" 
                                                   << pids.pid1() << "," << pids.pid2() << "], "
                                                   << std::scientific << std::setprecision(5)
                                                   << LO_proc_xsec << endl;

            // Get list of all unique PID_pairs
            if (std::find(all_pid_pairs.begin(), all_pid_pairs.end(), pids) == all_pid_pairs.end())
            {
              all_pid_pairs.push_back( PID_pair(pids) );
            }

            // Cound pcode
            pcode_counter[pcode]++;
          }

          // Loop over PID_pairs
          for (const PID_pair& pids : all_pid_pairs)
          {
            double pids_xsec_val = 0.0;

            double sgn_pid1 = double(pids.pid1()) / double(abs(pids.pid1()));
            double sgn_pid2 = double(pids.pid2()) / double(abs(pids.pid2()));

            PID_pair cc_pid_pair = pids.cc_pid_pair();

            auto mm_pid2proc_range = all_PID_pairs_to_process_codes().equal_range(pids);

            // Loop over these elements in the multimap
            for (auto mm_it = mm_pid2proc_range.first; mm_it != mm_pid2proc_range.second; ++mm_it)
            {
              int pcode = mm_it->second;

              double factor = 1.0 / pcode_counter.at(pcode);

              // if (pcode == 1491)
              // {
              //   cout << "DEBUG: pcode==1491: factor start at: " << factor << endl;
              //   cout << "DEBUG: pcode==1491: pid1() = " << pids.pid1() << ",  pid2() = " << pids.pid2() << endl;
              //   cout << "DEBUG: pcode==1491: sgn_pid1 = " << sgn_pid1 << ",  sgn_pid2 = " << sgn_pid2 << endl;
              // }

              // if ((!pids.is_antiparticle_pair()) && (sgn_pid1 * sgn_pid2 < 0))
              // {
              //   // factor = factor * 2.0;
              //   if (std::find(all_pid_pairs.begin(), all_pid_pairs.end(), cc_pid_pair) != all_pid_pairs.end() )
              //   {
              //     factor = factor * 2.0;

              //     if (pcode == 1491)
              //     {
              //       cout << "DEBUG: pcode==1491: factor adjusted by 2: " << factor << endl;
              //     }

              //   }                
              // }

              // if ((!pids.is_antiparticle_pair()) && (sgn_pid1 * sgn_pid2 > 0))
              // {
              //   if (std::find(all_pid_pairs.begin(), all_pid_pairs.end(), cc_pid_pair) != all_pid_pairs.end() )
              //   {
              //     factor = factor * 0.5;

              //     if (pcode == 1491)
              //     {
              //       cout << "DEBUG: pcode==1491: factor adjusted by 0.5: " << factor << endl;
              //     }

              //   }                
              // }

              // if (pcode == 1491)
              // {
              //   cout << "DEBUG: pcode==1491: factor ends at: " << factor << endl;
              // }

              // else if (pids.pid1() == pids.pid2())
              // {
              //   // factor = factor * 0.5;
              //   if (std::find(all_pid_pairs.begin(), all_pid_pairs.end(), cc_pid_pair) != all_pid_pairs.end() )
              //   {
              //     factor = factor * 0.0;
              //   }                
              // }

              pids_xsec_val += (*Dep::HardScatteringSim)->xsec_fb(pcode) * factor;
            }


            cout << std::fixed << std::setprecision(7);
            cout << DEBUG_PREFIX << "PIDs xsecs:  " << "[" 
                                                   << pids.pid1() << "," << pids.pid2() << "]: "
                                                   << std::scientific << std::setprecision(5)
                                                   << pids_xsec_val << endl;
          }

          // _Anders DEBUG END

        }
      }

      // Gather the xsecs from all threads into one
      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        result.gather_xsecs();
      }

    }

    /// Return MC_xsec_container as const base_xsec_container*
    void get_MC_xsec_as_base(const base_xsec_container*& result)
    {
      using namespace Pipes::get_MC_xsec_as_base;
      result = &(*Dep::TotalCrossSectionFromMC);
    }

    /// Return xsec_container as const base_xsec_container*
    void get_xsec_as_base(const base_xsec_container*& result)
    {
      using namespace Pipes::get_xsec_as_base;
      result = &(*Dep::TotalCrossSection);
    }



    /// Get a cross-section from NLL-FAST
    void getNLLFastxsec(xsec_container& result)
    {
      using namespace Pipes::getNLLFastxsec;

      // Use a static variable to communicate the result calculated on thread 0 during 
      // iteration XSEC_CALCULATION to all threads during iteration START_SUBPROCESS
      static xsec_container shared_result;

      // Don't bother if there are no analyses that will use this.
      if (Dep::RunMC->analyses.empty()) return;

      // Only thread 0
      if(*Loop::iteration == COLLIDER_INIT) shared_result.reset();
      
      // All threads
      if (*Loop::iteration == COLLIDER_INIT_OMP) result.reset();

      // Set the xsec and its error.
      // Only thread 0
      if (*Loop::iteration == XSEC_CALCULATION)
      {
        double xs_fb = 0.1;             // replace with xsec from NLL-Fast
        double xserr_fb = 0.1 * xs_fb;  // or whatever
        shared_result.set_xsec(xs_fb, xserr_fb);

        // Let thread 0 return the correct result already after iteration XSEC_CALCULATION
        result = shared_result;
      }

      // All threads
      if (*Loop::iteration == START_SUBPROCESS)
      {
        // All threads copy the result from shared_result
        result = shared_result;
      }

    }


    /// A function that reads the total cross-section from the input file, but builds up the number of events from the event loop
    void getYAMLxsec(xsec_container& result)
    {
      using namespace Pipes::getYAMLxsec;

      // Use a static variable to communicate the result calculated on thread 0 during 
      // iteration XSEC_CALCULATION to all threads during iteration START_SUBPROCESS
      static xsec_container shared_result;

      // Don't bother if there are no analyses that will use this.
      if (Dep::RunMC->analyses.empty()) return;

      static std::pair<str,str> xsec_pnames;
      static str input_unit; 
      static bool input_fractional_uncert = false;

      static bool first = true;
      if (*Loop::iteration == BASE_INIT)
      {

        if (first)
        {
          // Determine the correct combination of parameters
          if ((runOptions->hasKey("xsec_fb")) && (runOptions->hasKey("xsec_uncert_fb")))
          {
            xsec_pnames.first = "xsec_fb";
            xsec_pnames.second = "xsec_uncert_fb";
            input_unit = "fb";
            input_fractional_uncert = false;
          }
          else if ((runOptions->hasKey("xsec_fb")) && (runOptions->hasKey("xsec_fractional_uncert")))
          {
            xsec_pnames.first = "xsec_fb";
            xsec_pnames.second = "xsec_fractional_uncert";
            input_unit = "fb";
            input_fractional_uncert = true;
          }
          else if ((runOptions->hasKey("xsec_pb")) && (runOptions->hasKey("xsec_uncert_pb")))
          {
            xsec_pnames.first = "xsec_pb";
            xsec_pnames.second = "xsec_uncert_pb";
            input_unit = "pb";
            input_fractional_uncert = false;
          }
          else if ((runOptions->hasKey("xsec_pb")) && (runOptions->hasKey("xsec_fractional_uncert")))
          {
            xsec_pnames.first = "xsec_pb";
            xsec_pnames.second = "xsec_fractional_uncert";
            input_unit = "pb";
            input_fractional_uncert = true;
          }
          else
          {
            std::stringstream errmsg_ss;
            errmsg_ss << "Unknown combination of options for function getYAMLxsec." << endl;
            errmsg_ss << "Needs one of the following sets of option names:" << endl;
            errmsg_ss << "  xsec_fb, xsec_uncert_fb" << endl;
            errmsg_ss << "  xsec_fb, xsec_fractional_uncert" << endl;
            errmsg_ss << "  xsec_pb, xsec_uncert_pb" << endl;
            errmsg_ss << "  xsec_pb, xsec_fractional_uncert" << endl;
            ColliderBit_error().raise(LOCAL_INFO, errmsg_ss.str());
          }

          first = false;
        }
      }

      // Retrieve the total cross-section and cross-section error
      const static double input_xsec = runOptions->getValue<double>(xsec_pnames.first);
      const static double input_xsec_uncert = runOptions->getValue<double>(xsec_pnames.second);

      // Only thread 0
      if(*Loop::iteration == COLLIDER_INIT) shared_result.reset();
      
      // All threads
      if (*Loop::iteration == COLLIDER_INIT_OMP) result.reset();

      // Set the xsec and its error
      // Only thread 0
      if (*Loop::iteration == XSEC_CALCULATION)
      {
        std::pair<double,double> temp = convert_xsecs_to_fb(input_xsec, input_xsec_uncert, input_unit, input_fractional_uncert);
        double xsec_fb = temp.first;
        double xsec_uncert_fb = temp.second;
        shared_result.set_xsec(xsec_fb, xsec_uncert_fb);

        // Let thread 0 return the correct result already after iteration XSEC_CALCULATION
        result = shared_result;
      }

      // All threads
      if (*Loop::iteration == START_SUBPROCESS)
      {
        // All threads copy the result from shared_result
        result = shared_result;
      }

    }



    /// A function that reads a list of (SLHA file, total cross-section) pairs from the input YAML file
    void getYAMLxsec_SLHA(xsec_container& result)
    {
      using namespace Pipes::getYAMLxsec_SLHA;

      // Use a static variable to communicate the result calculated on thread 0 during 
      // iteration XSEC_CALCULATION to all threads during iteration START_SUBPROCESS
      static xsec_container shared_result;

      // Don't bother if there are no analyses that will use this.
      if (Dep::RunMC->analyses.empty()) return;

      static std::pair<str,str> xsec_pnames;
      static str input_unit; 
      static bool input_fractional_uncert = false;

      static bool first = true;
      if (*Loop::iteration == BASE_INIT)
      {

        if (first)
        {
          // Determine the correct combination of parameters
          if ((runOptions->hasKey("xsec_fb")) && (runOptions->hasKey("xsec_uncert_fb")))
          {
            xsec_pnames.first = "xsec_fb";
            xsec_pnames.second = "xsec_uncert_fb";
            input_unit = "fb";
            input_fractional_uncert = false;
          }
          else if ((runOptions->hasKey("xsec_fb")) && (runOptions->hasKey("xsec_fractional_uncert")))
          {
            xsec_pnames.first = "xsec_fb";
            xsec_pnames.second = "xsec_fractional_uncert";
            input_unit = "fb";
            input_fractional_uncert = true;
          }
          else if ((runOptions->hasKey("xsec_pb")) && (runOptions->hasKey("xsec_uncert_pb")))
          {
            xsec_pnames.first = "xsec_pb";
            xsec_pnames.second = "xsec_uncert_pb";
            input_unit = "pb";
            input_fractional_uncert = false;
          }
          else if ((runOptions->hasKey("xsec_pb")) && (runOptions->hasKey("xsec_fractional_uncert")))
          {
            xsec_pnames.first = "xsec_pb";
            xsec_pnames.second = "xsec_fractional_uncert";
            input_unit = "pb";
            input_fractional_uncert = true;
          }
          else
          {
            std::stringstream errmsg_ss;
            errmsg_ss << "Unknown combination of options for function getYAMLxsec_SLHA." << endl;
            errmsg_ss << "Needs one of the following sets of option names:" << endl;
            errmsg_ss << "  xsec_fb, xsec_uncert_fb" << endl;
            errmsg_ss << "  xsec_fb, xsec_fractional_uncert" << endl;
            errmsg_ss << "  xsec_pb, xsec_uncert_pb" << endl;
            errmsg_ss << "  xsec_pb, xsec_fractional_uncert" << endl;
            ColliderBit_error().raise(LOCAL_INFO, errmsg_ss.str());
          }

          first = false;
        }
      }

      // Get the filename, look for it in the xsec and uncertainty lists
      const static YAML::Node colNode_xsec = runOptions->getValue<YAML::Node>(xsec_pnames.first);
      const static Options colOptions_xsec(colNode_xsec);
      const static YAML::Node colNode_uncert = runOptions->getValue<YAML::Node>(xsec_pnames.second);
      const static Options colOptions_uncert(colNode_uncert);
      static str filename;

      if (*Loop::iteration == BASE_INIT)
      {
        // Update the SLHA filename
        filename = Dep::SLHAFileNameAndContent->first;

        // Look for the filename in the xsec lists
        if (!colOptions_xsec.hasKey(filename)) piped_invalid_point.request(str("No cross-section found for SLHA file ").append(filename));
        if (!colOptions_uncert.hasKey(filename)) piped_invalid_point.request(str("No fractional cross-section uncertainty found for SLHA file ").append(filename));
      }

      // Only thread 0
      if(*Loop::iteration == COLLIDER_INIT) shared_result.reset();
      
      // All threads
      if (*Loop::iteration == COLLIDER_INIT_OMP) result.reset();

      // Set the xsec and its error
      if (*Loop::iteration == XSEC_CALCULATION)
      {
        double input_xsec = colOptions_xsec.getValue<double>(filename);
        double input_xsec_uncert = colOptions_uncert.getValue<double>(filename);

        std::pair<double,double> temp = convert_xsecs_to_fb(input_xsec, input_xsec_uncert, input_unit, input_fractional_uncert);
        double xsec_fb = temp.first;
        double xsec_uncert_fb = temp.second;
        shared_result.set_xsec(xsec_fb, xsec_uncert_fb);

        // Let thread 0 return the correct result already after iteration XSEC_CALCULATION
        result = shared_result;
      }

      // All threads
      if (*Loop::iteration == START_SUBPROCESS)
      {
        // All threads copy the result from shared_result
        result = shared_result;
      }

    }


    /// A function that assigns a total cross-sections directly from the scan parameters
    /// (for models CB_SLHA_simpmod_scan_model and CB_SLHA_scan_model)
    void getYAMLxsec_param(xsec_container& result)
    {
      using namespace Pipes::getYAMLxsec_param;

      // Use a static variable to communicate the result calculated on thread 0 during 
      // iteration XSEC_CALCULATION to all threads during iteration START_SUBPROCESS
      static xsec_container shared_result;

      // Don't bother if there are no analyses that will use this.
      if (Dep::RunMC->analyses.empty()) return;

      static std::vector<str> pnames;
      static std::pair<str,str> xsec_pnames;

      static str input_unit; 
      static bool input_fractional_uncert = false;

      static bool first = true;
      if (*Loop::iteration == BASE_INIT)
      {

        if (first)
        {

          // Get all parameter names
          for (const auto& parname_parptr_pair : Param)
          {
            pnames.push_back(parname_parptr_pair.first);
          }

          // Determine the correct combination of parameters
          if ((std::find(pnames.begin(), pnames.end(), "xsec_fb") != pnames.end()) 
               && (std::find(pnames.begin(), pnames.end(), "xsec_uncert_fb") != pnames.end()))
          {
            xsec_pnames.first = "xsec_fb";
            xsec_pnames.second = "xsec_uncert_fb";
            input_unit = "fb";
            input_fractional_uncert = false;
          }
          else if ((std::find(pnames.begin(), pnames.end(), "xsec_fb") != pnames.end()) 
                    && (std::find(pnames.begin(), pnames.end(), "xsec_fractional_uncert") != pnames.end()))
          {
            xsec_pnames.first = "xsec_fb";
            xsec_pnames.second = "xsec_fractional_uncert";
            input_unit = "fb";
            input_fractional_uncert = true;
          }
          else if ((std::find(pnames.begin(), pnames.end(), "xsec_pb") != pnames.end()) 
                    && (std::find(pnames.begin(), pnames.end(), "xsec_uncert_pb") != pnames.end()))
          {
            xsec_pnames.first = "xsec_pb";
            xsec_pnames.second = "xsec_uncert_pb";
            input_unit = "pb";
            input_fractional_uncert = false;
          }
          else if ((std::find(pnames.begin(), pnames.end(), "xsec_pb") != pnames.end()) 
                    && (std::find(pnames.begin(), pnames.end(), "xsec_fractional_uncert") != pnames.end()))
          {
            xsec_pnames.first = "xsec_pb";
            xsec_pnames.second = "xsec_fractional_uncert";
            input_unit = "pb";
            input_fractional_uncert = true;
          }
          else
          {
            std::stringstream errmsg_ss;
            errmsg_ss << "Unknown combination of parameters for function getYAMLxsec_param." << endl;
            errmsg_ss << "Needs one of the following sets of parameter names:" << endl;
            errmsg_ss << "  xsec_fb, xsec_uncert_fb" << endl;
            errmsg_ss << "  xsec_fb, xsec_fractional_uncert" << endl;
            errmsg_ss << "  xsec_pb, xsec_uncert_pb" << endl;
            errmsg_ss << "  xsec_pb, xsec_fractional_uncert" << endl;
            ColliderBit_error().raise(LOCAL_INFO, errmsg_ss.str());
          }

          first = false;
        }
      }

      // Only thread 0
      if(*Loop::iteration == COLLIDER_INIT) shared_result.reset();
      
      // All threads
      if (*Loop::iteration == COLLIDER_INIT_OMP) result.reset();

      // Set the xsec and its error
      // Only thread 0
      if (*Loop::iteration == XSEC_CALCULATION)
      {
        double input_xsec = *Param.at(xsec_pnames.first);
        double input_xsec_uncert = *Param.at(xsec_pnames.second); 

        std::pair<double,double> temp = convert_xsecs_to_fb(input_xsec, input_xsec_uncert, input_unit, input_fractional_uncert);
        double xsec_fb = temp.first;
        double xsec_uncert_fb = temp.second;
        shared_result.set_xsec(xsec_fb, xsec_uncert_fb);

        // Let thread 0 return the correct result already after iteration XSEC_CALCULATION
        result = shared_result;
      }

      // All threads
      if (*Loop::iteration == START_SUBPROCESS)
      {
        // All threads copy the result from shared_result
        result = shared_result;
      }

    }


    /// Get cross-section info as map_str_dbl (for simple printing)
    void getTotalCrossSectionAsMap(map_str_dbl& result)
    {
      using namespace Pipes::getTotalCrossSectionAsMap;

      // @todo Do we need this to ensure that the result map is always of the same length (for the printer)?
      // // Append the xsec info for the current collider to the result map
      // if (*Loop::iteration == COLLIDER_INIT)
      // {
      //   xsec empty_xs;
      //   for(auto s_d_pair : empty_xs.get_content_as_map())
      //   {
      //     std::string new_key(Dep::RunMC->current_collider());
      //     new_key.append("__").append(s_d_pair.first);
      //     result[new_key] = s_d_pair.second;
      //   }
      // }

      // Append the xsec info for the current collider to the result map
      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        for(auto s_d_pair : (*Dep::TotalCrossSection)->get_content_as_map())
        {
          std::string new_key(Dep::RunMC->current_collider());
          new_key.append("__").append(s_d_pair.first);
          result[new_key] = s_d_pair.second;
        }
      }
    }

  }
}
