# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  CMake configuration script for standalone
#  programs that use some GAMBIT libraries.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2016
#
#  \author Tomas Gonzalo
#          (tomas.gonzalo@monash.edu)
#  \date 2020
#
#  \author Anders Kvellestad
#          (anders.kvellestad@fys.uio.no)
#  \date 2023
#
#************************************************

# Add some programs that use the GAMBIT physics libraries but not GAMBIT itself.
add_standalone(ExampleBit_A_standalone SOURCES ExampleBit_A/examples/ExampleBit_A_standalone_example.cpp MODULES ExampleBit_A)
add_standalone(CBS SOURCES ColliderBit/examples/solo.cpp MODULES ColliderBit DEPENDENCIES hepmc BACKENDS nulike_1.0.9 ATLAS_FullLikes_1.0 rivet_3.1.5 contur_2.1.1)
add_standalone(DarkBit_standalone_MSSM SOURCES DarkBit/examples/DarkBit_standalone_MSSM.cpp MODULES DarkBit BACKENDS nulike_1.0.9 gamlike_1.0.1 ddcalc_2.3.0 darksusy_5.1.3 darksusy_MSSM_6.1.1 darksusy_MSSM_6.2.5 darksusy_MSSM_6.4.0 micromegas_MSSM_3.6.9.2)
add_standalone(DarkBit_standalone_ScalarSingletDM_Z2 SOURCES DarkBit/examples/DarkBit_standalone_ScalarSingletDM_Z2.cpp MODULES DarkBit BACKENDS nulike_1.0.9 gamlike_1_0_1 micromegas_ScalarSingletDM_Z2_3.6.9.2 darksusy_generic_wimp_6.4.0 ddcalc_2.3.0)
add_standalone(DarkBit_standalone_WIMP SOURCES DarkBit/examples/DarkBit_standalone_WIMP.cpp MODULES DarkBit BACKENDS ddcalc_2.3.0 gamlike_1_0_1 darksusy_generic_wimp_6.4.0 micromegas_MSSM_3.6.9.2 pbarlike_1.0)
add_standalone(3bithit SOURCES DecayBit/examples/3bithit.cpp MODULES DecayBit SpecBit PrecisionBit BACKENDS spheno_4.0.3 susyhit_1.5 feynhiggs_2.11.3 )
add_standalone(FlavBit_standalone SOURCES FlavBit/examples/FlavBit_standalone_example.cpp MODULES FlavBit BACKENDS feynhiggs_2.11.3 superiso_4.1)
