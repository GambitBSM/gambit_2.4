cd ../../../downloaded
tar -xvf *Rivet*
echo "diff -rupN Rivet-3.1.3/Makefile.in ../installed/rivet/3.1.3/Makefile.in" >> patch_rivet_3.1.3.dif
diff -rupN Rivet-3.1.3/Makefile.in ../installed/rivet/3.1.3/Makefile.in >> patch_rivet_3.1.3.dif
echo "diff -rupN Rivet-3.1.3/include/Rivet/Makefile.in ../installed/rivet/3.1.3/include/Rivet/Makefile.in" >> patch_rivet_3.1.3.dif
diff -rupN Rivet-3.1.3/include/Rivet/Makefile.in ../installed/rivet/3.1.3/include/Rivet/Makefile.in >> patch_rivet_3.1.3.dif
echo "diff -rupN Rivet-3.1.3/src/Core/Makefile.in ../installed/rivet/3.1.3/src/Core/Makefile.in" >> patch_rivet_3.1.3.dif
diff -rupN Rivet-3.1.3/src/Core/Makefile.in ../installed/rivet/3.1.3/src/Core/Makefile.in >> patch_rivet_3.1.3.dif
echo "diff -rupN Rivet-3.1.3/include/Rivet/AnalysisHandler.hh ../installed/rivet/3.1.3/include/Rivet/AnalysisHandler.hh" >> patch_rivet_3.1.3.dif
diff -rupN Rivet-3.1.3/include/Rivet/AnalysisHandler.hh ../installed/rivet/3.1.3/include/Rivet/AnalysisHandler.hh >> patch_rivet_3.1.3.dif
echo "diff -rupN Rivet-3.1.3/include/Rivet/Tools/RivetHepMC.hh ../installed/rivet/3.1.3/include/Rivet/Tools/RivetHepMC.hh" >> patch_rivet_3.1.3.dif
diff -rupN Rivet-3.1.3/include/Rivet/Tools/RivetHepMC.hh ../installed/rivet/3.1.3/include/Rivet/Tools/RivetHepMC.hh >> patch_rivet_3.1.3.dif
echo "diff -rupN Rivet-3.1.3/src/Core/AnalysisHandler.cc ../installed/rivet/3.1.3/src/Core/AnalysisHandler.c" >> patch_rivet_3.1.3.dif
diff -rupN Rivet-3.1.3/src/Core/AnalysisHandler.cc ../installed/rivet/3.1.3/src/Core/AnalysisHandler.cc >> patch_rivet_3.1.3.dif
echo "diff -rupN Rivet-3.1.3/analyses/Makefile.in ../installed/rivet/3.1.3/analyses/Makefile.in" >> patch_rivet_3.1.3.dif
diff -rupN Rivet-3.1.3/analyses/Makefile.in ../installed/rivet/3.1.3/analyses/Makefile.in >> patch_rivet_3.1.3.dif
mv patch_rivet_3.1.3.dif ../patches/rivet/3.1.3/
cd ../patches/rivet/3.1.3
