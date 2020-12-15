/*
 * SteppingAction.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#include <SteppingAction.hh>
// #include "G4VPhysicalVolume.hh"
// #include "G4LogicalVolume.hh"
// #include <G4StepPoint.hh>
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(HistoManager* histoManager) : G4UserSteppingAction(), fHistoManager(histoManager) {
}

SteppingAction::~SteppingAction() {
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
//  G4VPhysicalVolume* prePhysicalVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
//  G4VPhysicalVolume* postPhysicalVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
//  if (prePhysicalVolume != NULL && postPhysicalVolume == NULL){
//    G4double x = step->GetPreStepPoint()->GetPosition().x()/m;
//    G4double y = step->GetPreStepPoint()->GetPosition().y()/m;
//    G4double z = step->GetPreStepPoint()->GetPosition().z()/m;
//    G4double energy = step->GetPreStepPoint()->GetKineticEnergy()/MeV;
//    fHistoManager->FillNtupleOutOfWorld(energy, x, y, z);
//  }

  if (step->GetPostStepPoint()->GetStepStatus() == fWorldBoundary){
    G4ThreeVector worldPosition = step->GetPostStepPoint()->GetPosition();
    // G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
    // G4double energy1 = step->GetPreStepPoint()->GetKineticEnergy()/MeV;   // save as below
    // G4double energy2 = step->GetPostStepPoint()->GetKineticEnergy()/MeV;  // save as below
    G4double kinEnergy = step->GetTrack()->GetKineticEnergy(); // MeV
    G4int pdg = step->GetTrack()->GetDefinition()->GetPDGEncoding(); // Particle ID
    const char* particleName = step->GetTrack()->GetDefinition()->GetParticleName().c_str(); // Particle Name
    fHistoManager->FillNtupleOutOfWorld(kinEnergy, worldPosition.x(), worldPosition.y(), worldPosition.z(), pdg, particleName);
  }
}
