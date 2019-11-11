#!/usr/bin/env python
#
#  GUM: GAMBIT Universal Models
#  ****************************
#  \file
#
#  Routines for SPheno output from SARAH
#
#  *************************************
#
#  \author Sanjay Bloor
#          (sanjay.bloor12@imperial.ac.uk)
#  \date 2019 July
#
#  \author Tomas Gonzalo
#          (tomas.gonzalo@monash.edu)
#  \date 2019 July, August
#
#  **************************************

from files import *
from setup import *
from models import *
from cmake_variables import *
from distutils.dir_util import copy_tree
from collections import defaultdict
import re
import os.path

# An empty defaultdict in scope, so we don't save to the 
# mug file, but can still use the writing routines in 
# the 'files' module.
d = defaultdict(list)

def copy_spheno_files(model_name, output_dir, spheno_oob_path, sarah_spheno_path):
    """
    Creates a copy of SPheno output in the 
    Outputs/... folder.
    Then create another one, for the patched version.
    """

    dirs = [output_dir + "/SPheno", output_dir+"/SPheno_patched"]

    for newdir in dirs:

        # Remove the directory if it already exists, then make it
        remove_tree_quietly(newdir)
        mkdir_if_absent(newdir)

        # Copy SPheno to the Output directory
        copy_tree(spheno_oob_path, newdir)

        # Now copy from SARAH to the Output directory
        modeldir = newdir + "/" + model_name
        mkdir_if_absent(modeldir)
        copy_tree(sarah_spheno_path, modeldir)

    print("SPheno files moved to output directory, and a copy made.")



"""
PATCHING
"""

def patch_spheno(model_name, patch_dir, flags, particles):
    """
    Applies all patches to SPheno in the GUM 
    Outputs/... directory.
    """

    patch_spheno_makefile(model_name, patch_dir)
    patch_spheno_model_makefile(model_name, patch_dir)
    patch_spheno_src_makefile(model_name, patch_dir)
    patch_control(model_name, patch_dir)
    patch_spheno_model(model_name, patch_dir)
    patch_brs(model_name, patch_dir)
    patch_addloopfunctions(model_name, patch_dir)


    if flags["SupersymmetricModel"] :
        patch_model_data(model_name, patch_dir)
        patch_3_body_decays_susy(model_name, patch_dir, particles)

    print("SPheno files patched.")


def patch_spheno_makefile(model_name, patch_dir):
    """
    Patches $SPheno/Makefile
    """

    filename = patch_dir + "/Makefile"

    if not os.path.exists(filename):
        raise GumError(("Tried to find the file located at " + filename +
                        " but it does not seem to exist!"))

    with open(filename, 'w') as f :
        content = "# please put here your preferred F95/F2003 compiler\n"\
                  "# the options in src/Makefile have been put for the\n"\
                  "# cases NAG's nagfor, gfortran, g95, Lahey's lf95 and Intels ifort\n"\
                  "# Please uncomment the corresponding line\n"\
                  "# F90 = nagfor\n"\
                  "F90 = gfortran\n"\
                  "# F90 = g95\n"\
                  "# F90 = lf95\n"\
                  "# F90 = ifort\n"\
                  "F90_temp := ${F90} # Added by GAMBIT\n"\
                  "override F90 = ${notdir ${F90_temp}} # Added by GAMBIT\n"\
                  "Model = src\n"\
                  "version = 400.00\n"\
                  "all: bin/SPheno lib/libSPheno"+model_name+".so\n"\
                  "bin/SPheno:\n"\
                  "\tcd ${Model} ; ${MAKE} F90=${F90} version=${version}\n"\
                  "lib/libSPheno"+model_name+".so:\n"\
                  "\tcd ${Model} ; ${MAKE} $@ F90=${F90} version=${version}\n"\
                  "clean:\n"\
                  "\trm -f *.o *~ */*.o */*~\n"\
                  "cleanall:\n"\
                  "\trm -f bin/SPheno lib/*.a lib/*.so *.o *~ */*.o */*~ include/*\n"\
                  ".PHONY: bin/SPheno lib/libSPheno"+model_name+" clean cleanall"

        f.write(content)


def patch_spheno_model_makefile(model_name, patch_dir):
    """
    Patches $SPheno/<MODEL>/Makefile
    """
    
    filename = patch_dir + "/" + model_name + "/Makefile" 
    temp_filename = filename + "_temp"  

    if not os.path.exists(filename):
        raise GumError(("Tried to find the file located at " + filename +
                        " but it does not seem to exist!"))

    with open(filename, 'r') as f, open(temp_filename, 'w') as g :
        skip_next_lines = False
        for line in f :
            if line.startswith("name = ") :
                g.write(line)
                g.write("shared = lib/libSPheno" + model_name + ".so\n")
            elif line.startswith("F90=gfortran") :
                content = "F90=gfortran\n"\
                          "comp= -c -O -fPIC -module ${Mdir} -I${InDir}\n"\
                          "LFlagsB= -O\n"\
                          "# Intels ifort,debug modus\n"\
                          "ifeq (${F90},ifort)\n"\
                          "F90=ifort\n"\
                          "comp= -c -g -fPIC -module ${Mdir} -I${InDir}\n"\
                          "LFlagsB= -g -fPIC\n"\
                          "endif\n"\
                          "# gfortran\n"\
                          "ifeq (${F90},gfortran)\n"\
                          "comp= -c -g -fPIC --free-line-length-none -J${Mdir} -I${InDir}\n"\
                          "LFlagsB= -g -fPIC\n"\
                          "endif\n"\
                          "# g95\n"\
                          "ifeq (${F90},g95)\n"\
                          "comp= -c -O -fPIC -fmod=${Mdir} -I${InDir}\n"\
                          "LFlagsB= -O -fPIC\n"\
                          "endif\n"\
                          "# Lahey F95 compiler\n"  \
                          "ifeq (${F90},lf95)\n"\
                          "comp=-c -O -fPIC -M ${Mdir} -I${InDir}\n"\
                          "LFlagsB=-O -fPIC\n"\
                          "endif\n"\
                          "# NAG f95/2003\n"\
                          "ifeq (${F90},nagfor)\n"\
                          "comp= -c -O -fPIC -mdir ${Mdir} -I${InDir}\n"\
                          "LFlagsB= -O -fPIC -DONLYDOUBLE -mdir ${MDir} -I${InDir}\n"\
                          "endif\n"\
                          ".SUFFIXES : .o .ps .f90 .F90 .a\n"
                g.write(content)
                skip_next_lines = True
            elif line.startswith("bin/SPheno" + model_name) :
                content = "${shared}:\n"\
                          "\tcd ../src ; ${MAKE} F90=${F90}\n"\
                          "\t${MAKE} F90=${F90} ${name}\n"\
                          "\t${MAKE} F90=${F90} SPheno" + model_name + ".o\n"\
                          "\t${F90} -c -fPIC TwoLoopMasses/effpotasat.f\n"\
                          "\t${F90} -shared -fPIC -o ../${shared} ${LFlagsB} SPheno" + model_name + ".o effpotasat.o ../lib/libSPheno" + model_name + ".a ../lib/libSPheno.a\n"\
                          "bin/SPheno" + model_name + ":\n"\
                          "ifeq (${cVersion},1)\n"\
                          "\tcd ../src ; ${MAKE} F90=${F90}\n"\
                          "\t${MAKE} F90=${F90} ${name}\n"\
                          "\t${MAKE} F90=${F90} SPheno" + model_name + ".o\n"\
                          "\t${F90} -c -fPIC TwoLoopMasses/effpotasat.f\n"\
                          "\t${F90} -o -fPIC SPheno" + model_name + " ${LFlagsB} SPheno" + model_name + ".o effpotasat.o ../lib/libSPheno" + model_name + ".a ../lib/libSPheno.a\n"\
                          "\tmv SPheno" + model_name + "../bin\n"\
                          "\trm SPheno" + model_name + ".o\n"
                g.write(content)
                for i in range(7): next(f)
                skip_next_lines = False
            elif line.startswith("cleanall:") :
                 g.write(line)
                 g.write("\trm -f bin/SPheno3 lib/*.a lib/*.so *~ */*.o */*~ include/*\n")
                 next(f)
            else :
                if not skip_next_lines :
                    g.write(line)

    os.remove(filename)
    os.rename(temp_filename, filename)

def patch_spheno_src_makefile(model_name, patch_dir):
    """
    Patches $SPheno/src/Makefile
    """

    filename = patch_dir + "/src/Makefile"
    temp_filename = filename + "_temp"

    if not os.path.exists(filename):
        raise GumError(("Tried to find the file located at " + filename +
                        " but it does not seem to exist!"))
            
    with open(filename, 'r') as f, open(temp_filename, 'w') as g :
        skip_next_lines = False
        for line in f :
            if line.startswith("F90 = ifort") :
                content = "F90 = ifort\n"\
                          "comp = -c -fPIC -O -module ${Mdir} -I${InDir}\n"\
                          "LFlagsB = -O -fPIC\n"\
                          "\n"\
                          "# Intels ifort, debug modus\n"\
                          "ifeq (${F90},ifortg)\n"\
                          " F90 = ifort\n"\
                          " comp = -c -g -fPIC -module ${Mdir} -I${InDir}\n"\
                          " LFlagsB = -g -fPIC\n"\
                          "endif\n"\
                          "\n"\
                          "# gfortran\n"\
                          "ifeq (${F90},gfortran)\n"\
                          " comp = -c -O -fPIC -J${Mdir} -I${InDir}\n"\
                          " LFlagsB = -O -fPIC\n"\
                          "endif\n"\
                          "\n"\
                          "# gfortran, any version (Added by GAMBIT)\n"\
                          "ifneq (,$(findstring gfortran,${F90}))\n"\
                          " comp = -c -O -fPIC -J${Mdir} -I${InDir}\n"\
                          " LFlagsB = -O -fPIC\n"\
                          "endif\n"\
                          "\n"\
                          "# g95 \n"\
                          "ifeq (${F90},g95)\n"\
                          " comp = -c -O -fPIC -fmod=${Mdir} -I${InDir}\n"\
                          " LFlagsB = -O -fPIC\n"\
                          "endif\n"\
                          "\n"\
                          "# Lahey F95 compiler\n"\
                          "ifeq (${F90},lf95)\n"\
                          " comp = -c -O -fPIC -M ${Mdir} -I${InDir}\n"\
                          " LFlagsB = -O -fPIC\n"\
                          "endif\n"\
                          "\n"\
                          "# NAG f95/2003\n"\
                          "ifeq (${F90},nagfor)\n"\
                          " comp = -c -O -fPIC -DONLYDOUBLE -mdir ${Mdir} -I${InDir}\n"\
                          " LFlagsB = -O -fPIC\n"\
                          "endif\n\n"
                g.write(content)
                skip_next_lines = True
            elif line.startswith(".SUFFIXES") :
                g.write(line)
                skip_next_lines = False
            else :
                if not skip_next_lines :
                    g.write(line)

            # We do not patch the -U flag in the ar command as it causes issues in OSX

    os.remove(filename)
    os.rename(temp_filename, filename)

def patch_control(model_name, patch_dir):
    """
    Patches $SPheno/src/Control.F90
    """
    
    filename = patch_dir + "/src/Control.F90"
    temp_filename = filename + "_temp"

    if not os.path.exists(filename):
        raise GumError(("Tried to find the file located at " + filename +
                        " but it does not seem to exist!"))
            
    with open(filename, 'r') as f, open(temp_filename, 'w') as g :
        skip_next_lines = False
        for line in f :
            if line.startswith(" Interface Is_NaN") :
                content = "! Added by GAMBIT\n"\
                          "Use, Intrinsic :: iso_c_binding\n"\
                          "Implicit none\n"\
                          "\n"\
                          "Type(c_funptr) :: ErrorHandler_cptr\n"\
                          "\n"\
                          "! Define interface of call-back routine.\n"\
                          "\n"\
                          "Abstract Interface\n"\
                          "  Subroutine callback ()\n"\
                          "    Use, Intrinsic :: iso_c_binding\n"\
                          "  End Subroutine callback\n"\
                          "End Interface\n"\
                          "\n"\
                          "! Variable to swith off screen output\n"\
                          "Logical :: SilenceOutput = .False.\n"\
                          "\n"\
                          "! GAMBIT addition end\n\n"
                g.write(content)
                g.write(line) 
            elif line.startswith(" Subroutine TerminateProgram") :
                content = " ! Subroutine modified by GAMBIT\n"\
                          " Subroutine TerminateProgram\n"\
                          " !-----------------------------------------------------------------------\n"\
                          " ! This subroutine terminates a program if a fatal error occurs.\n"\
                          " ! Before doing this, it writes the tree of calling subroutines to\n"\
                          " ! the file which is connected to the channel ErrCan\n"\
                          " ! written by Werner Porod, 20.9.2000\n"\
                          " !-----------------------------------------------------------------------\n"\
                          " Use, Intrinsic :: iso_c_binding\n"\
                          " Implicit None\n"\
                          " \n"\
                          "  Procedure(callback), Pointer :: ErrorHandler_fptr\n"\
                          "  Integer :: i1\n"\
                          " \n"\
                          "  Write (ErrCan,*) \"  \"\n"\
                          "  Write (ErrCan,*) \"ErrorLevel, Iname:\",ErrorLevel, Iname\n"\
                          "  Write (ErrCan,*) &\n"\
                          "    & \"The error has occured in the following chain of subroutines:\"\n"\
                          "  Do i1=1,Size(NameOfUnit)\n"\
                          "   Write (ErrCan,*) NameOfUnit(i1)\n"\
                          "  End Do\n"\
                          "  Write (ErrCan,*) \"  \"\n"\
                          "  Write (ErrCan,*) \"Hopefully you find the error soon\"\n"\
                          "  Do i1=ErrCan,ErrCan+NumberOfOpenFiles-1\n"\
                          "   Close(i1)\n"\
                          "  End Do\n"\
                          " \n"\
                          "  ! Convert C to Fortran procedure pointer.\n"\
                          "  Call c_f_procpointer(ErrorHandler_cptr, ErrorHandler_fptr)\n"\
                          " \n"\
                          "  ! Call the ErrorHandler\n"\
                          "  Call ErrorHandler_fptr()\n"\
                          " \n"\
                          "  ! This should never happen\n"\
                          "  Write(*,*) \"DEBUG: SPheno has continued past the ErrorHandler call. This should never happen...\"\n"\
                          "  Stop \"Subroutine TerminateProgram\"\n"\
                          " \n"\
                          " End Subroutine TerminateProgram\n\n"
                g.write(content)
                skip_next_lines = True
            elif line.startswith(" Subroutine") :
                g.write(line)
                skip_next_lines = False
            else :
                if not skip_next_lines :
                    g.write(line)

    os.remove(filename)
    os.rename(temp_filename, filename)

def patch_spheno_model(model_name, patch_dir):
    """
    Patches $SPheno/<MODEL>/SPheno<MODEL>.f90
    """
 
    filename = patch_dir + "/" + model_name + "/SPheno" + model_name + ".f90"
    temp_filename = filename + "_temp"

    if not os.path.exists(filename):
        raise GumError(("Tried to find the file located at " + filename +
                        " but it does not seem to exist!"))
            
    with open(filename, 'r') as f, open(temp_filename, 'w') as g:
        for line in f:
            if line.startswith("Program SPheno" + model_name) :
                g.write("!Program SPheno" + model_name + " ! Commented by GAMBIT\n")
                g.write("Module SPheno" + model_name + " ! Added by GAMBIT\n")
            elif line.startswith("Tpar = 0._dp") :
                content = '\n'\
                    "Contains ! Added by GAMBIT\n"\
                    "\n"\
                    "Subroutine SPheno_Main() ! Added by GAMBIT\n"
                g.write(content)
                g.write(line)
            elif line.startswith("Call Set_All_Parameters_0()") :
                content = "!Call Set_All_Parameters_0() ! Commented by GAMBIT\n"\
                    "\n"\
                    "!Qin = SetRenormalizationScale(1.0E3_dp**2)  ! Commented by GAMBIT\n"\
                    "!kont = 0 ! Commented by GAMBIT\n"\
                    "!delta_Mass = 0.0001_dp ! Commented by GAMBIT\n"\
                    "!CalcTBD = .false. ! Commented by GAMBIT\n"\
                    "!Call ReadingData(kont) ! Commented by GAMBIT\n"
                g.write(content)
                for i in range(6) : next(f)
            elif line.startswith(" Call CalculateBR") :
                g.write(line[:17] + "_2" + line[17:])
            elif line.startswith("Call LesHouches_Out") :
                g.write("!"+line)
                line2 = next(f)
                while line2.startswith("&") :
                  g.write("!"+line2)
                  line2 = next(f)
                g.write(line2)
            elif line.startswith("Contains") :
                content = "\n"\
                    "End Subroutine SPheno_Main ! Added by GAMBIT\n"\
                    "\n"\
                    "!Contains ! Commented by GAMBIT\n"
                g.write(content)
            elif line.startswith("kont = 0") :
                line2 = next(f)
                if line2.startswith("Call FirstGuess") :
                    content = "! Added by GAMBIT\n"\
                        "If (SilenceOutput) Then\n"\
                        " close(unit=6)\n"\
                        "Else\n"\
                        " open(unit=6, file=\"/dev/stdout\")\n"\
                        "Endif\n"\
                        "\n"\
                        "kont = 0\n"
                    g.write(content)
                    g.write(line2)
            elif line.startswith("!If (kont.ne.0) Call TerminateProgram") :
                g.write("If (kont.ne.0) Call TerminateProgram\n")
            elif line.startswith("End Program SPheno" + model_name):
                g.write("!End Program SPheno" + model_name + " ! Commented by GAMBIT\n")
                g.write("End Module SPheno" + model_name + " ! Added by GAMBIT\n")
            else:
                g.write(line)

    os.remove(filename)
    os.rename(temp_filename, filename)

