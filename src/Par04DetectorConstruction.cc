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

// THESE LINES ARE ONLY FOR THE MANUAL READING OF THE GEOMETRY
/*
Par04DetectorConstruction::Par04DetectorConstruction()
  : G4VUserDetectorConstruction()
{
  fDetectorMessenger         = new Par04DetectorMessenger(this);
  G4NistManager* nistManager = G4NistManager::Instance();
  fAbsorberMaterial[0]       = nistManager->FindOrBuildMaterial("G4_PbWO4");
}
*/ 

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
  // COMMENT OUT THIS FILE IF YOU NEED TO 
  // CONSTRUCT THE DETECTOR MANUALLY
  /*
  //--------- Material definition ---------
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* air            = nistManager->FindOrBuildMaterial("G4_AIR");

  //--------- Derived dimensions ---------
  G4double full2Pi = 2. * CLHEP::pi * rad;
  G4double layerThickness =
    std::accumulate(fAbsorberThickness.begin(), fAbsorberThickness.end(), 0.);
  G4double detectorOuterRadius = fDetectorInnerRadius + fNbOfLayers * layerThickness;
  G4double worldSizeXY         = detectorOuterRadius * 4.;
  G4double worldSizeZ          = fDetectorLength * 2;
  // check number of materials: (1 = homogeneous calo, 2 = sampling calo)
  G4int nbOfMaterials = 0;
  for(const auto material : fAbsorberMaterial)
  {
    if(material != nullptr)
      nbOfMaterials++;
  }

  //--------- World ---------
  auto fSolidWorld  = new G4Box("World",                  // name
                                worldSizeXY / 2.,         // half-width in X
                                worldSizeXY / 2.,         // half-width in Y
                                worldSizeZ / 2.);         // half-width in Z
  auto fLogicWorld  = new G4LogicalVolume(fSolidWorld,    // solid
                                          air,            // material
                                          "World");       // name
  auto fPhysicWorld = new G4PVPlacement(0,                // no rotation
                                        G4ThreeVector(),  // at (0,0,0)
                                        fLogicWorld,      // logical volume
                                        "World",          // name
                                        0,                // mother volume
                                        false,            // not used
                                        999,              // copy number
                                        true);            // copy number
  fLogicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  //--------- Detector envelope ---------
  auto fSolidDetector = new G4Tubs("Detector",               // name
                                   fDetectorInnerRadius,     // inner radius
                                   detectorOuterRadius,      // outer radius
                                   fDetectorLength / 2.,     // half-width in Z
                                   0,                        // start angle
                                   full2Pi);                 // delta angle
  auto fLogicDetector = new G4LogicalVolume(fSolidDetector,  // solid
                                            air,             // material
                                            "Detector");     // name
  new G4PVPlacement(0,                                       // no rotation
                    G4ThreeVector(0, 0, 0),                  // detector centre at (0,0,0)
                    fLogicDetector,                          // logical volume
                    "Detector",                              // name
                    fLogicWorld,                             // mother volume
                    false,                                   // not used
                    99,                                      // copy number
                    true);                                   // check overlaps

  // Region for fast simulation
  auto detectorRegion = new G4Region("DetectorRegion");
  detectorRegion->AddRootLogicalVolume(fLogicDetector);

  //--------- Detector layers: material ---------
  std::array<G4VisAttributes, 2> attribs;
  attribs[0].SetColour(G4Colour(0, 0, 1, 0.1));
  attribs[0].SetForceSolid(true);
  attribs[1].SetColour(G4Colour(1, 0, 0, 0.1));
  attribs[1].SetForceSolid(true);
  /// useful variable
  G4double innerRadius = fDetectorInnerRadius;
  for(G4int iLayer = 0; iLayer < fNbOfLayers; iLayer++)
  {
    for(G4int iMaterial = 0; iMaterial < nbOfMaterials; iMaterial++)
    {
      auto fSolidLayer         = new G4Tubs("Layer",      // name
                                            innerRadius,  // inner radius
                                            innerRadius + fAbsorberThickness[iMaterial],  // outer radius
                                            fDetectorLength / 2.,  // half-width in Z
                                            0,                     // start angle
                                            full2Pi);              // delta angle
      G4LogicalVolume* logical = new G4LogicalVolume(fSolidLayer,  // solid
                                                     fAbsorberMaterial[iMaterial],  // material
                                                     "Layer");                      // name
      new G4PVPlacement(0,                                                          // no rotation
                        G4ThreeVector(),                     // place at centre of mother volume
                        logical,                             // logical volume
                        "Layer",                             // name
                        fLogicDetector,                      // mother volume
                        false,                               // not used
                        iLayer * nbOfMaterials + iMaterial,  // copy number
                        true);                               // check overlaps
      logical->SetVisAttributes(attribs[iMaterial]);
      innerRadius += fAbsorberThickness[iMaterial];
      if(fAbsorberSensitivity[iMaterial])
      {
        fLayerLogical.push_back(logical);
      }
    }
  }

  Print();
  return fPhysicWorld;
  */

  // THIS LINES ARE FOR READING THE DETECTOR FROM THE .gdml FILE
  G4VPhysicalVolume* fWorld = fParser.GetWorldVolume();
  return fWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::ConstructSDandField()
{
  /*
  Par04SensitiveDetector* caloSD =
    new Par04SensitiveDetector("sensitiveDetector", fMeshNbOfCells, fMeshSizeOfCells);
  G4SDManager::GetSDMpointer()->AddNewDetector(caloSD);
  for(const auto logical : fLayerLogical)
  {
    SetSensitiveDetector(logical, caloSD);
  }

  auto detectorRegion = G4RegionStore::GetInstance()->GetRegion("DetectorRegion");
  // Par04DefineMeshModel needs to be first model to call
  */

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
{
  /*
  G4cout << "\n------------------------------------------------------"
         << "\n--- Detector length:\t" << G4BestUnit(fDetectorLength, "Length")
         << "\n--- Detector inner radius:\t" << G4BestUnit(fDetectorInnerRadius, "Length")
         << "\n--- Number of layers:\t" << fNbOfLayers << G4endl << "\n--- 1st layer: \t"
         << G4BestUnit(fAbsorberThickness[0], "Length") << " of "
         << (fAbsorberSensitivity[0] ? "active " : "passive ") << fAbsorberMaterial[0]->GetName()
         << G4endl;
  if(fAbsorberMaterial[1] != nullptr)
    G4cout << "--- 2nd layer: \t" << G4BestUnit(fAbsorberThickness[1], "Length") << " of "
           << (fAbsorberSensitivity[1] ? "active " : "passive ") << fAbsorberMaterial[1]->GetName()
           << G4endl;
  G4cout << "-----------------------------------------------------" << G4endl;
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetAbsorberMaterial(const std::size_t aLayer, const G4String& aName)
{
  // search material by its name
  /*
  G4Material* material = G4NistManager::Instance()->FindOrBuildMaterial(aName);
  if(material)
    fAbsorberMaterial[aLayer] = material;
  else
    G4Exception("Par04DetectorConstruction::SetAbsorberMaterial()", "InvalidSetup", FatalException,
                ("Unknown material name: " + aName).c_str());
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetAbsorberThickness(const std::size_t aLayer, const G4double aThickness)
{
  /*
  if(aLayer < fAbsorberThickness.size())
    fAbsorberThickness[aLayer] = aThickness;
  else
    G4Exception("Par04DetectorConstruction::SetAbsorberThickness()", "InvalidSetup", FatalException,
                ("Requested layer " + std::to_string(aLayer) +
                 " is larger than number of available layers (" +
                 std::to_string(fAbsorberThickness.size()) + ").")
                  .c_str());
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetAbsorberSensitivity(const std::size_t aLayer, const G4bool aSensitivity)
{
  /*
  if(aLayer < fAbsorberSensitivity.size())
    fAbsorberSensitivity[aLayer] = aSensitivity;
  else
    G4Exception(
      "Par04DetectorConstruction::SetAbsorberSensitivity()", "InvalidSetup", FatalException,
      ("Requested layer " + std::to_string(aLayer) + " is larger than number of available layers (" +
       std::to_string(fAbsorberSensitivity.size()) + ").")
        .c_str());
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetInnerRadius(G4double aRadius) { fDetectorInnerRadius = aRadius; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04DetectorConstruction::SetLength(G4double aLength) { fDetectorLength = aLength; }
