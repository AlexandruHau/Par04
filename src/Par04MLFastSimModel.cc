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
#include "Par04MLFastSimModel.hh"
#include <stddef.h>                      // for size_t
#include <G4FastStep.hh>                 // for G4FastStep
#include <G4FastTrack.hh>                // for G4FastTrack
#include <G4Track.hh>                    // for G4Track
#include <G4VFastSimulationModel.hh>     // for G4VFastSimulationModel
#include "G4Electron.hh"                 // for G4Electron
#include "G4FastHit.hh"                  // for G4FastHit
#include "G4FastSimHitMaker.hh"          // for G4FastSimHitMaker
#include "G4Gamma.hh"                    // for G4Gamma
#include "G4Positron.hh"                 // for G4Positron
#include "Par04InferenceSetup.hh"        // for Par04InferenceSetup
#include "G4UnitsTable.hh"
#include "G4AnalysisManager.hh"
class G4ParticleDefinition;
class G4Region;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Par04MLFastSimModel::Par04MLFastSimModel(G4String aModelName, G4Region* aEnvelope)
  : G4VFastSimulationModel(aModelName, aEnvelope)
  , fInference(new Par04InferenceSetup)
  , fHitMaker(new G4FastSimHitMaker)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Par04MLFastSimModel::Par04MLFastSimModel(G4String aModelName)
  : G4VFastSimulationModel(aModelName)
  , fInference(new Par04InferenceSetup)
  , fHitMaker(new G4FastSimHitMaker)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Par04MLFastSimModel::~Par04MLFastSimModel() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool Par04MLFastSimModel::IsApplicable(const G4ParticleDefinition& aParticleType)
{
  return &aParticleType == G4Electron::ElectronDefinition() ||
         &aParticleType == G4Positron::PositronDefinition() ||
         &aParticleType == G4Gamma::GammaDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool Par04MLFastSimModel::ModelTrigger(const G4FastTrack& aFastTrack)
{
  return fInference->IfTrigger(aFastTrack.GetPrimaryTrack()->GetKineticEnergy());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Par04MLFastSimModel::DoIt(const G4FastTrack& aFastTrack, G4FastStep& aFastStep)
{
  // remove particle from further processing by G4
  aFastStep.KillPrimaryTrack();
  aFastStep.SetPrimaryTrackPathLength(0.0);
  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
  aFastStep.SetTotalEnergyDeposited(energy);
  G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  // calculate the incident angle
  G4float angle = direction.theta();
  G4float phi = direction.phi();

  G4cout << "Incident theta angle: " << angle << G4endl;
  G4cout << "Incident phi angle: " << phi << G4endl;

  // calculate how to deposit energy within the detector
  // get it from inference model

  G4cout << "ML Inference test: " << G4endl;
  // G4cout << "One coordinate: " << fPositions[0].getX() << G4endl;
  fInference->GetEnergies(fEnergies, energy, angle);
  // G4cout << "One coordinate: " << fPositions[0].getX() << G4endl;
  fInference->GetPositions(fPositions, position, direction, fEnergies, angle, phi);

  // Implement the pointer for the analysis manager in order to store the data in 
  // root files
  auto analysisManager = G4AnalysisManager::Instance();

  // deposit energy in the detector using calculated values of energy deposits
  // and positions
  G4cout << "One coordinate: " << fPositions[0].getX() << G4endl;
  for(size_t iHit = 0; iHit < fPositions.size(); iHit++)
  {
    fHitMaker->make(G4FastHit(fPositions[iHit], fEnergies[iHit]), aFastTrack);
    // G4cout << "Energy hit: " << G4BestUnit(fEnergies[iHit], "Energy") << " of energy value: " << fEnergies[iHit] <<  G4endl;
    // G4cout << "Count: " << iHit << G4endl;
    G4cout << "x-coordinate: " << G4BestUnit(fPositions[iHit].getX(), "Length") << " of coordinate value: "
	   << fPositions[iHit].getX()  << G4endl;
    G4cout << "y-coordinate: " << G4BestUnit(fPositions[iHit].getY(), "Length") << " of coordinate value: " 
           << fPositions[iHit].getY()  << G4endl; 
    G4cout << "z-coordinate: " << G4BestUnit(fPositions[iHit].getZ(), "Length") << " of coordinate value: " 
           << fPositions[iHit].getZ()  << G4endl;
    analysisManager->FillH1(0, fEnergies[iHit]);
    analysisManager->FillNtupleDColumn(0, fEnergies[iHit]);
    analysisManager->FillNtupleDColumn(1, fPositions[iHit].getX());
    analysisManager->FillNtupleDColumn(2, fPositions[iHit].getY());
    analysisManager->FillNtupleDColumn(3, fPositions[iHit].getZ());
    analysisManager->AddNtupleRow(0);
  }
}
#endif