def patch_brs(model_name, patch_dir):
    """
    Patches $SPheno/<MODEL>/BranchingRatios_<MODEL>.f90
    """

    filename = patch_dir + "/" + model_name + "/BranchingRatios_" + model_name + ".f90"
    temp_filename = filename + "_temp"

    if not os.path.exists(filename):
        raise GumError(("Tried to find the file located at " + filename +
                        " but it does not seem to exist!"))
            
    with open(filename, 'r') as f, open(temp_filename, 'w') as g :
        for line in f :
            if line.startswith("Subroutine CalculateBR") :
                g.write(line[:22] + "_2" + line[22:])
            elif line.startswith("NameOfUnit(Iname) = \'CalculateBR\'") :
                content = "NameOfUnit(Iname) = \'CalculateBR_2\'\n"\
                    "\n"\
                    "! Added by GAMBIT\n"\
                    "If (SilenceOutput) Then\n"\
                    " close(unit=6)\n"\
                    "Else\n"\
                    " open(unit=6, file=\"/dev/stdout\")\n"\
                    "Endif\n"
                g.write(content)
            elif line.startswith("End Subroutine CalculateBR") :
                content = "End Subroutine CalculateBR_2\n"
                g.write(content)
            else :
                g.write(line)


    os.remove(filename)
    os.rename(temp_filename, filename)

def patch_addloopfunctions(model_name, patch_dir):
    """
    Patches $SPheno/<MODEL>/AddLoopFunctions.f90
    """

    filename = patch_dir + "/" + model_name + "/AddLoopFunctions.f90"
    temp_filename = filename + "_temp"

    if not os.path.exists(filename):
        raise GumError(("Tried to find the file located at " + filename +
                        " but it does not seem to exist!"))
            
    with open(filename, 'r') as f, open(temp_filename, 'w') as g :
        for line in f :
            if line.startswith("  SA_DerB00 = 3._dp * (xm1 * xm1 - 2._dp * xm1 * xm2") :
                content = "  If ((xm1.Eq.0._dp).And.(xm2.Eq.0._dp)) Then\n"\
                                    "    SA_DerB00 = -2._dp * xp * xp - 3._dp * xp * xp * VB0 - 3._dp * xp * xp * xp * VDerB0\n"\
                                    "  Else\n"
                g.write(content)
            elif line.startswith("  SA_DerB00 = SA_DerB00 / (36._dp * xp * xp)") :
                g.write("  Endif\n")
            g.write(line)

    os.remove(filename)
    os.rename(temp_filename, filename)

# SUSY-only patches

def patch_model_data(model_name, patch_dir):
    """
    Patches $SPheno/<MODEL>/Model_Data_<MODEL>.f90
    """
    
    filename = patch_dir + "/" + model_name + "/Model_Data_" + model_name + ".f90"
    temp_filename = filename + "_temp"

    if not os.path.exists(filename):
        raise GumError(("Tried to find the file located at " + filename +
                        " but it does not seem to exist!"))
            
    with open(filename, 'r') as f, open(temp_filename, 'w') as g :
        for line in f :
            if line.startswith("Logical, Save :: CalcLoopDecay_LoopInducedOnly=.False."):
                g.write(line)
                g.write("Logical, Save :: CalcSUSY3BodyDecays=.False. ! Added by GAMBIT\n")
            else :
                g.write(line)
           

    os.remove(filename)
    os.rename(temp_filename, filename)


def patch_3_body_decays_susy(model_name, patch_dir, bsm_particles):
    """
    Patches the 3-body decays in: 
    $SPheno/<MODEL>/3-Body-Decays/X_<MODEL>.f90
    where X is a superfield.
    """

    partnames = []
    for part in bsm_particles:
        name = re.sub("\d","",part.alt_name)
        if name not in partnames:
           partnames.append(name)

    # Check if any of these susy particles are in the particle list
    susy_particles = ["Cha", "Chi", "Glu", "Sd", "Su", "Se", "Sv"]
    susy_channels = {"Cha": ["ChacChaCha", "ChaChiChi"],
                     "Chi": ["ChicChaCha", "ChiChiChi"],
                     "Glu": [],
                     "Sd": ["ChaGluSu", "SdChacCha", "SdChiChi", "ChiGluSd", "GluGluSd"],
                     "Su": ["SuChiChi", "ChiGluSu", "SdChicCha", "GluGluSu", "GluSdcCha", "SuChacCha"],
                     "Se": ["SvChaChi", "SeChacCha", "SeChiChi"],
                     "Sv": ["SvChiChi", "SeChicCha", "SvChacCha"]}
    particles = []
    channels = {}
    for susy_particle in susy_particles:
        if susy_particle in partnames:
            particles.append(susy_particle)
            channels[susy_particle] = susy_channels[susy_particle]

    for particle in particles :
 
        filename = patch_dir + "/" + model_name + "/3-Body-Decays/" + particle + "_" + model_name + ".f90"
        temp_filename = filename + "_temp"

        if not os.path.exists(filename):
            raise GumError(("Tried to find the file located at " + filename +
                            " but it does not seem to exist!"))
            
        with open(filename, 'r') as f, open(temp_filename, 'w') as g :
            for line in f :
                if line.startswith("Use ThreeBodyPhaseSpace") :
                    g.write(line)
                    g.write("Use Model_Data_" + model_name + " ! Added by GAMBIT\n")
                elif any([line.startswith("Call " + particle + "To" + channel) for channel in channels[particle]]) :
                    g.write("If (CalcSUSY3BodyDecays) Then ! Added by GAMBIT\n")
                    g.write(line)
                elif any([line.startswith("g" + particle + channel + "(i_run,:,:,:) = g" + particle + channel + "i") for channel in channels[particle]]) :
                    g.write("End If ! Added by GAMBIT\n\n")
                    g.write(line)
                else :
                    g.write(line)

        os.remove(filename)
        os.rename(temp_filename, filename)


"""
FRONTEND ROUTINES
"""

class SPhenoParameter:
    """
    Container type for a SPheno parameter.
    """
    
    def __init__(self, _name, _type, _size, _block="", _index=0, _alt_name="",
                 _bcs=""):

        self.name = _name
        self.type = _type
        self.size = _size
        self.block = _block
        self.index = _index
        self.alt_name = _alt_name
        self.bcs = _bcs


def write_spheno_frontends(model_name, parameters, particles, flags, 
                           spheno_path, output_dir, blockparams, gambit_pdgs, 
                           mixings, reality_dict, sphenodeps, bcs, 
                           charged_higgses, neutral_higgses):
    """
    Writes the frontend source and header files for SPheno.
    """

    # Firstly, scrape the function signatures from the spheno source
    functions, arguments, locations = scrape_functions_from_spheno(spheno_path,
                                                                   model_name)

    # Convert the arguments to GAMBIT types
    type_dictionary = get_fortran_shapes(arguments)

    # Add the new types to backend_types
    backend_types, linenum = add_to_spheno_backend_types(type_dictionary)

    # Get all of the variables used in SPheno so we can store them as 
    # BE_VARIABLES. Keep track of those used for HiggsBounds too.
    variables, hb_variables = harvest_spheno_model_variables(spheno_path, 
                                                             model_name, 
                                                             parameters)

    # Convert these to GAMBIT types too
    variable_dictionary = get_fortran_shapes(variables)
    hb_variable_dictionary = get_fortran_shapes(hb_variables)

    # Get the indices for the mass_uncertainties
    mass_uncertainty_dict = get_mass_uncert(spheno_path, model_name)

    # Get the source and header files
    spheno_src = write_spheno_frontend_src(model_name, functions,
                                           variables, flags, particles, 
                                           parameters, blockparams, 
                                           gambit_pdgs, mixings, reality_dict,
                                           sphenodeps, hb_variables, bcs,
                                           charged_higgses, neutral_higgses,
                                           mass_uncertainty_dict)

    spheno_header = write_spheno_frontend_header(model_name, functions, 
                                                 type_dictionary, 
                                                 locations, variables,
                                                 variable_dictionary,
                                                 hb_variables,
                                                 hb_variable_dictionary, flags)


    return spheno_src, spheno_header, backend_types, linenum

def write_spheno_function(name, function_signatures, no_star = []) :
    """
    Write the function for spheno
    """
    args = function_signatures[name]
    function = name + "(" +  ''.join([("*" if arg not in no_star else "") + arg + "," for arg in args[:-1]])
    function += ("*" if args[-1] not in no_star else "") + args[-1] + ");"

    return function

def get_fortran_shapes(parameters):
    """
    Returns a dictionary of GAMBIT fortran-shape for a SPheno parameter

    TODO some types have size e.g. (3,0) -- figure out what to do with these
    """

    type_dictionary = {}

    for name, parameter in parameters.iteritems():

        if not isinstance(parameter, SPhenoParameter):
            raise GumError(("Parameter not passed as instance of "
                            "SPhenoParameter to function get_fortran_shapes."))

        fortran_type = ""
        
        # If it is not a scalar, then it's an Farray
        if parameter.size:
            fortran_type += "Farray_"

        # If it's complex
        if parameter.type == "Complex(dp)":
            fortran_type += "Fcomplex16"

        elif parameter.type == "Real(dp)":
            fortran_type += "Freal8"

        elif parameter.type == "Integer":
            fortran_type += "Finteger"

        elif parameter.type == "Logical":
            fortran_type += "Flogical"

        # Now define the size of the array, if there is one
        if parameter.size:
            fortran_type += "_1_"
            fortran_type += parameter.size.replace(',','_1_')

        # If we haven't been able to convert it, throw an error.
        if not fortran_type:
            raise GumError(("GUM has not been able to convert the type of the "
                            "parameter " + name + ". Please check "
                            "your SPheno file, and if necessary, add the type "
                            "to get_fortran_shape."))

        type_dictionary[name] = fortran_type  

    return type_dictionary

def add_to_spheno_backend_types(type_dictionary):
    """
    Checks all new types are in the SPheno backend_types.
    """

    # New types from the harvesting
    types = []
    for t in type_dictionary.values():
        if t.startswith('Farray_'): types.append(t)

    # Remove dupes
    types = list(set(types))

    btypes = "../Backends/include/gambit/Backends/backend_types/SPheno.hpp"

    # Already registered types that GAMBIT knows about 
    registered_types = []

    num = 0 # Index of last entry
    with open(btypes, "r") as f:
        for i, line in enumerate(f, start=1):
            if "typedef" in line:
                # Save the type
                r = re.search(r"Farray_(.*?);", line)
                if r:
                    registered_types.append('Farray_'+r.group(1))
            elif "particle2" in line:
                # Done
                num = i
                break
            
    toadd = []
    for t in types:
        if t not in registered_types:
            t2 = t.replace('_','<',1) # Replace the first _ with a <
            t2 = t2[::-1].replace('_',',') # Replace all other _ with ,
            t2 = t2[::-1]
            string = "typedef {0}> {1};".format(t2, t)
            toadd.append(string)

    towrite = "\n".join(toadd) + "\n"

    return dumb_indent(4, towrite), num

# harvesting

def scrape_functions_from_spheno(spheno_path, model_name):
    """
    Reads the SPheno source to identify the function signatures
    we want to include in the frontend.
    """
   
    clean_model_name = model_name.replace('-','')

    # Create a dictionary of function names, and the string of 
    # parameters that go into them.
    function_dictionary = {}
    args_dictionary = {}

    # Dictionary of which files each function lives in.
    locations_dictionary = {
        clean_model_name+"/BranchingRatios_"+clean_model_name  : "CalculateBR_2",
        clean_model_name+"/InputOutput_"+clean_model_name      : ["Switch_to_superCKM",
                                                                  "Switch_to_superPMNS"],
    }

    for location, functions in locations_dictionary.iteritems():
        filename = spheno_path+"/"+location+".f90"
        get_arguments_from_file(functions, filename, function_dictionary, args_dictionary)

    return function_dictionary, args_dictionary, locations_dictionary


def harvest_spheno_model_variables(spheno_path, model_name, model_parameters):
    """
    Harvests the model variables from $SPHENO/<MODEL>/Model_Data_<MODEL>.f90.
    Returns a dictionary of key: parameter name, value: GAMBIT fortran type, 
    and the same for the HiggsBounds output.
    """

    clean_model_name = model_name.replace('-','')
    location = "{0}/{1}/Model_Data_{1}.f90".format(spheno_path, 
                                                   clean_model_name)

        
    parameters = {}
    hb_parameters = {}



    src = ""
    # First entry we care about is the line with "mass_uncertainty_Q" on it.
    # Last one will have the line "Contains". Read everything else in, then.
    with open(location, 'r') as f:
        started = False
        for line in f:
            if "mass_uncertainty" in line:
                src += line
                started = True
            # If we are done.
            elif "Contains" in line:
                started = False
                break
            # If its a Logical flag defined before that mass_uncertanity, add it
            elif line.startswith("Logical"):
                src += line
            # Additional control variables we need and are before mass_uncertainty
            elif "unitarity_s" in line or "TUcutLevel" in line :
                src += line
            elif started:
                src += line

    # Source output -- clean it up a bit to make it easier to parse
    src = src.replace(' ','').replace('&\n',' ').replace('&','').split('\n')
    #hb_src = hb_src.replace(' ','').replace('&\n',' ').replace('&','').split('\n')

    # The list of possible types a parameter could be
    possible_types = ["Real(dp)", "Integer", "Complex(dp)", "Logical", 
                      "Character"]

    # A list of strings to match if we want to section it off to HB.
    # Just the starts of strings, there will be various suffixes.
    hb_strings = ["ratioP", "ratioGG", "CPL_H_H", "CPL_A_A", "CPL_A_H", "rHB", 
                  "BR_H", "BR_t"]

    # Each line looks like - TYPE :: definition(s)
    for line in src:
        # Each "split" is either an empty list, or a list with type first,
        # and parameter definition second.
        split = filter(None, line.split('::'))
        if not split: continue # Empty list -- skip it
        if "HiggsBounds" in line: continue # Just a comment
        # Get the type.
        _type = ""
        for pt in possible_types:
            if pt in split[0]:
                _type = pt
        if _type == "":
            raise GumError("No type scraped from Model_Data.")

        # Split the RHS up but *not* if there is a comma between parentheses.
        names = re.split(r',\s*(?![^()]*\))', split[1])
        for name in names:
            # If it's defaulted, don't wanna save that.
            if "=" in name:
                name = name.split('=')[0]
            # Any size information? Like (3,3) or (2)...
            pat = r'\((.*?)\)'
            r = re.search(pat, name)
            if r:
                size = r.group(1)
                name = name.split('(')[0] # Save the name without the (..)
            else: 
                size = ""

            # If the variable is part of the model parameters, add the block
            block = "" 
            index = 0
            alt_name = ""
            bcs = ""
            for model_par in model_parameters:
              if name == model_par.name or name == model_par.alt_name :
                block = model_par.block
                index = model_par.index
                alt_name = model_par.alt_name
                bcs = model_par.bcs
            par = SPhenoParameter(name, _type, size, block, index, alt_name, bcs)

            # Finally check to see if the name matches anything we want to
            # section off into the HiggsBounds parameters
            hb = False
            for i in hb_strings:
                if name.startswith(i):
                    hb_parameters[name] = par
                    hb = True
            # If it's not a HB parameter, leave with the rest
            if not hb: parameters[name] = par
    
    return parameters, hb_parameters

def get_mass_uncert(spheno_path, model_name):
    """
    Scrape the mass_uncertainty PDG code : index and return a dict.
    """

    mud = {}

    location = "{0}/{1}/InputOutput_{1}.f90".format(spheno_path, 
                                                    model_name)

    src = []
    # Scrape every line with a Sqrt(mass_uncertainty_Q in it
    with open(location, 'r') as f:
        for line in f:
            if "Sqrt(mass_uncertainty_Q" in line:
                src.append( line )

    for entry in src:
        pdg = re.search(r'INT\(Abs\((.*?)\)\)', entry).group(1)
        index = re.search(r'Sqrt\(mass_uncertainty_Q\((.*?)\)', entry).group(1)
        mud[pdg] = index

    return mud

def get_arguments_from_file(functions, file_path, function_dictionary,
                            argument_dictionary):
    """
    Helper function to obtain the function signatues from a given file.
    Also pull the types of each argument while we are there.
    """

    # The list of possible types a parameter could be
    possible_types = ["Real(dp)", "Integer", "Complex(dp)", "Logical", 
                      "Character"]

    with open(file_path) as f:
        data = f.readlines()
    # Remove line breaks -- makes regex easier to use
    file = "".join(data).replace("\n","")

    # If you pass a string instead of a list, wrap it into one
    if not isinstance(functions, list):
        funcs = []
        funcs.append(functions)
        functions = funcs

    for function in functions:

        # Extract the signature for a given function
        pattern = r'Subroutine {0}\((.*?)\)'.format(function)
        s = re.search(pattern, file)
        if s:
            signature = s.group(1).replace('&','').replace(' ','').split(',')
            function_dictionary[function] = signature
        else:
            raise GumError("Error extracting " + function +" from SPheno.")

        # Get some function definitions while we are here too.

        # This is the entire function definition we're grabbing
        pattern = 'Subroutine {0}(.*?)End Subroutine {0}'.format(function)
        s = re.search(pattern, file)
        if s:
            func = s.group(1)
            # Split the definition up by the :: sign for definitions.
            defs = func.split('::')

            # Firstly, find out which arguments we need to obtain types for
            for v in function_dictionary.values():
                for arg in v:
                    # If we've got it already
                    if arg in argument_dictionary: 
                        continue
                    else:
                        # Go through each split; if we find a possible Fortran type
                        # in the string, scrape the parameter name and add to the 
                        # dictionary.
                        for i in range(len(defs)):
                            # ... unless we've already got it.
                            if arg in argument_dictionary: 
                                continue
                            else:
                                for j in possible_types:
                                    if j in defs[i]:
                                        if arg in defs[i+1]:
                                            # Also check the size here by looking
                                            # at the size of the matrix
                                            pat = r'{}\((.*?)\)'.format(arg)
                                            r = re.search(pat, defs[i+1])
                                            if r:
                                                size = r.group(1)
                                            else: 
                                                size = ""
                                            # Size can also be given by "Dimension"
                                            # in FORTRAN
                                            if "Dimension" in defs[i]:
                                                size = re.search(r'Dimension\((.*?)\)', defs[i]).group(1)
                                            par = SPhenoParameter(arg, j, size, "None")
                                            argument_dictionary[arg] = par
                                            break

        else:
            raise GumError(("Couldn't match the pattern between the definition"
                            " and undefinition of the Subroutine " + function +
                            ", please inspect the SARAH-SPheno source code, "
                            "as I don't think it will work anyway."))


