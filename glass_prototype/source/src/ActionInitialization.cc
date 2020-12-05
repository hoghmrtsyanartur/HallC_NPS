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
/// \file ActionInitialization.cc
/// \brief Implementation of the ActionInitialization class

#include "ActionInitialization.hh"
#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "OutsideWorldSteppingAction.hh"
#include "SteppingAction.hh"
#include "HistoManager.hh"

class DetectorConstruction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::ActionInitialization(DetectorConstruction *detector)
 : G4VUserActionInitialization(), fDetector(detector)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::~ActionInitialization()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
  // PS: this is invoked in Multi-threading mode
  HistoManager* histoManager = new HistoManager();
  RunAction* runAction = new RunAction(histoManager);
  SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
  // PS: this is invoked in Sequential mode
  HistoManager* histoManager = new HistoManager();

  // General Particle Source
  PrimaryGeneratorAction* primaryGeneratorAction = new PrimaryGeneratorAction(histoManager);
  SetUserAction(primaryGeneratorAction);

  // Action Events before /run/initialize
  RunAction* runAction = new RunAction(histoManager);
  SetUserAction(runAction);

  // Action invoked before and after every event
  EventAction* eventAction = new EventAction(histoManager);
  SetUserAction(eventAction);

  // PS: Save output to file for stepping points >= 1
  SteppingAction *steppingAction = new SteppingAction(histoManager, fDetector, eventAction);
  SetUserAction(steppingAction);

  OutsideWorldSteppingAction* outsideWorldSteppingAction = new OutsideWorldSteppingAction(histoManager);
  SetUserAction(outsideWorldSteppingAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......