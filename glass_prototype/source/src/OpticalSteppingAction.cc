/*
 * OpticalSteppingAction.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#include "OpticalSteppingAction.hh"
#include "OpticalEventAction.hh"
#include "G4VPhysicalVolume.hh"
// #include "G4LogicalVolume.hh"
// #include <G4StepPoint.hh>
#include "G4SystemOfUnits.hh"
#include "QuantumEfficiency.hh"

OpticalSteppingAction::OpticalSteppingAction(HistoManager* histoManager, OpticalEventAction* opticalEventAction) :
  G4UserSteppingAction(), fHistoManager(histoManager), fOpticalEventAction(opticalEventAction) {
}

OpticalSteppingAction::~OpticalSteppingAction() {
}

void OpticalSteppingAction::UserSteppingAction(const G4Step* step) {
  // Refer to OpNoviceSteppingAction.cc

  G4StepPoint* point1 = step->GetPreStepPoint();
  G4StepPoint* point2 = step->GetPostStepPoint();
  G4TouchableHandle touch1 = point1->GetTouchableHandle();
  G4TouchableHandle touch2 = point2->GetTouchableHandle();

  G4VPhysicalVolume* volume1 = touch1->GetVolume();
  G4VPhysicalVolume* volume2= touch1->GetVolume();

  // If particle registered in PMT
  if (volume2->GetName() == "PMT_pos"){

    G4Track* track = step->GetTrack();
    G4String particleName = track->GetDynamicParticle()->GetParticleDefinition()->GetParticleName();

    if (particleName == "opticalphoton"){
      // const char* motherName = volume1->GetMotherLogical()->GetName();

      // Energy and wavelength
      G4double energy = track->GetDynamicParticle()->GetKineticEnergy()/eV; // in eV
      G4double hc = 1239.84193;
      G4double waveLength = hc/(energy); // in nm

      // Crystal copy number
      G4int copyNumber = touch1->GetCopyNumber(1);

      // Quantum efficiency
      QuantumEfficiency* qe = new QuantumEfficiency(PhotoCathode::Bialkali);
      G4double efficiency = qe->getEfficiency(waveLength);

      // Output
      std::cout << "Track ID " << track->GetTrackID()
                << ". OP in PMT " << copyNumber
                << ". Vol1=" << volume1->GetName() << " Vol2=" << volume2->GetName()
                << ". E=" << energy << " eV, wl="<< waveLength << " nm, " << "eff=" << efficiency << " %" << std::endl;

      // Save number of photoelectrons to OpticalEventAction
      G4double peNumber = efficiency/100;
      fOpticalEventAction->IncreasePENumber(peNumber, copyNumber);

      // Terminate track
      track->SetTrackStatus(fStopAndKill);

      // Instead terminating track we remember track Id and process it only once
      // fOpticalEventAction->processedTrackIds.push_back()

    }
  }
}
