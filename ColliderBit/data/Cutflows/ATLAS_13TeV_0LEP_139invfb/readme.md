

# Install relevant programs

## Stand-alone Pythia for 

Download HepMC-2.06.09, and then do
    
    ./configure --with-momentum=GEV --with-length=MM
    make -j4
    sudo make install

Download pythia8244, and then do
    
    ./configure --with-hepmc2
    make -j4
    cd examples/
    make main89
   
## MadAnalysis
   

Error in cling::AutoloadingVisitor::InsertIntoAutoloadingState:
   Missing FileEntry for ExRootAnalysis/ExRootTreeReader.h
   requested to autoload type ExRootTreeReader
   
This can be solved by run

    export ROOT_INCLUDE_PATH=<Your_path_to_madanalysis5>/tools/delphes/external

before run "./bin/ma5 -R" in command line.

The codes of analysis are in 
    madanalysis5/tools/PAD/Build/SampleAnalyzer/User/Analyzer




# Generate events
 
## Generate LO events

OMP_NUM_THREADS=1 ./gambit -f ATLAS_0lep_139invfb_table_16_gen_events.yaml
    
    
## Generate events with up to 2 extra jets

Generate events using MG5_aMC_v2_7_2 by reference to the SquarkPairDirect/run_01_tag_1_banner.txt . Note that in run_card.dat, the follwoing setting are modified:
    
    0 = ickkw           ! 0 no matching, 1 MLM
    0.0   = xqcut       ! minimum kt jet measure between partons
    300.0  =  ktdurham  ! merging scale value (squark/gluino mass divided by 4 )
 
 
Merging using the CWKKL algorithm by

    ./main89 pythia8_card.cmnd pythia.hepmc

In pythia8_card.cmnd, the "Beams:LHEF", "Merging:TMS", and "Merging:Process" are different for different processes.



# Obtain cut-flows

## Obtain cut-flows from GAMBIT

    cd GAMBIT
    OMP_NUM_THREADS=1 ./CBS CBS_yaml/ATLAS_0lep_139invfb_table_solo_Verif.yaml

## Obtain cut-flows from CheckMATE2

    cd CheckMATE-2.0.26/bin
    ./CheckMATE checkmate_analysis.dat

Note that in "checkmate_analysis.dat", the path of HEP2 events file need to be fixed, and the suffix must be ".hep".

    The cut-flows are saved in 


## Obtain cut-flows from MadAnylsis

    cd madanalysis5
    ./bin/ma5 -R
    
    ma5> set main.recast = on
    ma5> set main.recast.store_root = False
    ma5> import /home/yzha0133/work/gambit/gambit/GAMBIT_collider_events.omp_thread_0.hepmc
    ma5> submit

The suffix of HEP2 events file must be ".hepmc" or ".hepmc2".

When it asks 
    MA5: Would you like to edit the recasting Card ? (Y/N)
answer 
    Y
If it is opend with vim, set all the anaylis to "off" by
    %s/on  /off /
and then turn on the analyese you want to use.
    

    
    
# Close smearing functions

## Close GAMBIT smearing functions

## Close Delphes smearing functions

    rm test.root && ./DelphesHepMC delphes_card_atlas_exot_2015_03.tcl test.root GAMBIT_collider_events.omp_thread_0.hepmc

    rm test.root && ./DelphesHepMC CheckMATE_ATLAS_13TeV.tcl test.root GAMBIT_collider_events.omp_thread_0.hepmc

    root -l examples/Example1.C'("test.root")'





