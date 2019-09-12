cd ../../../downloaded
tar -xvf *Rivet*
echo "diff -rupN Rivet-3.0.1/Makefile.in ../installed/rivet/3.0.1/Makefile.in" >> patch_rivet_3.0.1.dif
diff -rupN Rivet-3.0.1/Makefile.in ../installed/rivet/3.0.1/Makefile.in >> patch_rivet_3.0.1.dif
echo "diff -rupN Rivet-3.0.1/include/Rivet/Makefile.in ../installed/rivet/3.0.1/include/Rivet/Makefile.in" >> patch_rivet_3.0.1.dif
diff -rupN Rivet-3.0.1/include/Rivet/Makefile.in ../installed/rivet/3.0.1/include/Rivet/Makefile.in >> patch_rivet_3.0.1.dif
echo "diff -rupN Rivet-3.0.1/src/Core/Makefile.in ../installed/rivet/3.0.1/src/Core/Makefile.in" >> patch_rivet_3.0.1.dif
diff -rupN Rivet-3.0.1/src/Core/Makefile.in ../installed/rivet/3.0.1/src/Core/Makefile.in >> patch_rivet_3.0.1.dif
echo "diff -rupN Rivet-3.0.1/include/Rivet/AnalysisHandler.hh ../installed/rivet/3.0.1/include/Rivet/AnalysisHandler.hh" >> patch_rivet_3.0.1.dif
diff -rupN Rivet-3.0.1/include/Rivet/AnalysisHandler.hh ../installed/rivet/3.0.1/include/Rivet/AnalysisHandler.hh >> patch_rivet_3.0.1.dif
echo "diff -rupN Rivet-3.0.1/include/Rivet/Tools/RivetHepMC.hh ../installed/rivet/3.0.1/include/Rivet/Tools/RivetHepMC.hh" >> patch_rivet_3.0.1.dif
diff -rupN Rivet-3.0.1/include/Rivet/Tools/RivetHepMC.hh ../installed/rivet/3.0.1/include/Rivet/Tools/RivetHepMC.hh >> patch_rivet_3.0.1.dif
mv patch_rivet_3.0.1.dif ../patches/rivet/3.0.1/
cd ../patches/rivet/3.0.1
