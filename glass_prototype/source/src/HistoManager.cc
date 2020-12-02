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
#include <TH1.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <CLHEP/Units/SystemOfUnits.h>

#include "G4UnitsTable.hh"
#include "TString.h"
#include "TObjString.h"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "HistoManagerMessenger.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SingleParticleSource.hh"
#include <g4root.hh>
#include "G4Utils.hh"
#include "G4Run.hh"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "RooConstVar.h"
#include "TVector3.h"
#pragma GCC diagnostic pop

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
  : fFileNamePattern("file_x_%dmm_y_%dmm_z_%dmm_%.1fGeV.root"),
    fWriteStepPoints(true),
    fRootFile(0),
    fNtuple(0),
    fNtuple_Flux(0),
    fNtupleOutOfWorld(0),
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
    fFluxPos_Z {0},
    fOutWorldEnergy(0),
    fOutWorldX(0),
    fOutWorldY(0),
    fOutWorldZ(0),
    fPdg(0),
    pdgNameMap(new TMap())
//    fTotalGPSEnergy(new RooRealVar("totalGpsEnergy", "Total energy shot from source", 0, "MeV"))
{
  // Custom UImessenger for Detector geometry modification
  fHistoManagerMessenger = new HistoManagerMessenger(this);
//  fPdgVector.clear();
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
  // Construct filename
  G4String crystalMaterial = G4Utils::getCrystalMaterial();
  G4int x = G4Utils::getCrystalX();
  G4int y = G4Utils::getCrystalY();
  G4int z = G4Utils::getCrystalZ();
  G4double gunEnergy = G4Utils::getGPSMonoEnergy();
  G4int numberOfEvents = G4Utils::getNumberOfEvents();
  TString fileName = TString::Format(fFileNamePattern.c_str(), crystalMaterial.c_str(), x, y, z, gunEnergy/1.E3, numberOfEvents);

  fRootFile = new TFile(fileName.Data(), "RECREATE");
  if (!fRootFile) {
    G4cout << "HistoManager::Book: cannot create output file \"" << fileName.Data() << "\"" << G4endl;
    return;
  }
  G4cout << " HistoManager::Book: created output file \"" << fileName.Data() << "\"" << G4endl;

  fNtuple = new TTree("t","Energy deposition and OP in crystas");
  // Writing arrays to tree:
  // https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html#cb22
  fNtuple->Branch("edep", fEdep, "fEdep[9]/D");
  fNtuple->Branch("sc", fOP_sc, "fOP_sc[9]/I");
  // fNtuple->Branch("ce", fOP_ce, "cerenkov OP[9]/I");
  fNtuple->Branch("op_cover", fOP_cover, "fOP_cover[9]/I");
  fNtuple->Branch("op_frontcover", fOP_frontcover, "fOP_frontcover[9]/I");
  fNtuple->Branch("op_pc", fOP_pmtcover, "fOP_pmtcover[9]/I");

  // fNtuple->Branch("PT", &fPrimaryTime, "Primary vertex time/D");
  // fNtuple->Branch("PPID", &fPrimaryPID, "Primary vertex PID/I");
  // fNtuple->Branch("PP", fPrimaryPos, "Primary vertex Position[3]/D");
  // fNtuple->Branch("PM", fPrimaryMom, "Primary vertex Momentum[3]/D");
  // fNtuple->Branch("PE", &fPrimaryEnergy, "Primary vertex Energy/D");
  if (fWriteStepPoints){
    fNtuple_Flux = new TTree("t_Flux","Checking the energy shower profile in crystals");
    fNtuple_Flux->Branch("evtNb", &fEvtNb, "fEvtNb/I");
    fNtuple_Flux->Branch("edep", fFluxEne, "fFluxEne[9]/D");
    fNtuple_Flux->Branch("x", fFluxPos_X, "fFluxPos_X[9]/D");
    fNtuple_Flux->Branch("y", fFluxPos_Y, "fFluxPos_Y[9]/D");
    fNtuple_Flux->Branch("z", fFluxPos_Z, "fFluxPos_Z[9]/D");
  }

  // Detect energy of the particles that leave the world
  fNtupleOutOfWorld = new TTree("t_escape","Energy of particles that escape the world");
  fNtupleOutOfWorld->Branch("energy", &fOutWorldEnergy, "fOutWorldEnergy/D");
  fNtupleOutOfWorld->Branch("x", &fOutWorldX, "fOutWorldX/D");
  fNtupleOutOfWorld->Branch("y", &fOutWorldY, "fOutWorldY/D");
  fNtupleOutOfWorld->Branch("z", &fOutWorldZ, "fOutWorldZ/D");
  fNtupleOutOfWorld->Branch("pdg", &fPdg, "Particle ID /I");
  // G4cout << "\n----> Output file is open in " << fFileName << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void HistoManager::BookBeginOfEventAction(){
//  G4double gunEnergy = G4Utils::getGPSMonoEnergy();
//  fTotalGPSEnergy->setVal(fTotalGPSEnergy->getVal() + gunEnergy);
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillNtupleOutOfWorld(G4double energy, G4double x, G4double y, G4double z, G4int pdg, const char* particleName){
  // Only book 40 events maximum
  // G4int eventNumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  // if (eventNumber > 40) return;

  // Book maximum 10000 events for the escape partic
  // if (fNtupleOutOfWorld->GetEntries() < 1E4) return;

  fOutWorldEnergy = energy;
  fOutWorldX = x;
  fOutWorldY = y;
  fOutWorldZ = z;
  fPdg = pdg;
//  fPdgVector.push_back(pdg); // Save pdg to vector

  fNtupleOutOfWorld->Fill();

  // Save pair of particle pdg and name
  TString pdgString = TString::Format("%d", pdg);
  TObjString* key = new TObjString(pdgString.Data());
  TObjString* value = new TObjString(particleName);
  if (pdgNameMap->GetValue(pdgString.Data())==NULL){
    pdgNameMap->Add(key, value);
  }
}

void HistoManager::FillNtuple()
{
  fNtuple->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HistoManager::FillNtuple_Flux()
{
  if (fWriteStepPoints){
    fNtuple_Flux->Fill();
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Save()
{
  if (! fRootFile) return;

  // Write particle PDG names
  // Save collection as a single object, otherwise each key will be saved independently
  // pdgNameMap->Print();
  pdgNameMap->Write("pdgNameMap", TObject::kSingleKey);

  // Write material
  TObjString* crystalMaterial = new TObjString(G4Utils::getCrystalMaterial().c_str());
  crystalMaterial->Write("crystalMaterial");

  // Write crystal size
  TVector3* crystalSize = new TVector3(G4Utils::getCrystalX(), G4Utils::getCrystalY(), G4Utils::getCrystalZ());
  crystalSize->Write("crystalSize");

  // Write incident particle name
  TObjString* particleName = new TObjString(G4Utils::getGPSParticleName().c_str());
  particleName->Write("gpsParticleName");

  // Write incident particle energy
  RooConstVar* gpsParticleEnergy = new RooConstVar("gpsParticleEnergy", "GPS particle energy", G4Utils::getGPSMonoEnergy());
  gpsParticleEnergy->Write();

  // Write number of events
  RooConstVar* numberOfEvents = new RooConstVar("numberOfEvents", "Number of Events", G4Utils::getNumberOfEvents());
  numberOfEvents->Write();

  // Write total GPS energy in the file
  // fTotalGPSEnergy->Write("gpsTotalEnergy");

  // Close file
  fRootFile->Write();       // Writing the histograms to the file
  fRootFile->Close();       // and closing the tree (and the file)
  G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;

//  auto manager = G4AnalysisManager::Instance();
//  manager->OpenFile("test.root");
//  manager->CreateNtuple("t", "Geant4 step points");
//  manager->CreateNtupleIColumn("pdg", fPdgVector);
//  manager->FinishNtuple();
//  manager->Write();
//  manager->CloseFile();

  // Save histogram with particles exiting the world to file

//  TH1* hist = new TH1();
//  hist->Draw();
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
  if (fWriteStepPoints){
    fEvtNb = evtNb;

    for (int i = 0; i < MaxNtuple; i++){
      fFluxEne[i] = 0.; fFluxPos_X[i] = 0.; fFluxPos_Y[i] = 0.; fFluxPos_Z[i] = 0.;
    }
    fFluxEne[id] = edep;
    fFluxPos_X[id] = position.x();
    fFluxPos_Y[id] = position.y();
    fFluxPos_Z[id] = position.z();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::setFileNamePattern(G4String fileNamePattern){
  fFileNamePattern = fileNamePattern;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String HistoManager::getFileNamePattern(){
  return fFileNamePattern;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::setWriteStepPoints(G4bool value){
  fWriteStepPoints = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool HistoManager::getWriteStepPoints(){
  return fWriteStepPoints;
}