def make_spheno_decay_tables(spheno_path, model_name):
    """
    Generates the DecayTables for a new SPheno version within GAMBIT.
    All information harvested from InputOutput_<Model>.f90.
    """

    clean_model_name = model_name.replace('-','')
    target = "{0}/{1}/InputOutput_{1}.f90".format(spheno_path, clean_model_name)

    towrite = (
            "# This file contains a list of all the {0} decays calculated by "
            "sarah-spheno_{0},\n"
            "# along with the spheno array index (INDEX) used internally in "
            "sarah-spheno_{0}.\n"
            "# For each decay there is also a correction factor (CORRF) which "
            "tells us\n"
            "# whether the BR on that line should be corrected to account "
            "for charge conjugate\n"
            "# final states (CORRF=0.5) or not (CORRF=1.0)\n\n"
            "# Autogenerated by GUM.\n"
    ).format(clean_model_name)

    pdgs = {}
    names = {}

    with open(target, 'r') as f:
        decays = False
        indices = []

        for line in f:

            # Get the PDGs
            if line.startswith("PDG") :
                particle = line.split('=')[0][3:]
                pdg = line.split('=')[1][0:-1]
                pdgs[particle] = pdg
                if pdg[0]=='-' :
                    pdgs["-"+particle] = pdg[1:]
                else :
                    pdgs["-"+particle] = "-"+pdg 
                pdgs["--"+particle] = pdg


            # Get names of particles
            if line.startswith("NameParticle") :
                particle = line.split('=')[0][12:] 
                name = line.split('=')[1][1:-2]
                names[particle] =  name
                names["-"+particle] = name+"^*"
                names["--"+particle] = name

            # Now find the decays entries
            if line.startswith("If ((gT") :

                # Start reading decays
                decays = True
                count = 0
     
                # get particle
                subline = line.split('.')[0]
                part = subline[7::]

                # print block entry
                towrite += (
                    "DECAY        {0}     0.0000000E+00   # {1}\n"
                    "#    INDEX  NDA      ID1      ID2     CORRF\n"
                ).format(str(abs(int(pdgs[part]))), names[part])

            # Get indices
            if "Do gt" in line and decays:

                indices.append( line.split('=')[1][0:-1].split(',') )

            # End of index loop
            if "End Do" in line and decays:
                indices = indices[:-1]

            # Get daugthers
            if line.startswith("CurrentPDG") and decays :

                ndaughters = int(line[10])
                daughters = []
     
                for i in range(ndaughters):
                    daughter = line.split("=")[1][:-2]
                    if daughter[1] == '-' :
                        daughters.append('-'+daughter[5:])
                    else :
                        daughters.append(daughter[4:])
                    line = next(f)

                # Check for repeated print lines to add correction factors
                corrf = "1.00"
                if line.startswith("Write") :
                    line = next(f)
                    line = next(f)
                    if line.startswith("Write") :
                        corrf = "0.50"

                # List of processes
                processes = [daughters]

                # Loop over indices
                if len(indices) > 0 :
                    processes = processes[:-1]
                    di = 0 if "gt" in daughters[0] else 1 if "gt" in daughters[1] else 2
                    for i in range(int(indices[0][0]),int(indices[0][1])+1) :
                        process =  [daught.replace("gt" + str(di+1),str(i)) for daught in daughters] 
                        processes.append(process)
                        if len(indices) > 1 :
                            processes = processes[:-1]
                            dj = (1 if "gt" in daughters[1] else 2) if di == 0 else 2
                            try:
                                j1 = int(indices[1][0])
                            except ValueError:
                                j1 = i
                            for j in range(j1,int(indices[1][1])+1) :
                                new_process = [proc.replace("gt" + str(dj+1),str(j)) for proc in process]
                                processes.append(new_process)
                                if len(indices) > 2 :
                                    processes = processes[:-1]
                                    try:
                                        k1 = int(indices[2][0])
                                    except ValueError:
                                        k1 = j
                                    for k in range(k1,int(indices[2][1])+1) :
                                        new_new_process = [proc.replace("gt3",str(k)) for proc in new_process]
                                        processes.append(new_new_process)
     
                # Print processes
                for process in processes:
                    count += 1
                    proc_str = str(count).rjust(8) + str(ndaughters).rjust(5) + "  "
                    for daughter in process :
                        proc_str += pdgs[daughter].rjust(11)
                    proc_str +=  corrf.rjust(7) + "  # BR(" + names[part] + "-> "
                    for daughter in process :
                        proc_str += names[daughter] + " "
                    proc_str += ")\n"
                    towrite += proc_str
                    if corrf == "0.50" :
                        proc_str = str(count).rjust(8) + str(ndaughters).rjust(5) + "  "
                        for daughter in process :
                            proc_str += pdgs["-"+daughter].rjust(11)
                        proc_str +=  corrf.rjust(7) + "  # BR(" + names[part] + "-> "
                        for daughter in process :
                            proc_str += names["-"+daughter] + " "
                        proc_str += ")\n"
                        towrite += proc_str
                    

                # Key to stop reading decays
                if "! Information needed by MadGraph" in line :
                    decays = False

    print(("Decay table for {0} done.").format(model_name))

    return towrite 

# /harvesting
# writing

