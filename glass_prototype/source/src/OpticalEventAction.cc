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
//
/// \file OpticalEventAction.cc
/// \brief Implementation of the OpticalEventAction class

#include <OpticalEventAction.hh>
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"
#include "DetectorConstruction.hh"
#include "G4VPrimitiveScorer.hh"

#include "G4Utils.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpticalEventAction::OpticalEventAction(HistoManager* histoManager) : G4UserEventAction(),
                                                           fHistoManager(histoManager) {
  // Initialize array for numbers of charge carriers in every crystal
	fNumberOfOpticalPhotons = new G4int[G4Utils::getNCrystals()];
	fNumberOfPhotoElectrons = new G4double[G4Utils::getNCrystals()];
  fTotalPhotons = 0;
  fScintillationPhotons = 0;
  fCherenkovPhotons = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpticalEventAction::~OpticalEventAction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpticalEventAction::BeginOfEventAction(const G4Event* event){
  std::cout << std::endl << "OpticalEventAction::BeginOfOpticalEventAction" << "\n" << "  Event: " << event->GetEventID() << std::endl;

  // In the beginning of the action set number of the PE in each crystal to zero
  for (int i = 0; i < G4Utils::getNCrystals(); i++){
  	fNumberOfOpticalPhotons[i] = 0;
    fNumberOfPhotoElectrons[i] = 0;
  }

  // Zero number of optical photons
  fTotalPhotons = 0;
  fScintillationPhotons = 0;
  fCherenkovPhotons = 0;

  // Clear processed track ids
  // processedTrackIds.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpticalEventAction::EndOfEventAction(const G4Event* event){
  for (int i = 0; i < G4Utils::getNCrystals(); i++){
    std::cout << "Crystal " << i << ": numberOfOpticalPhotonps = " << fNumberOfOpticalPhotons[i] << std::endl;
    std::cout << "        " << i << "  numberOfPhotoElectrons = " << fNumberOfPhotoElectrons[i] << std::endl;
  }

  std::cout << "Total number of optical photons produced in event " << event->GetEventID() << ": " << fTotalPhotons << std::endl;
  std::cout << "Number of Scintillation photons produced in event " << event->GetEventID() << ": " << fScintillationPhotons << std::endl;
  std::cout << "Number of Cerenkov photons produced in event " << event->GetEventID() << ": " << fCherenkovPhotons << std::endl;

  // In the end of the action pass total accumulated number of the PE in each crystal to HistoManager
  fHistoManager->FillNtupleOptical(fNumberOfOpticalPhotons, fNumberOfPhotoElectrons, fTotalPhotons, fScintillationPhotons, fCherenkovPhotons);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpticalEventAction::IncreaseOPNumber(Int_t crystalIndex){
  fNumberOfPhotoElectrons[crystalIndex] ++;
}

void OpticalEventAction::IncreasePENumber(Double_t number, Int_t crystalIndex){
  fNumberOfPhotoElectrons[crystalIndex] += number;
}

void OpticalEventAction::IncreaseScintillationNumber(){
  fScintillationPhotons++;
}

void OpticalEventAction::IncreaseCherenkovNumber(){
  fCherenkovPhotons++;
}

void OpticalEventAction::IncreaseOpNumber(){
  fTotalPhotons++;
}
