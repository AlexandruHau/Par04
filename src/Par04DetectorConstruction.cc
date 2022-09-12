//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
#include "Par04DetectorConstruction.hh"
#include <G4Colour.hh>                     // for G4Colour
#include <G4Exception.hh>                  // for G4Exception
#include <G4ExceptionSeverity.hh>          // for FatalException
#include <G4SystemOfUnits.hh>              // for rad
#include <G4ThreeVector.hh>                // for G4ThreeVector
#include <G4VUserDetectorConstruction.hh>  // for G4VUserDetectorConstruction
#include <G4ios.hh>                        // for G4endl, G4cout
#include <algorithm>                       // for max
#include <numeric>                         // for accumulate
#include <ostream>                         // for operator<<, basic_ostream
#include <string>                          // for allocator, char_traits
#include "G4Box.hh"                        // for G4Box
#include "G4LogicalVolume.hh"              // for G4LogicalVolume
#include "G4Material.hh"                   // for G4Material
#include "G4NistManager.hh"                // for G4NistManager
#include "G4PVPlacement.hh"                // for G4PVPlacement
#include "G4Region.hh"                     // for G4Region
#include "G4RegionStore.hh"                // for G4RegionStore
#include "G4RunManager.hh"                 // for G4RunManager
#include "G4SDManager.hh"                  // for G4SDManager
#include "G4Tubs.hh"                       // for G4Tubs
#include "G4UnitsTable.hh"                 // for operator<<, G4BestUnit
#include "G4VisAttributes.hh"              // for G4VisAttributes
#include "Par04DefineMeshModel.hh"         // for Par04DefineMeshModel
#include "Par04DetectorMessenger.hh"       // for Par04DetectorMessenger
#include "Par04SensitiveDetector.hh"       // for Par04SensitiveDetector
class G4VPhysicalVolume;
#ifdef USE_INFERENCE
#include "Par04MLFastSimModel.hh"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

// THESE LINES ARE FOR THE GEOMETRY LOAD FROM THE .gdml FILES
Par04DetectorConstruction::Par04DetectorConstruction(const G4GDMLParser& parser)
  : G4VUserDetectorConstruction(), fParser(parser)
{
  fDetectorMessenger         = new Par04DetectorMessenger(this);
  G4NistManager* nistManager = G4NistManager::Instance(); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Par04DetectorConstruction::~Par04DetectorConstruction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* Par04DetectorConstruction::Construct()
{
  // THIS LINES ARE FOR READING THE DETECTOR FROM THE .gdml FILE
  G4VPhysicalVolume* fWorld = fParser.GetWorldVolume();
  return fWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::ConstructSDandField()
{
  // THESE LINES ARE FOR GEOMETRY READING FROM THE .gdml FILES
  Par04SensitiveDetector* caloSD = new Par04SensitiveDetector("Tracker");
  G4SDManager::GetSDMpointer()->AddNewDetector(caloSD);

  // Yield the logical volume of the world
  G4LogicalVolume* logVol = fParser.GetWorldVolume()->GetLogicalVolume();
  G4LogicalVolume* daughterVol = logVol->GetDaughter(0)->GetLogicalVolume();
  G4int noDaughters_0 = logVol->GetNoDaughters();
  daughterVol->SetSensitiveDetector(caloSD);

  // Use as iterator a physical volume element
  G4LogicalVolume* logVolElement_0;
  G4LogicalVolume* logVolElement_1;
  G4LogicalVolume* logVolElement_2;
  G4LogicalVolume* logVolElement_3;
  G4LogicalVolume* logVolElement_4;
  G4int i0; 
  G4int i1;
  G4int i2;
  G4int i3;
  G4int i4;
  
  // Iterate firstly through the 11 daughters (the outer staves)
  for(i0=0; i0<noDaughters_0; i0++){
 
    logVolElement_0 = logVol->GetDaughter(i0)->GetLogicalVolume();
    G4int noDaughters_1 = logVolElement_0->GetNoDaughters();
                                           
    // Add this daughter to the sensitive detector
    logVolElement_0->SetSensitiveDetector(caloSD);
    G4cout << "Name of logVol class 0: " << logVolElement_0->GetName() << G4endl;
  
    for(i1=0; i1<noDaughters_1; i1++){
                                                                 
      logVolElement_1 = logVolElement_0->GetDaughter(i1)->GetLogicalVolume();
      G4cout << "Name of logVol class 1: " << logVolElement_1->GetName() << G4endl;
      G4int noDaughters_2 = logVolElement_1->GetNoDaughters();
  
      // Add this daughter to the sensitive detector
      logVolElement_1->SetSensitiveDetector(caloSD);
  
      for(i2=0; i2<noDaughters_2; i2++){
                                                                                                           
        logVolElement_2 = logVolElement_1->GetDaughter(i2)->GetLogicalVolume();
        G4cout << "Name of logVol class 2: " << logVolElement_2->GetName() << G4endl;
        G4int noDaughters_3 = logVolElement_2->GetNoDaughters();

        // Add this daughter to the sensitive detector  
        logVolElement_2->SetSensitiveDetector(caloSD);

        for(i3=0; i3<noDaughters_3; i3++){                                                                                                                                                                                                                                                            logVolElement_3 = logVolElement_2->GetDaughter(i3)->GetLogicalVolume();
          G4cout << "Name of logVol class 3: " << logVolElement_3->GetName() << G4endl;
          G4int noDaughters_4 = logVolElement_3->GetNoDaughters();
     
          // Add this daughter as well to the sensitive 
          logVolElement_3->SetSensitiveDetector(caloSD);
          for(i4=0; i4<noDaughters_4; i4++){
         
            logVolElement_4 = logVolElement_3->GetDaughter(i4)->GetLogicalVolume();
            G4cout << "Name of logVol class 4: " << logVolElement_4->GetName() << G4endl;
            logVolElement_4->SetSensitiveDetector(caloSD);
          }
        } 
      }  
    }
  }


  // Set up the detector region for inference as well
  G4Region* detectorRegion = new G4Region("Inference Region");
  detectorRegion->AddRootLogicalVolume(daughterVol);

  new Par04DefineMeshModel("defineMesh", detectorRegion);
#ifdef USE_INFERENCE
  new Par04MLFastSimModel("inferenceModel", detectorRegion);
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::Print() const
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetAbsorberMaterial(const std::size_t aLayer, const G4String& aName)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetAbsorberThickness(const std::size_t aLayer, const G4double aThickness)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetAbsorberSensitivity(const std::size_t aLayer, const G4bool aSensitivity)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetInnerRadius(G4double aRadius) { fDetectorInnerRadius = aRadius; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetLength(G4double aLength) { fDetectorLength = aLength; }
