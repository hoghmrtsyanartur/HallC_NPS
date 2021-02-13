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
/// \file OpticalEventAction.hh
/// \brief Definition of the OpticalEventAction class

#ifndef OpticalEventAction_h
#define OpticalEventAction_h 1

#include "G4UserEventAction.hh"

#include "G4THitsMap.hh"
#include "globals.hh"
#include "HistoManager.hh"
/// Event action class
///
/// In EndOfOpticalEventAction(), it prints the accumulated quantities of the energy
/// deposit and track lengths of charged particles in Absober and Gap layers
/// stored in the hits collections.

class OpticalEventAction : public G4UserEventAction
{
public:
  OpticalEventAction(HistoManager* histoManager);
  virtual ~OpticalEventAction();

  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);

  void IncreaseOPNumber(Int_t crystalIndex);
  void IncreasePENumber(Double_t number, Int_t crystalIndex);
  void IncreaseScintillationNumber();
  void IncreaseCherenkovNumber();
  void IncreaseOpNumber();

  // std::vector<G4int> processedTrackIds;

private:
  HistoManager* fHistoManager;

  G4int* fNumberOfOpticalPhotons; // that ended up on the cathode
  G4double* fNumberOfPhotoElectrons;

  // For counting of the scintillation/cherenkov photons
  G4int fTotalPhotons;
  G4int fScintillationPhotons;
  G4int fCherenkovPhotons;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
