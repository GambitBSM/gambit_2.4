//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend header for the plc backend.
///
///  Compile-time registration of available
///  functions and variables from this backend.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Selim C. Hotinli
///          (selim.hotinli14@pimperial.ac.uk)
///  \date 2016 Sep
///
///  \author Patrick Stoecker
///          (stoecker@physik.rwth-aachen.de)
///  \date 2017 Nov
///
///  *********************************************

#define BACKENDNAME plc
#define BACKENDLANG CC
#define VERSION 2.0
#define SAFE_VERSION 2_0

#ifndef PLC_PATH
#define PLC_PATH "EMPTY"
#endif

#include "gambit/Utils/util_functions.hpp"

// Load it
LOAD_LIBRARY

BE_FUNCTION(clik_init, clik_object*, (char*,clik_error**),"clik_init","clik_initialize")
BE_FUNCTION(clik_lensing_init, clik_lensing_object*, (char*,clik_error**),"clik_lensing_init","clik_lensing_initialize")
BE_FUNCTION(initError, clik_error* , () ,"initError","clik_initialize_error")
BE_FUNCTION(clik_compute, double, (clik_object*,double*,clik_error**), "clik_compute","clik_compute_loglike")
BE_FUNCTION(clik_lensing_compute, double, (clik_lensing_object*,double*,clik_error**), "clik_lensing_compute","clik_lensing_compute_loglike")
BE_FUNCTION(clik_get_has_cl, void, (clik_object*,int,clik_error**), "clik_get_has_cl","clik_get_list_cls")
BE_FUNCTION(clik_get_extra_parameter_names, int ,(clik_object*,parname**,clik_error**),"clik_get_extra_parameter_names","clik_get_extra_parameter_names")
BE_FUNCTION(clik_get_lmax, void , (clik_object*,int,clik_error**), "clik_get_lmax", "clik_get_lmax")
BE_FUNCTION(clik_cleanup, void , (clik_object**) , "clik_cleanup", "clik_cleanup")
BE_FUNCTION(clik_get_version, char*, (clik_object*,clik_error**), "clik_get_version" , "clik_get_version")

BE_CONV_FUNCTION(return_clikid_plik_dx11dr2_HM_v18_TTTEEE, clik_object*, (), "return_high_TTTEEE", ())
BE_CONV_FUNCTION(return_clikid_plik_dx11dr2_HM_v18_TT, clik_object*, (), "return_high_TT", ())
BE_CONV_FUNCTION(return_clikid_plik_lite_v18_TT, clik_object*, (), "return_high_TT_lite", ())
BE_CONV_FUNCTION(return_clikid_plik_lite_v18_TTTEEE, clik_object*, (), "return_high_TTTEEE_lite", ())
BE_CONV_FUNCTION(return_clikid_lowl_SMW_70_dx11d_2014, clik_object*, (), "return_lowp_TT", ())
BE_CONV_FUNCTION(return_smica_g30_ftl_full_pp, clik_lensing_object*, (), "return_lensing", ())
BE_CONV_FUNCTION(initialize_high_TTTEEE,void,(),"initialize_high_TTTEEE",())
BE_CONV_FUNCTION(initialize_high_TT,void,(),"initialize_high_TT",())
BE_CONV_FUNCTION(initialize_high_TT_lite,void,(),"initialize_high_TT_lite",())
BE_CONV_FUNCTION(initialize_high_TTTEEE_lite,void,(),"initialize_high_TTTEEE_lite",())
BE_CONV_FUNCTION(initialize_lowp_TT,void,(),"initialize_lowp_TT",())
BE_CONV_FUNCTION(initialize_lensing,void,(),"initialize_lensing",())
BE_CONV_FUNCTION(data_cleanup,void,(),"data_cleanup",())
BE_CONV_FUNCTION(set_planck_path,void,(std::string),"set_planck_path",())