def write_spheno_frontend_src(model_name, function_signatures, variables, flags, 
                              particles, parameters, blockparams, gambit_pdgs, mixings,
                              reality_dict, sphenodeps, hb_variables, bcs,
                              charged_higgses, neutral_higgses,
                              mass_uncertainty_dict):

    """
    Writes source for 
    Backends/src/frontends/SARAHSPheno_<MODEL>_<VERSION>.cpp
    """ 
    intro_message = "Frontend for SARAH-SPheno "+SPHENO_VERSION+" backend, for the "\
                    " "+model_name+" model."

    safe_version = SPHENO_VERSION.replace('.','_')

    towrite = blame_gum(intro_message)

    # Headers, macros and callback function
    towrite += (
            "#include \"gambit/Backends/frontend_macros.hpp\"\n"
            "#include \"gambit/Backends/frontends/SARAHSPheno_{0}_{1}.hpp\"\n"
            "#include \"gambit/Elements/spectrum_factories.hpp\"\n"
            "#include \"gambit/Models/SimpleSpectra/{0}SimpleSpec.hpp\"\n"
            "#include \"gambit/Utils/version.hpp\"\n"
            "\n"
            "#define BACKEND_DEBUG 0\n"
            "\n"
            "// Callback function for error handling\n"
            "BE_NAMESPACE\n"
            "{{\n"
            "// This function will be called from SPheno. Needs C linkage, and thus also\n"
            "// a backend-specific name to guard against name clashes.\n"
            "extern \"C\"\n"
            "void CAT_4(BACKENDNAME,_,SAFE_VERSION,_ErrorHandler)()\n"
            "{{\n"
            "throw std::runtime_error(\"SARAHSPheno_{0} backend called TerminateProgram.\");\n"
            "}}\n"
            "}}\n"
            "END_BE_NAMESPACE\n"
            "\n"
    ).format(model_name, safe_version)

    # Convenience functions
    towrite += (
            "// Convenience functions (definition)\n"
            "BE_NAMESPACE\n"
            "{{\n"
            "\n"
            "// Variables and functions to keep and access decay info\n"
            "typedef std::tuple<std::vector<int>,int,double> channel_info_triplet; // {{pdgs of daughter particles}}, spheno index, correction factor\n"
            "namespace Fdecays\n"
            "{{\n"
            "// A (pdg,vector) map, where the vector contains a channel_info_triplet for each\n"
            "// decay mode of the mother particle. (See typedef of channel_info_triplet above.)\n"
            "static std::map<int,std::vector<channel_info_triplet> > all_channel_info;\n"
            "\n"
            "// Flag indicating whether the decays need to be computed or not.\n"
            "static bool BRs_already_calculated = false;\n"
            "\n"
            "// Function that reads a table of all the possible decays in SARAHSPheno_{0}\n"
            "// and fills the all_channel_info map above\n"
            "void fill_all_channel_info(str);\n"
            "\n"
            "// Helper function to turn a vector<int> into a vector<pairs<int,int> > needed for\n"
            "// when calling the GAMBIT DecayTable::set_BF function\n"
            "std::vector<std::pair<int,int> > get_pdg_context_pairs(std::vector<int>);\n"
            "}}\n"
            "\n"
    ).format(model_name)

    # run_SPheno function
    towrite += (
            "// Convenience function to run SPheno and obtain the spectrum\n"
            "int run_SPheno(Spectrum &spectrum, const Finputs &inputs)\n"
            "{\n"
            "\n"
            "*epsI = 1.0E-5;\n"
            "*deltaM = 1.0E-6;\n"
            "*mGUT = -1.0;\n"
            "*ratioWoM = 0.0;\n"
            "\n"
            "Set_All_Parameters_0();\n"
            "\n"
            "*kont = 0;\n"
            "*delta_mass = 1.0E-4;\n"
            "*CalcTBD = false;\n"
            "\n"
            "ReadingData(inputs);\n"
            "\n"
            "try{ SPheno_Main(); }\n"
            "catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n"
            "\n"
            "if(*kont != 0)\n"
            "  ErrorHandling(*kont);\n"
            "if(*FoundIterativeSolution or *WriteOutputForNonConvergence)\n"
            "{\n"
            "\n"
            "spectrum = Spectrum_Out(inputs);\n"
            "\n"
            "}\n"
            "\n"
            "if(*kont != 0)\n"
            "  ErrorHandling(*kont);\n"
            "\n"
            "return *kont\n"
            "}\n"
    )
    # End of run_SPheno function

    # fill_spectrum_calculate_BRs function
    towrite += (
            "\n"
            "// Helper function to pass the spectrum object to the SPheno frontend and compute the BRs.\n"
            "void fill_spectrum_calculate_BRs(const Spectrum &spectrum, const Finputs& inputs)\n"
            "{\n"
            "if (Fdecays::BRs_already_calculated) return;\n"
            "\n"
            "// Initialize some variables\n"
            "*Iname = 1;\n"
            "*CalcTBD = false;\n"
            "*ratioWoM = 0.0;\n"
            "*epsI = 1.0E-5;\n"
            "*deltaM = 1.0e-6;\n"
            "*kont =  0;\n"
            "\n"
            "// Read options and decay info\n"
            "ReadingData_decays(inputs);\n"
            "\n"
            "// Fill input parameters with spectrum imformation\n"
            "\n"
            "// Masses\n"
            "SMInputs sminputs = spectrum.get_SMInputs();\n"
            "(*MFd)(1) = sminputs.mD;\n"
            "(*MFd)(2) = sminputs.mS;\n"
            "(*MFd)(3) = sminputs.mBmB;\n"
            "(*MFe)(1) = sminputs.mE;\n"
            "(*MFe)(2) = sminputs.mMu;\n"
            "(*MFe)(3) = sminputs.mTau;\n"
            "(*MFu)(1) = sminputs.mU;\n"
            "(*MFu)(2) = sminputs.mCmC;\n"
            "(*MFu)(3) = sminputs.mT;\n"
            "for (int i=1; i<=3; i++)\n"
            "{\n"
            "(*MFd2)(i) = pow((*MFd)(i),2);\n"
            "(*MFe2)(i) = pow((*MFe)(i),2);\n"
            "(*MFu2)(i) = pow((*MFu)(i),2);\n"
            "}\n"
    )

    # A dictionary in which we save the SPheno particle name alongside the 
    # gambit one. We'll use this for the mixings in a second.
    mixingdict = {}

    # Add each particle to the spectrum
    for particle in particles:
        mass = re.sub(r"\d","",particle.alt_mass_name)
        index = re.sub(r"[A-Za-z]","",particle.alt_mass_name)
        specmass = pdg_to_particle(particle.PDG_code, gambit_pdgs)
        brace = "(" + str(index) + ")" if index else ""
        
        eig = re.sub(r"\d","",particle.alt_name)
        if not eig in mixingdict:
            mixingdict[eig] = specmass.split('_')[0]

        towrite += (
                "(*{0}){1} = spectrum.get(Par::Pole_Mass, \"{2}\");\n"
                "(*{0}2){1} = pow((*{0}){1}, 2);\n"
        ).format(mass, brace, specmass)

    towrite += (
            "*MVWm = spectrum.get(Par::Pole_Mass, \"W-\");\n"
            "*MVWm2 = pow(*MVWm,2);\n"
            "*MVZ = spectrum.get(Par::Pole_Mass, \"Z0\");\n"
            "*MVZ2 = pow(*MVZ,2);\n"
            "\n\n"
    )

    # Mixings.
    towrite += "// Mixings\n"
    for par in parameters:
        # If there's no block, er... skip this.
        if not par.block: continue
        if not par.block in blockparams: continue
        # Is it a mixing matrix?
        if 'mixingmatrix' in blockparams[par.block]:
            size = blockparams[par.block]['mixingmatrix']
            i,j = size.split('x')

            # This is the eigenstate 
            eigenstate = mixings[par.name]
            # TODO: currently doesn't work for SM particle mixing matrices: ZUL, ZEL, etc.
            # These correspond to the U(3) rotations performed to diagonalise the Yukawa 
            # terms of the Standard Model so don't correspond to a "pole_mixing" entry in the
            # spectrum object.

            # Decision: if Ye, Yu, Yd are defined in the SARAH file, then we don't need to 
            # save the output of these matrices since they don't do anything.
            entry = ""
            if eigenstate in mixingdict:
                entry = mixingdict[eigenstate]

            if par.name in ["ZUL", "ZUR", "ZDL", "ZDR", "ZEL", "ZER"]:
                continue
            else:
                towrite += (
                        "for(int i=1; i<={0}; i++)\n"
                        "{{\n"
                        "for(int j=1; j<={1}; j++)\n"
                        "{{\n"
                        "(*{2})(i, j) = spectrum.get(Par::Pole_Mixing, \"{3}\", i, j);\n"
                        "}}\n"
                        "}}\n"
                ).format(i, j, par.name, entry)

      
    # Fill model dependent other parameters
    towrite += "// Other parameters\n"
    for par in parameters:

        # Don't want output (mixing matrices, basically) or masses. Already done those.
        if not par.is_output and par.block != "MASS":

            entry = par.alt_name if par.alt_name else par.name
            
            # Matrix case
            if par.shape.startswith('m'):
                size = par.shape[1:]
                i,j = size.split('x')
                towrite += (
                        "for(int i=1; i<={0}; i++)\n"
                        "{{\n"
                        "for(int j=1; j<={1}; j++)\n"
                        "{{\n"
                        "(*{2})(i, j) = spectrum.get(Par::{3}, \"{2}\", i, j);\n"
                        "}}\n"
                        "}}\n"
                ).format(i, j, entry, par.tag, par.name)
            
            else:
                towrite += (
                    "*{0} = spectrum.get(Par::{1}, \"{2}\");\n"
                ).format(entry, par.tag, par.name)

    # Go through the SPhenodeps and add these
    for par, definition in sphenodeps.iteritems():
        towrite += "*{0} = {1};\n".format(par, definition)

    towrite += (
            "\n"
            "// Call SPheno's function to calculate decays\n"
            "try{{ {0} }}\n"
            "catch(std::runtime_error e) {{ invalid_point().raise(e.what()); }}\n"
            "\n"
            "// Check for errors\n"
            "if(*kont != 0)\n"
            "  ErrorHandling(*kont);\n"
            "\n"
            "Fdecays::BRs_already_calculated = true;\n"
            "\n"
            "}}\n\n"
    ).format( write_spheno_function("CalculateBR_2", function_signatures) )
    # End of fill_spectrum_calculate_BRs function

    # run_SPheno_decays function
    towrite += (
            "// Convenience function to run Spheno and obtain the decays\n"
            "int run_SPheno_decays(const Spectrum &spectrum, DecayTable& decays, const Finputs& inputs)\n"
            "{\n"
            "\n"
            "double BRMin = inputs.options->getValueOrDef<double>(1e-5, \"BRMin\");\n"
            "\n"
            "// Pass the GAMBIT spectrum to SPheno and fill the internal decay objects\n"
            "fill_spectrum_calculate_BRs(spectrum, inputs);\n"
            "\n"
            "if(*kont != 0)\n"
            "  ErrorHandling(*kont);\n"
            "\n"
            "// Fill in info about the entry for all decays\n"
            "DecayTable::Entry entry;\n"
            "entry.calculator = STRINGIFY(BACKENDNAME);\n"
            "entry.calculator_version = STRINGIFY(VERSION);\n"
            "entry.positive_error = 0.0;\n"
            "entry.negative_error = 0.0;\n"
            "\n"
            "// Helper variables\n"
            "std::vector<int> daughter_pdgs;\n"
            "int spheno_index;\n"
            "double corrf;\n"
            "\n"
    )

    towrite += "std::vector<int> pdg = {\n"
    nparticles = len(particles);
    for i, particle in enumerate(particles):
        towrite += str(particle.PDG_code)
        if i < nparticles-1: towrite += ", // " + particle.name + '\n'
        else : towrite += " // " + particle.name + '\n'

    towrite += "};\n"\
               "int n_particles = pdg.size();\n"

    towrite += "auto gT = [&](int i)\n"\
               "{\n"
    for i, particle in enumerate(particles) :
        name = re.sub(r"\d","",particle.alt_name)
        index = re.sub(r"[A-Za-z]","",particle.alt_name)
        brace = "(i-" + str(i-int(index)+1) + ")" if index else ""
        if i == 0:
            towrite += "if(i==1) return (*gT" + name + ")" + brace + ";\n"
        else :
            towrite += "else if(i==" + str(i+1) + ") return (*gT" + name + ")" + brace + ";\n"
    towrite += "return 0.0;\n"\
               "};\n"\
               "\n"\
               "auto BR = [&](int i, int j)\n"\
               "{\n"
    for i, particle in enumerate(particles) :
        name = re.sub(r"\d","",particle.alt_name)
        index = re.sub(r"[A-Za-z]","",particle.alt_name)
        brace = "(i-" + str(i-( int(index) if index != "" else 0 )+1) + " ,j)"
        if i == 0:
            towrite += "if(i==1) return (*gT" + name + ")" + brace + ";\n"
        else :
            towrite += "else if(i==" + str(i+1) + ") return (*gT" + name + ")" + brace + ";\n"
    towrite += "return 0.0;\n"\
               "};\n\n"

    towrite += (
            "for(int i=0; i<n_particles; i++)\n"
            "{\n"
            "std::vector<channel_info_triplet> civ = Fdecays::all_channel_info.at(pdg[i]);\n"
            "entry.width_in_GeV = gT(i+1);\n"
            "entry.channels.clear();\n"
            "for(channel_info_triplet ci : civ)\n"
            "{\n"
            "std::tie(daughter_pdgs, spheno_index, corrf) = ci;\n"
            "if(BR(i+1,spheno_index) * corrf > BRMin)\n"
            "  entry.set_BF(BR(i+1,spheno_index) * corrf, 0.0, Fdecays::get_pdg_context_pairs(daughter_pdgs));\n"
            "// If below the minimum BR, add the decay to the DecayTable as a zero entry.\n"
            "else\n"
            "entry.set_BF(0., 0., Fdecays::get_pdg_context_pairs(daughter_pdgs));\n"
            "}\n"
            "// SM fermions in flavour basis, everything else in mass basis\n"
            "if(abs(pdg[i]) < 17)\n"
            "  decays(Models::ParticleDB().long_name(pdg[i],1)) = entry;\n"
            "else\n"
            "  decays(Models::ParticleDB().long_name(pdg[i],0)) = entry;\n"
            "}\n"
            "\n"
            "return *kont;\n"
            "}\n\n"
    )
    # End of run_SPheno_decays

    # Spectrum_Out function
    towrite += (
            "// Convenience function to convert internal SPheno variables into a Spectrum object\n"
            "Spectrum Spectrum_Out(const Finputs &inputs)\n"
            "{\n"
            "\n"
            "SLHAstruct slha;\n"
            "\n"
            "Freal8 Q;\n"
            "try{ Q = sqrt(GetRenormalizationScale()); }\n"
            "catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n"
            "\n"
    )
    if flags["SupersymmetricModel"] and any([particle.alt_name.startswith("Chi") for particle in particles]):
        size = ""
        for par in parameters:
            if par.name == "ZN":
                size = blockparams[par.block]['mixingmatrix']
        i,j = size.split('x')

        towrite += (
                "// Make sure to rotate back the sign on MChi\n"
                "if(not *RotateNegativeFermionMasses)\n"
                "{\n"
                "for(int i=1; i<={" + str(i) + "}; i++)\n"
                "{\n"
                "double remax = 0, immax = 0;\n"
                "for(int j=1; j<=" + str(j) +"; j++)\n"
                "{\n"
                "if(abs((*ZN)(i,j).re) > remax) remax = abs((*ZN)(i,j).re);\n"
                "if(abs((*ZN)(i,j).im) > immax) immax = abs((*ZN)(i,j).im);\n"
                "}\n"
                "if(immax > remax)\n"
                "{\n"
                "(*MChi)(i) *= -1;\n"
                "for(int j=1; j<=" + str(j) + "; j++)\n"
                "{\n"
                "double old = (*ZN)(i,j).re;\n"
                "(*ZN)(i,j).re = (*ZN)(i,j).im;\n"
                "(*ZN)(i,j).im = -old;\n"
                "}\n"
                "}\n"
                "}\n"
                "}\n"
                "\n"
        )

    towrite += "// Spectrum generator information\n"\
      'SLHAea_add_block(slha, "SPINFO");\n'\
      'SLHAea_add(slha, "SPINFO", 1, "GAMBIT, using "+str(STRINGIFY(BACKENDNAME))+" from SARAH");\n'\
      'SLHAea_add(slha, "SPINFO", 2, gambit_version()+" (GAMBIT); "+str(STRINGIFY(VERSION))+" ("+str(STRINGIFY(BACKENDNAME))+"); "+str(STRINGIFY(SARAH_VERSION))+" (SARAH)");\n'\
      "\n"\
      "// Block MODSEL\n"\
      'SLHAea_add_block(slha, "MODSEL");\n'
    if not flags["OnlyLowEnergySPheno"] :
      towrite += 'if(*HighScaleModel == "LOW")\n'\
        '  slha["MODSEL"][""] << 1 << 0 << "# ' + 'SUSY' if flags["SupersymmetricModel"] else 'Renormalization' + ' scale input";\n'\
        "else\n"\
        "  "
    towrite += 'slha["MODSEL"][""] << 1 << 1 << "# GUT scale input";\n'\
      'slha["MODSEL"][""] << 2 << *BoundaryCondition << "# Boundary conditions";\n'\
      'slha["MODSEL"][""] << 5 << 1 << "# Switching on CP violations";\n'\
      'if(*GenerationMixing)\n'\
      '  slha["MODSEL"][""] << 6 << 1 << "# switching on flavour violation";\n'\
      'if(inputs.param.find("Qin") != inputs.param.end())\n'\
      '  slha["MODSEL"][""] << 12 << *inputs.param.at("Qin") << "# Qin";\n'\
      '\n'\


    towrite += '\n'\
      '// Block SMINPUTS\n'\
      'SLHAea_add_block(slha, "SMINPUTS");\n'\
      'slha["SMINPUTS"][""] << 1 << 1.0 / *Alpha_mZ_MS << "# alpha_em^-1(MZ)^MSbar";\n'\
      'slha["SMINPUTS"][""] << 2 << *G_F << "# G_mu [GeV^-2]";\n'\
      'slha["SMINPUTS"][""] << 3 << *AlphaS_mZ << "# alpha_s(MZ)^MSbar";\n'\
      'slha["SMINPUTS"][""] << 4 << *mZ << "# m_Z(pole)";\n'\
      'slha["SMINPUTS"][""] << 5 << (*mf_d)(3) << "# m_b(m_b), MSbar";\n'\
      'slha["SMINPUTS"][""] << 6 << (*mf_u)(3) << "# m_t(pole)";\n'\
      'slha["SMINPUTS"][""] << 7 << (*mf_l)(3) << "# m_tau(pole)";\n'\
      'slha["SMINPUTS"][""] << 8 << (*mf_nu)(3) << "# m_nu_3";\n'\
      'slha["SMINPUTS"][""] << 11 << (*mf_l)(1) << "# m_e(pole)";\n'\
      'slha["SMINPUTS"][""] << 12 << (*mf_nu)(1) << "# m_nu_1";\n'\
      'slha["SMINPUTS"][""] << 13 << (*mf_l)(2) << "# m_muon(pole)";\n'\
      'slha["SMINPUTS"][""] << 14 << (*mf_nu)(2) << "# m_nu_2";\n'\
      'slha["SMINPUTS"][""] << 21 << (*mf_d)(1) << "# m_d(2 GeV), MSbar";\n'\
      'slha["SMINPUTS"][""] << 22 << (*mf_u)(1) << "# m_u(2 GeV), MSbar";\n'\
      'slha["SMINPUTS"][""] << 23 << (*mf_d)(2) << "# m_s(2 GeV), MSbar";\n'\
      'slha["SMINPUTS"][""] << 24 << (*mf_u)(2) << "# m_c(m_c), MSbar";\n'\

    if not flags["OnlyLowEnergySPheno"] :
      towrite += "// Write output in the super-CKM basis\n"\
        "if(*SwitchToSCKM)\n"\
        "{\n"\
        'SLHAea_add_block(slha, "VCKMIN");\n'\
        'slha["VCKMIN"][""] << 1 << *lam_wolf << "# lambda";\n'\
        'slha["VCKMIN"][""] << 2 << *A_wolf << "# A";\n'\
        'slha["VCKMIN"][""] << 3 << *rho_wolf << "# rho bar";\n'\
        'slha["VCKMIN"][""] << 4 << *eta_wolf << "# eta bar";\n'

      if flags["SupersymmetricModel"] :
        towrite += '// Initialize temporary variables\n'\
          'Farray<Fcomplex16,1,6,1,6> ZU_ckm = *ZU;\n'\
          'Farray<Fcomplex16,1,6,1,6> ZD_ckm = *ZD;\n'\
          'Farray<Fcomplex16,1,6,1,6> ZE_pmns = *ZE;\n'\
          'Farray<Fcomplex16,1,3,1,3> ZV_pmns = *ZV;\n'\
          '\n'\
          'Farray<Fcomplex16,1,3,1,3> Yu_ckm, Yd_ckm, Tu_ckm, Td_ckm, mq2_ckm, mu2_ckm, md2_ckm;\n'\
          'Farray<Fcomplex16,1,3,1,3> Ye_pmns, Te_pmns, ml2_pmns, me2_pmns, Ye_transpose, id3C;\n'\
          'Farray<Fcomplex16,1,3,1,3> CKM_Q, PMNS_Q;\n'\
          'for(int i=1; i<=3; i++)\n'\
          '{\n'\
          'for(int j=1; j<=3; j++)\n'\
          '{\n'\
          'Yu_ckm(i,j) = {0.0,0.0};\n'\
          'Yd_ckm(i,j) = {0.0,0.0};\n'\
          'Tu_ckm(i,j) = {0.0,0.0};\n'\
          'Td_ckm(i,j) = {0.0,0.0};\n'\
          'mq2_ckm(i,j) = {0.0,0.0};\n'\
          'mu2_ckm (i,j) = {0.0,0.0};\n'\
          'md2_ckm(i,j) = {0.0,0.0};\n'\
          'Ye_pmns(i,j) = {0.0,0.0};\n'\
          'Te_pmns(i,j) = {0.0,0.0};\n'\
          'ml2_pmns(i,j) = {0.0,0.0};\n'\
          'me2_pmns(i,j) = {0.0,0.0};\n'\
          'Ye_transpose(i,j) = (*Ye)(j,i);\n'\
          'id3C(i,j) = {0.0,0.0};\n'\
          '}\n'\
          'id3C(i,i) = {1.0,0.0};\n'\
          '}\n'\
          '\n'\
          '// Convert to super-CKM and super-PMNS variables\n'\
          'Flogical False = false;\n'\
          'try{ Switch_to_superCKM(*Yd, *Yu, *Td, *Tu, *md2, *mq2, *mu2, Td_ckm, Tu_ckm, md2_ckm, mq2_ckm,mu2_ckm, False, ZD_ckm, ZU_ckm, *ZD, *ZU, CKM_Q, Yd_ckm, Yu_ckm); }\n'\
          'catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n'\
          'try{ Switch_to_superPMNS(Ye_transpose, id3C, *Te, *me2, *ml2, Te_pmns, me2_pmns, ml2_pmns, False, ZE_pmns, ZV_pmns, *ZE, *ZV, PMNS_Q, Ye_pmns); }\n'\
          'catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n'\
          '\n'\
          '// Save rotated values to old variables\n'\
          '*Yd = Yd_ckm;\n'\
          '*Yu = Yu_ckm;\n'\
          '*Td = Td_ckm;\n'\
          '*Tu = Tu_ckm;\n'\
          '*md2 = md2_ckm;\n'\
          '*mu2 = mu2_ckm;\n'\
          '*mq2 = mq2_ckm;\n'\
          '*ZU = ZU_ckm;\n'\
          '*ZD = ZD_ckm;\n'\
          '*Ye = Ye_pmns;\n'\
          '*Te = Te_pmns;\n'\
          '*ml2 = ml2_pmns;\n'\
          '*me2 = me2_pmns;\n'\
          '*ZE = ZE_pmns;\n'\
          '*ZV = ZV_pmns;\n'\
          '\n'\
          '// Write output to new blocks\n'\
          'SLHAea_add_block(slha, "VCKM", Q);\n'\
          'SLHAea_add_block(slha, "IMVCKM", Q);\n'\
          'for(int i=1; i<=3; i++)\n'\
          '{\n'\
          'for(int j=1; j<=3; j++)\n'\
          '{\n'\
          'slha["VCKM"][""] << i << j << CKM_Q(i,j).re << "# V_" << i << j;\n'\
          'slha["IMVCKM"][""] << i << j << CKM_Q(i,j).im << "# Im(V_" << i << j << ")";\n'\
          '}\n'\
          '}\n'\
          '}\n'

    # List of added blocks
    addedblocks = []

    # Go through each LH block we know about, and assign each entry to the SLHAea object.
    # TG: This assumes these are all complex, but it's not always so
    # SB: added param.is_real output, queried by SARAH, using this
    for block, entry in blockparams.iteritems():

        # Firstly create the block
        towrite += (
                "\n"
                "SLHAea_add_block(slha, \"{0}\", Q);\n"
        ).format(block)

        addedblocks.append(block)

        matrix = False
        size = ""

        # If it's a matrix, flag it, and get the size. 
        if 'matrix' in entry:
            matrix = True
            size = entry['matrix']

        elif 'mixingmatrix' in entry:
            matrix = True
            size = entry['mixingmatrix']

        # If we don't have a matrix, use the block indices
        if not matrix:
            for index, param in entry.iteritems():

                # Is it a real parameter?
                real = reality_dict[param]                   

                # If it's a real parameter don't need to take the real part.
                # Just add it.
                if real == True:
                    towrite += (
                        "slha[\"{0}\"][\"\"] << {1} << *{2} << \"# {2}\";\n"
                    ).format(block, index, param)
                # If it's complex, add both real and imaginary entries
                else:
                    # Make sure the block is added if we need it
                    if not "IM"+block in addedblocks:
                        towrite += "SLHAea_add_block(slha, \"IM{0}\", Q);\n".format(block)
                        addedblocks.append("IM"+block)
                    towrite += (
                        "slha[\"{0}\"][\"\"] << {1} << *{2}.re << \"# Re({2})\";\n"
                        "slha[\"IM{0}\"][\"\"] << {1} << *{2}.im << \"# Im({2})\";\n"
                    ).format(block, index, param)


        # Matrix - gotta do these element by element
        else:
            # If there is an outputname, save it
            oname = entry['outputname'] if 'outputname' in entry else block

            # Size of the matrix
            i,j = size.split('x')

            # Is it a real matrix?
            real = reality_dict[oname]

            # If it's a real matrix don't need to take the real part.
            # Just add it.
            if real == True:
                towrite += (
                        "for(int i=1; i<={1}; i++)\n"
                        "{{\n"
                        "for(int j=1; j<={2}; j++)\n"
                        "{{\n"
                        "slha[\"{0}\"][\"\"] << i << j << (*{3})(i,j) << "
                        "\"# {0}(\" << i << \",\" << j << \")\";\n"
                        "}}\n"
                        "}}\n"
                ).format(block, i, j, oname)
            else:
                # Add the imaginary block if it's not declared real. 
                # Then add the loop for the matrix.
                addedblocks.append("IM"+block)
                towrite += (
                        "SLHAea_add_block(slha, \"IM{0}\", Q);\n"
                        "for(int i=1; i<={1}; i++)\n"
                        "{{\n"
                        "for(int j=1; j<={2}; j++)\n"
                        "{{\n"
                        "slha[\"{0}\"][\"\"] << i << j << (*{3})(i,j).re << "
                        "\"# {0}(\" << i << \",\" << j << \")\";\n"
                        "slha[\"IM{0}\"][\"\"] << i << j << (*{3})(i,j).im << "
                        "\"# Im({0}(\" << i << \",\" << j << \"))\";\n"
                        "}}\n"
                        "}}\n"
                ).format(block, i, j, oname)


    towrite += (
            "\n"
            "// Block MASS\n"
            "SLHAea_add_block(slha, \"MASS\")\n"
    )

    for particle in particles:
        mass = re.sub(r"\d","",particle.alt_mass_name)
        index = re.sub(r"[A-Za-z]","",particle.alt_mass_name)
        brace = "(" + str(index) + ")" if index else ""

        towrite += (
                "slha[\"MASS\"][\"\"] << {0} << (*{1}){2} << \"# {3}_{4}\";\n"
        ).format(particle.PDG_code, mass, brace, particle.name, str(index))

    towrite +=  (
            "slha[\"MASS\"][\"\"] << 23 << *MVZ << \"# VZ\";\n"
            "slha[\"MASS\"][\"\"] << 24 << *MVWm << \"# VWm\";\n"
            "\n"
            "// Check whether any of the masses is NaN\n"
            "auto block = slha[\"MASS\"];\n"
            "for(auto it = block.begin(); it != block.end(); it++)\n"
            "{\n"
            "if((*it)[0] != \"BLOCK\" and Utils::isnan(stod((*it)[1])) )\n"
            "{\n"
            "std::stringstream message;\n"
            "message << \"Error in spectrum generator: mass of \" "
            "<< Models::ParticleDB().long_name(std::pair<int,int>(stoi((*it)[0]),0))"
            " << \" is NaN\";\n"
            "logger() << message.str() << EOM;\n"
            "invalid_point().raise(message.str());\n"
            "}\n"
            "}\n"
            "\n"
            "// Block DMASS\n"
            "if(*GetMassUncertainty)\n"
            "{\n"
            "SLHAea_add_block(slha, \"DMASS\");\n"
    )

    # S.B. Harvested the PDG code : index from SPheno -- the order is 
    # non-trivial due the way mass lists are imported in SARAH
    for i, particle in enumerate(particles):
        mass = re.sub(r"\d","",particle.alt_mass_name)
        index = re.sub(r"[A-Za-z]","",particle.alt_mass_name)
        mud_index = mass_uncertainty_dict[str(particle.PDG_code)]    

        towrite += (
                "slha[\"DMASS\"][\"\"] << {0} << "
                "sqrt(pow((*mass_uncertainty_Q)({1}),2)"
                "+pow((*mass_uncertainty_Yt)({1}),2)) << "
                "\"# {2}_{3}\";\n"
        ).format(str(abs(int(particle.PDG_code))), str(mud_index), particle.name, str(index))
   
    towrite += (
                "\n"
                "// Do the W mass separately.  Here we use 10 MeV based on "
                "the size of corrections from two-loop papers and advice from "
                "Dominik Stockinger.\n"
                "slha[\"DMASS\"][\"\"] << 24 << 0.01 / *mW << \" # mW\";\n"
                "}\n"
    )

    towrite += (
            "\n"
            "// Block SPhenoINFO\n"
            "SLHAea_add_block(slha, \"SPhenoInput\");\n"
            "slha[\"SPheno\"][\"\"] << 1 << *ErrorLevel << \"# ErrorLevel\";\n"
            "slha[\"SPheno\"][\"\"] << 2 << *SPA_convention << \"# SPA_conventions\";\n"
            "slha[\"SPheno\"][\"\"] << 8 << *TwoLoopMethod << \"# Two Loop Method\";\n"
            "slha[\"SPheno\"][\"\"] << 9 << *GaugelessLimit << \"# Gauge-less limit\";\n"
            "slha[\"SPheno\"][\"\"] << 31 << *mGUT << \"# GUT scale\";\n"
            "slha[\"SPheno\"][\"\"] << 33 << Q << \"# Renormalization scale\";\n"
            "slha[\"SPheno\"][\"\"] << 34 << *delta_mass << \"# Precision\";\n"
            "slha[\"SPheno\"][\"\"] << 35 << *n_run << \"# Iterations\";\n"
            "if(*TwoLoopRGE)\n"
            "  slha[\"SPheno\"][\"\"] << 38 << 2 << \"# RGE level\";\n"
            "else\n"
            "slha[\"SPheno\"][\"\"] << 38 << 1 << \"# RGE level\";\n"
            "slha[\"SPheno\"][\"\"] << 40 << 1.0 / *Alpha << \"# Alpha^-1\";\n"
            "slha[\"SPheno\"][\"\"] << 41 << *gamZ << \"# Gamma_Z\";\n"
            "slha[\"SPheno\"][\"\"] << 42 << *gamW << \"# Gamma_W\";\n"
            "slha[\"SPheno\"][\"\"] << 50 << *RotateNegativeFermionMasses << \"# Rotate negative fermion masses\";\n"
            "slha[\"SPheno\"][\"\"] << 51 << *SwitchToSCKM << \"# Switch to SCKM matrix\";\n"
            "slha[\"SPheno\"][\"\"] << 52 << *IgnoreNegativeMasses << \"# Ignore negative masses\";\n"
            "slha[\"SPheno\"][\"\"] << 53 << *IgnoreNegativeMassesMZ << \"# Ignore negative masses at MZ\";\n"
            "slha[\"SPheno\"][\"\"] << 55 << *CalculateOneLoopMasses << \"# Calculate one loop masses\";\n"
            "slha[\"SPheno\"][\"\"] << 56 << *CalculateTwoLoopHiggsMasses << \"# Calculate two-loop Higgs masses\";\n"
            "slha[\"SPheno\"][\"\"] << 57 << *CalculateLowEnergy << \"# Calculate low energy\";\n"
            "slha[\"SPheno\"][\"\"] << 60 << *KineticMixing << \"# Include kinetic mixing\";\n"
            "slha[\"SPheno\"][\"\"] << 65 << *SolutionTadpoleNr << \"# Solution of tadpole equation\";\n"
            "\n"
            "// Retrieve mass cuts\n"
            "static const Spectrum::cuts_info mass_cuts = Spectrum::retrieve_mass_cuts(inputs.options);\n"
            "\n"
            "// Has the user chosen to override any pole mass values?\n"
            "// This will typically break consistency, but may be useful in some special cases\n"
            "if (inputs.options->hasKey(\"override_pole_masses\"))\n"
            "{\n"
            "std::vector<str> particle_names = inputs.options->getNames(\"override_pole_masses\");\n"
            "for (auto& name : particle_names)\n"
            "{\n"
            "double mass = inputs.options->getValue<double>(\"override_pole_masses\", name);\n"
            "SLHAea_add(slha, \"MASS\", Models::ParticleDB().pdg_pair(name).first, mass, name, true);\n"
            "}\n"
            "}\n"
            "\n"
            "//Create Spectrum object\n"
            "Spectrum spectrum = spectrum_from_SLHAea<'+model_name+'SimpleSpec, SLHAstruct>(slha,slha,mass_cuts);\n"
            "\n"
            "return spectrum;\n"
            "\n"
            "}\n"
            "\n"
    )
    # End of Spectrum_Out

    # get_HiggsCouplingsTable function
    # The targets for HiggsBounds
    hb_targets = ["rHB_S_S_Fd",  "rHB_P_P_Fd", "rHB_S_S_Fu",  "rHB_P_P_Fu", 
                  "rHB_S_S_Fe",  "rHB_P_P_Fe", "rHB_S_VWm",   "rHB_P_VWm",  
                  "rHB_S_VZ",  "rHB_P_VZ", "ratioPP",  "ratioPPP",  
                  "ratioGG",  "ratioPGG", "CPL_H_H_Z",  "CPL_A_H_Z",  "CPL_A_A_Z"]

    hboutput = True
    # If the targets aren't all there then don't write HiggsBounds output. 
    # They should be!
    if not all(param in hb_variables for param in hb_targets):
        hboutput = False

    if hboutput:
        # Get number of Higgses from the size of the vector 
        # interacting with gauge bosons
        numh0 = ( int(hb_variables["rHB_S_VZ"].size) if "rHB_S_VZ" 
                    in hb_variables else 0 )
        # Subtract 1 since A starts at (2), not (1)
        numA0 = ( (int(hb_variables["rHB_P_VZ"].size)-1) 
                   if "rHB_S_VZ" in hb_variables else 0 )

    # Check there's more than 1 Higgs otherwise no need for this really. 
    if hboutput and (numh0 + numA0 > 1):
        towrite += (
                "// Convenience function to obtain a HiggsCouplingsTable "
                "object for HiggsBounds\n"
                "int get_HiggsCouplingsTable(const Spectrum& spectrum, "
                "HiggsCouplingsTable& hctbl, const Finputs& inputs)\n{\n"
                "\n"
                "// Pass the GAMBIT spectrum to SPheno and fill the "
                "internal decay objects (if necessary)\n"
                "fill_spectrum_calculate_BRs(spectrum, inputs);\n"
                "\n"
                "if(*kont != 0)\n"
                "  ErrorHandling(*kont);\n"
                "\n"
                "/* Fill in effective coupling ratios.\n"
                "  These are the ratios of BR_BSM(channel)/BR_SM(channel) */\n"
        )

        # Go through each entry in what we've harvested to give to HiggsBounds.
        # Quarks and leptons first:
        hb = "// Couplings to SM fermions and gauge bosons\n"

        for i in range(numh0):
            hb += (
               "hctbl.C_ss2[{0}] = pow( (*rHB_S_S_Fd)({1},2), 2 );"
               " // Coupling (h0_{1} s sbar)\n"
               "hctbl.C_bb2[{0}] = pow( (*rHB_S_S_Fd)({1},3), 2 );"
               " // Coupling (h0_{1} b bbar)\n"
               "hctbl.C_cc2[{0}] = pow( (*rHB_S_S_Fu)({1},2), 2 );"
               " // Coupling (h0_{1} c cbar)\n"
               "hctbl.C_tt2[{0}] = pow( (*rHB_S_S_Fu)({1},3), 2 );"
               " // Coupling (h0_{1} t tbar)\n"
               "hctbl.C_mumu2[{0}] = pow( (*rHB_S_S_Fe)({1},2), 2 );"
               " // Coupling (h0_{1} mu+ mu-)\n"
               "hctbl.C_tautau2[{0}] = pow( (*rHB_S_S_Fe)({1},3), 2 );"
               " // Coupling (h0_{1} tau+ tau-)\n"
               "hbtbl.C_WW2[{0}] = (*rHB_S_VWm)({1});"
               " // Coupling (h0_{1} W+ W-)\n"
               "hbtbl.C_ZZ2[{0}] = (*rHB_S_VZ)({1});"
               " // Coupling (h0_{1} Z Z)\n"
               "hctbl.C_gaga2[{0}] = pow( (*ratioPP)({1}).re, 2 );"
               " // Coupling (h0_{1} gamma gamma)\n"
               "hctbl.C_gg2[{0}] = pow( (*ratioGG)({1}).re, 2 );"
               " // Coupling (h0_{1} glu glu)\n"
            ).format(i, i+1)
        for i in range(numA0):
            hb += (
               "hctbl.C_ss2[{0}] = pow( (*rHB_P_P_Fd)({1},2), 2 );"
               " // Coupling (A0_{2} s sbar)\n"
               "hctbl.C_bb2[{0}] = pow( (*rHB_P_P_Fd)({1},3), 2 );"
               " // Coupling (A0_{2} b bbar)\n"
               "hctbl.C_cc2[{0}] = pow( (*rHB_P_P_Fu)({1},2), 2 );"
               " // Coupling (A0_{2} c cbar)\n"
               "hctbl.C_tt2[{0}] = pow( (*rHB_P_P_Fu)({1},3), 2 );"
               " // Coupling (A0_{2} t tbar)\n"
               "hctbl.C_mumu2[{0}] = pow( (*rHB_P_P_Fe)({1},2), 2 );"
               " // Coupling (A0_{2} mu+ mu-)\n"
               "hctbl.C_tautau2[{0}] = pow( (*rHB_P_P_Fe)({1},3), 2 );"
               " // Coupling (A0_{2} tau+ tau-)\n"
               "hbtbl.C_WW2[{0}] = (*rHB_P_VWm)({1});"
               " // Coupling (A0_{2} W+ W-)\n"
               "hbtbl.C_ZZ2[{0}] = (*rHB_P_VZ)({1});"
               " // Coupling (A0_{2} Z Z)\n"
               "hctbl.C_gaga2[{0}] = pow( (*ratioPPP)({1}).re, 2 );"
               " // Coupling (A0_{2} gamma gamma)\n"
               "hctbl.C_gg2[{0}] = pow( (*ratioPGG)({1}).re, 2 );"
               " // Coupling (A0_{2} glu glu)\n"
            ).format(numh0+i, i+2, i+1)

        # Make this alphabetical, because it looks nice
        towrite += "\n".join(sorted(hb.split("\n")))

        # Two Higgses + Z
        towrite += (
            "\n\n// hhZ effective couplings. This is symmetrised "
            "(i.e. j,i = i,j)\n"
        )
        # First HHZ
        for i in range(numh0):
            for j in range(numh0):
                towrite += (
                        "hctbl.C_hiZ2[{0}][{1}] = (*CPL_H_H_Z)({2},{3}).re;"
                        " // Coupling (h0_{2} h0_{3} Z)\n"
                ).format(i, j, i+1, j+1)
        # Now HAZ
        for i in range(numh0):
            for j in range(numA0):
                towrite += (
                    "hctbl.C_hiZ2[{0}][{1}] = (*CPL_A_H_Z)({2},{3}).re;"
                        " // Coupling (A0_{4} h0_{3} Z)\n"
                    "hctbl.C_hiZ2[{1}][{0}] = (*CPL_A_H_Z)({2},{3}).re;"
                        " // Coupling (h0_{3} A0_{4} Z)\n"
                ).format(i, j, i+1, j+2, j+1)
        # Finally AAZ
        for i in range(numA0):
            for j in range(numA0):
                towrite += (
                        "hctbl.C_hiZ2[{0}][{1}] = (*CPL_A_A_Z)({2},{3}).re;"
                        " // Coupling (A0_{4} A0_{5} Z)\n" 
                ).format(i, j, i+2, j+2, i+1, j+1)

        towrite += (
                "\n"
                "// Check there's no errors\n"
                "if(*kont != 0)\n"
                "  ErrorHandling(*kont);\n"
                "\n"
                "return *kont;\n"
                "}\n"
        )
        # End of get_HiggsCouplingsTable

    # ReadingData function
    towrite += "\n"\
      "// Function to read data from the Gambit inputs and fill SPheno internal variables\n"\
      "void ReadingData(const Finputs &inputs)\n"\
      "{\n"\
      "\n"\
      "InitializeStandardModel(inputs.sminputs);\n"\
      "try{ InitializeLoopFunctions(); }\n"\
      "catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n"\
      "\n"\
      "*ErrorLevel = -1;\n"\
      "//*GenerationMixing = true;\n"\
      "\n"\
      "try{ Set_All_Parameters_0(); }\n"\
      "catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n"\
      "\n"\
      "*TwoLoopRGE = true;\n"\
      "\n"\
      "*kont = 0;\n"\
      '\n'\
      '/****************/\n'\
      '/* Block MODSEL */\n'\
      '/****************/\n'\
      '// Already in Backend initialization function\n'\
      '\n'\
      '/******************/\n'\
      '/* Block SMINPUTS */\n'\
      '/******************/\n'\
      '// Already in InitializeStandardModel\n'\
      '\n'\
      '/****************/\n'\
      '/* Block VCKMIN */\n'\
      '/****************/\n'\
      '// Already in SMInputs\n'\
      '\n'\
      '/****************/\n'\
      '/* Block FCONST */\n'\
      '/****************/\n'\
      '// Some hadron constants, not really needed\n'\
      '\n'\
      '/***************/\n'\
      '/* Block FMASS */\n'\
      '/***************/\n'\
      '// Masses of hadrons, not really needed\n'\
      '\n'\
      '/***************/\n'\
      '/* Block FLIFE */\n'\
      '/***************/\n'\
      '// Lifetimes of hadrons, not really needed\n'\
      '\n'\
      '/*******************************/\n'\
      '/* Block SPHENOINPUT (options) */\n'\
      '/*******************************/\n'\
      '// 1, Error_Level\n'\
      '*ErrorLevel = inputs.options->getValueOrDef<Finteger>(-1, "ErrorLevel");\n'\
      '\n'\
      '// 2, SPA_convention\n'\
      '*SPA_convention = inputs.options->getValueOrDef<bool>(false, "SPA_convention");\n'\
      'if(*SPA_convention)\n'\
      '{\n'\
      'Freal8 scale = 1.0E6;  // SPA convention is 1 TeV\n'\
      'try {SetRGEScale(scale); }\n'\
      'catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n'\
      '}\n'\
      '\n'\
      '// 3, External_Spectrum\n'\
      '// GAMBIT: no need for external spectrum options\n'\
      '*External_Spectrum = false;\n'\
      '*External_Higgs = false;\n'\
      '\n'\
      '// 4, Use_Flavour_States\n'\
      '// GAMBIT: private variable, cannot import\n'\
      '\n'\
      '// 5, FermionMassResummation\n'\
      '// GAMBIT: not covered\n'\
      '*FermionMassResummation = true;\n'\
      '\n'\
      '// 6, RXiNew\n'\
      '*RXiNew = inputs.options->getValueOrDef<Freal8>(1.0, "RXiNew");\n'\
      '\n'\
      '// 7, Caclulate Two Loop Higgs Masses\n'\
      '*CalculateTwoLoopHiggsMasses = inputs.options->getValueOrDef<bool>(true, "CalculateTwoLoopHiggsMasses");\n'\
      '\n'\
      '// 8, Two Loop method\n'\
      '*TwoLoopMethod = inputs.options->getValueOrDef<Finteger>(3, "TwoLoopMethod");\n'\
      'switch(*TwoLoopMethod)\n'\
      '{\n'\
      'case 1:\n'\
      '  *PurelyNumericalEffPot = true;\n'\
      '  *CalculateMSSM2Loop = false;\n'\
      '  break;\n'\
      'case 2:\n'\
      '  *PurelyNumericalEffPot = false;\n'\
      '  *CalculateMSSM2Loop = false;\n'\
      '  break;\n'\
      'case 3:\n'\
      '  *CalculateMSSM2Loop = false;\n'\
      '  break;\n'\
      'case 8:\n'\
      '  *CalculateMSSM2Loop = true;\n'\
      '  break;\n'\
      'case 9:\n'\
      '  *CalculateMSSM2Loop = true;\n'\
      '  break;\n'\
      'default:\n'\
      '  *CalculateTwoLoopHiggsMasses = false;\n'\
      '}\n'\
      '\n'\
      '// 9, GaugelessLimit\n'\
      '*GaugelessLimit = inputs.options->getValueOrDef<bool>(true, "GaugelessLimit");\n'\
      '\n'\
      '// 400, hstep_pn\n'\
      '*hstep_pn = inputs.options->getValueOrDef<Freal8>(0.1, "hstep_pn");\n'\
      '\n'\
      '// 401, hstep_pn\n'\
      '*hstep_sa = inputs.options->getValueOrDef<Freal8>(0.001, "hstep_sa");\n'\
      '\n'\
      '// 410, TwoLoopRegulatorMass\n'\
      '*TwoLoopRegulatorMass = inputs.options->getValueOrDef<Freal8>(0.0, "TwoLoopRegulatorMass");\n'\
      '\n'\
      '// 10, TwoLoopSafeMode\n'\
      '*TwoLoopSafeMode = inputs.options->getValueOrDef<bool>(false, "TwoLoopSafeMode");\n'\
      '\n'\
      '// 11, whether to calculate branching ratios or not, L_BR\n'\
      '// All BR details are taken by other convenience function\n'\
      '*L_BR = false;\n'\
      '\n'\
      '// 12, minimal value such that a branching ratio is written out, BRMin\n'\
      '// All BR details are taken by other convenience function\n'\
      '\n'\
      '// 13, 3 boday decays\n'\
      '// All BR details are taken by other convenience function\n'\
      '\n'\
      '// 14, run SUSY couplings to scale of decaying particle\n'\
      '// All BR details are taken by other convenience function\n'\
      '\n'\
      '// 15, MinWidth\n'\
      '// All BR details are taken by other convenience function\n'\
      '\n'\
      '// 16. OneLoopDecays\n'\
      '// All BR details are taken by other convenience function\n'\
      '\n'\
      '// 19, MatchingOrder: maximal number of iterations\n'\
      '*MatchingOrder = inputs.options->getValueOrDef<Finteger>(-2, "MatchingOrder");\n'\
      '\n'\
      '// 20, GetMassUncertainty\n'\
      '*GetMassUncertainty = inputs.options->getValueOrDef<bool>(false, "GetMassUncertainty");\n'\
      '\n'\
      '// 21-26, whether to calculate cross sections or not, L_CS\n'\
      '*L_CS = false;\n'\
      '\n'\
      '// 31, setting a fixed GUT scale, GUTScale\n'\
      'Freal8 GUTScale = inputs.options->getValueOrDef<Freal8>(0.0, "GUTScale");\n'\
      'if(GUTScale > 0.0)\n'\
      '{\n'\
      'try{ SetGUTScale(GUTScale); }\n'\
      'catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n'\
      '}\n'\
      '\n'\
      '// 32, requires strict unification, StrictUnification\n'\
      'Flogical StrictUnification = inputs.options->getValueOrDef<bool>(false, "StrictUnification");\n'\
      'if(StrictUnification)\n'\
      '{\n'\
      'try{ SetStrictUnification(StrictUnification); }\n'\
      'catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n'\
      '}\n'\
      '\n'\
      '// 33, setting a fixed renormalization scale\n'\
      'Freal8 RGEScale = inputs.options->getValueOrDef<Freal8>(0.0, "RGEScale");\n'\
      'if(RGEScale > 0.0)\n'\
      '{\n'\
      'RGEScale *= RGEScale;\n'\
      'try{ SetRGEScale(RGEScale); }\n'\
      'catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n'\
      '}\n'\
      '\n'\
      '// 34, precision of mass calculation, delta_mass\n'\
      '*delta_mass = inputs.options->getValueOrDef<Freal8>(0.00001, "delta_mass");\n'\
      '\n'\
      '// 35, maximal number of iterations, n_run\n'\
      '*n_run = inputs.options->getValueOrDef<Finteger>(40, "n_run");\n'\
      '\n'\
      '// 36, minimal number of iterations\n'\
      '*MinimalNumberIterations = inputs.options->getValueOrDef<Finteger>(5, "MinimalNumberIterations");\n'\
      '\n'\
      '// 37, if = 1 -> CKM through V_u, if = 2 CKM through V_d, YukawaScheme\n'\
      'Finteger YukawaScheme = inputs.options->getValueOrDef<Finteger>(1, "YukawaScheme");\n'\
      'if(YukawaScheme == 1 or YukawaScheme == 2)\n'\
      '{\n'\
      'try{ SetYukawaScheme(YukawaScheme); }\n'\
      'catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n'\
      '}\n'\
      '\n'\
      '// 38, set looplevel of RGEs, TwoLoopRGE\n'\
      '*TwoLoopRGE = inputs.options->getValueOrDef<bool>(true, "TwoLoopRGE");\n'\
      '\n'\
      '// 39, write additional SLHA1 file, Write_SLHA1\n'\
      '// GABMIT: Always false, no file output\n'\
      '*WriteSLHA1 = false;\n'\
      '\n'\
      '// 40, alpha(0), Alpha\n'\
      'Freal8 alpha = 1.0/137.035999074;\n'\
      '*Alpha = inputs.options->getValueOrDef<Freal8>(alpha,"Alpha");\n'\
      '\n'\
      '// 41, Z-boson width, gamZ\n'\
      '*gamZ = inputs.options->getValueOrDef<Freal8>(2.49,"gamZ");\n'\
      '\n'\
      '// 42, W-boson width, gamW\n'\
      '*gamW = inputs.options->getValueOrDef<Freal8>(2.06,"gamW");\n'\
      '\n'\
      '// 50, RotateNegativeFermionMasses\n'\
      '// Never rotate the masses, it\'s agains SLHA convention and Gambit cannot handle complex couplings\n'\
      '*RotateNegativeFermionMasses = false;\n'\
      '\n'\
      '// 51, Switch to SCKM\n'\
      '// This the default behaviour with GAMBIT\n'\
      '*SwitchToSCKM = true;\n'\
      '\n'\
      '// 52, Ignore negative masses\n'\
      '*IgnoreNegativeMasses = inputs.options->getValueOrDef<bool>(false, "IgnoreNegativeMasses");\n'\
      '\n'\
      '// 53, Ignore negative masses at MZ\n'\
      '*IgnoreNegativeMassesMZ = inputs.options->getValueOrDef<bool>(false, "IgnoreNegativeMassesMZ");\n'\
      '\n'\
      '// 54, Write Out for non convergence\n'\
      '*WriteOutputForNonConvergence = inputs.options->getValueOrDef<bool>(false, "WriteOutputForNonConvergence");\n'\
      '\n'\
      '// 55, calculate one loop masses\n'\
      '*CalculateOneLoopMasses = inputs.options->getValueOrDef<bool>(true, "CalculateOneLoopMasses");\n'\
      '\n'\
      '// 57, calculate low energy observables\n'\
      '*CalculateLowEnergy = false;\n'\
      '\n'\
      '// 58, include delta and/or BSM delta VB\n'\
      '*IncludeDeltaVB = inputs.options->getValueOrDef<bool>(true, "IncludeDeltaVB");\n'\
      'if(*IncludeDeltaVB)\n'\
      '  *IncludeBSMdeltaVB = inputs.options->getValueOrDef<bool>(true, "IncludeBSMdeltaVB");\n'\
      '\n'\
      '// 60, kinetic mixing\n'\
      '*KineticMixing = inputs.options->getValueOrDef<bool>(true, "KineticMixing");\n'\
      '\n'\
      '// 62,\n'\
      '*RunningSUSYparametersLowEnergy = inputs.options->getValueOrDef<bool>(true, "RunningSUSYparametersLowEnergy");\n'\
      '  \n'\
      '// 63,\n'\
      '*RunningSMparametersLowEnergy = inputs.options->getValueOrDef<bool>(true, "RunningSMparametersLowEnergy");\n'\
      '\n'\
      '// 64\n'\
      '*WriteParametersAtQ = inputs.options->getValueOrDef<bool>(false, "WriteParametersAtQ");\n'\
      '\n'\
      '// 65\n'\
      '*SolutionTadpoleNr = inputs.options->getValueOrDef<Finteger>(1, "SolutionTadpoleNr");\n'\
      '\n'\
      '// 66\n'\
      '*DecoupleAtRenScale = inputs.options->getValueOrDef<bool>(false, "DecoupleAtRenScale");\n'\
      '\n'\
      '// 67\n'\
      '*Calculate_mh_within_SM = inputs.options->getValueOrDef<bool>(true, "Calculate_mh_within_SM");\n'\
      'if(*Calculate_mh_within_SM)\n'\
      '  *Force_mh_within_SM = inputs.options->getValueOrDef<bool>(false, "Force_mh_within_SM");\n'\
      '\n'\
      '// 68\n'\
      '*MatchZWpoleMasses = inputs.options->getValueOrDef<bool>(false, "MatchZWpolemasses");\n'\
      '\n'\
      '// 75,  Writes the parameter file for WHIZARD\n'\
      '// GAMBIT: no output\n'\
      '*Write_WHIZARD = false;\n'\
      '\n'\
      '// 76, Writes input files for HiggsBounds\n'\
      '// GAMBIT: no output\n'\
      '*Write_HiggsBounds = false;\n'\
      '\n'\
      '// 77, Use conventions for MO\n'\
      '// GAMBIT: no output\n'\
      '*OutputForMO = false;\n'\
      '\n'\
      '// 78,  Use conventions for MG\n'\
      '// GAMBIT: no output\n'\
      '*OutputForMG = false;\n'\
      '\n'\
      '// 79, Writes Wilson coefficients in WCXF format\n'\
      '*Write_WCXF = inputs.options->getValueOrDef<bool>(false, "Write_WCXF");\n'\
      '\n'\
      '// 80, exit for sure with non-zero value if problem occurs, Non_Zero_Exit\n'\
      '// GAMBIT: never brute exit, let GAMBIT do a controlled exit\n'\
      '*Non_Zero_Exit = false;\n'\
      '\n'\
      '// 86, width to be counted as invisible in HiggsBounds input\n'\
      '*WidthToBeInvisible = inputs.options->getValueOrDef<Freal8>(0.0, "WidthToBeInvisible");\n'\
      '  \n'\
      '// 88, maximal mass allowedin loops\n'\
      '*MaxMassLoop = pow(inputs.options->getValueOrDef<Freal8>(1.0E16, "MaxMassLoop"), 2);\n'\
      '\n'\
      '// 80, maximal mass counted as numerical zero\n'\
      '*MaxMassNumericalZero = inputs.options->getValueOrDef<Freal8>(1.0E-8, "MaxMassNumericalZero");\n'\
      '\n'\
      '// 95, force mass matrices at 1-loop to be real\n'\
      '*ForceRealMatrices = inputs.options->getValueOrDef<bool>(false, "ForceRealMatrices");\n'\
      '\n'\
      '// 150, use 1l2lshifts\n'\
      '*include1l2lshift=inputs.options->getValueOrDef<bool>(false,"include1l2lshift");\n'\
      '\n'\
      '// 151\n'\
      '*NewGBC=inputs.options->getValueOrDef<bool>(true,"NewGBC");\n'\
      '\n'\
      '// 440\n'\
      '*TreeLevelUnitarityLimits=inputs.options->getValueOrDef<bool>(true,"TreeLevelUnitarityLimits");\n'\
      '\n'\
      '// 441\n'\
      '*TrilinearUnitarity=inputs.options->getValueOrDef<bool>(true,"TrilinearUnitarity");\n'\
      '\n'\
      '// 442\n'\
      '*unitarity_s_min = inputs.options->getValueOrDef<Freal8>(2000,"unitarity_s_min");\n'\
      '\n'\
      '// 443\n'\
      '*unitarity_s_max = inputs.options->getValueOrDef<Freal8>(3000,"unitarity_s_max");\n'\
      '\n'\
      '// 444\n'\
      '*unitarity_steps = inputs.options->getValueOrDef<Finteger>(5,"unitarity_steps");\n'\
      '\n'\
      '// 445\n'\
      '*RunRGEs_unitarity=inputs.options->getValueOrDef<bool>(false,"RunRGEs_unitarity");\n'\
      '\n'\
      '// 446\n'\
      '*TUcutLevel = inputs.options->getValueOrDef<Finteger>(2,"TUcutLevel");\n'\
      '\n'\
      '// 510, Write tree level tadpole solutions\n'\
      '// Doesn\'t seem to have any effect, but add option anyway\n'\
      '*WriteTreeLevelTadpoleSolutions = inputs.options->getValueOrDef<bool>(false, "WriteTreeLevelTadpoleSolutions");\n'\
      '\n'\
      '// 515, Write GUT values\n'\
      '// In SPheno the default is true, but in GAMBIT we don\'t often need these, so false\n'\
      '*WriteGUTvalues = inputs.options->getValueOrDef<bool>(false, "WriteGUTvalues");\n'\
      '\n'\
      '// 520, write effective higgs coupling ratios\n'\
      '// Already done by the getCouplingsTable convenience function, but check\n'\
      '*WriteEffHiggsCouplingRatios = false;\n'\
      '\n'\
      '// 521, Higher order diboson\n'\
      '*HigherOrderDiboson = inputs.options->getValueOrDef<bool>(true, "HigherOrderDiboson");\n'\
      '\n'\
      '// 522\n'\
      '*PoleMassesInLoops = inputs.options->getValueOrDef<bool>(true, "PoleMassesInLoops");\n'\
      '\n'\
      '// 525, write higgs diphoton loop contributions\n'\
      '// As with 520, these should be in getCouplingsTable\n'\
      '*WriteHiggsDiphotonLoopContributions = false;\n'\
      '\n'\
      '// 530, write tree level tadpole parameters\n'\
      '*WriteTreeLevelTadpoleParameters = inputs.options->getValueOrDef<bool>(false, "WriteTreeLevelTadpoleParameters");\n'\
      '\n'\
      '// 550, CalcFT\n'\
      '// Does nothing so we don\'t include it\n'\
      '\n'\
      '// 551, one loop FT\n'\
      '// Does nothing so we don\'t include it\n'\
      '\n'\
      '// 990, make Q test\n'\
      '// Does nothing so we don\'t include it\n'\
      '\n'\
      '// 000, print debug information\n'\
      '// GAMBIT: no output\n'\
      '*PrintDebugInformation = false;\n'\
      '\n'\
      '// Silence screen output, added by GAMBIT to SPheno\n'\
      '*SilenceOutput = inputs.options->getValueOrDef<bool>(false, "SilenceOutput");\n'\
      '\n'\
      '/**********************/\n'\
      '/* Block DECAYOPTIONS */\n'\
      '/**********************/\n'\
      '\n'\
      '// All in ReadingData_decays\n'\
      '\n'\
      '\n'\
      '/****************/\n'\
      '// Block MINPAR //\n'\
      '/****************/\n'

    for par in parameters:
        if par.block == "MINPAR":
            c = "*"+par.name if par.is_real else par.name+"->re"

            # If there's no BC then assume it's the parameter name?
            e = par.name

            # Check to see if the parameter has a boundary condition
            if par.name in bcs: 
                e = bcs[par.name]
            elif par.alt_name in bcs:
                e = bcs[par.alt_name]

            towrite += (
                "if(inputs.param.find(\"{0}\") != inputs.param.end())\n"
                "  {1}"
                " = *inputs.param.at(\"{0}\");\n"
            ).format(e, c)

    towrite += "\n"\
      "/****************/\n"\
      "/* Block EXTPAR */\n"\
      "/****************/\n"

    for par in parameters:
        if par.block == "EXTPAR":
            c = "*"+par.name if par.is_real else par.name+"->re"

            # If there's no BC then assume it's the parameter name?
            e = par.name

            # Check to see if the parameter has a boundary condition
            if par.name in bcs: 
                e = bcs[par.name]
            elif par.alt_name in bcs:
                e = bcs[par.alt_name]

            towrite += (
                "if(inputs.param.find(\"{0}\") != inputs.param.end())\n"
                "  {1}"
                " = *inputs.param.at(\"{0}\");\n"
            ).format(e, c)

    # List all the blocks:
    blocks = []
    for name, var in variables.iteritems():
        if var.block != None and var.block.endswith("IN"):
            blocks.append(var.block)
    blocks = list(set(blocks))

    text = ""
    for block in blocks:
        text += "/* - {0: <13} */ \n".format(block)
    towrite += (
            "\n"
            "/*****************/\n"
            "/* Blocks:       */\n"
            "{0}"
            "/*****************/\n"
            ).format(text)

    # Input parameters
    # TODO:  The name of model parameters might be wrong
    # TODO: this does not work for matrices
    for name, var in variables.iteritems():

        if var.block != None and var.block.endswith("IN") :
            

            model_par = get_model_par_name(name, variables)
            
            # If it's *not* a matrix
            if not var.size:

                e = (name+"IN->re" if var.type.startswith("Complex") 
                                   else "*"+name+"IN" )
                towrite += (
                        "if(inputs.param.find(\"{0}\") != inputs.param.end())\n"
                        "{{\n"
                        "{1}"
                        " = *inputs.param.at(\"{0}\");\n"
                        "*InputValuefor{2} = true;\n"
                        "}}\n"
                ).format(model_par, e, name)

            # Matrix case
            else:
                i,j = var.size.split(',')

                e = ".re" if var.type.startswith("Complex") else ""

                towrite += (
                        "for (int i=1; i<={0}; i++)\n"
                        "for (int j=1; j<={1}; j++)\n"
                        "{{\n"
                        "std::stringstream parname;\n"
                        "parname << {2}_ << i << j;\n"
                        "if(inputs.param.find(parname.str()) != "
                        "inputs.param.end())\n"
                        "{{\n"
                        "*InputValueFor{3} = true;"
                        "(*{3}IN)(i,j){4} = *inputs.param.at(parname.str());\n"
                        "}}\n"
                        "}}\n"
                ).format(str(i), str(j), model_par, name, e)

     
    # So we don't need Block GAUGEIN generically?
    towrite += (
            "\n"
            "/*****************/\n"
            "/* Block GAUGEIN */\n"
            "/*****************/\n"
            "// Irrelevant\n"
            "\n"
            "// No other blocks are relevant at this stage\n"
            "\n"
            "}\n"
    )
    # end of ReadingData

    # ReadingData_decays function
    towrite += "// Function to read decay tables and options\n"\
      '{\n'\
      '// Read the file with info about decay channels\n'\
      'static bool scan_level_decays = true;\n'\
      'if (scan_level_decays)\n'\
      '{\n'\
      '// str decays_file = inputs.options->getValueOrDef<str>("", "decays_file");\n'\
      'str decays_file = str(GAMBIT_DIR) + "/Backends/data/" + STRINGIFY(BACKENDNAME) + "_" + STRINGIFY(SAFE_VERSION) + "_decays_info.dat";\n'\
      '\n'\
      'Fdecays::fill_all_channel_info(decays_file);\n'\
      '\n'\
      'scan_level_decays = false;\n'\
      '}\n'\
      ' \n'\
      '// Options for decays only\n'\
      ' \n'\
      '/********************/\n'\
      '/* Block SPhenoInput */\n'\
      '/********************/\n'\
      ' \n'\
      '// 11, whether to calculate branching ratios or not, L_BR\n'\
      '*L_BR = true;\n'\
      ' \n'\
      '// 12, minimal value such that a branching ratio is written out, BRMin\n'\
      '// This really only affects output so we don\'t care\n'\
      ' \n'\
      '// 13, 3 boday decays\n'\
      '*Enable3BDecaysF = inputs.options->getValueOrDef<bool>(true, "Enable3BDecaysF");\n'\
      '*Enable3BDecaysS = inputs.options->getValueOrDef<bool>(true, "Enable3BDecaysS");\n'\
      ' \n'\
      '// 14, run SUSY couplings to scale of decaying particle\n'\
      '*RunningCouplingsDecays = inputs.options->getValueOrDef<bool>(true, "RunningCouplingsDecays");\n'\
      '\n'\
      '// 15, MinWidth\n'\
      '*MinWidth = inputs.options->getValueOrDef<Freal8>(1.0E-30, "MinWidth");\n'\
      ' \n'\
      '// 16. OneLoopDecays\n'\
      '*OneLoopDecays = inputs.options->getValueOrDef<bool>(true, "OneLoopDecays");\n'\
      '\n'\
      '/**********************/\n'\
      '/* Block DECAYOPTIONS */\n'\
      '/**********************/\n'\
      '\n'

    # Calc3BodyDecay_<particle_i>,  CalcLoopDecay_<particle_i>
    for name, var in variables.iteritems() :
        if name.startswith("Calc3BodyDecay_") or name.startswith("CalcLoopDecay_") :
            towrite += "// " + name + '\n'\
                '*' + name + ' = inputs.options->getValueOrdef<bool>(true, "' + name + '");\n'\
                '\n'
      
    if flags["SupersymmetricModel"] :
      towrite += '// Calculate 3 body decays with only SUSY particles\n'\
        '*CalcSUSY3BodyDecays = inputs.options->getValueOrDef<bool>(false, "CalcSUSY3BodyDecays");\n'\
        '\n'
    towrite += '// 1000, Loop induced only\n'\
      '*CalcLoopDecay_LoopInducedOnly = inputs.options->getValueOrDef<bool>(false, "CalcLoopDecay_LoopInducedOnly");\n'\
      '\n'

    towrite += '// 1101, divonly_save\n'\
      '*divonly_save = inputs.options->getValueOrDef<Finteger>(1,"divonly_save");\n'\
      '\n'\
      '// 1102, divergence_save\n'\
      '*divergence_save = inputs.options->getValueOrDef<Freal8>(0.0,"divergence_save");\n'\
      '\n'\
      '// 1110, Simplistic Loop Decays\n'\
      '*SimplisticLoopDecays = inputs.options->getValueOrDef<bool>(false, "SimplisticLoopDecays");\n'\
      '\n'\
      '// 1111, Shift IR divergence\n'\
      '*ShiftIRdiv = inputs.options->getValueOrDef<bool>(true, "ShiftIRdiv");\n'\
      '\n'\
      '// 1103, Debug loop decays\n'\
      '*DebugLoopDecays = inputs.options->getValueOrDef<bool>(false, "DebugLoopDecays");\n'\
      '\n'\
      '// 1104, Only Tree Level Contributions\n'\
      '*OnlyTreeLevelContributions = inputs.options->getValueOrDef<bool>(false, "OnlyTreeLevelContributions");\n'\
      '\n'\
      '// 1114, External Z factors\n'\
      '*ExternalZfactors = inputs.options->getValueOrDef<bool>(true, "ExternalZfactors");\n'\
      '  if(*ExternalZfactors)\n'\
      '{\n'\
      '*UseZeroRotationMatrices = inputs.options->getValueOrDef<bool>(false, "UseZeroRotationMatrices");\n'\
      '*UseP2Matrices = inputs.options->getValueOrDef<bool>(true, "UseP2Matrices");\n'\
      '}\n'\
      '\n'\
      '// 1115, OS kinematics\n'\
      '*OSkinematics = inputs.options->getValueOrDef<bool>(true, "OSkinematics");\n'\
      '\n'\
      '// 1116, ew/yuk OS in decays\n'\
      '*ewOSinDecays = inputs.options->getValueOrDef<bool>(true, "ewOSinDecays");\n'\
      '*yukOSinDecays = inputs.options->getValueOrDef<bool>(false, "yukOSinDecays");\n'\
      '\n'\
      '// 1117, CT in loop decays\n'\
      '*CTinLoopDecays = inputs.options->getValueOrDef<bool>(false, "CTinLoopDecays");\n'\
      '\n'\
      '// 1118, Loop induced decays OS\n'\
      '*LoopInducedDecaysOS = inputs.options->getValueOrDef<bool>(true, "LoopInducedDecaysOS");\n'\
      '\n'\
      '// 1201, Mass regulator for photon and gluon\n'\
      '*Mass_Regulator_PhotonGluon = inputs.options->getValueOrDef<Freal8>(1e-10, "Mass_Regulator_PhotonGluon");\n'\
      '\n'\
      '// 1205, Extra scale for loop decays\n'\
      '*Extra_Scale_LoopDecays = inputs.options->getValueOrDef<bool>(false, "Extra_Scale_LoopDecays");\n'\
      'if(*Extra_Scale_LoopDecays)\n'\
      '*Scale_LoopDecays = inputs.options->getValue<Freal8>("Scale_LoopDecays");\n'\
      '\n'\
      '}\n'\
      '\n'
    # end of ReadingData_decays

    # InitializeStandardModel function
    towrite += 'void InitializeStandardModel(const SMInputs &sminputs)\n'\
      '{\n'\
      '\n'\
      '*kont = 0;\n'\
      '\n'\
      '// Contributions to alpha(m_Z), based on F. Jegerlehner, hep-ph/0310234 and Fanchiotti, Kniehl, Sirlin PRD 48 (1993) 307\n'\
      '*Delta_Alpha_Lepton = 0.04020;\n'\
      '*Delta_Alpha_Hadron = 0.027651;\n'\
      '\n'\
      '// Z-boson\n'\
      '*mZ = sminputs.mZ;          // mass\n'\
      '*gamZ = 2.4952;             // width, values henceforth from StandardModel.f90\n'\
      '(*BrZqq)(1) = 0.156;        // branching ratio in d \bar{d}\n'\
      '(*BrZqq)(2) = 0.156;        // branching ratio in s \bar{s}\n'\
      '(*BrZqq)(3) = 0.151;        // branching ratio in b \bar{b}\n'\
      '(*BrZqq)(4) = 0.116;        // branching ratio in u \bar{u}\n'\
      '(*BrZqq)(5) = 0.12;         // branching ratio in c \bar{c}\n'\
      '(*BrZll)(1) = 0.0336;       // branching ratio in e+ e-\n'\
      '(*BrZll)(2) = 0.0336;       // branching ratio in mu+ mu-\n'\
      '(*BrZll)(3) = 0.0338;       // branching ratio in tau+ tau-\n'\
      '*BrZinv = 0.2;              // invisible branching ratio\n'\
      '\n'\
      '*mZ2 = *mZ * *mZ;\n'\
      '*gamZ2 = *gamZ * *gamZ;\n'\
      '*gmZ = *gamZ * *mZ;\n'\
      '*gmZ2 = *gmZ * *gmZ;\n'\
      '\n'\
      '// W-boson\n'\
      '*mW = 80.385;\n'\
      '*gamW = 2.085;\n'\
      '(*BrWqq)(1) = 0.35;\n'\
      '(*BrWqq)(2) = 0.35;\n'\
      'for(int i=1; i<=3; i++)\n'\
      '(*BrWln)(i) = 0.1;\n'\
      '\n'\
      '*mW2 = pow(*mW, 2);\n'\
      '*gamW2 = pow(*gamW, 2);\n'\
      '*gmW = *gamW * *mW;\n'\
      '*gmW2 = pow(*gmW, 2);\n'\
      '\n'\
      '// lepton masses: e, muon, tau\n'\
      '(*mf_l)(1) = sminputs.mE;\n'\
      '(*mf_l)(2) = sminputs.mMu;\n'\
      '(*mf_l)(3) = sminputs.mTau;\n'\
      '\n'\
      '// default for neutrino masses\n'\
      '(*mf_nu)(1) = 0.0;\n'\
      '(*mf_nu)(2) = 0.0;\n'\
      '(*mf_nu)(3) = 0.0;\n'\
      '\n'\
      '// scale where masses of light quarks are defined [in GeV]\n'\
      '(*Q_light_quarks) = 2;\n'\
      '\n'\
      '// up-quark masses: u, c, t\n'\
      '(*mf_u)(1) = sminputs.mU;\n'\
      '(*mf_u)(2) = sminputs.mCmC;\n'\
      '(*mf_u)(3) = sminputs.mT;\n'\
      '\n'\
      '// down-quark masses: d, s, b\n'\
      '(*mf_d)(1) = sminputs.mD;\n'\
      '(*mf_d)(2) = sminputs.mS;\n'\
      '(*mf_d)(3) = sminputs.mBmB;\n'\
      '\n'\
      'for(int i=1; i<=3; i++)\n'\
      '{\n'\
      '(*mf_l2)(i) = pow((*mf_l)(i),2);\n'\
      '(*mf_u2)(i) = pow((*mf_u)(i),2);\n'\
      '(*mf_d2)(i) = pow((*mf_d)(i),2);\n'\
      '}\n'\
      '\n'\
      '// couplings: Alpha(Q=0), Alpha(mZ), Alpha_S(mZ), Fermi constant G_F\n'\
      '*Alpha =  1.0/137.035999074;\n'\
      '*Alpha_mZ = 1.0/sminputs.alphainv;\n'\
      '*Alpha_mZ_MS = *Alpha_mZ; // from SMINPUTS\n'\
      '*MZ_input = true;\n'\
      '*AlphaS_mZ = sminputs.alphaS;\n'\
      '*G_F = sminputs.GF;\n'\
      '\n'\
      '// for ISR correction in e+e- annihilation\n'\
      '*KFactorLee = 1.0 + (M_PI/3.0 - 1.0/(2*M_PI))*(*Alpha);\n'\
      '\n'\
      '// CKM matrix\n'\
      '*lam_wolf = sminputs.CKM.lambda;\n'\
      '*A_wolf = sminputs.CKM.A;\n'\
      '*rho_wolf = sminputs.CKM.rhobar;\n'\
      '*eta_wolf = sminputs.CKM.etabar;\n'\
      '\n'\
      '\n'\
      'float s12 = sminputs.CKM.lambda;\n'\
      'float s23 = pow(s12,2) * sminputs.CKM.A;\n'\
      'float s13 = s23 * sminputs.CKM.lambda * sqrt(pow(sminputs.CKM.etabar,2) + pow(sminputs.CKM.rhobar,2));\n'\
      'float phase = atan(sminputs.CKM.etabar/sminputs.CKM.rhobar);\n'\
      '\n'\
      'float c12 = sqrt(1.0 - s12*s12);\n'\
      'float c23 = sqrt(1.0 - s23*s23);\n'\
      'float c13 = sqrt(1.0 - s13*s13);\n'\
      '\n'\
      'std::complex<float> i = -1;\n'\
      'i = sqrt(i);\n'\
      '\n'\
      '(*CKM)(1,1) = c12 * c13;\n'\
      '(*CKM)(1,2) = s12 * c13;\n'\
      '(*CKM)(1,3) = s13 * exp(-i * phase);\n'\
      '(*CKM)(2,1) = -s12*c23 -c12*s23*s13 * exp(i * phase);\n'\
      '(*CKM)(2,2) = c12*c23 -s12*s23*s13 * exp(i * phase );\n'\
      '(*CKM)(2,3) = s23 * c13;\n'\
      '(*CKM)(3,1) = s12*s23 -c12*c23*s13 * exp(i * phase );\n'\
      '(*CKM)(3,2) = -c12*s23 - s12*c23*s13 * exp( i * phase );\n'\
      '(*CKM)(3,3) = c23 * c13;\n'\
      '\n'\
      'for(int i=1; i<=3; i++)\n'\
      '{\n'\
      '(*mf_l_mZ)(i) = 0.0;\n'\
      '(*mf_d_mZ)(i) = 0.0;\n'\
      '(*mf_u_mZ)(i) = 0.0;\n'\
      '}\n'\
      'try{ CalculateRunningMasses(*mf_l, *mf_d, *mf_u, *Q_light_quarks, *Alpha_mZ, *AlphaS_mZ, *mZ, *mf_l_mZ, *mf_d_mZ, *mf_u_mZ, *kont); }\n'\
      'catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n'\
      'if(*kont != 0)\n'\
      'ErrorHandling(*kont);\n'\
      '\n'\
      '}\n'\
      '\n'
    # end of InitializeStandardModel

    # ErrorHandling function
    towrite += "/ Function that handles errors\n"\
      'void ErrorHandling(const int &kont)\n'\
      '{\n'\
      '\n'\
      'str message;\n'\
      '\n'\
      'if (kont > 0 and kont <= 31)\n'\
      'message = (*Math_Error)(kont).str();\n'\
      'else if (kont > 100 and kont <= 102)\n'\
      'message = (*SM_Error)(kont-100).str();\n'\
      'else if (kont > 200 and kont <= 233)\n'\
      'message = (*SusyM_Error)(kont-200).str();\n'\
      'else if (kont > 300 and kont <= 315)\n'\
      'message = (*InOut_Error)(kont-300).str();\n'\
      'else if (kont > 400 and kont <= 422)\n'\
      'message = (*Sugra_Error)(kont-400).str();\n'\
      'else if (kont > 500 and kont <= 525)\n'\
      'message = (*LoopMass_Error)(kont-500).str();\n'\
      'else if (kont > 600 and kont <= 609)\n'\
      'message = (*TwoLoopHiggs_Error)(kont-600).str();\n'\
      'else if (kont > 1000 and kont <= 1010)\n'\
      'message = (*MathQP_Error)(kont-1000).str();\n'\
      'else\n'\
      'message = "GAMBIT caught an error in SPheno. Check the SPheno output for more info.";\n'\
      '\n'\
      'logger() << message << EOM;\n'\
      'invalid_point().raise(message);\n'\
      '\n'\
      'return ;\n'\
      '\n'\
      '}\n'\
      '\n'
    # end of ErrorHandling

    # Helper functions
    towrite += '//Helper functions\n'\
      'void Fdecays::fill_all_channel_info(str decays_file)\n'\
      '{\n'\
      'std::ifstream file(decays_file);\n'\
      'if(file.is_open())\n'\
      '{\n'\
      'str line;\n'\
      'int parent_pdg;\n'\
      'while(getline(file, line))\n'\
      '{\n'\
      'std::istringstream sline(line);\n'\
      'str first;\n'\
      'sline >> first;\n'\
      '// Ignore the line if it is a comment\n'\
      'if(first[0] != \'#\' and first != "")\n'\
      '{\n'\
      '// If the line starts with DECAY read up the pdg of the decaying particle\n'\
      'if(first == "DECAY")\n'\
      '{\n'\
      'sline >> parent_pdg;\n'\
      '}\n'\
      'else\n'\
      '{\n'\
      '// Read up the decay index, number of daughters, pdgs for the daughters and the correction factor\n'\
      'int index, nda, pdg;\n'\
      'double corrf;\n'\
      'std::vector<int> daughter_pdgs;\n'\
      'index = stoi(first);\n'\
      'sline >> nda;\n'\
      'for(int i=0; i<nda; i++)\n'\
      '{\n'\
      'sline >> pdg;\n'\
      'daughter_pdgs.push_back(pdg);  //< filling a vector of (PDG code, context int) pairs\n'\
      '}\n'\
      'sline >> corrf;\n'\
      '\n'\
      '// Now fill the map all_channel_info in the Fdecays namespace\n'\
      'if(BACKEND_DEBUG)\n'\
      'std::cout << "DEBUG: Filled channel: parent_pdg=" << parent_pdg << ", index=" << index << ", corrf=" << corrf << std::endl;\n'\
      'all_channel_info[parent_pdg].push_back(channel_info_triplet (daughter_pdgs, index, corrf));\n'\
      '}\n'\
      '}\n'\
      '}\n'\
      '\n'\
      'file.close();\n'\
      '}\n'\
      'else\n'\
      '{\n'\
      'str message = "Unable to open decays info file " + decays_file;\n'\
      'logger() << message << EOM;\n'\
      'backend_error().raise(LOCAL_INFO, message);\n'\
      '// invalid_point().raise(message);\n'\
      '}\n'\
      '}\n'\
      '\n'\
      'std::vector<std::pair<int,int> > Fdecays::get_pdg_context_pairs(std::vector<int> pdgs)\n'\
      '{\n'\
      'std::vector<std::pair<int,int> > result;\n'\
      'for(int pdg : pdgs)\n'\
      '{\n'\
      '// SM fermions in flavour basis, everything else in mass basis\n'\
      'if(abs(pdg) < 17)\n'\
      'result.push_back(std::pair<int,int> (pdg,1));\n'\
      'else\n'\
      'result.push_back(std::pair<int,int> (pdg,0));\n'\
      '}\n'\
      'return result;\n'\
      '}\n'\
      '\n'\
      "}\n"\
      "END_BE_NAMESPACE\n"\
      "\n"

    # Ini function
    towrite += "BE_INI_FUNCTION\n"\
      "{\n"\
      "\n"\
      "// Scan-level initialisation\n"\
      "static bool scan_level = true;\n"\
      "if (scan_level)\n"\
      "{\n"\
      "// Dump all internal output to stdout\n"\
      "*ErrCan = 6;\n"\
      "\n"\
      "// Set the function pointer in SPheno to our ErrorHandler callback function\n"\
      "*ErrorHandler_cptr = & CAT_4(BACKENDNAME,_,SAFE_VERSION,_ErrorHandler);\n"\
      "\n"\
      "try{ Set_All_Parameters_0(); }\n"\
      "catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n"\
      "\n"\
      "/****************/\n"\
      "/* Block MODSEL */\n"\
      "/****************/\n"\
      "if((*ModelInUse)(\"" + model_name + "\"))\n"\
      "{\n"\
      "  *HighScaleModel = \"LOW\";\n"\
      "  // BC where all parameters are taken at the low scale\n"\
      "  *BoundaryCondition = 3;\n"\
      "}\n"\
      "else\n"\
      "{\n"\
      "  str message = \"Model not recognised\";\n"\
      "  logger() << message << EOM;\n"\
      "  invalid_point().raise(message);\n"\
      "}\n"\
      " \n"\
      "// GAMBIT default behaviour\n"\
      "*GenerationMixing = true;\n"\
      "\n"\
      "}\n"\
      "scan_level = false;\n"\
      "\n"
    if flags["SupersymmetricModel"] :
      towrite += "*Qin = 1.0E3;  // Default value if there's no input\n"
    else :
      towrite += "*Qin = 1.0E6;  // Default value if there's no input\n"
    towrite += "Freal8 scale_sq = pow(*Qin, 2);\n"\
      "try{ SetRenormalizationScale(scale_sq); }\n"\
      "catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n"\
      "if(Param.find(\"Qin\") != Param.end())\n"\
      "{ \n"\
      "*Qin = *Param.at(\"Qin\");\n"\
      "scale_sq = pow(*Qin,2);\n"\
      "try{ SetRGEScale(scale_sq); } \n"\
      "catch(std::runtime_error e) { invalid_point().raise(e.what()); }\n"\
      "}\n"\
      "\n"\
      "// Reset the global flag that indicates whether or not BRs have been computed yet or not for this parameter point.\n"\
      "Fdecays::BRs_already_calculated = false;\n"\
      "\n"\
      "}\n"\
      "END_BE_INI_FUNCTION\n"\

      
    return indent(towrite)

