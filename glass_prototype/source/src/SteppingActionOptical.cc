/*
 * SteppingActionOptical.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#include <SteppingActionOptical.hh>
#include "G4VPhysicalVolume.hh"
// #include "G4LogicalVolume.hh"
// #include <G4StepPoint.hh>
#include "G4SystemOfUnits.hh"

SteppingActionOptical::SteppingActionOptical(HistoManager* histoManager) : G4UserSteppingAction(), fHistoManager(histoManager) {
}

SteppingActionOptical::~SteppingActionOptical() {
}

void SteppingActionOptical::UserSteppingAction(const G4Step* step)
{
  G4StepPoint* point1 = step->GetPreStepPoint();
  // G4StepPoint* point2 = step->GetPostStepPoint();
  G4TouchableHandle touch1 = point1->GetTouchableHandle();
  G4VPhysicalVolume* volume = touch1->GetVolume();

  // If particle registered in PMT
  if (volume->GetName() == "PMT_log"){

    G4Track* track = step->GetTrack();
    G4String particleName = track->GetDynamicParticle()->GetParticleDefinition()->GetParticleName();

    if (particleName == "opticalphoton"){
      G4int copyNumber = touch1->GetCopyNumber(-1);
      std::cout << "optical photon in PMT" << copyNumber << std::endl;

      // Terminate track
      track->SetTrackStatus(fStopAndKill);

      // Log to HistoManager or EventAction?
    }
  }
}
