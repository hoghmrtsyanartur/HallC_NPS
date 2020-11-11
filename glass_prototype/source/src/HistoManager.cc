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
// $Id: HistoManager.cc 92374 2015-08-31 08:52:09Z gcosmo $
// GEANT4 tag $Name: geant4-09-04 $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"
//#include <TH1D.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <CLHEP/Units/SystemOfUnits.h>

#include "G4UnitsTable.hh"
#include "TString.h"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
  : fFileName("output_file.root"),
    fRootFile(0),
    fNtuple(0),
    fNtuple_Flux(0),
    fPrimaryTime(-999.),
    fPrimaryPID(-999),
    fPrimaryEnergy(-999.),
    fEvtNb(0),
    // PS: use the initializer list for array members (C++11)
    fEdep {0},
    fOP_sc {0},
    fOP_ce {0},
    fOP_cover {0},
    fOP_frontcover {0},
    fOP_pmtcover {0},
    fPrimaryPos {-999,-999,-999},
    fPrimaryMom {-999,-999,-999},
    fFluxEne {0},
    fFluxPos_X {0},
    fFluxPos_Y {0},
    fFluxPos_Z {0}
{
  // PS: set output filename as "ene_edep_res_x_20mm_y_20mm_z_200mm.root"
  // We're getting crystal ize from the DetectorConstruction
  DetectorConstruction* detectorConstruction = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction != NULL){
    G4ThreeVector* vector = detectorConstruction->GetCrystalSize();
    G4int x = vector->getX();
    G4int y = vector->getY();
    G4int z = vector->getZ();
    TString fileName = TString::Format("ene_edep_res_x_%dmm_y_%dmm_z_%dmm.root", x, y, z);
    fFileName = fileName.Data();
  }
}

HistoManager::~HistoManager()
{
//  if (fRootFile !=0 ){
//    delete fRootFile;
//    fRootFile = 0;
//  }
//  if (fNtuple !=0 ){
//    delete fNtuple;
//    fNtuple = 0;
//  }
//  if (fNtuple_Flux !=0 ){
//    delete fNtuple_Flux;
//    fNtuple_Flux = 0;
//  }
  G4cout << "Histogram Manager deleted" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book()
{
  // Creating a tree container to handle histograms and ntuples.
  // This tree is associated to an output file.
  //
  fRootFile = new TFile(fFileName.c_str(), "RECREATE");
  if (!fRootFile) {
    G4cout << " HistoManager::Book :"
           << " problem creating the ROOT TFile "
           << G4endl;
    return;
  } else {
    G4cout << " HistoManager::Book :"
           << " created output file \"" << fFileName.c_str() << "\""
           << G4endl;
  }

  fNtuple = new TTree("t","Energy deposition and OP in crystas");
  fNtuple->Branch("edep", fEdep, "energy_deposition[9]/D");
  fNtuple->Branch("sc", fOP_sc, "scintillated OP[9]/I");
  // fNtuple->Branch("ce", fOP_ce, "cerenkov OP[9]/I");
  fNtuple->Branch("op_cover", fOP_cover, "OP on the side of the crystal wrapper[9]/I");
  fNtuple->Branch("op_frontcover", fOP_frontcover, "OP on the front side of the crystal wrapper[9]/I");
  fNtuple->Branch("op_pc", fOP_pmtcover, "OP arrived at the pmt cover[9]/I");

  // fNtuple->Branch("PT", &fPrimaryTime, "Primary vertex time/D");
  // fNtuple->Branch("PPID", &fPrimaryPID, "Primary vertex PID/I");
  // fNtuple->Branch("PP", fPrimaryPos, "Primary vertex Position[3]/D");
  // fNtuple->Branch("PM", fPrimaryMom, "Primary vertex Momentum[3]/D");
  // fNtuple->Branch("PE", &fPrimaryEnergy, "Primary vertex Energy/D");

  fNtuple_Flux = new TTree("t_Flux","Checking the energy shower profile in crystals");
  fNtuple_Flux->Branch("evtNb", &fEvtNb, "Event number/I");
  fNtuple_Flux->Branch("edep", fFluxEne, "Energy deposition per step[9]/D");
  fNtuple_Flux->Branch("x", fFluxPos_X, "Postion x per step[9]/D");
  fNtuple_Flux->Branch("y", fFluxPos_Y, "Postion y per step[9]/D");
  fNtuple_Flux->Branch("z", fFluxPos_Z, "Postion z per step[9]/D");


  G4cout << "\n----> Output file is open in " << fFileName << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillNtuple()
{
  fNtuple->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HistoManager::FillNtuple_Flux()
{
  fNtuple_Flux->Fill();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Save()
{
  if (! fRootFile) return;
  fRootFile->Write();       // Writing the histograms to the file
  fRootFile->Close();       // and closing the tree (and the file)
  G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HistoManager::SetPrimaryParticle(G4double time, G4int PID, G4ThreeVector pos, G4ThreeVector mom, G4double energy)
{
  fPrimaryTime = time;
  fPrimaryPID = PID;
  fPrimaryPos[0] = pos.x();
  fPrimaryPos[1] = pos.y();
  fPrimaryPos[2] = pos.z();
  fPrimaryMom[0] = mom.x();
  fPrimaryMom[1] = mom.y();
  fPrimaryMom[2] = mom.z();
  fPrimaryEnergy = energy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::SetEnergy(G4int id, G4double edep, G4int sc, G4int ce, G4int opc, G4int opfc, G4int oppc)
{
  fEdep[id] = edep;
  fOP_sc[id] = sc;
  fOP_ce[id] = ce;
  fOP_cover[id] = opc;
  fOP_frontcover[id] = opfc;
  fOP_pmtcover[id] = oppc;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::SetFluxEnergy(G4int evtNb, G4int id, G4double edep, G4ThreeVector position)
{

  fEvtNb = evtNb;

  for(int i = 0 ; i < MaxNtuple ; i++) {fFluxEne[i] = 0.; fFluxPos_X[i] = 0.; fFluxPos_Y[i] = 0.; fFluxPos_Z[i] = 0.;}
  fFluxEne[id] = edep;
  fFluxPos_X[id] = position.x();
  fFluxPos_Y[id] = position.y();
  fFluxPos_Z[id] = position.z();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
