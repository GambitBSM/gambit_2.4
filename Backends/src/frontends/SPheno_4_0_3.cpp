//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend for SPheno 4.3.0 backend 
///  (out of the box version)
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2020 Apr
///
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/SPheno_4_0_3.hpp"
#include "gambit/Elements/spectrum_factories.hpp"
#include "gambit/Models/SimpleSpectra/MSSMSimpleSpec.hpp"
#include "gambit/Utils/version.hpp"

// Callback function for error handling
BE_NAMESPACE
{
  // This function will be called from SPheno. Needs C linkage, and thus also
  // a backend-specific name to guard against name clashes.
  extern "C"
  void CAT_4(BACKENDNAME,_,SAFE_VERSION,_ErrorHandler)()
  {
    throw std::runtime_error("SPheno backend called TerminateProgram.");
  }
}
END_BE_NAMESPACE

// Convenience functions (definition)
BE_NAMESPACE
{
  // Convenience function to run SPheno and obtain the spectrum
  int run_SPheno(Spectrum &spectrum, const Finputs &inputs)
  {

    try{ Set_All_Parameters_0(); }
    catch(std::runtime_error e) { invalid_point().raise(e.what()); }

    ReadingData(inputs);

    *epsI = 1.0E-5;
    *deltaM = 1.0E-3;
    *CalcTBD = false;
    *ratioWoM = 0.0;

    try{ SPheno_Main(); }
    catch(std::runtime_error e) { invalid_point().raise(e.what()); }

    if(*kont != 0)
       ErrorHandling(*kont);
      
    spectrum = Spectrum_Out(inputs);

    return *kont;

  }

  // Convenience function to convert internal SPheno variables into a Spectrum object
  Spectrum Spectrum_Out(const Finputs &inputs)
  {

    SLHAstruct slha;

    Freal8 Q;
    try{ Q = sqrt(GetRenormalizationScale()); }
    catch(std::runtime_error e) { invalid_point().raise(e.what()); }

    // TODO: Chi masses are not rotated, I think. Check

    // Spectrum generator information
    SLHAea_add_block(slha, "SPINFO");
    SLHAea_add(slha, "SPINFO", 1, "GAMBIT, using "+str(STRINGIFY(BACKENDNAME)));
    SLHAea_add(slha, "SPINFO", 2, gambit_version()+" (GAMBIT); "+str(STRINGIFY(VERSION))+" ("+str(STRINGIFY(BACKENDNAME))+");");

    // Block MODSEL
    SLHAea_add_block(slha, "MODSEL");
    if(inputs.param.find("Qin") != inputs.param.end())
      slha["MODSEL"][""] << 1 << 0 << "# SUSY scale input";
    else
      slha["MODSEL"][""] << 1 << 1 << "# GUT scale input";
    slha["MODSEL"][""] << 5 << 1 << "# Switching on CP violations";
    if(*GenerationMixing)
      slha["MODSEL"][""] << 6 << 1 << "# switching on flavour violation";
    if(inputs.param.find("Qin") != inputs.param.end())
      slha["MODSEL"][""] << 12 << *inputs.param.at("Qin") << "# Qin";

    // Block MINPAR
    SLHAea_add_block(slha, "MINPAR");
    if(inputs.param.find("M0") != inputs.param.end())
      slha["MINPAR"][""] << 1 << *inputs.param.at("M0") << "# m0";
    if(inputs.param.find("M12") != inputs.param.end())
      slha["MINPAR"][""] << 2 << *inputs.param.at("M12") << "# m12";
    if(inputs.param.find("TanBeta") != inputs.param.end())
      slha["MINPAR"][""] << 3 << *inputs.param.at("TanBeta") << "# tanb at m_Z";
    if(inputs.param.find("SignMu") != inputs.param.end())
      slha["MINPAR"][""] << 4 << *inputs.param.at("SignMu") << "# sign(mu)";
    if(inputs.param.find("A0") != inputs.param.end())
      slha["MINPAR"][""] << 5 << *inputs.param.at("A0") << "# A0";

    // Block EXTPAR
    SLHAea_add_block(slha, "EXTPAR");
    if(inputs.param.find("Qin") != inputs.param.end())
      slha["EXTPAR"][""] << 0 << *inputs.param.at("Qin") << "# scale Q where the parameters below are defined";
    if(inputs.param.find("M1") != inputs.param.end())
      slha["EXTPAR"][""] << 1 << *inputs.param.at("M1") << "# M_1";
    if(inputs.param.find("M2") != inputs.param.end())
      slha["EXTPAR"][""] << 2 << *inputs.param.at("M2") << "# M_2";
    if(inputs.param.find("M3") != inputs.param.end())
      slha["EXTPAR"][""] << 3 << *inputs.param.at("M3") << "# M_3";
    if(inputs.param.find("Au_33") != inputs.param.end())
      slha["EXTPAR"][""] << 11 << *inputs.param.at("Au_33") << "# A_t";
    if(inputs.param.find("Ad_33") != inputs.param.end())
      slha["EXTPAR"][""] << 12 << *inputs.param.at("Ad_33") << "# A_b";
    if(inputs.param.find("Ae_33") != inputs.param.end())
      slha["EXTPAR"][""] << 13 << *inputs.param.at("Ae_33") << "# A_l";
    if(inputs.param.find("mHd2") != inputs.param.end())
      slha["EXTPAR"][""] << 21 << *inputs.param.at("mHd2") << "# m_Hd^2";
    if(inputs.param.find("mHu2") != inputs.param.end())
      slha["EXTPAR"][""] << 22 << *inputs.param.at("mHd2") << "# m_Hu^2";
    if(inputs.param.find("mu") != inputs.param.end())
      slha["EXTPAR"][""] << 23 << *inputs.param.at("mu") << "# mu";
    if(inputs.param.find("mA") != inputs.param.end())
      slha["EXTPAR"][""] << 24 << pow(*inputs.param.at("mA"),2) << "# mA";
    if(inputs.param.find("ml2_11") != inputs.param.end())
    if(inputs.param.find("ml2_11") != inputs.param.end())
      slha["EXTPAR"][""] << 31 << sqrt(*inputs.param.at("ml2_11")) << "# M_(L,11)";
    if(inputs.param.find("ml2_22") != inputs.param.end())
      slha["EXTPAR"][""] << 32 << sqrt(*inputs.param.at("ml2_22")) << "# M_(L,22)";
    if(inputs.param.find("ml2_33") != inputs.param.end())
      slha["EXTPAR"][""] << 33 << sqrt(*inputs.param.at("ml2_33")) << "# M_(L,33)";
    if(inputs.param.find("me2_11") != inputs.param.end())
      slha["EXTPAR"][""] << 34 << sqrt(*inputs.param.at("me2_11")) << "# M_(E,11)";
    if(inputs.param.find("me2_22") != inputs.param.end())
      slha["EXTPAR"][""] << 35 << sqrt(*inputs.param.at("me2_22")) << "# M_(E,22)";
    if(inputs.param.find("me2_33") != inputs.param.end())
      slha["EXTPAR"][""] << 36 << sqrt(*inputs.param.at("me2_33")) << "# M_(E,33)";
    if(inputs.param.find("mq2_11") != inputs.param.end())
      slha["EXTPAR"][""] << 41 << sqrt(*inputs.param.at("mq2_11")) << "# M_(Q,11)";
    if(inputs.param.find("mq2_22") != inputs.param.end())
      slha["EXTPAR"][""] << 42 << sqrt(*inputs.param.at("mq2_22")) << "# M_(Q,22)";
    if(inputs.param.find("mq2_33") != inputs.param.end())
      slha["EXTPAR"][""] << 43 << sqrt(*inputs.param.at("mq2_33")) << "# M_(Q,33)";
    if(inputs.param.find("mu2_11") != inputs.param.end())
      slha["EXTPAR"][""] << 44 << sqrt(*inputs.param.at("mu2_11")) << "# M_(U,11)";
    if(inputs.param.find("mu2_22") != inputs.param.end())
      slha["EXTPAR"][""] << 45 << sqrt(*inputs.param.at("mu2_22")) << "# M_(U,22)";
    if(inputs.param.find("mu2_33") != inputs.param.end())
      slha["EXTPAR"][""] << 46 << sqrt(*inputs.param.at("mu2_33")) << "# M_(U,33)";
    if(inputs.param.find("md2_11") != inputs.param.end())
      slha["EXTPAR"][""] << 47 << sqrt(*inputs.param.at("md2_11")) << "# M_(D,11)";
    if(inputs.param.find("md2_22") != inputs.param.end())
      slha["EXTPAR"][""] << 48 << sqrt(*inputs.param.at("md2_22")) << "# M_(D,22)";
    if(inputs.param.find("md2_33") != inputs.param.end())
      slha["EXTPAR"][""] << 49 << sqrt(*inputs.param.at("md2_33")) << "# M_(D,33)";

    // Block SMINPUTS
    SLHAea_add_block(slha, "SMINPUTS");
    slha["SMINPUTS"][""] << 1 << 1.0 / *Alpha_mZ_MS << "# alpha_em^-1(MZ)^MSbar";
    slha["SMINPUTS"][""] << 2 << *G_F << "# G_mu [GeV^-2]";
    slha["SMINPUTS"][""] << 3 << *AlphaS_mZ << "# alpha_s(MZ)^MSbar";
    slha["SMINPUTS"][""] << 4 << *mZ << "# m_Z(pole)";
    slha["SMINPUTS"][""] << 5 << (*mf_d)(3) << "# m_b(m_b), MSbar";
    slha["SMINPUTS"][""] << 6 << (*mf_u)(3) << "# m_t(pole)";
    slha["SMINPUTS"][""] << 7 << (*mf_l)(3) << "# m_tau(pole)";
    slha["SMINPUTS"][""] << 8 << (*mf_nu)(3) << "# m_nu_3";
    slha["SMINPUTS"][""] << 11 << (*mf_l)(1) << "# m_e(pole)";
    slha["SMINPUTS"][""] << 12 << (*mf_nu)(1) << "# m_nu_1";
    slha["SMINPUTS"][""] << 13 << (*mf_l)(2) << "# m_muon(pole)";
    slha["SMINPUTS"][""] << 14 << (*mf_nu)(2) << "# m_nu_2";
    slha["SMINPUTS"][""] << 21 << (*mf_d)(1) << "# m_d(2 GeV), MSbar";
    slha["SMINPUTS"][""] << 22 << (*mf_u)(1) << "# m_u(2 GeV), MSbar";
    slha["SMINPUTS"][""] << 23 << (*mf_d)(2) << "# m_s(2 GeV), MSbar";
    slha["SMINPUTS"][""] << 24 << (*mf_u)(2) << "# m_c(m_c), MSbar";


    Farray<Fcomplex16,1,6,1,6> RDsq_ckm, RUsq_ckm, RSl_pmns;
    Farray<Fcomplex16,1,3,1,3> RSn_pmns, id3C;
    Farray<Fcomplex16,1,3,1,3> CKM_Q, PMNS_Q;
    Farray<Freal8,1,3> Yu, Yd, Yl;
    if(*GenerationMixing)
    {
      SLHAea_add_block(slha, "VCKMIN");
      slha["VCKMIN"][""] << 1 << *lam_wolf << "# lambda";
      slha["VCKMIN"][""] << 2 << *A_wolf << "# A";
      slha["VCKMIN"][""] << 3 << *rho_wolf << "# rho bar";
      slha["VCKMIN"][""] << 4 << *eta_wolf << "# eta bar";

      Flogical False = false;
      try{ Switch_to_superCKM(*Y_d,*Y_u,*A_d,*A_u,*M2_D,*M2_Q,*M2_U,*Ad_sckm,*Au_sckm,*M2D_sckm,*M2Q_sckm,*M2U_sckm,False,*RSdown,*RSup,RDsq_ckm,RUsq_ckm,CKM_Q,Yd,Yu); }
      catch(std::runtime_error e) { invalid_point().raise(e.what()); }

      SLHAea_add_block(slha, "UPMNSIN");
      slha["UPMNSIN"][""] << 1 << *theta_12 << "# theta_12, solar";
      slha["UPMNSIN"][""] << 2 << *theta_23<< "# theta_23, atmospheric";
      slha["UPMNSIN"][""] << 3 << *theta_13 << "# theta_13";
      slha["UPMNSIN"][""] << 4 << *delta_nu << "# delta_nu";
      slha["UPMNSIN"][""] << 5 << *alpha_nu1 << "# alpha_1";
      slha["UPMNSIN"][""] << 6 << *alpha_nu2 << "# alpha_2";

      try{ Switch_to_superPMNS(*Y_l,id3C,*A_l,*M2_E,*M2_L,*Al_pmns,*M2E_pmns,*M2L_pmns,False,*RSlepton,*RSneut,RSl_pmns,RSn_pmns,PMNS_Q,Yl); }
      catch(std::runtime_error e) { invalid_point().raise(e.what()); }


    }
    else
    {
      for(int i=1; i<=3; i++)
      {
        Yu(i) = (*Y_u)(i,i).re;
        Yd(i) = (*Y_d)(i,i).re;
        Yl(i) = (*Y_l)(i,i).re;
      }
      *Al_pmns = *A_l;
      *Ad_sckm = *A_d;
      *Au_sckm = *A_u;

      *M2D_sckm = *M2_D;
      *M2U_sckm = *M2_U;
      *M2Q_sckm = *M2_Q;
      *M2E_pmns = *M2_E;
      *M2L_pmns = *M2_L;

      RUsq_ckm = *RSup;
      RDsq_ckm = *RSdown;

      RSn_pmns = *RSneut;
      RSl_pmns = *RSlepton;

    }

    // Block GAUGE
    SLHAea_add_block(slha, "GAUGE", Q);
    slha["GAUGE"][""] << 1 << (*gauge)(1) << "# g'(Q)^DRbar";
    slha["GAUGE"][""] << 2 << (*gauge)(2) << "# g(Q)^DRbar";
    slha["GAUGE"][""] << 3 << (*gauge)(3) << "# g3(Q)^DRbar";

    // Blocks Yu, Yd, Ye
    SLHAea_add_block(slha, "Yu", Q);
    SLHAea_add_block(slha, "Yd", Q);
    SLHAea_add_block(slha, "Ye", Q);
    for(int i=1; i<=3; i++)
    {
      slha["Yu"][""] << i << i << Yu(i) << "# Yu(" << i << "," << i << ")(Q)^DRbar";
      slha["Yd"][""] << i << i << Yd(i) << "# Yd(" << i << "," << i << ")(Q)^DRbar";
      slha["Ye"][""] << i << i << Yl(i) << "# Ye(" << i << "," << i << ")(Q)^DRbar";
      for(int j=1; j<=3; j++)
      {
        slha["Yu"][""] << i << j << 0.0 << "# Yu(" << i << "," << j << ")(Q)^DRbar";
        slha["Yd"][""] << i << j << 0.0 << "# Yd(" << i << "," << j << ")(Q)^DRbar";
        slha["Ye"][""] << i << j << 0.0 << "# Ye(" << i << "," << j << ")(Q)^DRbar";
      }
    }

    if(*GenerationMixing)
    {
      // Blocks VKCM and UPMNS
      SLHAea_add_block(slha, "VCKM", Q);
      SLHAea_add_block(slha, "IMVCKM", Q);
      SLHAea_add_block(slha, "UPMNS", Q);
      SLHAea_add_block(slha, "IMUPMNS", Q);
      for(int i=1; i<=3; i++)
        for(int j=1; j<=3; j++)
        {
          slha["VCKM"][""] << i << j << CKM_Q(i,j).re << "# V_" << i << j;
          slha["IMVCKM"][""] << i << j << CKM_Q(i,j).im << "# Im(V_" << i << j << ")";
          slha["UPMNS"][""] << i << j << PMNS_Q(i,j).re << "# UPMNS_" << i << j;
          slha["IMUPMNS"][""] << i << j << PMNS_Q(i,j).im << "# Im(UPMNS_" << i << j << ")";
        }
     
      // Blocks Te, Tu, Td
      SLHAea_add_block(slha, "Te", Q);
      SLHAea_add_block(slha, "Tu", Q);
      SLHAea_add_block(slha, "Td", Q);
      SLHAea_add_block(slha, "IMTe", Q);
      SLHAea_add_block(slha, "IMTu", Q);
      SLHAea_add_block(slha, "IMTd", Q);
      for(int i=1; i<=3; i++)
        for(int j=1; j<=3; j++)
        {
          slha["Te"][""] << i << j << (*Al_pmns)(i,j).re << "# Te(" << i << "," << j << ")";
          slha["Tu"][""] << i << j << (*Au_sckm)(i,j).re << "# Tu(" << i << "," << j << ")";
          slha["Td"][""] << i << j << (*Ad_sckm)(i,j).re << "# Td(" << i << "," << j << ")";
          slha["IMTe"][""] << i << j << (*Al_pmns)(i,j).im << "# Im(Te(" << i << "," << j << "))";
          slha["IMTu"][""] << i << j << (*Au_sckm)(i,j).im << "# Im(Tu(" << i << "," << j << "))";
          slha["IMTd"][""] << i << j << (*Ad_sckm)(i,j).im << "# Im(Td(" << i << "," << j << "))";
        }

    }

    // Blocks Au, Ad, Ae
    SLHAea_add_block(slha, "Ae", Q);
    SLHAea_add_block(slha, "Au", Q);
    SLHAea_add_block(slha, "Ad", Q);
    SLHAea_add_block(slha, "IMAe", Q);
    SLHAea_add_block(slha, "IMAu", Q);
    SLHAea_add_block(slha, "IMAd", Q);
    for(int i=1; i<=3; i++)
    {
      for(int j=1; j<=3; j++)
      {
        if((*Y_l)(i,j).abs() > 0.0)
        {
          slha["Ae"][""] << i << j << ((*Al_pmns)(i,j)/(*Y_l)(i,j)).re << "# Ae(" << i << "," << j << ")";
          slha["IMAe"][""] << i << j << ((*Al_pmns)(i,j)/(*Y_l)(i,j)).im << "# Im(Ae(" << i << "," << j << "))";
        }
        else
        {
          slha["Ae"][""] << i << j << 0.0 << "# Ae(" << i << "," << j << ")";
          slha["IMAe"][""] << i << j << 0.0 << "# Im(Ae(" << i << "," << j << "))";
        }
        if((*Y_u)(i,i).abs() > 0.0)
        {
          slha["Au"][""] << i << j << ((*Au_sckm)(i,j)/(*Y_u)(i,j)).re << "# Au(" << i << "," << j << ")";
          slha["IMAu"][""] << i << j << ((*Au_sckm)(i,j)/(*Y_u)(i,j)).im << "# Im(Au(" << i << "," << j << "))";
        }
        else
        {
          slha["Au"][""] << i << j << 0.0 << "# Au(" << i << "," << j << ")";
          slha["IMAu"][""] << i << j << 0.0 << "# Im(Au(" << i << "," << j << "))";
        }
        if((*Y_d)(i,i).abs() > 0.0)
        {
          slha["Ad"][""] << i << j << ((*Ad_sckm)(i,i)/(*Y_d)(i,j)).re << "# Ad(" << i << "," << j << ")";
          slha["IMAd"][""] << i << j << ((*Ad_sckm)(i,i)/(*Y_d)(i,j)).im << "# Im(Ad(" << i << "," << j << "))";
        }
        else
        {
          slha["Ad"][""] << i << j << 0.0 << "# Ad(" << i << "," << j << ")";
          slha["IMAd"][""] << i << j << 0.0 << "# Im(Ad(" << i << "," << j << "))";
        }
      }

    }

    // Block MSOFT
    SLHAea_add_block(slha, "MSOFT", Q);
    slha["MSOFT"][""] << 1 << (*Mi)(1).re << "# M_1";
    slha["MSOFT"][""] << 2 << (*Mi)(2).re << "# M_2";
    slha["MSOFT"][""] << 3 << (*Mi)(3).re << "# M_3";
    slha["MSOFT"][""] << 21 << (*M2_H)(1) << "# M^2_(H,d)";
    slha["MSOFT"][""] << 22 << (*M2_H)(2) << "# M^2_(H,u)";

    slha["MSOFT"][""] << 31 << sqrt((*M2L_pmns)(1,1).re) << "# M_(L,11)";
    slha["MSOFT"][""] << 32 << sqrt((*M2L_pmns)(2,2).re) << "# M_(L,22)";
    slha["MSOFT"][""] << 33 << sqrt((*M2L_pmns)(3,3).re) << "# M_(L,33)";
    slha["MSOFT"][""] << 34 << sqrt((*M2E_pmns)(1,1).re) << "# M_(E,11)";
    slha["MSOFT"][""] << 35 << sqrt((*M2E_pmns)(2,2).re) << "# M_(E,22)";
    slha["MSOFT"][""] << 36 << sqrt((*M2E_pmns)(3,3).re) << "# M_(E,33)";
    slha["MSOFT"][""] << 41 << sqrt((*M2Q_sckm)(1,1).re) << "# M_(Q,11)";
    slha["MSOFT"][""] << 42 << sqrt((*M2Q_sckm)(2,2).re) << "# M_(Q,22)";
    slha["MSOFT"][""] << 43 << sqrt((*M2Q_sckm)(3,3).re) << "# M_(Q,33)";
    slha["MSOFT"][""] << 44 << sqrt((*M2U_sckm)(1,1).re) << "# M_(U,11)";
    slha["MSOFT"][""] << 45 << sqrt((*M2U_sckm)(2,2).re) << "# M_(U,22)";
    slha["MSOFT"][""] << 46 << sqrt((*M2U_sckm)(3,3).re) << "# M_(U,33)";
    slha["MSOFT"][""] << 47 << sqrt((*M2D_sckm)(1,1).re) << "# M_(D,11)";
    slha["MSOFT"][""] << 48 << sqrt((*M2D_sckm)(2,2).re) << "# M_(D,22)";
    slha["MSOFT"][""] << 49 << sqrt((*M2D_sckm)(3,3).re) << "# M_(D,33)";

    if((*Mi)(1).im != 0 or (*Mi)(2).im != 0 or (*Mi)(3).im != 0)
    {
      SLHAea_add_block(slha, "IMMSOFT", Q);
      slha["IMMSOFT"][""] << 1 << (*Mi)(1).im << "# M_1";
      slha["IMMSOFT"][""] << 2 << (*Mi)(2).im << "# M_2";
      slha["IMMSOFT"][""] << 3 << (*Mi)(3).im << "# M_3";
    }

    // Blocks MSL2, MSE2, MSQ2, MSU2, MSD2
    SLHAea_add_block(slha, "MSL2", Q);
    SLHAea_add_block(slha, "MSE2", Q);
    SLHAea_add_block(slha, "MSQ2", Q);
    SLHAea_add_block(slha, "MSU2", Q);
    SLHAea_add_block(slha, "MSD2", Q);
    SLHAea_add_block(slha, "IMMSL2", Q);
    SLHAea_add_block(slha, "IMMSE2", Q);
    SLHAea_add_block(slha, "IMMSQ2", Q);
    SLHAea_add_block(slha, "IMMSU2", Q);
    SLHAea_add_block(slha, "IMMSD2", Q);
    for(int i=1; i<=3; i++)
      for(int j=1; j<=3; j++)
      {
        slha["MSL2"][""] << i << j << (*M2L_pmns)(i,j).re << "# ml2(" << i << "," << j << ")";
        slha["MSE2"][""] << i << j << (*M2E_pmns)(i,j).re << "# me2(" << i << "," << j << ")";
        slha["MSQ2"][""] << i << j << (*M2Q_sckm)(i,j).re << "# mq2(" << i << "," << j << ")";
        slha["MSU2"][""] << i << j << (*M2U_sckm)(i,j).re << "# mu2(" << i << "," << j << ")";
        slha["MSD2"][""] << i << j << (*M2D_sckm)(i,j).re << "# md2(" << i << "," << j << ")";
        slha["IMMSL2"][""] << i << j << (*M2L_pmns)(i,j).im << "# Im(ml2(" << i << "," << j << "))";
        slha["IMMSE2"][""] << i << j << (*M2E_pmns)(i,j).im << "# Im(me2(" << i << "," << j << "))";
        slha["IMMSQ2"][""] << i << j << (*M2Q_sckm)(i,j).im << "# Im(mq2(" << i << "," << j << "))";
        slha["IMMSU2"][""] << i << j << (*M2U_sckm)(i,j).im << "# Im(mu2(" << i << "," << j << "))";
        slha["IMMSD2"][""] << i << j << (*M2D_sckm)(i,j).im << "# Im(md2(" << i << "," << j << "))";
      }
   

    // Block MASS
    SLHAea_add_block(slha, "MASS");
    slha["MASS"][""] << 6 << (*mf_u)(3) << "# m_t(pole)";
    slha["MASS"][""] << 23 << *mZ << "# m_Z(pole)";
    slha["MASS"][""] << 24 << *mW << "# m_W(pole)";
    slha["MASS"][""] << 15 << (*mf_l)(3) << "# m_tau(pole)";

    slha["MASS"][""] << 25 << (*S0)(1).m << "# h0";
    slha["MASS"][""] << 35 << (*S0)(2).m << "# H0";
    slha["MASS"][""] << 36 << (*P0)(2).m << "# A0";
    slha["MASS"][""] << 37 << (*Spm)(2).m << "# H+";

    if(*GenerationMixing)
    {
      std::vector<int> id_sd = {1000001, 1000003, 1000005, 
                                2000001, 2000003, 2000005};
      for(int i=1; i<=6; i++)
        slha["MASS"][""] << id_sd[i-1] << (*Sdown)(i).m << "# ~d_" << i;

      std::vector<int> id_su = {1000002, 1000004, 1000006, 
                                2000002, 2000004, 2000006};
      for(int i=1; i<=6; i++)
        slha["MASS"][""] << id_su[i-1] << (*Sup)(i).m << "# ~u_" << i;

      std::vector<int> id_snu = {1000012, 1000014, 1000016};
      for(int i=1; i<=3; i++)
        slha["MASS"][""] << id_snu[i-1] << (*Sneut)(i).m << "# ~nu_" << i;

      std::vector<int> id_sle = {1000011, 1000013, 1000015,
                                 2000011, 2000013, 2000015};
      for(int i=1; i<=6; i++)
        slha["MASS"][""] << id_sle[i-1] << (*Slepton)(i).m << "# ~l_" << i;

    }
    else
    {
      if((*RSdown)(1,1).abs() > 0.5)
      { 
        slha["MASS"][""] << 1000001 << (*Sdown)(1).m << "# ~d_L";
        slha["MASS"][""] << 2000001 << (*Sdown)(2).m << "# ~d_R";
      }
      else
      {
        slha["MASS"][""] << 1000001 << (*Sdown)(2).m << "# ~d_L";
        slha["MASS"][""] << 2000001 << (*Sdown)(1).m << "# ~d_R";
      }
      if((*RSup)(1,1).abs() > 0.5)
      {
        slha["MASS"][""] << 1000002 << (*Sup)(1).m << "# ~u_L";
        slha["MASS"][""] << 2000002 << (*Sup)(2).m << "# ~u_R";
      }
      else
      {
        slha["MASS"][""] << 1000002 << (*Sup)(2).m << "# ~u_L";
        slha["MASS"][""] << 2000002 << (*Sup)(1).m << "# ~u_R";
      }
      if((*RSdown)(3,3).abs() > 0.5)
      {
        slha["MASS"][""] << 1000003 << (*Sdown)(3).m << "# ~s_L";
        slha["MASS"][""] << 2000003 << (*Sdown)(4).m << "# ~s_R";
      }
      else
      {
        slha["MASS"][""] << 1000003 << (*Sdown)(4).m << "# ~s_L";
        slha["MASS"][""] << 2000003 << (*Sdown)(3).m << "# ~s_R";
      }
      if((*RSup)(3,3).abs() > 0.5)
      {
        slha["MASS"][""] << 1000004 << (*Sup)(3).m << "# ~c_L";
        slha["MASS"][""] << 2000004 << (*Sup)(4).m << "# ~c_R";
      }
      else
      {
        slha["MASS"][""] << 1000004 << (*Sup)(4).m << "# ~c_L";
        slha["MASS"][""] << 2000004 << (*Sup)(3).m << "# ~c_R";
      }
      slha["MASS"][""] << 1000005 << (*Sdown)(5).m << "# ~b_1";
      slha["MASS"][""] << 2000005 << (*Sdown)(6).m << "# ~b_2";
      slha["MASS"][""] << 1000006 << (*Sup)(5).m << "# ~t_1";
      slha["MASS"][""] << 2000006 << (*Sup)(6).m << "# ~t_2";

      if((*RSlepton)(1,1).abs() > 0.5)
      {
        slha["MASS"][""] << 1000011 << (*Slepton)(1).m << "# ~e_L-";
        slha["MASS"][""] << 2000011 << (*Slepton)(2).m << "# ~e_R-";
      }
      else
      {
        slha["MASS"][""] << 1000011 << (*Slepton)(2).m << "# ~e_L-";
        slha["MASS"][""] << 2000011 << (*Slepton)(1).m << "# ~e_R-";
      }
      slha["MASS"][""] << 1000012 << (*Sneut)(1).m << "# ~nu_eL";
      if((*RSlepton)(3,3).abs() > 0.5)
      {
        slha["MASS"][""] << 1000013 << (*Slepton)(3).m << "# ~mu_L-";
        slha["MASS"][""] << 2000013 << (*Slepton)(4).m << "# ~mu_R-";
      }
      else
      {
        slha["MASS"][""] << 1000013 << (*Slepton)(4).m << "# ~mu_L-";
        slha["MASS"][""] << 2000013 << (*Slepton)(3).m << "# ~mu_R-";
      }
      slha["MASS"][""] << 1000014 << (*Sneut)(2).m << "# ~nu_muL";
      slha["MASS"][""] << 1000015 << (*Slepton)(5).m << "# ~tau_1-";
      slha["MASS"][""] << 2000015 << (*Slepton)(6).m << "# ~tau_2-";
      slha["MASS"][""] << 1000016 << (*Sneut)(3).m << "# ~nu_tauL";
    }

    slha["MASS"][""] << 1000021 << Glu->m << "# ~g";

    Farray_Freal8_1_4 mNr;
    Farray_Fcomplex16_1_4_1_4 Nr;

    for(int i=1; i<=4; i++)
    {
      Freal8 sum = 0;
      for(int j=1; j<=4; j++)
        sum += (*N)(i,j).re;
      if(sum == 0)
      {
        mNr(i) = -(*Chi0)(i).m;
        for(int j=1; j<=4; j++)
          Nr(i,j).re = (*N)(i,j).im;
      }
      else
      {
        mNr(i) = (*Chi0)(i).m;
        for(int j=1; j<=4; j++)
          Nr(i,j) = (*N)(i,j);
      }
    }
    slha["MASS"][""] << 1000022 << mNr(1) << "# ~chi_10";
    slha["MASS"][""] << 1000023 << mNr(2) << "# ~chi_20";
    slha["MASS"][""] << 1000025 << mNr(3) << "# ~chi_30";
    slha["MASS"][""] << 1000035 << mNr(4) << "# ~chi_40";
    slha["MASS"][""] << 1000024 << (*ChiPm)(1).m << "# ~chi_1+";
    slha["MASS"][""] << 1000037 << (*ChiPm)(2).m << "# ~chi_2+";


    // Check whether any of the masses is NaN
    auto block = slha["MASS"];
    for(auto it = block.begin(); it != block.end(); it++)
    {
      if((*it)[0] != "BLOCK" and Utils::isnan(stod((*it)[1])) )
      {
        std::stringstream message;
        message << "Error in spectrum generator: mass of " << Models::ParticleDB().long_name(std::pair<int,int>(stoi((*it)[0]),0)) << " is NaN";
        logger() << message.str() << EOM;
        invalid_point().raise(message.str());
      }
    }

    // Block ALPHA
    SLHAea_add_block(slha, "ALPHA");
    slha["ALPHA"][""] << -asin((*RS0)(1,1)) << "# alpha";

    // BLOCK HMIX
    SLHAea_add_block(slha, "HMIX", Q);
    slha["HMIX"][""] << 1 << mu->re << "# mu";
   slha["HMIX"][""] << 2 << *tanb_Q << "# tan[beta](Q)";
   slha["HMIX"][""] << 3 << *vev_Q << "# v(Q)";
   slha["HMIX"][""] << 4 << *mA2_Q << "# m^2_A(Q)";
   slha["HMIX"][""] << 101 << B->re << "# Bmu DRBar";
   slha["HMIX"][""] << 102 << (*vevSM)(1) << "# vd DRBar";
   slha["HMIX"][""] << 103 << (*vevSM)(2) << "# vu DRBar";

    if(mu->im != 0)
    {
      SLHAea_add_block(slha, "IMHMIX", Q);
      slha["IMHMIX"][""] << 1 << mu->im << "# Im(mu)";
    }

    // Blocks SCALARMIX, PSEUDOSCALARMIX, CHARGEMIX
    SLHAea_add_block(slha, "SCALARMIX");
    SLHAea_add_block(slha, "PSEUDOSCALARMIX");
    SLHAea_add_block(slha, "CHARGEMIX");
    for(int i=1; i<=2; i++)
      for(int j=1; j<=2; j++)
      {
        slha["SCALARMIX"][""] << i << j << (*RS0)(i,j) << "# ZH(" << i << "," << j << ")";
        slha["PSEUDOSCALARMIX"][""] << i << j << (*RP0)(i,j) << "# ZA(" << i << "," << j << ")";
        slha["CHARGEMIX"][""] << i << j << (*RSpm)(i,j).re << "# ZP(" << i << "," << j << ")";
      }

    // Blocks USQMIX, DSQMIX, SELMIX, SNUMIX or STOPMIX, SBOTMIX, STAUMIX
    if(*GenerationMixing)
    {
      SLHAea_add_block(slha, "USQMIX");
      for(int i=1; i<=6; i++)
        for(int j=1; j<=6; j++)
        {
          slha["USQMIX"][""] << i << j << RUsq_ckm(i,j).re << "# R_Su(" << i << "," << j << ")";
          if(RUsq_ckm(i,j).im != 0)
          {
            SLHAea_check_block(slha, "IMUSQMIX", i, true);
            slha["IMUSQMIX"][""] << i << j << RUsq_ckm(i,j).im << "# Im(R_Su)(" << i << "," << j << ")";
          }
        }

      SLHAea_add_block(slha, "DSQMIX");
      for(int i=1; i<=6; i++)
        for(int j=1; j<=6; j++)
        {
          slha["DSQMIX"][""] << i << j << RDsq_ckm(i,j).re << "# R_Sd(" << i << "," << j << ")";
          if(RDsq_ckm(i,j).im != 0)
          {
            SLHAea_check_block(slha, "IMDSQMIX", i, true);
            slha["IMDSQMIX"][""] << i << j << RDsq_ckm(i,j).im << "# Im(R_Sd)(" << i << "," << j << ")";
          }
        }

      SLHAea_add_block(slha, "SELMIX");
      for(int i=1; i<=6; i++)
        for(int j=1; j<=6; j++)
        {
          slha["SELMIX"][""] << i << j << RSl_pmns(i,j).re << "# R_Sl(" << i << "," << j << ")";
          if(RSl_pmns(i,j).im != 0)
          {
            SLHAea_check_block(slha, "IMSELMIX", i, true);
            slha["IMSELMIX"][""] << i << j << RSl_pmns(i,j).im << "# Im(R_Sl)(" << i << "," << j << ")";
          }
        }

      SLHAea_add_block(slha, "SNUMIX");
      for(int i=1; i<=3; i++)
        for(int j=1; j<=3; j++)
        {
          slha["SNUMIX"][""] << i << j << RSn_pmns(i,j).re << "# R_Sn(" << i << "," << j << ")";
          if(RSn_pmns(i,j).im != 0)
          {
            SLHAea_check_block(slha, "IMSNUMIX", i, true);
            slha["IMSNUMIX"][""] << i << j << RSn_pmns(i,j).im << "# Im(R_Sn)(" << i << "," << j << ")";
          }
        }
 
    }
    else
    {
      SLHAea_add_block(slha, "STOPMIX");
      for(int i=1; i<=2; i++)
        for(int j=1; j<=2; j++)
        {
          slha["STOPMIX"][""] << i << j << RUsq_ckm(i+4,j+4).re << "# R_st(" << i << "," << j << ")";
          if(RUsq_ckm(i+4,j+4).im != 0)
          {
            SLHAea_check_block(slha, "IMSTOPMIX", i, true);
            slha["IMSTOPMIX"][""] << i << j << RUsq_ckm(i+4,j+4).im << "# Im(R_st)(" << i << "," << j << ")";
          }
        }

      SLHAea_add_block(slha, "SBOTMIX");
      for(int i=1; i<=2; i++)
        for(int j=1; j<=2; j++)
        {
          slha["SBOTMIX"][""] << i << j << RDsq_ckm(i+4,j+4).re << "# R_sb(" << i << "," << j << ")";
          if(RDsq_ckm(i+4,j+4).im != 0)
          {
            SLHAea_check_block(slha, "IMSBOTMIX", i, true);
            slha["IMSBOTMIX"][""] << i << j << RDsq_ckm(i+4,j+4).im << "# Im(R_sb)(" << i << "," << j << ")";
          }
        }

      SLHAea_add_block(slha, "STAUMIX");
      for(int i=1; i<=2; i++)
        for(int j=1; j<=2; j++)
        {
          slha["STAUMIX"][""] << i << j << RSl_pmns(i+4,j+4).re << "# R_sta(" << i << "," << j << ")";
          if(RSl_pmns(i+4,j+4).im != 0)
          {
            SLHAea_check_block(slha, "IMSTAUMIX", i, true);
            slha["IMSTAUMIX"][""] << i << j << RSl_pmns(i+4,j+4).im << "# Im(R_sta)(" << i << "," << j << ")";
          }
        }
    }


    // Blocks NMIX, UMIX, VMIX
    SLHAea_add_block(slha, "NMIX");
    for(int i=1; i<=4; i++)
      for(int j=1; j<=4; j++)
      {
        slha["NMIX"][""] << i << j << Nr(i,j).re << "# N(" << i << j << ")";
        if(Nr(i,j).im != 0)
        { 
          SLHAea_check_block(slha, "IMNMIX", i, true);
          slha["IMNMIX"][""] << i << j << Nr(i,j).im << "# Im(N)(" << i << j << ")";
        }
      }

    SLHAea_add_block(slha, "UMIX");
    for(int i=1; i<=2; i++)
      for(int j=1; j<=2; j++)
      {
        slha["UMIX"][""] << i << j << (*U)(i,j).re << "# U(" << i << j << ")";
        if((*U)(i,j).im != 0)
        {
          SLHAea_check_block(slha, "IMUMIX", i, true);
          slha["IMUMIX"][""] << i << j << (*U)(i,j).im << "# Im(U)(" << i << j << ")";
        }
      }


    SLHAea_add_block(slha, "VMIX");
    for(int i=1; i<=2; i++)
      for(int j=1; j<=2; j++)
      {
        slha["VMIX"][""] << i << j << (*V)(i,j).re << "# V(" << i << j << ")";
        if((*V)(i,j).im != 0)
        {
          SLHAea_check_block(slha, "IMVMIX", i, true);
          slha["IMVMIX"][""] << i << j << (*V)(i,j).im << "# Im(V)(" << i << j << ")";
        }
      }

    // Block GAMBIT
    SLHAea_add_block(slha, "GAMBIT");
    slha["GAMBIT"][""] << 1 << *m_GUT << "# Input scale of (upper) boundary contidions, e.g. GUT scale";

    //Create Spectrum object
    static const Spectrum::mc_info mass_cut;
    static const Spectrum::mr_info mass_ratio_cut;
    Spectrum spectrum = spectrum_from_SLHAea<MSSMSimpleSpec, SLHAstruct>(slha,slha,mass_cut,mass_ratio_cut);

    // Add the high scale variable by hand
    spectrum.get_HE().set_override(Par::mass1, SLHAea::to<double>(slha.at("GAMBIT").at(1).at(1)), "high_scale", true);

    return spectrum;

  }

  // Function to read data from the Gambit inputs and fill SPheno internal variables
  void ReadingData(const Finputs &inputs)
  {

    InitializeStandardModel(inputs.sminputs);
    try{ InitializeLoopFunctions(); }
    catch(std::runtime_error e) { invalid_point().raise(e.what()); }

    *ErrorLevel = -1;
    *GenerationMixing = false;
    *L_BR = false;
    *L_CS = false;

    try{ Set_All_Parameters_0(); }
    catch(std::runtime_error e) { invalid_point().raise(e.what()); }

    *TwoLoopRGE = true;

    *kont = 0;

    /****************/
    /* Block MODSEL */
    /****************/

    *GenerationMixing = inputs.options->getValueOrDef<bool>(true, "GenerationMixing");

    /******************/
    /* Block SMINPUTS */
    /******************/
    // Already in InitializeStandardModel

    /****************/
    /* Block VCKMIN */
    /****************/
    // Already in SMInputs

    /****************/
    /* Block FCONST */
    /****************/
    // Some hadron constants, not really needed

    /***************/
    /* Block FMASS */
    /***************/
    // Masses of hadrons, not really needed

    /***************/
    /* Block FLIFE */
    /***************/
    // Lifetimes of hadrons, not really needed

    /*******************************/
    /* Block SPHENOINPUT (options) */
    /*******************************/

    // 1, Error_Level
    *ErrorLevel = inputs.options->getValueOrDef<Finteger>(-1, "ErrorLevel");

    // 2, SPA_convention
    *SPA_convention = inputs.options->getValueOrDef<bool>(false, "SPA_convention");
    if(*SPA_convention)
    {
      Freal8 scale = 1.0E6;  // SPA convention is 1 TeV
      try {SetRGEScale(scale); }
      catch(std::runtime_error e) { invalid_point().raise(e.what()); }
    }

    // 3, External_Spectrum
    // GAMBIT: no need for external spectrum options
    *External_Spectrum = false;
    *External_Higgs = false;

    // 4, Use_Flavour_States
    // GAMBIT: private variable, cannot import

    // 5, FermionMassResummation
    *FermionMassResummation = inputs.options->getValueOrDef<bool>(true, "FermionMassResummation");

    // 6, Ynu_at_MR3, Fixed_Nu_Yukawas
    *Ynu_at_MR3 = false;
    *Fixed_Nu_Yukawas = false;

    // 7, Only_1loop_Higgsmass
    *Only_1loop_Higgsmass = inputs.options->getValueOrDef<bool>(false, "Only_1loop_Higgsmass");

    // 8, calculates Masses for extra scales if required
    *Calc_Mass = inputs.options->getValueOrDef<bool>(false, "Calc_Mass");

    // 9, use old version of BoundaryEW
    *UseNewBoundaryEW = inputs.options->getValueOrDef<bool>(true, "UseNewBoundaryEW");

    // 10, use old version to calculate scale
    *UseNewScale = inputs.options->getValueOrDef<bool>(true, "UseNewScale");

    // 11-13, whether to calculate branching ratios or not, L_BR
    *L_BR = false;

    // 21-26, whether to calculate cross sections or not, L_CS
    *L_CS = false;

    // 31, setting a fixed GUT scale, GUTScale
    Freal8 GUTScale = inputs.options->getValueOrDef<Freal8>(0.0, "GUTScale");
    if(GUTScale > 0.0)
    {
      try{ SetGUTScale(GUTScale); }
      catch(std::runtime_error e) { invalid_point().raise(e.what()); }
    }

    // 32, requires strict unification, StrictUnification
    Flogical StrictUnification = inputs.options->getValueOrDef<bool>(false, "StrictUnification");
    if(StrictUnification)
    {
      try{ SetStrictUnification(StrictUnification); }
      catch(std::runtime_error e) { invalid_point().raise(e.what()); }
    }

    // 34, precision of mass calculation, delta_mass
    *delta_mass = inputs.options->getValueOrDef<Freal8>(0.00001, "delta_mass");

    // 35, maximal number of iterations, n_run
    *n_run = inputs.options->getValueOrDef<Finteger>(40, "n_run");

    // 36 write out debug information
    *WriteOut = false;

    // 37, if = 1 -> CKM through V_u, if = 2 CKM through V_d, YukawaScheme
    Finteger YukawaScheme = inputs.options->getValueOrDef<Finteger>(1, "YukawaScheme");
    if(YukawaScheme == 1 or YukawaScheme == 2)
    {
      try{ SetYukawaScheme(YukawaScheme); }
      catch(std::runtime_error e) { invalid_point().raise(e.what()); }
    }

    // 38, set looplevel of RGEs, TwoLoopRGE
    *TwoLoopRGE = inputs.options->getValueOrDef<bool>(true, "TwoLoopRGE");
    if(*TwoLoopRGE)
      *ThreeLoopRGE = inputs.options->getValueOrDef<bool>(false, "ThreeLoopRGE");

    // 39, write additional SLHA1 file, Write_SLHA1
    // GAMBIT: Always SLHA2
    *Write_SLHA1 = false;

    // 40, alpha(0), Alpha
    Freal8 alpha = 1.0/137.035999074;
    *Alpha = inputs.options->getValueOrDef<Freal8>(alpha,"Alpha");

    // 41, Z-boson width, gamZ
    *gamZ = inputs.options->getValueOrDef<Freal8>(2.49,"gamZ");

    // 42, W-boson width, gamW
    *gamW = inputs.options->getValueOrDef<Freal8>(2.06,"gamW");

    // 45, in case of large logs for m_h switch to 1-loop calculation
    *Switch_to_1_loop_mh = inputs.options->getValueOrDef<bool>(false, "Switch_to_1_loop_mh");

    // 48, switch on NNNL fit formula for m_t and alpha_s values at Q=m_t 
    *l_mt_3loop = inputs.options->getValueOrDef<bool>(false,"l_mt_3loop");

    // 49, switch on SM decoupling
    *l_SM_decoupling = inputs.options->getValueOrDef<bool>(true, "l_SM_decoupling");

    // 80, exit for sure with non-zero value if a problem occurs
    *Non_Zero_Exit = false;

    // 89, quick and dirty way to implement model by Suchita Kulkarni
    *Model_Suchita = false;

    // 90, add R-parity at low energies
    *Add_Rparity = false;

    // 91, fit RP parameters such, that neutrino data are o.k.
    *l_fit_RP_parameters = false;

    // 92, for Pythia input
    // GAMBIT: private variable, cannot import

    // 93, calculates cross section in case of RP, only partially implemented
    *l_CSrp = false;

    // 94, calculates cross section in case of RP, only partially implemented
    // GAMBIT: private variable, cannot import

    // 99, MADGraph output style, some additional information
    // GAMBIT: private variable, cannot import

    // 100, use bsstep instead of rkqs
    Flogical Use_bsstep_instead_of_rkqs = inputs.options->getValueOrDef<bool>(false, "Use_bsstep_instead_of_rkqs");
    if(Use_bsstep_instead_of_rkqs)
    {
      try{ Set_Use_bsstep_instead_of_rkqs(Use_bsstep_instead_of_rkqs); }
      catch(std::runtime_error e) { invalid_point().raise(e.what()); }
    }

    // 101, use rzextr instead of pzextr
    Flogical Use_rzextr_instead_of_pzextr = inputs.options->getValueOrDef<bool>(false, "Use_rzextr_instead_of_pzextr");
    if(Use_rzextr_instead_of_pzextr)
    {
      try{ Set_Use_rzextr_instead_of_pzextr(Use_rzextr_instead_of_pzextr); }
      catch(std::runtime_error e) { invalid_point().raise(e.what()); }
    }

    // 110, write output for LHC observables
    // GAMBIT: private variable, cannot import

    // Silence screen output, added by GAMBIT to SPheno
    *SilenceOutput = inputs.options->getValueOrDef<bool>(false, "SilenceOutput");

    /****************/
    // Block MINPAR //
    /****************/
    if(inputs.param.find("M0") != inputs.param.end())
    {
      for(int i=1; i<=3; i++)
        (*M2D_0_sckm)(i,i).re = pow(*inputs.param.at("M0"),2);
      *M2E_0_pmns = *M2D_0_sckm;
      *M2L_0_pmns = *M2D_0_sckm;
      *M2_R_0 = *M2D_0_sckm;
      *M2Q_0_sckm = *M2D_0_sckm;
      *M2U_0_sckm = *M2D_0_sckm;
      for(int i=1; i<=2; i++)
        (*M2_H_0)(i) = pow(*inputs.param.at("M0"),2);
      *M2_T_0 = *M2_H_0;
    }
    // M12
    if(inputs.param.find("M12") != inputs.param.end())
    {
      for(int i=1; i<=3; i++)
        (*Mi_0)(i).re = *inputs.param.at("M12");
    }
    // TanBeta
    if(inputs.param.find("TanBeta") != inputs.param.end())
    {
      *tanb = *inputs.param.at("TanBeta");
      *tanb_mZ = *tanb;
    }
    // SignMu
    if(inputs.param.find("SignMu") != inputs.param.end())
    {
      phase_mu->re = *inputs.param.at("SignMu");
    }
    // A0
    if(inputs.param.find("A0") != inputs.param.end())
    {
      for(int i=1; i<=3; i++)
        (*AoY_d_0)(i,i).re = *inputs.param.at("A0");
      *AoY_l_0 = *AoY_d_0;
      *AoY_u_0 = *AoY_d_0;
      *AoY_nu_0 = *AoY_d_0;
      *AoT_0 = *AoY_d_0;
      for(int i=1; i<=2; i++)
        (*Aolam12_0)(i).re = *inputs.param.at("A0");
    }

    /****************/
    /* Block EXTPAR */
    /****************/
    // Q_in
    if(inputs.param.find("Qin") != inputs.param.end())
      SetRGEScale(*inputs.param.at("Qin"));
    // M_1
    if(inputs.param.find("M1") != inputs.param.end())
    {
      (*Mi_0)(1).re = *inputs.param.at("M1");
      (*Mi)(1).re = *inputs.param.at("M1");
    }
    // M_2
    if(inputs.param.find("M2") != inputs.param.end())
    {
      (*Mi_0)(2).re = *inputs.param.at("M2");
      (*Mi)(2).re = *inputs.param.at("M2");
    }
    // M_3
    if(inputs.param.find("M3") != inputs.param.end())
    {
      (*Mi_0)(3).re = *inputs.param.at("M3");
      (*Mi)(3).re = *inputs.param.at("M3");
    }
    // A_t
    if(inputs.param.find("Au_33") != inputs.param.end())
    {
      (*AoY_u)(3,3).re = *inputs.param.at("Au_33");
      *At_save = (*AoY_u)(3,3);
      *AoY_u_0 = *AoY_u;
    }
    // A_b
    if(inputs.param.find("Ad_33") != inputs.param.end())
    {
      (*AoY_d)(3,3).re = *inputs.param.at("Ad_33");
      *Ab_save = (*AoY_d)(3,3);
      *AoY_d_0 = *AoY_d;
    }
    // A_tau
    if(inputs.param.find("Ae_33") != inputs.param.end())
    {
      (*AoY_l)(3,3).re = *inputs.param.at("Ae_33");
      *Atau_save = (*AoY_l)(3,3);
      *AoY_l_0 = *AoY_l;
    }
    // M^2_Hd
    if(inputs.param.find("mHd2") != inputs.param.end())
    {
      (*M2_H)(1) = *inputs.param.at("mHd2");
      (*M2_H_0)(1) = *inputs.param.at("mHd2");
    }
    // M^2_Hu
    if(inputs.param.find("mHu2") != inputs.param.end())
    {
      (*M2_H)(2) = *inputs.param.at("mHu2");
      (*M2_H_0)(2) = *inputs.param.at("mHu2");
    }
    // Mu
    if(inputs.param.find("mu") != inputs.param.end())
    {
      mu->re = *inputs.param.at("mu");
    }
    // MA^2
    if(inputs.param.find("mA") != inputs.param.end())
    {
      (*mP0)(2) = *inputs.param.at("mA");
      (*mP02)(2) = pow((*mP0)(2),2); 
    }

    for(int i=1; i<=3; i++)
      for(int j=1; j<=3; j++)
      {
        /********/
        /* TUIN */
        /********/
        std::stringstream parname;
        parname << "Au_" << i << j;
        if(inputs.param.find(parname.str()) != inputs.param.end())
        {
          (*Au_0_sckm)(i,j).re = *inputs.param.at(parname.str());
          // unfortunatly there is a transpose due to the RGE implemenation
          (*Au_sckm)(j,i).re = *inputs.param.at(parname.str());
          *l_Au = true;
        }

        /********/
        /* TDIN */
        /********/
        parname.str(std::string());
        parname << "Ad_" << i << j;
        if(inputs.param.find(parname.str()) != inputs.param.end())
        {
          (*Ad_0_sckm)(i,j).re = *inputs.param.at(parname.str());
          // unfortunatly there is a transpose due to the RGE implemenation
          (*Ad_sckm)(j,i).re = *inputs.param.at(parname.str());
          *l_Ad = true;
        }

        /********/
        /* TEIN */
        /********/
        parname.str(std::string());
        parname << "Ae_" << i << j;
        if(inputs.param.find(parname.str()) != inputs.param.end())
        {
          (*Al_0_pmns)(i,j).re = *inputs.param.at(parname.str());
          // unfortunatly there is a transpose due to the RGE implemenation
          (*Al_pmns)(j,i).re = *inputs.param.at(parname.str());
          *l_Al = true;
        }
      }

    for(int i=1; i<=3; i++)
      for(int j=i; j<=3; j++)
      {
        /**********/
        /* MSL2IN */
        /**********/
        std::stringstream parname;
        parname << "ml2_" << i << j;
        if(inputs.param.find(parname.str()) != inputs.param.end())
        {
          (*M2L_pmns)(i,j).re = *inputs.param.at(parname.str());
          *M2L_0_pmns = *M2L_pmns;
          *l_ML = true;
        }
        /**********/
        /* MSE2IN */
        /**********/
        parname.str(std::string());
        parname << "me2_" << i << j;
        if(inputs.param.find(parname.str()) != inputs.param.end())
        {
          (*M2E_pmns)(i,j).re = *inputs.param.at(parname.str());
          *M2E_0_pmns = *M2E_pmns;
          *l_ME = true;
        }
        /**********/
        /* MSQ2IN */
        /**********/
        parname.str(std::string());
        parname << "mq2_" << i << j;
        if(inputs.param.find(parname.str()) != inputs.param.end())
        {
          (*M2Q_sckm)(i,j).re = *inputs.param.at(parname.str());
          *M2Q_0_sckm = *M2Q_sckm;
          *l_MQ = true;
        }
        /**********/
        /* MSU2IN */
        /**********/
        parname.str(std::string());
        parname << "mu2_" << i << j;
        if(inputs.param.find(parname.str()) != inputs.param.end())
        {
          (*M2U_sckm)(i,j).re = *inputs.param.at(parname.str());
          *M2U_0_sckm = *M2U_sckm;
          *l_MU = true;
        }
        /**********/
        /* MSD2IN */
        /**********/
        parname.str(std::string());
        parname << "md2_" << i << j;
        if(inputs.param.find(parname.str()) != inputs.param.end())
        {
          (*M2D_sckm)(i,j).re = *inputs.param.at(parname.str());
          *M2D_0_sckm = *M2D_sckm;
          *l_MD = true;
        }
      }

    // No other blocks are relevant at this stage

  }

  void InitializeStandardModel(const SMInputs &sminputs)
  {

    *kont = 0;

    // Contributions to alpha(m_Z), based on F. Jegerlehner, hep-ph/0310234 and Fanchiotti, Kniehl, Sirlin PRD 48 (1993) 307
    *Delta_Alpha_Lepton = 0.04020;
    *Delta_Alpha_Hadron = 0.027651;

    // Z-boson
    *mZ = sminputs.mZ;          // mass
    *gamZ = 2.4952;             // width, values henceforth from StandardModel.f90
    (*BrZqq)(1) = 0.156;        // branching ratio in d \bar{d}
    (*BrZqq)(2) = 0.156;        // branching ratio in s \bar{s}
    (*BrZqq)(3) = 0.151;        // branching ratio in b \bar{b}
    (*BrZqq)(4) = 0.116;        // branching ratio in u \bar{u}
    (*BrZqq)(5) = 0.12;         // branching ratio in c \bar{c}
    (*BrZll)(1) = 0.0336;       // branching ratio in e+ e-
    (*BrZll)(2) = 0.0336;       // branching ratio in mu+ mu-
    (*BrZll)(3) = 0.0338;       // branching ratio in tau+ tau-
    *BrZinv = 0.2;              // invisible branching ratio

    *mZ2 = *mZ * *mZ;
    *gamZ2 = *gamZ * *gamZ;
    *gmZ = *gamZ * *mZ;
    *gmZ2 = *gmZ * *gmZ;

    // W-boson
    *mW = 80.385;
    *gamW = 2.085;
    (*BrWqq)(1) = 0.35;
    (*BrWqq)(2) = 0.35;
    for(int i=1; i<=3; i++)
      (*BrWln)(i) = 0.1;

    *mW2 = pow(*mW, 2);
    *gamW2 = pow(*gamW, 2);
    *gmW = *gamW * *mW;
    *gmW2 = pow(*gmW, 2);

    // lepton masses: e, muon, tau
    (*mf_l)(1) = sminputs.mE;
    (*mf_l)(2) = sminputs.mMu;
    (*mf_l)(3) = sminputs.mTau;

    // neutrino masses
    (*mf_nu)(1) = sminputs.mNu1;
    (*mf_nu)(2) = sminputs.mNu2;
    (*mf_nu)(3) = sminputs.mNu3;

    // scale where masses of light quarks are defined [in GeV]
    (*Q_light_quarks) = 2;

    // up-quark masses: u, c, t
    (*mf_u)(1) = sminputs.mU;
    (*mf_u)(2) = sminputs.mCmC;
    (*mf_u)(3) = sminputs.mT;

    // down-quark masses: d, s, b
    (*mf_d)(1) = sminputs.mD;
    (*mf_d)(2) = sminputs.mS;
    (*mf_d)(3) = sminputs.mBmB;

    for(int i=1; i<=3; i++)
    {
       (*mf_l2)(i) = pow((*mf_l)(i),2);
       (*mf_u2)(i) = pow((*mf_u)(i),2);
       (*mf_d2)(i) = pow((*mf_d)(i),2);
    }

   // couplings: Alpha(Q=0), Alpha(mZ), Alpha_S(mZ), Fermi constant G_F
    *Alpha =  1.0/137.035999074;
    *Alpha_mZ = 1.0/sminputs.alphainv;
    *AlphaS_mZ = sminputs.alphaS;
    *G_F = sminputs.GF;

    // for ISR correction in e+e- annihilation
    *KFactorLee = 1.0 + (M_PI/3.0 - 1.0/(2*M_PI))*(*Alpha);

    // CKM matrix
    *lam_wolf = sminputs.CKM.lambda;
    *A_wolf = sminputs.CKM.A;
    *rho_wolf = sminputs.CKM.rhobar;
    *eta_wolf = sminputs.CKM.etabar;


    double s12 = sminputs.CKM.lambda;
    double s23 = pow(s12,2) * sminputs.CKM.A;
    double s13 = s23 * sminputs.CKM.lambda * sqrt(pow(sminputs.CKM.etabar,2) + pow(sminputs.CKM.rhobar,2));
    double phase = atan(sminputs.CKM.etabar/sminputs.CKM.rhobar);

    double c12 = sqrt(1.0 - s12*s12);
    double c23 = sqrt(1.0 - s23*s23);
    double c13 = sqrt(1.0 - s13*s13);

    std::complex<double> i = -1;
    i = sqrt(i);

    (*CKM)(1,1) = c12 * c13;
    (*CKM)(1,2) = s12 * c13;
    (*CKM)(1,3) = s13 * exp(-i * phase);
    (*CKM)(2,1) = -s12*c23 -c12*s23*s13 * exp(i * phase);
    (*CKM)(2,2) = c12*c23 -s12*s23*s13 * exp(i * phase );
    (*CKM)(2,3) = s23 * c13;
    (*CKM)(3,1) = s12*s23 -c12*c23*s13 * exp(i * phase );
    (*CKM)(3,2) = -c12*s23 - s12*c23*s13 * exp( i * phase );
    (*CKM)(3,3) = c23 * c13;

    for(int i=1; i<=3; i++)
    {
      (*mf_l_mZ)(i) = 0.0;
      (*mf_d_mZ)(i) = 0.0;
      (*mf_u_mZ)(i) = 0.0;
    }
    try{ CalculateRunningMasses(*mf_l, *mf_d, *mf_u, *Q_light_quarks, *Alpha_mZ, *AlphaS_mZ, *mZ, *mf_l_mZ, *mf_d_mZ, *mf_u_mZ, *kont); }
    catch(std::runtime_error e) { invalid_point().raise(e.what()); }

    // PMNS matrix
    *theta_12 = sminputs.PMNS.theta12;
    *theta_23 = sminputs.PMNS.theta23;
    *theta_13 = sminputs.PMNS.theta13;
    *delta_nu = sminputs.PMNS.delta13;
    *alpha_nu1 = sminputs.PMNS.alpha1;
    *alpha_nu2 = sminputs.PMNS.alpha2;

    s12 = sin(*theta_12);
    s23 = sin(*theta_23);
    s13 = sin(*theta_13);

    c12 = sqrt(1.0 - pow(s12,2));
    c23 = sqrt(1.0 - pow(s23,2));
    c13 = sqrt(1.0 - pow(s13,2));

    (*Unu)(1,1) = c12 * c13 * exp(-0.5*i * *alpha_nu1);
    (*Unu)(1,2) = s12 * c13 * exp(-0.5*i * *alpha_nu1);
    (*Unu)(1,3) = s13 * exp(-i * *delta_nu) * exp(-0.5*i * *alpha_nu1);
    (*Unu)(2,1) = -s12*c23 - c12*s23*s13 * exp(i * *delta_nu) * exp(-0.5*i * *alpha_nu2);
    (*Unu)(2,2) = c12*c23 - s12*s23*s13 * exp(i * *delta_nu) * exp(-0.5*i * *alpha_nu2);
    (*Unu)(2,3) = s23 * c13 * exp(-0.5*i * *alpha_nu2);
    (*Unu)(3,1) = s12*s23 - c12*c23*s13 * exp(i * *delta_nu);
    (*Unu)(3,2) = -c12*s23 - s12*c23*s13 * exp(i * *delta_nu);
    (*Unu)(3,3) = c23 * c13;

    if(*kont != 0)
      ErrorHandling(*kont);
  }

  // Function that handles errors
  void ErrorHandling(const int &kont)
  {

    str message;

    if (kont > 0 and kont <= 31)
      message = (*Math_Error)(kont).str();
    else if (kont > 100 and kont <= 102)
      message = (*SM_Error)(kont-100).str();
    else if (kont > 200 and kont <= 233)
      message = (*SusyM_Error)(kont-200).str();
    else if (kont > 300 and kont <= 315)
      message = (*InOut_Error)(kont-300).str();
    else if (kont > 400 and kont <= 422)
      message = (*Sugra_Error)(kont-400).str();
    else if (kont > 500 and kont <= 525)
      message = (*LoopMass_Error)(kont-500).str();
    else if (kont > 600 and kont <= 609)
      message = (*TwoLoopHiggs_Error)(kont-600).str();
    else if (kont > 1000 and kont <= 1010)
      message = (*MathQP_Error)(kont-1000).str();
    else
      message = "GAMBIT caught an error in SPheno. Check the SPheno output for more info.";

    logger() << message << EOM;
    invalid_point().raise(message);

    return ;

  }
}
END_BE_NAMESPACE


// Initialisation function (definition)
BE_INI_FUNCTION
{

  // Scan-level initialisation
  static bool scan_level = true;
  if (scan_level)
  {
    // Dump all internal output to stdout
    *ErrCan = 6;

    // Set the function pointer in SPheno to our ErrorHandler callback function
    *ErrorHandler_cptr = & CAT_4(BACKENDNAME,_,SAFE_VERSION,_ErrorHandler);

    try{ Set_All_Parameters_0(); }
    catch(std::runtime_error e) { invalid_point().raise(e.what()); }

    /****************/
    /* Block MODSEL */
    /****************/
    if((*ModelInUse)("CMSSM") or (*ModelInUse)("MSSM63atMGUT"))
    {
      *HighScaleModel = "mSUGRA";
      SetHighScaleModel("SUGRA");
    }
    else if((*ModelInUse)("MSSM63atQ"))
    {
      *HighScaleModel = "MSSM";
    }
    else
    {
      str message = "Model not recognised";
      logger() << message << EOM;
      invalid_point().raise(message);
    }

  }
  scan_level = false;


}
END_BE_INI_FUNCTION

