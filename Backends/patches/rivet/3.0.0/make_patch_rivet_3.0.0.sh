cd ../../../downloaded
tar -xvf *Rivet*
echo "diff -rupN Rivet-3.0.0/analyses/pluginLEP/DELPHI_2000_I524694.cc ../installed/rivet/3.0.0/analyses/pluginLEP/DELPHI_2000_I524694.cc" >> patch_rivet_3.0.0.dif
diff -rupN Rivet-3.0.0/analyses/pluginLEP/DELPHI_2000_I524694.cc ../installed/rivet/3.0.0/analyses/pluginLEP/DELPHI_2000_I524694.cc >> patch_rivet_3.0.0.dif
mv patch_rivet_3.0.0.dif ../patches/rivet/3.0.0/
cd ../patches/rivet/3.0.0
