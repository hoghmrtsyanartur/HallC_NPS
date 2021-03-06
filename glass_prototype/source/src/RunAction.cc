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
/// \file analysis/shared/src/RunAction.cc
/// \brief Implementation of the RunAction class
//
//
// $Id: RunAction.cc 92322 2015-08-27 14:54:05Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <CLHEP/Vector/ThreeVector.h>
#include "RunAction.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4ScoringManager.hh"
#include "G4VScoringMesh.hh"
#include "G4Utils.hh"
#include "G4VScoreColorMap.hh"
#include "G4DefaultLinearColorMap.hh"

using CLHEP::Hep3Vector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(HistoManager* histoManager)
 : G4UserRunAction(), fHistoManager(histoManager)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  // PS:
  // This method is invoked before entering the event loop (BeamOn?).
  // A typical use of this method would be to initialize and/or book histograms for a particular run.

  // PS: added automatic time-based random seeds for each run
  long seeds[2];
  time_t systime = time(NULL);
  seeds[0] = (long) systime;
  seeds[1] = (long) (systime*G4UniformRand());
  G4Random::setTheSeeds(seeds);
  G4Random::showEngineStatus();

  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  // Instantiate histograms
  fHistoManager->Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* aRun)
{
  // PS:
  // This method is invoked at the very end of the run processing.
  // It is typically used for a simple analysis of the processed run.

  G4int NbOfEvents = aRun->GetNumberOfEvent();
  if (NbOfEvents == 0) return;

  // Equalize scoring mesh color maps for Crystals and PMTs (just for visualization)
  // Obtain maximum value of the crystals mesh
  Double_t maxCrystalMeshValue = G4Utils::getProjectionZMaximumQuantityFromMesh("crystalsMesh", "eneDepCrystal");
  Double_t maxPMTMeshValue = G4Utils::getProjectionZMaximumQuantityFromMesh("pmtsMesh", "eneDepPMT");

  // Assign this value to the 1st bin of the PMT mesh
  G4VScoreColorMap* myColorMap = new G4DefaultLinearColorMap("myColorMap");
  myColorMap->SetMinMax(0, std::max(maxCrystalMeshValue, maxPMTMeshValue));
  myColorMap->SetFloatingMinMax(0);

  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
  scoringManager->RegisterScoreColorMap(myColorMap);
  scoringManager->ListScoreColorMaps();


  // Save histograms
  fHistoManager->Save();

 //  if (maxCrystalMeshValue > maxPMTMeshValue){
//    G4Utils::setMaximumMeshQuantity("pmtsMesh", "eneEdepPMT", maxCrystalMeshValue);
//  } else {
//    G4Utils::setMaximumMeshQuantity("crystalsMesh", "eneDepCrystal", maxPMTMeshValue);
//  }
//
//  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
//  scoringManager->DrawMesh(meshName, psName, colorMapName, axflg)
}
