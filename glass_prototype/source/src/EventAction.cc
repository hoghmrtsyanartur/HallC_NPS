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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include <EventAction.hh>
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

EventAction::EventAction(HistoManager* histoManager) : G4UserEventAction(),
                                                           fEventNumber(-1),
                                                           fCrystalEdepHCID(-1),
                                                           fHistoManager(histoManager)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
//G4THitsMap<G4double>*
//EventAction::GetHitsCollection(G4int hcID,
//                                  const G4Event* event) const
//{
//  auto hitsCollection
//    = static_cast<G4THitsMap<G4double>*>(
//        event->GetHCofThisEvent()->GetHC(hcID));
//
//  if ( ! hitsCollection ) {
//    G4ExceptionDescription msg;
//    msg << "Cannot access hitsCollection ID " << hcID;
//    G4Exception("EventAction::GetHitsCollection()",
//      "MyCode0003", FatalException, msg);
//  }
//
//  return hitsCollection;
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double EventAction::GetSum(G4THitsMap<G4double>* hitsMap) const {
  G4double sumValue = 0.;
  for ( auto it : *hitsMap->GetMap() ) {
    sumValue += *(it.second);
  }
  return sumValue;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void EventAction::PrintEventStatistics(
//                            G4double absoEdep, G4double absoTrackLength,
//                            G4double gapEdep, G4double gapTrackLength) const
//{
//  // Print event statistics
//  //
//  G4cout
//     << "   Absorber: total energy: "
//     << std::setw(7) << G4BestUnit(absoEdep, "Energy")
//     << "       total track length: "
//     << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
//     << G4endl
//     << "        Gap: total energy: "
//     << std::setw(7) << G4BestUnit(gapEdep, "Energy")
//     << "       total track length: "
//     << std::setw(7) << G4BestUnit(gapTrackLength, "Length")
//     << G4endl;
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event){
  fEventNumber = event->GetEventID();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  DetectorConstruction* detectorConstruction = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL) return;
  G4VPrimitiveScorer* eDepScorer = detectorConstruction->GetEdepScorer(); // G4PSEnergyDeposit
  eDepScorer->PrintAll();

  // Get hit collection ID
  if (fCrystalEdepHCID == -1){
    fCrystalEdepHCID = G4SDManager::GetSDMpointer()->GetCollectionID("crystal/edep");
  }

  // Get hits collections of current event
  G4HCofThisEvent* HCE = event->GetHCofThisEvent();
  if (!HCE) return;

  G4THitsMap<G4double>* evtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCrystalEdepHCID));

  // Create array with energy deposition values, fill with zeros
  G4double* eDep = new G4double[G4Utils::getNCrystals()];
  for (G4int i = 0; i < G4Utils::getNCrystals(); i++){
    eDep[i] = 0;
  }

  // Iterate the Hits Map. Map index corresponds to the copy number of the Single Volume,
  // therefore we take it as a crystal index
  std::cout << "Event " << fEventNumber << std::endl;
  std::map<G4int,G4double*>::iterator itr;
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
    G4int copyNumber = itr->first;
    G4double energy = *(itr->second)/eDepScorer->GetUnitValue();
    std::cout << "  crystal " << copyNumber << ": " << energy << " " << eDepScorer->GetUnit() << std::endl;
    // Remember energy deposition in crystal with corresponding index
    eDep[copyNumber] = energy;
  }

  // Save to Histogram Manager
  fHistoManager->FillNtupleEnergyDep(eDep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int EventAction::GetEventNumber(){
  return fEventNumber;
}
