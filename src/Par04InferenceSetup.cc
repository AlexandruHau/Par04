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
#ifdef USE_INFERENCE
#include "Par04InferenceSetup.hh"
#include "Par04InferenceInterface.hh"   // for Par04InferenceInterface
#include "Par04InferenceMessenger.hh"   // for Par04InferenceMessenger
#ifdef USE_INFERENCE_ONNX
#include "Par04OnnxInference.hh"        // for Par04OnnxInference
#endif
#ifdef USE_INFERENCE_LWTNN
#include "Par04LwtnnInference.hh"       // for Par04LwtnnInference
#endif
#include <CLHEP/Units/SystemOfUnits.h>  // for pi, GeV, deg
#include <CLHEP/Vector/Rotation.h>      // for HepRotation
#include <CLHEP/Vector/ThreeVector.h>   // for Hep3Vector
#include <G4Exception.hh>               // for G4Exception
#include <G4ExceptionSeverity.hh>       // for FatalException
#include <G4ThreeVector.hh>             // for G4ThreeVector
#include "CLHEP/Random/RandGauss.h"     // for RandGauss
#include "G4RotationMatrix.hh"          // for G4RotationMatrix
#include <algorithm>                    // for max, copy
#include <string>                       // for char_traits, basic_string
#include <ext/alloc_traits.h>           // for __alloc_traits<>::value_type
#include <cmath>                        // for cos, sin
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Par04InferenceSetup::Par04InferenceSetup()
  : fInferenceMessenger(new Par04InferenceMessenger(this))
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Par04InferenceSetup::~Par04InferenceSetup() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool Par04InferenceSetup::IfTrigger(G4double aEnergy)
{
  /// Energy of electrons used in training dataset
  if(aEnergy > 1 * CLHEP::GeV || aEnergy < 1024 * CLHEP::GeV)
    return true;
  return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04InferenceSetup::SetInferenceLibrary(G4String aName)
{
  fInferenceLibrary = aName;

#ifdef USE_INFERENCE_ONNX
  if(fInferenceLibrary == "ONNX")
    fInferenceInterface = std::unique_ptr<Par04InferenceInterface>(
      new Par04OnnxInference(fModelPathName, fProfileFlag, fOptimizationFlag, fIntraOpNumThreads));
#endif
#ifdef USE_INFERENCE_LWTNN
  if(fInferenceLibrary == "LWTNN")
    fInferenceInterface =
      std::unique_ptr<Par04InferenceInterface>(new Par04LwtnnInference(fModelPathName));
#endif
  CheckInferenceLibrary();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04InferenceSetup::CheckInferenceLibrary()
{
  G4String msg = "Please choose inference library from available libraries (";
#ifdef USE_INFERENCE_ONNX
  msg += "ONNX,";
#endif
#ifdef USE_INFERENCE_LWTNN
  msg += "LWTNN";
#endif
  if(fInferenceInterface == nullptr)
    G4Exception("Par04InferenceSetup::CheckInferenceLibrary()", "InvalidSetup", FatalException,
                (msg + "). Current name: " + fInferenceLibrary).c_str());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04InferenceSetup::GetEnergies(std::vector<G4double>& aEnergies, G4double aInitialEnergy,
                                      G4float aInitialAngle)
{
  // First check if inference library was set correctly
  CheckInferenceLibrary();
  // size represents the size of the output vector
  int size = fMeshNumber.x() * fMeshNumber.y() * fMeshNumber.z();

  // randomly sample from a gaussian distribution in the latent space
  std::vector<G4float> genVector(fSizeLatentVector + fSizeConditionVector, 0);
  for(int i = fSizeConditionVector; i < (fSizeConditionVector + fSizeLatentVector); ++i)
  {
    genVector[i] = CLHEP::RandGauss::shoot(0., 1.);
  }

  // Vector of condition
  // this is application specific it depdens on what the model was condition on
  // and it depends on how the condition values were encoded at the training time
  // in this example the energy of each particle is normlaized to the highest
  // energy in the considered range (1GeV-500GeV)
  // the angle is also is normlaized to the highest angle in the considered range
  // (0-90 in dergrees)
  genVector[0] = aInitialEnergy / (f3DGANConstant * fG4rescale);
  G4cout << "Incident energy: " << G4BestUnit(aInitialEnergy, "Energy") << " " << aInitialEnergy << G4endl;
  G4cout << "First input element: " << genVector[0] << G4endl; 
  // 2. angle
  genVector[1] = (aInitialAngle / (CLHEP::deg));
  G4cout << "Incident angle: " << aInitialAngle << "in the units of: " << G4BestUnit(aInitialAngle, "Angle") <<  G4endl;
  // Convert the angle to radians
  genVector[1] = genVector[1] * CLHEP::pi / 180;
  // 3.geometry
  /*
  genVector[fSizeLatentVector + 2] = 0;
  genVector[fSizeLatentVector + 3] = 1;
  */

  // Run the inference
  fInferenceInterface->RunInference(genVector, aEnergies, size);

  // After the inference rescale back to the initial energy (in this example the
  // energies of cells were normalized to the energy of the particle)
  for(int i = 0; i < size; ++i)
  {
    aEnergies[i] = std::pow(aEnergies[i], fPowerStable);
    aEnergies[i] = aEnergies[i] / fDivideStable;
    // G4cout << "Inferenced energy: " << G4BestUnit(aEnergies[i], "Energy") << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04InferenceSetup::GetPositions(std::vector<G4ThreeVector>& aPositions, G4ThreeVector pos0,
                                       G4ThreeVector direction, std::vector<G4double>& aEnergies,
				       G4float theta, G4float phi)
{
  aPositions.resize(fMeshNumber.x() * fMeshNumber.y() * fMeshNumber.z());
  
  // Print out the pos0 vector
  G4cout << "Pos 0 - x: " << G4BestUnit(pos0.getX(), "Length") << G4endl;
  G4cout << "Pos 0 - y: " << G4BestUnit(pos0.getY(), "Length") << G4endl;
  G4cout << "Pos 0 - z: " << G4BestUnit(pos0.getZ(), "Length") << G4endl;   
  // Iterate through the first layer of the cube and work out
  // the barycentre for the plane corresponding to z=0. Also define
  // som variables for the barycentre coordinates for the z=0 plane
  G4double bary_x = 0;
  G4double bary_y = 0;

  // Now define variable for the energy sum in the cube
  G4double E = 0;
  G4int cpt = 0;

  for (G4int x = 0; x < 51; x++){
    for(G4int y = 0; y < 51; y++){
      for(G4int z = 0; z < 25; z++){

        if(z == 0){
          bary_x += aEnergies[cpt] * x;
          bary_y += aEnergies[cpt] * y;
          E += aEnergies[cpt];

          if(aEnergies[cpt] > 0)
            G4cout << "Energy deposit for z=0: " << G4BestUnit(aEnergies[cpt], "Energy") << G4endl;
        }
        cpt++;
      }
    }
  }
  
  bary_x = bary_x / E;
  bary_y = bary_y / E;

  G4cout << "Coordinate x of 0-plane barycentre: " << G4BestUnit(bary_x, "Length") << " in units: " << bary_x << " and cell: "
	 << std::floor(bary_x) << G4endl;
  G4cout << "Coordinate y of 0-plane barycentre: " << G4BestUnit(bary_y, "Length") << " in units: " << bary_y << " and cell: "
	 << std::floor(bary_y) << G4endl;
  
  // Now get the barycentre of plane z=0 in global coordinates
  G4float r = pos0.getY() / (std::sin(theta) * std::sin(phi));

  // Define now the x and z global coordinates of the barycentre of the
  // plane corresponding to z = 0
  G4float x_0 = r * std::sin(theta) * std::cos(phi);
  G4float z_0 = r * std::cos(theta);

  // Print out the coordinates of the hits of the barycentre in the 
  // global frame of the detector
  G4cout << "Global x-coordinate: " << G4BestUnit(x_0, "Length") << " in units of: " << x_0 << G4endl;
  G4cout << "Global z-coordinate: " << G4BestUnit(z_0, "Length") << " in units of: " << z_0 << G4endl;

  // For simplicity, assume right now that the hit occurs at the centre of module 3
  // Neglect the tracker, and assume that all hits occur in the same module, hence neglect
  // the transition from one module to another
  // Take the coordinates of the baycentre in the z=0 plane in integer values
  G4int local_bary_x = std::floor(bary_x);
  G4int local_bary_y = std::floor(bary_y);

  // Each cell in the local frame has the length of 0.51mm hence declare the
  // calibration cell factor as well
  G4float calibration = 0.51;

  // Take the two thicknesses for the first 17 lauyers and the last 8 layers
  // as well - these values will be implemented in the reconstruction of the hits
  G4float thickness_17 = 4.4;
  G4float thickness_8 = 6.8;

  // Now perform the reconstruction of the hits
  G4int count = 0;
  
  for(G4int x = 0; x < 51; x++){
    for(G4int y = 0; y < 51; y++){
      for(G4int z = 0; z < 25; z++){
      
        // Update the value for the corresponding coordinates of each vector
        // in the aPositions list
        G4float x_global = x_0 + (x - local_bary_x) * calibration;
        G4float z_global = z_0 + (y - local_bary_y) * calibration;
        G4float y_global;

        // Now take two different cases for the y-coordinate, due to the two
        // different values of the first 17 layers and the last 8 layers
        if(z < 17){
          y_global = pos0.getY() + z * thickness_17;
        }
        
        // For the latter case, add the thicknesses from the first 17 layers and
        // then put in the remaining part from the last 8 layers
        else{
          y_global = pos0.getY() + 17 * thickness_17 + (z - 17) * thickness_8;
        }
        
        // Update the counter loop and the aPositions list of vectors
        aPositions[count] = G4ThreeVector(x_global, y_global, z_global);
        count++;
      }
    }
  } 
}

#endif
