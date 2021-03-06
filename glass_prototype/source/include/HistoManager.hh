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
// $Id: HistoManager.hh 92322 2015-08-27 14:54:05Z gcosmo $
// GEANT4 tag $Name: geant4-09-04 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>

// Ignoring warning shadow messages (doiPETAnalysis.hh)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TFile.h"
#include "TTree.h"
#pragma GCC diagnostic pop

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Forward Declaration of Messenger
class HistoManagerMessenger;

//class TFile;
//class TTree;
//class TH1D;

const G4int MaxHisto = 4;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager {
public:
  static HistoManager* getInstance();
  ~HistoManager();

  void Book();
//  void FillNtuple();
//  void FillNtuple_Flux();
  void FillNtupleEnergyDep(G4double* energy);
  void FillNtupleOptical(G4int* opNumber, G4double* peNumber, G4int totalPhotons, G4int scintPhotons, G4int cherenkPhotons);
  void FillNtupleOutOfWorld(G4double energy, G4double x, G4double y, G4double z, G4int pdg, const char* particleName);
  void Save();

  // void SetPrimaryParticle(G4double, G4int, G4ThreeVector, G4ThreeVector, G4double);
  // void SetEnergy(G4int, G4double, G4int, G4int, G4int, G4int, G4int);
  // void SetFluxEnergy(G4int, G4int , G4double, G4ThreeVector);

  void PrintStatistic();

  // HistoManagerMessenger methods
  void setFileNamePattern(G4String fileNamePattern);
  G4String getFileNamePattern();
  G4String getFileName();

//  void setWriteStepPoints(G4bool value);
//  G4bool getWriteStepPoints();

  void setWriteWorldEscape(G4bool value);
  G4bool getWriteWorldEscape();

  //  void BookBeginOfEventAction();

  G4double getTotalWorldOutEnergy();

private:
  HistoManager();
  static HistoManager* instance;

  HistoManagerMessenger* fHistoManagerMessenger;
  G4String fFileNamePattern;

//  G4bool fWriteStepPoints;

  // Controls if world escape locations are saved to output ROOT file
  G4bool fWriteWorldEscape;

  TFile*   fRootFile;
  TTree*   fNtupleCrystals;
  TTree*   fNtupleOptical; // ntuple for number of charge carriers

  TTree*   fNtuplePMT;
//  TTree*   fNtuple_Flux;
  TTree*   fNtupleOutOfWorld;

  G4double fPrimaryTime;
  G4int    fPrimaryPID;
  G4double fPrimaryEnergy;
  G4int    fEvtNb;

  G4double* fEdep;
  G4double fEdepTotal;

  // Photo-electrons counter
  G4double* fOP;
  G4double* fPE;

  // Optical photons counters


  //  G4int    fOP_sc[MaxNtuple];
//  G4int    fOP_ce[MaxNtuple];
//  G4int    fOP_cover[MaxNtuple];
//  G4int    fOP_frontcover[MaxNtuple];
//  G4int    fOP_pmtcover[MaxNtuple];

//  G4double fPrimaryPos[3];
//  G4double fPrimaryMom[3];

//  G4double fFluxEne[MaxNtuple];
//  G4double fFluxPos_X[MaxNtuple];
//  G4double fFluxPos_Y[MaxNtuple];
//  G4double fFluxPos_Z[MaxNtuple];

  G4double fTotalOutWorldEnergy;
  G4double fOutWorldEnergy;
  G4double fOutWorldX;
  G4double fOutWorldY;
  G4double fOutWorldZ;

  G4int fTotalPhotonsPerEvent;
  G4int fScintPhotonsPerEvent;
  G4int fCherePhotonsPerEvent;

  G4int fPdg;
  TMap* pdgNameMap;
//  std::vector<int> fPdgVector;

//  RooRealVar* fTotalGPSEnergy;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

