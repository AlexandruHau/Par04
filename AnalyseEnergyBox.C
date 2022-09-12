#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TH3.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TRandom3.h"
#include "TMath.h"

#include <iostream> 

using namespace std;
using namespace TMath;

void AnalyseEnergyBox() {
  
  
  // INPUT
  // Declaration of leaf types
  cout << "Declaration of variables: " << endl;
  Double_t FullSims_energy;
  Double_t Infered_energy;
  Double_t x_coordinate;
  Double_t y_coordinate;
  Double_t z_coordinate;
  
  cout << "Declaration of branch: " << endl;
  TBranch* E_abs;

  cout << "Input file: " << endl;
  // TFile *inputFile = new TFile("FullSims.root", "read");
  TFile *inputFile = new TFile("ONNXInference.root", "read");

  ofstream myFile;
  // myFile.open("FullSims.txt");
  // myFile.open("Infered_energy.txt");
  myFile.open("FastSims.txt");

  cout << "Get directory from the ntuple and declare the tree: " << endl;
  TDirectory * dir = (TDirectory*)inputFile->Get("ONNXInference.root:/ntuple");
  // TDirectory *dir = (TDirectory*)inputFile->Get("FullSims.root:/ntuple");
  TTree *inputTree;

  cout << "Fill directory: " << endl;
  // dir->GetObject("FullSims energy", inputTree);
  dir->GetObject("Infered energy", inputTree);

  cout << "Set the branch address: " << endl;
  // inputTree->SetBranchAddress("FullSims_energy", &FullSims_energy, &E_abs);
  inputTree->SetBranchAddress("Infered_energy", &Infered_energy, &E_abs);
  inputTree->SetBranchAddress("x_coordinate", &x_coordinate, &E_abs);
  inputTree->SetBranchAddress("y_coordinate", &y_coordinate, &E_abs);
  inputTree->SetBranchAddress("z_coordinate", &z_coordinate, &E_abs);

  cout << "Number of entries: " << endl;
  Long64_t nentries = inputTree->GetEntriesFast();

  // Create the 3D histogram
  TH1D * hits = new TH1D("hits", "z-coordinate; z-coord (mm); counts", 
			     200, -1, 1); 
  TNtuple *ntuple = new TNtuple("ntuple","Infered Energy","x_coordinate:y_coordinate:z_coordinate:Infered_energy");
  cout << "Iterate through the entries: " << endl;
  for(Long64_t entry = 0; entry < nentries; entry++){
    
    inputTree->GetEntry(entry);
    // cout << "FullSims energy: " << endl;
    // cout << FullSims_energy << endl;
    cout << "Entry: " << entry << endl;    
    cout << "Infered energy: " << endl;
    cout << Infered_energy << endl;
    cout << "x coordinate: " << endl;
    cout << x_coordinate << endl;
    cout << "y coordinate: " << endl;
    cout << y_coordinate << endl;
    cout << "z coordinate: " << endl;
    cout << z_coordinate << endl;
    // myFile << FullSims_energy << " " << x_coordinate << " " << y_coordinate << " "<< z_coordinate <<  "\n";
    myFile << Infered_energy << " " << x_coordinate << " " << y_coordinate << " "<< z_coordinate <<  "\n";

    // if(Infered_energy > 0)
    //   hits->Fill(z_coordinate);
    // dir->Draw("x_coordinate:y_coordinate:z_coordinate:Infered_energy", "", "COLZ");
  }
  // hits->SetLineColor(kBlue);
  // hits->Draw();
  // ntuple->Draw();  
}