def write_spheno_frontend_header(model_name, function_signatures, 
                                 type_dictionary, locations, 
                                 variables, var_dict, hb_variables, hb_dict, flags):
    """
    Writes code for 
    Backends/include/gambit/Backends/SARAHSPheno_<MODEL>_<VERSION>.hpp
    """

    clean_model_name = model_name.replace('-','')

    intro_message = "Frontend header for SARAH-SPheno {0} backend,\n" \
                    "/// for the {1} model.".format(SPHENO_VERSION, model_name)
                    
    towrite = blame_gum(intro_message)

    # Some nice model-independent functions to begin
    towrite += (
            "#define BACKENDNAME SARAHSPheno_{0}\n"
            "#define BACKENDLANG FORTRAN\n"
            "#define VERSION {1}\n"
            "#define SARAH_VERSION {2}\n"
            "#define SAFE_VERSION {3}\n"
            "\n"
            "// Begin\n"
            "LOAD_LIBRARY\n"
            "\n"
            "// Allow for {0} only\n"
            "BE_ALLOW_MODELS({0})\n"
            "\n"
            "\n"
            "// Functions\n"
            "BE_FUNCTION(Set_All_Parameters_0, void, (), \"__model_data_{4}_MOD_set_all_parameters_0\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_FUNCTION(SetRenormalizationScale, Freal8, (Freal8&), \"__loopfunctions_MOD_setrenormalizationscale\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_FUNCTION(InitializeLoopFunctions, void, (), \"__loopfunctions_MOD_initializeloopfunctions\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_FUNCTION(CalculateRunningMasses, void, (Farray_Freal8_1_3&, //mf_l_in\n"
            "                                           Farray_Freal8_1_3&, // mf_d_in\n"
            "                                           Farray_Freal8_1_3&, // mf_u_in\n"
            "                                           Freal8&, // Qlow\n"
            "                                           Freal8&, // Alpha\n"
            "                                           Freal8&, // AlphaS\n"
            "                                           Freal8&, // Qhigh\n"
            "                                           Farray_Freal8_1_3&, // mf_l_out\n"
            "                                           Farray_Freal8_1_3&, // mf_d_out\n"
            "                                           Farray_Freal8_1_3&, // mf_u_out\n"
            "                                           Finteger&), //kont))\n"
            "     \"__standardmodel_MOD_calculaterunningmasses\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_FUNCTION(GetRenormalizationScale, Freal8, (), \"__loopfunctions_MOD_getrenormalizationscale\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_FUNCTION(SetRGEScale, void, (Freal8&), \"__model_data_{4}_MOD_setrgescale\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_FUNCTION(SetGUTScale, void, (Freal8&), \"__model_data_{4}_MOD_setgutscale\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_FUNCTION(SetStrictUnification, Flogical, (Flogical&), \"__model_data_{4}_MOD_setstrictunification\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_FUNCTION(SetYukawaScheme, Finteger, (Finteger&), \"__model_data_{4}_MOD_setyukawascheme\", \"SARAHSPheno_{0}_internal\")\n"
    ).format(clean_model_name, SPHENO_VERSION, SARAH_VERSION, SPHENO_VERSION.replace('.','_'),
             clean_model_name.lower())

    # Some model-dependent functions:
    """
    Switch_to_superPMNS; Switch_to_superCKM;
    CalculateBR_2
    """

    towrite += "\n// Model-dependent arguments auto-scraped by GUM\n"
    
    # The functions that we have scraped from SPheno directly.
    for function, sig in function_signatures.iteritems():
        # And the locations where each function lives
        for k, v in locations.iteritems():
            # If they match, store the module name (usually the filename)
            # as this is going to be the name of the symbol in SPheno
            if function in v:
                loc = k.lower().split('/')[-1]
                symbol = "__{0}_MOD_{2}".format(loc, 
                         clean_model_name.lower(), function.lower())
        # Now list all arguments, with a nice comment next to it, to make it 
        # lovely and readable.
        arguments = []
        arguments.append(type_dictionary[sig[0]] + '&, // ' + sig[0])
        for argument in sig[1:-1]:
            arguments.append("   "+type_dictionary[argument]+"&, // "+argument)
        arguments.append("   "+type_dictionary[sig[-1]]+"& // "+sig[-1]+"\n")
        args = "\n".join(arguments)
        # Wrap it all up.
        towrite += (
            "BE_FUNCTION({0}, void,\n"
            "  ({1}),"
            " \"{2}\", \"SARAHSPheno_{3}_internal\")\n"
        ).format(function, args, symbol, clean_model_name)
    
    # All scraped from Model_Data_<MODEL>.f90
    # todo: check these are all present. I think they are.
    # MODEL VARIABLES
    # MASS + OUTPUT VARIABLES
    # MODEL VARIABLES
    # EXTPAR VARIABLES
    # MINPAR VARIABLES
    # SPHENOINPUT VARIABLES
    towrite += "\n// Model-dependent variables\n" 
    br_entry = "" # Branching ratio parameters can go later, with the rest.

    # Let's do this alphabetically so it looks nicer.
    for name, param in sorted(variables.iteritems()):

        # We'll put this in with SMINPUTS, otherwise it'll be a duplicate.
        if name == "MZ_input": continue
 
        # These go with decay info
        if name.startswith("Calc3BodyDecay_") or name.startswith("CalcLoopDecay_") : continue

        string = (
               "BE_VARIABLE({0}, {1}, \"__model_data_{2}_MOD_{3}\",\"SARAHSPheno_{4}_internal\")\n"
        ).format(name, var_dict[name], clean_model_name.lower(), name.lower(), clean_model_name)

        # Organise branching ratio stuff separately
        if name.startswith("gT") or name.startswith("BR"):
            br_entry += string
        else: 
            towrite += string

    # Add MODSEL variable if missing
    if "HighScaleModel" not in [name for name, param in variables.iteritems()] :
        towrite += 'BE_VARIABLE(HighScaleModel, Fstring<15>, "__settings_MOD_highscalemodel", "SARAHSPheno_' + clean_model_name + '_internal")\n'

    # SMINPUTS
    towrite += (
            "\n"
            "// SMINPUT Variables\n"
            "BE_VARIABLE(mZ, Freal8, \"__standardmodel_MOD_mz\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mZ2, Freal8,  \"__standardmodel_MOD_mz2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(gamZ, Freal8, \"__standardmodel_MOD_gamz\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(gamZ2, Freal8, \"__standardmodel_MOD_gamz2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(gmZ, Freal8, \"__standardmodel_MOD_gmz\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(gmZ2, Freal8, \"__standardmodel_MOD_gmz2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(BrZqq, Farray_Freal8_1_5, \"__standardmodel_MOD_brzqq\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(BrZll, Farray_Freal8_1_3, \"__standardmodel_MOD_brzll\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(BrZinv, Freal8, \"__standardmodel_MOD_brzinv\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mW, Freal8, \"__standardmodel_MOD_mw\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mW_SM, Freal8, \"__model_data_{0}_MOD_mw_sm\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mW2, Freal8, \"__standardmodel_MOD_mw2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(gamW, Freal8, \"__standardmodel_MOD_gamw\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(gamW2, Freal8, \"__standardmodel_MOD_gamw2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(gmW, Freal8, \"__standardmodel_MOD_gmw\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(gmW2, Freal8, \"__standardmodel_MOD_gmw2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(BrWqq, Farray_Freal8_1_2, \"__standardmodel_MOD_brwqq\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(BrWln, Farray_Freal8_1_3, \"__standardmodel_MOD_brwln\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_l, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_l\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_l_mZ, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_l_mz\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_nu, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_nu\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_u, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_u\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_u_mZ, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_u_mz\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_d, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_d\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_d_mZ, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_d_mz\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_l2, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_l2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_u2, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_u2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mf_d2, Farray_Freal8_1_3, \"__standardmodel_MOD_mf_d2\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(MNuR, Freal8, \"__model_data_MOD_mnur\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Q_light_quarks, Freal8, \"__standardmodel_MOD_q_light_quarks\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Delta_Alpha_Lepton, Freal8, \"__standardmodel_MOD_delta_alpha_lepton\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Delta_Alpha_Hadron, Freal8, \"__standardmodel_MOD_delta_alpha_hadron\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Alpha, Freal8, \"__standardmodel_MOD_alpha\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Alpha_mZ, Freal8, \"__standardmodel_MOD_alpha_mz\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Alpha_mZ_MS, Freal8, \"__standardmodel_MOD_alpha_mz_ms\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(MZ_input, Flogical, \"__model_data_{0}_MOD_mz_input\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(AlphaS_mZ, Freal8, \"__standardmodel_MOD_alphas_mz\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(G_F, Freal8, \"__standardmodel_MOD_g_f\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(KFactorLee, Freal8, \"__standardmodel_MOD_kfactorlee\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(CKM, Farray_Fcomplex16_1_3_1_3, \"__standardmodel_MOD_ckm\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(lam_wolf, Freal8, \"__standardmodel_MOD_lam_wolf\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(A_wolf, Freal8, \"__standardmodel_MOD_a_wolf\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(rho_wolf, Freal8, \"__standardmodel_MOD_rho_wolf\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(eta_wolf, Freal8, \"__standardmodel_MOD_eta_wolf\", \"SARAHSPheno_{0}_internal\")\n"
    ).format(clean_model_name)

    # CONTROL + "OTHER" VARIABLES
    towrite += (
            "\n"
            "// Control Variables\n"
            "BE_VARIABLE(Iname, Finteger, \"__control_MOD_iname\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(kont, Finteger, \"__spheno{1}_MOD_kont\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(WriteOut, Flogical, \"__control_MOD_writeout\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(epsI, Freal8, \"__spheno{1}_MOD_epsi\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(deltaM, Freal8, \"__spheno{1}_MOD_deltam\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(mGUT, Freal8, \"__spheno{1}_MOD_mgut\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(ErrCan, Finteger, \"__control_MOD_errcan\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(FoundIterativeSolution, Flogical, \"__settings_MOD_founditerativesolution\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Math_Error, Farray_Fstring60_1_31, \"__control_MOD_math_error\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(SM_Error, Farray_Fstring60_1_2, \"__control_MOD_sm_error\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(SusyM_Error, Farray_Fstring60_1_33, \"__control_MOD_susym_error\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(InOut_Error, Farray_Fstring60_1_15, \"__control_MOD_inout_error\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Sugra_Error, Farray_Fstring60_1_22, \"__control_MOD_sugra_error\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(LoopMass_Error, Farray_Fstring60_1_25, \"__control_MOD_loopmass_error\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(TwoLoopHiggs_Error, Farray_Fstring60_1_9, \"__control_MOD_twoloophiggs_error\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(MathQP_Error, Farray_Fstring60_1_10, \"__control_MOD_mathqp_error\", \"SARAHSPheno_{0}_internal\")\n"
            "\n"   
            "// Other variables\n"
            "BE_VARIABLE(Qin, Freal8, \"__spheno{1}_MOD_qin\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(ratioWoM, Freal8, \"__spheno{1}_MOD_ratiowom\",\"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(CalcTBD, Flogical, \"__spheno{1}_MOD_calctbd\",\"SARAHSPheno_{0}_internal\")\n"
    ).format(clean_model_name, clean_model_name.lower())

    # BRANCHING RATIOS
    towrite += (
            "\n"
            "// Branching Ratio variables\n"
            "BE_VARIABLE(L_BR, Flogical, \"__control_MOD_l_br\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Enable3BDecaysF, Flogical, \"__settings_MOD_enable3bdecaysf\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Enable3BDecaysS, Flogical, \"__settings_MOD_enable3bdecayss\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(RunningCouplingsDecays, Flogical, \"__settings_MOD_runningcouplingsdecays\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(MinWidth, Freal8, \"__settings_MOD_minwidth\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(OneLoopDecays, Flogical, \"__settings_MOD_oneloopdecays\", \"SARAHSPheno_{0}_internal\")\n"
    ).format(clean_model_name)

    towrite += br_entry

    # DECAY OPTIONS
    towrite += (
            "\n"
            "// Decay options\n"
            "BE_VARIABLE(divonly_save, Finteger, \"__settings_MOD_divonly_save\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(divergence_save, Freal8, \"__settings_MOD_divergence_save\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(SimplisticLoopDecays, Flogical, \"__settings_MOD_simplisticloopdecays\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(ShiftIRdiv, Flogical, \"__settings_MOD_shiftirdiv\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(DebugLoopDecays, Flogical, \"__settings_MOD_debugloopdecays\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(OnlyTreeLevelContributions, Flogical, \"__settings_MOD_onlytreelevelcontributions\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(ExternalZfactors, Flogical, \"__settings_MOD_externalzfactors\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(UseZeroRotationMatrices, Flogical, \"__settings_MOD_usezerorotationmatrices\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(UseP2Matrices, Flogical, \"__settings_MOD_usep2matrices\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(OSkinematics, Flogical, \"__settings_MOD_oskinematics\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(ewOSinDecays, Flogical, \"__settings_MOD_ewosindecays\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(yukOSinDecays, Flogical, \"__settings_MOD_yukosindecays\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(CTinLoopDecays, Flogical, \"__settings_MOD_ctinloopdecays\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(LoopInducedDecaysOS, Flogical, \"__settings_MOD_loopinduceddecaysos\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Mass_Regulator_PhotonGluon, Freal8, \"__settings_MOD_mass_regulator_photongluon\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Extra_Scale_LoopDecays, Flogical, \"__settings_MOD_extra_scale_loopdecays\", \"SARAHSPheno_{0}_internal\")\n"
            "BE_VARIABLE(Scale_LoopDecays, Freal8, \"__settings_MOD_scale_loopdecays\", \"SARAHSPheno_{0}_internal\")\n"
    ).format(clean_model_name)

    for name, param in sorted(variables.iteritems()):

        # Add Calc3BodyDecay_<particle> and CalcLoopDecay_<particle>
        if name.startswith("Calc3BodyDecay_") or name.startswith("CalcLoopDecay_") :

            towrite += (
                "BE_VARIABLE({0}, Flogical, \"__model_data_{1}_MOD_{2}\", \"SARAHSPheno_{3}_internal\")\n"
            ).format(name, clean_model_name.lower(), name.lower(), clean_model_name)
 
    if flags["SupersymmetricModel"] : 
        towrite += (
                "BE_VARIABLE(CalcSUSY3BodyDecays, Flogical, \"__model_data_{0}_MOD_calcsusy3bodydecays\", \"SARAHSPheno_{1}_internal\")\n"
        ).format(clean_model_name.lower(), clean_model_name)

    # HIGGSBOUNDS OUTPUT
    towrite += "\n// HiggsBounds variables\n" 

    # Let's do this alphabetically so it looks nicer.
    for name, param in sorted(hb_variables.iteritems()):

        towrite += (
               "BE_VARIABLE({0}, {1}, \"__model_data_{2}_MOD_{3}\",\"SARAHSPheno_{4}_internal\")\n"
        ).format(name, hb_dict[name], clean_model_name.lower(), name.lower(), clean_model_name)

    # Wrap it up.
    towrite += (
            "\n"
            "// Convenience functions (registration)\n"
            "BE_CONV_FUNCTION(run_SPheno, int, (Spectrum&, const Finputs&), \"{0}_spectrum\")\n"
            "BE_CONV_FUNCTION(run_SPheno_decays, int, (const Spectrum &, DecayTable &, const Finputs&), \"{0}_decays\")\n"
            "BE_CONV_FUNCTION(Spectrum_Out, Spectrum, (const std::map<str, safe_ptr<double> >&), \"SARAHSPheno_{0}_internal\")\n"
            "BE_CONV_FUNCTION(get_HiggsCouplingsTable, int, (const Spectrum&, HiggsCouplingsTable&, const Finputs&), \"{0}_HiggsCouplingsTable\")\n"
            "BE_CONV_FUNCTION(ReadingData, void, (const Finputs&), \"SARAHSPheno_{0}_internal\")\n"
            "BE_CONV_FUNCTION(ReadingData_decays, void, (const Finputs&), \"SARAHSPheno_{0}_internal\")\n"
            "BE_CONV_FUNCTION(InitializeStandardModel, void, (const SMInputs&), \"SARAHSPheno_{0}_internal\")\n"
            "BE_CONV_FUNCTION(ErrorHandling, void, (const int&), \"SARAHSPheno_{0}_internal\")\n"
            "\n"
            "// Initialisation functions (dependencies)\n"
            "\n"
            "// Function pointer variable for error handling\n"
            "BE_VARIABLE(ErrorHandler_cptr, type_fptr_SPhenoErrorHandler, \"__control_MOD_errorhandler_cptr\", \"SARAHSPheno_{0}_internal\")\n"
            "\n"
            "// End\n"
            "#include \"gambit/Backends/backend_undefs.hpp\"\n"
    ).format(clean_model_name)

    # DONE!

    # Debugging: just checking for duplications
    matches = re.findall(r'BE_VARIABLE\((.*?)\)', towrite)

    counts = []
    for match in matches:
        counts.append(match.split(',')[0])

    from collections import Counter
    c = Counter(counts)

    for k, v in c.iteritems():
        if v == 2:
            print "Duplication of ", k, "- it appeared", v, "times."

    return indent(towrite)