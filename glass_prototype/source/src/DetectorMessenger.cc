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
#include "G4UIcmdWithAnInteger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det) : G4UImessenger(), fDetectorConstruction(Det) {
	// Instantiate the Command directory
	fDirectory = new G4UIdirectory("/detector/");
	fDirectory->SetGuidance("Commands for Detector construction");

	// Instantiate command for setting the gap between crystals
	fSetGapCmd = new G4UIcmdWithADoubleAndUnit("/detector/setGap", this);
	fSetGapCmd->SetGuidance("Set the gap between the crystals.");
	fSetGapCmd->SetUnitCategory("Length");
	fSetGapCmd->SetParameterName("choice", false);
	fSetGapCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

	// Instantiate command for setting crystal size
	fSetCrystalSizeCmd = new G4UIcmdWith3VectorAndUnit("/detector/setCrystalSize", this);
	fSetCrystalSizeCmd->SetGuidance("Set crystal size");
	fSetCrystalSizeCmd->SetDefaultUnit("mm");
	fSetCrystalSizeCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for setting crystal Material
  fSetCrystalMaterialCmd = new G4UIcmdWithAString("/detector/setCrystalMaterial", this);
  fSetCrystalMaterialCmd->SetGuidance("Set crystal material type, \"BaSi2O5\" (default) or \"PbWO4\"");
  fSetCrystalMaterialCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for setting crystal Material
  fSetDetectorTypeCmd = new G4UIcmdWithAString("/detector/setDetectorType", this);
  fSetDetectorTypeCmd->SetGuidance("Set detector type, \"PMT\" (default) or \"MPPC\"");
  fSetDetectorTypeCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for setting PMT diameter
  fSetPmtDiameter = new G4UIcmdWithADoubleAndUnit("/detector/setPmtDiameter", this);
  fSetPmtDiameter->SetGuidance("Set PMT diameter");
  fSetPmtDiameter->SetDefaultUnit("mm");
  fSetPmtDiameter->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for PMT grease thickness
  fSetGreaseThickness = new G4UIcmdWithADoubleAndUnit("/detector/setGreaseThickness", this);
  fSetGreaseThickness->SetGuidance("Set PMT grease thickness");
  fSetGreaseThickness->SetDefaultUnit("mm");
  fSetGreaseThickness->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate commands for setting number of crystals
  fSetCrystalNumberXCmd = new G4UIcmdWithAnInteger("/detector/setCrystalNumberX", this);
  fSetCrystalNumberXCmd->SetGuidance("Number of crystals along X axis");
  fSetCrystalNumberXCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);
  fSetCrystalNumberYCmd = new G4UIcmdWithAnInteger("/detector/setCrystalNumberY", this);
  fSetCrystalNumberYCmd->SetGuidance("Number of crystals along Y axis");
  fSetCrystalNumberYCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger() {
  delete fSetGapCmd;
  delete fSetCrystalSizeCmd;
  delete fSetCrystalMaterialCmd;
  delete fSetDetectorTypeCmd;
//  delete fDirectory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
  if (command == fSetGapCmd){
	  fDetectorConstruction->SetDetectorGap(fSetGapCmd->GetNewDoubleValue(newValue));
  } else if (command == fSetCrystalSizeCmd){
	  fDetectorConstruction->SetCrystalSize(fSetCrystalSizeCmd->GetNew3VectorValue(newValue));
  } else if (command==fSetCrystalMaterialCmd){
    fDetectorConstruction->SetCrystalMaterial(newValue.c_str());
  } else if (command==fSetDetectorTypeCmd){
    fDetectorConstruction->SetDetectorType(newValue);
  } else if (command==fSetCrystalNumberXCmd){
    fDetectorConstruction->SetCrystalNumberX(fSetCrystalNumberXCmd->GetNewIntValue(newValue));
  } else if (command==fSetCrystalNumberYCmd){
    fDetectorConstruction->SetCrystalNumberY(fSetCrystalNumberYCmd->GetNewIntValue(newValue));
  } else if (command == fSetPmtDiameter){
	fDetectorConstruction->SetPmtDiameter(fSetPmtDiameter->GetNewDoubleValue(newValue));
  } else if (command == fSetGreaseThickness){
	fDetectorConstruction->SetGreaseThickness(fSetGreaseThickness->GetNewDoubleValue(newValue));
  }
}

//G4String DetectorMessenger::GetCurrentValue(G4UIcommand* command){
//  if (command == fSetGapCmd){
//    return fSetGapCmd->ConvertToString(fDetectorConstruction->GetDetectorGap());
//  } else if (command == fSetCrystalSizeCmd){
//    fSetCrystalSizeCmd->ConvertToString(*(fDetectorConstruction->GetCrystalSize()));
//  } else if (command==fSetCrystalMaterialCmd){
//    return fDetectorConstruction->GetCrystalMaterial();
//  }
//  return "";
//}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
