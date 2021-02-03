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
#include <G4RunManager.hh>
#include "G4SystemOfUnits.hh"
#include "QuantumEfficiency.hh"
#include "G4OpticalPhoton.hh"

OpticalSteppingAction::OpticalSteppingAction(HistoManager* histoManager, OpticalEventAction* opticalEventAction) :
  G4UserSteppingAction(), fHistoManager(histoManager), fOpticalEventAction(opticalEventAction) {
}

OpticalSteppingAction::~OpticalSteppingAction() {
}

void OpticalSteppingAction::UserSteppingAction(const G4Step* step) {
  // Refer to OpNoviceSteppingAction.cc

  G4StepPoint* point1 = step->GetPreStepPoint();
  // G4StepPoint* point2 = step->GetPostStepPoint();
  G4TouchableHandle touch1 = point1->GetTouchableHandle();
  // G4TouchableHandle touch2 = point2->GetTouchableHandle();

  G4VPhysicalVolume* volume1 = touch1->GetVolume();
  // G4VPhysicalVolume* volume2 = touch2->GetVolume();

  // Ensure particle is inside PMT (provide physical volume name)
  if (volume1->GetName() != "pmtCathodePhys") return;

  // Ensure particle type is optical photon
  if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::Definition()) return;

  // Energy and wavelength
  G4double energy = step->GetTrack()->GetDynamicParticle()->GetKineticEnergy()/eV; // in eV
  G4double hc = 1239.84193;
  G4double waveLength = hc/(energy); // in nm

  // Crystal copy number
  G4int copyNumber = touch1->GetCopyNumber(3);

  // Quantum efficiency
  QuantumEfficiency* qe = new QuantumEfficiency(PhotoCathode::Bialkali);
  G4double efficiency = qe->getEfficiency(waveLength); //  in percent

  // Output
  if (G4RunManager::GetRunManager()->GetVerboseLevel() != 0){
    std::cout << "Track ID " << step->GetTrack()->GetTrackID()
              << ". OP in PMT " << copyNumber
              << ". Volume is " << volume1->GetName()
              << ". E=" << energy << " eV, wl="<< waveLength << " nm, " << "eff=" << efficiency << " %" << std::endl;
  }

  // Save number of photo-electrons to OpticalEventAction
  G4double peNumber = efficiency/100;
  fOpticalEventAction->IncreasePENumber(peNumber, copyNumber);

  // Terminate track
  // step->GetTrack()->SetTrackStatus(fStopAndKill);

}