BE_INI_FUNCTION
{
  static bool scan_level = true;
  if (scan_level)
  {
    std::string planck_path = runOptions->getValueOrDef<std::string>(PLC_PATH,"planck_path");
    set_planck_path(byVal(planck_path));
    if (*InUse::return_clikid_plik_dx11dr2_HM_v18_TTTEEE) initialize_high_TTTEEE();
    if (*InUse::return_clikid_plik_dx11dr2_HM_v18_TT) initialize_high_TT();
    if (*InUse::return_clikid_plik_lite_v18_TT) initialize_high_TT_lite();
    if (*InUse::return_clikid_plik_lite_v18_TTTEEE) initialize_high_TTTEEE_lite();
    if (*InUse::return_clikid_lowl_SMW_70_dx11d_2014) initialize_lowp_TT();
    if (*InUse::return_smica_g30_ftl_full_pp) initialize_lensing();
  }
  scan_level = false;
}
END_BE_INI_FUNCTION

BE_NAMESPACE
{
  std::string plc_location;

  clik_object * clikid_plik_dx11dr2_HM_v18_TT;
  clik_object * clikid_plik_dx11dr2_HM_v18_TTTEEE;
  clik_object * clikid_plik_lite_v18_TT;
  clik_object * clikid_plik_lite_v18_TTTEEE;
  clik_object * clikid_lowl_SMW_70_dx11d_2014;
  clik_lensing_object * smica_g30_ftl_full_pp;
  clik_error *_err;

  void set_planck_path(std::string planck_path)
  {
    if (planck_path.compare("EMPTY") == 0) backend_error().raise(LOCAL_INFO,"The location of the planck data is not set properly!\n\nEither change the PLC_PATH macro in Backends/include/... .../plc_2_0.hpp or include\n\n  - capability:   plc_2_0_init\n    options:\n      planck_path: /YOUR/PATH/TO/plc_2.0/\n\nin the rules section of your input.");
    if (planck_path.back() != '/') planck_path.push_back('/');
    plc_location = planck_path;
  }

  void initialize_high_TT()
  {
    std::cout << "Loading the high-l TT likelihood" << std::endl;
    std::string full_path = plc_location;
    full_path += "hi_l/plik/plik_dx11dr2_HM_v18_TT.clik";
    if (not Utils::file_exists(full_path))
    {
      std::string ErrMssg = "Error while loading likelihood in plc_2.0:\n\nThe file \'";
      ErrMssg += full_path;
      ErrMssg += "\' does not exist. Hence we will fail to load it.\n(Hint: Is PLC_PATH set properly?)";
      backend_error().raise(LOCAL_INFO,ErrMssg.c_str());
    }
    char* clik_path = (char*)full_path.c_str();
    _err = initError();
    clikid_plik_dx11dr2_HM_v18_TT = clik_init(clik_path,&_err);
  }

  void initialize_high_TTTEEE()
  {
    std::cout << "Loading the high-l TTTEEE likelihood" << std::endl;
    std::string full_path = plc_location;
    full_path += "hi_l/plik/plik_dx11dr2_HM_v18_TTTEEE.clik";
    if (not Utils::file_exists(full_path))
    {
      std::string ErrMssg = "Error while loading likelihood in plc_2.0:\n\nThe file \'";
      ErrMssg += full_path;
      ErrMssg += "\' does not exist. Hence we will fail to load it.\n(Hint: Is PLC_PATH set properly?)";
      backend_error().raise(LOCAL_INFO,ErrMssg.c_str());
    }
    char* clik_path = (char*)full_path.c_str();
    _err = initError();
    clikid_plik_dx11dr2_HM_v18_TTTEEE = clik_init(clik_path,&_err);
  }

  void initialize_high_TT_lite()
  {
    std::cout << "Loading the high-l TT-lite likelihood" << std::endl;
    std::string full_path = plc_location;
    full_path += "hi_l/plik_lite/plik_lite_v18_TT.clik";
    if (not Utils::file_exists(full_path))
    {
      std::string ErrMssg = "Error while loading likelihood in plc_2.0:\n\nThe file \'";
      ErrMssg += full_path;
      ErrMssg += "\' does not exist. Hence we will fail to load it.\n(Hint: Is PLC_PATH set properly?)";
      backend_error().raise(LOCAL_INFO,ErrMssg.c_str());
    }
    char* clik_path = (char*)full_path.c_str();
    _err = initError();
    clikid_plik_lite_v18_TT = clik_init(clik_path,&_err);
  }

  void initialize_high_TTTEEE_lite()
  {
    std::cout << "Loading the high-l TTTEEE-lite likelihood" << std::endl;
    std::string full_path = plc_location;
    full_path += "hi_l/plik_lite/plik_lite_v18_TTTEEE.clik";
    if (not Utils::file_exists(full_path))
    {
      std::string ErrMssg = "Error while loading likelihood in plc_2.0:\n\nThe file \'";
      ErrMssg += full_path;
      ErrMssg += "\' does not exist. Hence we will fail to load it.\n(Hint: Is PLC_PATH set properly?)";
      backend_error().raise(LOCAL_INFO,ErrMssg.c_str());
    }
    char* clik_path = (char*)full_path.c_str();
    _err = initError();
    clikid_plik_lite_v18_TTTEEE = clik_init(clik_path,&_err);
  }

  void initialize_lowp_TT()
  {
    std::cout << "Loading the low-l likelihood (This may take some time)" << std::endl;
    std::string full_path = plc_location;
    full_path += "low_l/bflike/lowl_SMW_70_dx11d_2014_10_03_v5c_Ap.clik";
    if (not Utils::file_exists(full_path))
    {
      std::string ErrMssg = "Error while loading likelihood in plc_2.0:\n\nThe file \'";
      ErrMssg += full_path;
      ErrMssg += "\' does not exist. Hence we will fail to load it.\n(Hint: Is PLC_PATH set properly?)";
      backend_error().raise(LOCAL_INFO,ErrMssg.c_str());
    }
    char* clik_path = (char*)full_path.c_str();
    _err = initError();
    clikid_lowl_SMW_70_dx11d_2014 = clik_init(clik_path,&_err);
  }

  void initialize_lensing()
  {
    std::cout << "Loading the lensing likelihood" << std::endl;
    std::string full_path = plc_location;
    full_path += "lensing/smica_g30_ftl_full_pp.clik_lensing";
    if (not Utils::file_exists(full_path))
    {
      std::string ErrMssg = "Error while loading likelihood in plc_2.0:\n\nThe file \'";
      ErrMssg += full_path;
      ErrMssg += "\' does not exist. Hence we will fail to load it.\n(Hint: Is PLC_PATH set properly?)";
      backend_error().raise(LOCAL_INFO,ErrMssg.c_str());
    }
    char* clik_path = (char*)full_path.c_str();
    _err = initError();
    smica_g30_ftl_full_pp = clik_lensing_init(clik_path,&_err);
  }

  void  data_cleanup()
  {
    clik_cleanup(byVal(&clikid_plik_dx11dr2_HM_v18_TT));
    clik_cleanup(byVal(&clikid_lowl_SMW_70_dx11d_2014));

    //cout << "cleaned-up the data files" << endl;
  }

  clik_object * return_clikid_plik_dx11dr2_HM_v18_TTTEEE()
  {
    return clikid_plik_dx11dr2_HM_v18_TTTEEE;
  }

  clik_object * return_clikid_plik_dx11dr2_HM_v18_TT()
  {
    return clikid_plik_dx11dr2_HM_v18_TT;
  }

  clik_object * return_clikid_plik_lite_v18_TT()
  {
    return clikid_plik_lite_v18_TT;
  }

  clik_object * return_clikid_plik_lite_v18_TTTEEE()
  {
    return clikid_plik_lite_v18_TTTEEE;
  }

  clik_object * return_clikid_lowl_SMW_70_dx11d_2014()
  {
    return clikid_lowl_SMW_70_dx11d_2014;
  }

  clik_lensing_object * return_smica_g30_ftl_full_pp()
  {
    return smica_g30_ftl_full_pp;
  }

}
END_BE_NAMESPACE

#undef PLC_PATH

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
