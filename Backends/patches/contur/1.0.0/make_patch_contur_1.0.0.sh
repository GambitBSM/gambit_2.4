cd ../../../downloaded
unzip -f 8407e09eb161.zip
echo "diff -rupN heprivet-contur-8407e09eb161/ ../installed/contur/1.0.0/" >> patch_contur_1.0.0.dif
diff -rupN heprivet-contur-8407e09eb161/ ../installed/contur/1.0.0/ >> patch_contur_1.0.0.dif
mv patch_contur_1.0.0.dif ../patches/contur/1.0.0
cd ../patches/contur/1.0.0
