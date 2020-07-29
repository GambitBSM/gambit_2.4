/*

Imitate MadAnalyses to apply atlas_conf_2019+_040

root -l ATLAS_0lep_139invfb.C'("GAMBIT.root")' 
*/

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

//------------------------------------------------------------------------------

void ATLAS_0lep_139invfb(const char *inputFile)
{
  gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();

  ofstream outputFile;
  
  // Get pointers to branches used in this analysis
//  TClonesArray *branchJet = treeReader->UseBranch("GenJet");
//  TClonesArray *branchElectron = treeReader->UseBranch("Electron");
//  TClonesArray *branchMuon = treeReader->UseBranch("Muon");
//  TClonesArray *branchMissingET = treeReader->UseBranch("GenMissingET");
//  outputFile.open("Delphes_0j_139_nosmearing.txt");
  
  TClonesArray *branchJet = treeReader->UseBranch("Jet");
  TClonesArray *branchElectron = treeReader->UseBranch("Electron");
  TClonesArray *branchMuon = treeReader->UseBranch("Muon");
  TClonesArray *branchMissingET = treeReader->UseBranch("MissingET");
  outputFile.open("Delphes_0j_139_smeared.txt");
  
  int total = 0;
  int Preselection = 0;
  int njetsGt2 = 0;
  int DPhi123 = 0;
  int DPhi456 = 0;


  // Loop over all events
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    total++;
    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);
    
    MissingET *met;
    met = (MissingET*) branchMissingET->At(0);
    
//    cout << "============================"<< endl;
//    cout << "MET: PT= "<< met->MET << ",\t Phi=" << met->Phi << endl;
////    
//    for(size_t i=0; i<branchJet->GetEntries(); i++){
//      Jet *jet = (Jet*) branchJet->At(i);
//      cout << "Jet["<< i << "]: PT="<< jet->PT << ",\t Eta="<< jet->Eta << ",\t Phi="<< jet->Phi <<endl;
//    }
    
    
//    for(size_t i=0; i<branchElectron->GetEntries(); i++){
//      Electron *e = (Electron*) branchElectron->At(i);
//      cout << "e["<< i << "]: PT="<< e->PT << ",\t Eta="<< e->Eta << ",\t Phi="<< e->Phi <<endl;
//    }
    
    
    vector<const Electron*> electrons;
    vector<const Muon*> muons;
    vector<const Jet*>    signalJets ; // jets

    // Selecting Jets
    double jetpt  = 50;
    double jeteta = 2.8 ;

    for(unsigned int i=0; i<branchJet->GetEntries(); i++) {
        Jet *thisJet = (Jet*) branchJet->At(i);  
        double pT = thisJet->PT , eta =  thisJet->Eta ;
        if (pT > jetpt and fabs(eta) < jeteta )
          { signalJets.push_back(thisJet); }
    }

    // Selecting leptons (veto on leptons, with different eta requirements for muons and electrons) 
    double muonpt      = 6.0;
    double muoneta     = 2.8;
    double electronpt  = 7.0;
    double electroneta = 2.47 ;

    //  Muons
    for(unsigned int i=0; i<branchMuon->GetEntries(); i++)
    { const Muon *thisMuon = (Muon*) branchMuon->At(i);
      double pt = thisMuon->PT , eta =  thisMuon->Eta ;
      if ( pt > muonpt  and fabs(eta) < muoneta) {
      muons.push_back(thisMuon); }
     }

    //  Electrons
    for(unsigned int i=0; i<branchElectron->GetEntries(); i++)
    { const Electron *thisElectron = (Electron*) branchElectron->At(i);
      double pt = thisElectron->PT , eta =  thisElectron->Eta ;
      if (pt> electronpt  and fabs(eta) < electroneta ) {
      electrons.push_back(thisElectron); }
     }
     
    // Overlap removal is NOT applied

    // Definition of HT
    double HT = 0;
    for(unsigned int j = 0; j < signalJets.size(); j++)
      { HT += signalJets[j]->PT; }

    // Definition of MET
    double MET = met->MET;

    // Definition of Meff
    double Meff = MET + HT ;
    
    // MET over sqrt(HT)
    double MET_HT = MET/sqrt(HT);

    bool j1_pt=false;
    bool j2_pt=false;
    if (signalJets.size()>0) 
	{ j1_pt = signalJets[0]->PT>200.0;
          }
    if (signalJets.size()>1)
	{ j2_pt = signalJets[1]->PT>50.0; }

    // b. lepton veto
    bool leptonVeto = ((electrons.size()+muons.size())==0);

    // c. Meff > 800 GeV
    bool Meff_pre   = (Meff > 800.0);
    // d. MET > 300 GeV
    bool MET_pre    = (MET > 300.0);


//    cout << "============================"<< endl;
//    cout << "Meff = "<< Meff << ",\t leptonVeto=" << leptonVeto << endl;

//    if (not j1_pt)
//        cout << total << ",\t j1_pt=" << signalJets[0]->PT << ",\t j2_pt=" << signalJets[1]->PT  << endl;


//    for(size_t i=0; i<branchJet->GetEntries(); i++){
//      Jet *jet = (Jet*) branchJet->At(i);
//      cout << "Jet["<< i << "]: PT="<< jet->PT << ",\t Eta="<< jet->Eta << ",\t Phi="<< jet->Phi <<endl;
//    }

        outputFile << MET << "\t" << MET_HT << "\t" << Meff << "\t" << sqrt(HT) << "\t";
        
        size_t i;
        for(i=0; i<branchJet->GetEntries(); i++){
            Jet *jet = (Jet*) branchJet->At(i);
            if (i<2){
                outputFile << jet->PT << "\t";
            }
        }
        if (i==0) {
            outputFile << 0. << "\t" << 0.;
        } else if (i==1){
            outputFile <<0.;
        }
        outputFile <<endl;


    // Preselection
    if (j1_pt && j2_pt && Meff_pre && leptonVeto && MET_pre){
        Preselection++;
    } else {
        continue;
    }

    if (signalJets.size()>=2){
        njetsGt2++;
    } else {
        continue;
    }

    bool Dphi_3jets_MET_08 = true;
    if (signalJets.size()==2) { 
        Dphi_3jets_MET_08 = ( acos(cos(signalJets[0]->Phi - met->Phi)) > 0.8 
                           && acos(cos(signalJets[1]->Phi - met->Phi)) > 0.8  ); 
    }
    if (signalJets.size()>=3) { 
        Dphi_3jets_MET_08 = ( acos(cos(signalJets[0]->Phi - met->Phi)) > 0.8 
                           && acos(cos(signalJets[1]->Phi - met->Phi)) > 0.8 
                           && acos(cos(signalJets[2]->Phi - met->Phi)) > 0.8); 
    }

    if (Dphi_3jets_MET_08) {
        DPhi123++;
    } else {
        continue;
    }


    // DPhi  of the rest of the jets (from the 4th on)
    bool DPhiRest04 = true;

    for(unsigned int j = 3; j < signalJets.size(); j++) {
	  if ( acos(cos(signalJets[j]->Phi - met->Phi)) < 0.4) 
            DPhiRest04 = false; 
    }

    if (DPhiRest04) {
        DPhi456++;
    } else {
        continue;
    }


  }

  cout << total <<endl;
  cout << Preselection <<endl;
  cout << njetsGt2 <<endl;
  cout << DPhi123 <<endl;
  cout << DPhi456 <<endl;

}

