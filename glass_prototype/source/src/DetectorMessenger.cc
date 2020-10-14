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
/// \file hadronic/Hadr03/src/DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class
//
// $Id: DetectorMessenger.cc 70755 2013-06-05 12:17:48Z ihrivnac $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det) : G4UImessenger(), fDetectorConstruction(Det) {
	// Instantiate the Command directory
//	fDirectory = new G4UIdirectory("/NPS_Energy_Resolution/det/");
//	fDirectory->SetGuidance("Commands for Detector construction");

	// Instantiate command for setting the gap between crystals
	fSetGapCmd = new G4UIcmdWithADoubleAndUnit("/NPS_Energy_Resolution/det/setGap", this);
	fSetGapCmd->SetGuidance("Set the gap between the crystals.");
	fSetGapCmd->SetUnitCategory("Length");
	fSetGapCmd->SetParameterName("choice", false);
	fSetGapCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

	// Instantiate command for setting crystal size
	fSetCrystalSizeCmd = new G4UIcmdWith3VectorAndUnit("/NPS_Energy_Resolution/det/setCrystalSize", this);
	fSetCrystalSizeCmd->SetGuidance("Set crystal size.");
	fSetGapCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger() {
  delete fSetGapCmd;
  delete fSetCrystalSizeCmd;
//  delete fDirectory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
  if (command == fSetGapCmd){
	  fDetectorConstruction->SetDetectorGap(fSetGapCmd->GetNewDoubleValue(newValue));
  } else if (command == fSetCrystalSizeCmd){
	  fDetectorConstruction->SetCrystalSize(fSetCrystalSizeCmd->GetNew3VectorValue(newValue));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
