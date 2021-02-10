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
#include "TVector2.h"
#include "TVector3.h"

#include "TSystem.h"

#pragma GCC diagnostic pop

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
  : fFileNamePattern("file_x_%dmm_y_%dmm_z_%dmm_%.1fGeV.root"),
    fWriteStepPoints(false),
    fWriteWorldEscape(false),
    fRootFile(0),
    fNtupleCrystals(0),
    fNtupleOptical(0),
    fNtuplePMT(0),
//    fNtuple_Flux(0),
    fNtupleOutOfWorld(0),
    fPrimaryTime(-999.),
    fPrimaryPID(-999),
    fPrimaryEnergy(-999.),
    fEvtNb(0),
    // PS: use the initializer list for array members (C++11)
    fEdep {0},
    fEdepTotal(0),
		fPE(0),
//    fOP_sc {0},
//    fOP_ce {0},
//    fOP_cover {0},
//    fOP_frontcover {0},
//    fOP_pmtcover {0},
//    fPrimaryPos {-999,-999,-999},
//    fPrimaryMom {-999,-999,-999},
//    fFluxEne {0},
//    fFluxPos_X {0},
//    fFluxPos_Y {0},
//    fFluxPos_Z {0},
    fTotalOutWorldEnergy(0),
    fOutWorldEnergy(0),
    fOutWorldX(0),
    fOutWorldY(0),
    fOutWorldZ(0),
	  fTotalPhotonsPerEvent(0),
		fScintPhotonsPerEvent(0),
		fCherePhotonsPerEvent(0),
    fPdg(0),
    pdgNameMap(new TMap())
//    fTotalGPSEnergy(new RooRealVar("totalGpsEnergy", "Total energy shot from source", 0, "MeV"))
{
  // Custom UImessenger for Detector geometry modification
  fHistoManagerMessenger = new HistoManagerMessenger(this);
//  fPdgVectfOutWorldZor.clear();
}

HistoManager* HistoManager::instance = NULL;

HistoManager* HistoManager::getInstance(){
    if (!instance){
        instance = new HistoManager;
    }
    return instance;
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
  // Create output directory
  gSystem->MakeDirectory("./output");

  // Construct filename
  TString fileName = getFileName();

  fRootFile = new TFile(fileName.Data(), "RECREATE");
  if (!fRootFile) {
    G4cout << "HistoManager::Book: cannot create output file \"" << fileName.Data() << "\"" << G4endl;
    return;
  }
  G4cout << " HistoManager::Book: created output file \"" << fileName.Data() << "\"" << G4endl;

  // Create variable length array for energy deposition
  fEdep = new G4double[G4Utils::getNCrystals()];

  fNtupleCrystals = new TTree("tree_crystals","Energy deposition in crystals");
  // Writing arrays to tree:
  // https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html#cb22
  TString eDepLeafList = TString::Format("fEdep[%d]/D", G4Utils::getNCrystals());
  fNtupleCrystals->Branch("edep", fEdep, eDepLeafList.Data());

  // Create variable length array for energy deposition
  fPE = new G4double[G4Utils::getNCrystals()];

  fNtupleOptical = new TTree("tree_crystals_pe","Tree for optical photons data");
  // Writing arrays to tree:
  // https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html#cb22
  TString peLeafList = TString::Format("fPE[%d]/D", G4Utils::getNCrystals());
  fNtupleOptical->Branch("pe", fPE, peLeafList.Data());
  fNtupleOptical->Branch("total", &fTotalPhotonsPerEvent, "fTotalPhotonsPerEvent/I");
  fNtupleOptical->Branch("cherenkov", &fCherePhotonsPerEvent, "fCherePhotonsPerEvent/I");
  fNtupleOptical->Branch("scintillation", &fScintPhotonsPerEvent, "fScintPhotonsPerEvent/I");

  if (fWriteStepPoints){
    // ... rudimentary Ho San's code
  }

  if (fWriteWorldEscape){
    // Detect energy of the particles that leave the world
    fNtupleOutOfWorld = new TTree("tree_escape","Energy of particles that escape the world");
    fNtupleOutOfWorld->Branch("energy", &fOutWorldEnergy, "fOutWorldEnergy/D");
    fNtupleOutOfWorld->Branch("x", &fOutWorldX, "fOutWorldX/D");
    fNtupleOutOfWorld->Branch("y", &fOutWorldY, "fOutWorldY/D");
    fNtupleOutOfWorld->Branch("z", &fOutWorldZ, "fOutWorldZ/D");
    fNtupleOutOfWorld->Branch("pdg", &fPdg, "Particle ID /I");
    // G4cout << "\n----> Output file is open in " << fFileName << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillNtupleOutOfWorld(G4double energy, G4double x, G4double y, G4double z, G4int pdg, const char* particleName){
  fTotalOutWorldEnergy += energy;
  if (fWriteWorldEscape){
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
    // fPdgVector.push_back(pdg); // Save pdg to vector

    fNtupleOutOfWorld->Fill();

    // Save pair of particle pdg and name. Will write this to file later.
    TString pdgString = TString::Format("%d", pdg);
    TObjString* key = new TObjString(pdgString.Data());
    TObjString* value = new TObjString(particleName);
    if (pdgNameMap->GetValue(pdgString.Data())==NULL){
      pdgNameMap->Add(key, value);
    }
  }
}

void HistoManager::FillNtupleEnergyDep(G4double* energyDeposition){
  // Copy energy deposition values
  for (G4int i = 0; i < G4Utils::getNCrystals(); i++){
    fEdep[i] = energyDeposition[i];
    fEdepTotal += energyDeposition[i];
  }
  // Save pair of particle pdg and name. Will write this to file later.
  fNtupleCrystals->Fill();
}

void HistoManager::FillNtupleOptical(G4double* peNumber, G4int totalOptical, G4int scintOptical, G4int cherenkOptical){
  // Copy energy deposition values
  for (G4int i = 0; i < G4Utils::getNCrystals(); i++){
    fPE[i] = peNumber[i];
  }
  // Copy optical photon numbers
  fTotalPhotonsPerEvent = totalOptical;
  fScintPhotonsPerEvent = scintOptical;
  fCherePhotonsPerEvent = cherenkOptical;

  // Save pair of particle pdg and name. Will write this to file later.
  fNtupleOptical->Fill();
}

//void HistoManager::FillNtuple()
//{
//  fNtuple->Fill();
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//void HistoManager::FillNtuple_Flux()
//{
//  if (fWriteStepPoints){
    // fNtuple_Flux->Fill();
//  }
//}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Save()
{
  if (! fRootFile) return;

  // Write particle PDG names
  // Save collection as a single object, otherwise each key will be saved independently
  // pdgNameMap->Print();
  pdgNameMap->Write("pdgNameMap", TObject::kSingleKey);

  // Write material
  std::cout << "Crystal material: " << G4Utils::getCrystalMaterial() << std::endl;
  TObjString* crystalMaterial = new TObjString(G4Utils::getCrystalMaterial().c_str());
  crystalMaterial->Write("crystalMaterial");

  // Write crystal size
  std::cout << "Crystal size: " << G4Utils::getCrystalX() << "x" << G4Utils::getCrystalY() << "x" << G4Utils::getCrystalZ() << std::endl;
  TVector3* crystalSize = new TVector3(G4Utils::getCrystalX(), G4Utils::getCrystalY(), G4Utils::getCrystalZ());
  crystalSize->Write("crystalSize");

  // Write crystal assembly size (1x1, 3x3, 5x5)
  std::cout << "Crystal assembly size: " << G4Utils::getNCrystalsX() << "x" << G4Utils::getNCrystalsY() << std::endl;
  TVector2* nCrystals = new TVector2(G4Utils::getNCrystalsX(), G4Utils::getNCrystalsY());
  nCrystals->Write("crystalsNumber");

  // Write incident particle name
  std::cout << "Incident particle name: " << G4Utils::getGPSParticleName() << std::endl;
  TObjString* particleName = new TObjString(G4Utils::getGPSParticleName().c_str());
  particleName->Write("gpsParticleName");

  // Write incident particle energy
  std::cout << "Incident particle energy, MeV: " << G4Utils::getGPSMonoEnergy() << std::endl;
  RooConstVar* energyParticle = new RooConstVar("gpsParticleEnergy", "Incident particle energy, MeV", G4Utils::getGPSMonoEnergy());
  energyParticle->Write();

  // Write number of events
  std::cout << "Number of events: " << G4Utils::getNumberOfEvents() << std::endl;
  RooConstVar* numberOfEvents = new RooConstVar("numberOfEvents", "Number of events", G4Utils::getNumberOfEvents());
  numberOfEvents->Write();

  std::cout << std::fixed;

  // Write total energy from GPS
  G4double energyTotalGPSDouble = G4Utils::getGPSMonoEnergy()*G4Utils::getNumberOfEvents();
  std::cout << "Total GPS energy, GeV: " << std::setprecision(0) << energyTotalGPSDouble/1000 << std::endl;
  RooConstVar* energyTotalGPS = new RooConstVar("energyGPSTotal", "Total energy from GPS, MeV", energyTotalGPSDouble);
  energyTotalGPS->Write();

  // Write total energy in crystals from scorer
  // std::cout << "Total energy in crystals from scorer, GeV: " << std::setprecision(1) << fEdepTotal/1000 << std::endl;
  RooConstVar* energyTotalCrystalsScorer = new RooConstVar("energyTotalCrystalsScorer", "Total energy in crystals from scorer, MeV", fEdepTotal);
  energyTotalCrystalsScorer->Write();

  // Write total energy in crystals from mesh
  G4double energyTotalCrystalsMeshDouble = G4Utils::getTotalQuantityFromMesh("crystalsMesh", "eneDepCrystal");
  std::cout << "In crystals, GeV: " << std::setprecision(1) << energyTotalCrystalsMeshDouble/1000 << " (" << std::setprecision(0) << energyTotalCrystalsMeshDouble/energyTotalGPSDouble*100 << " %)" << std::endl;
  RooConstVar* energyTotalCrystalsMesh = new RooConstVar("energyTotalCrystalsMesh", "Total energy in crystals from mesh, MeV", energyTotalCrystalsMeshDouble);
  energyTotalCrystalsMesh->Write();

  // Write total energy in PMTs from mesh
  G4double energyTotalPMTMeshDouble = G4Utils::getTotalQuantityFromMesh("pmtsMesh", "eneDepPMT");
  std::cout << "In PMTs, GeV: " << std::setprecision(1) << energyTotalPMTMeshDouble/1000 << " (" << std::setprecision(0) << energyTotalPMTMeshDouble/energyTotalGPSDouble*100 << " %)" << std::endl;
  RooConstVar* energyTotalPMTsMesh = new RooConstVar("energyTotalPMTsMesh", "Total energy in PMTs from mesh, MeV", energyTotalPMTMeshDouble);
  energyTotalPMTsMesh->Write();

  // Write total energy escaped the world
  std::cout << "Escaped the world, GeV: " << std::setprecision(1) << fTotalOutWorldEnergy/1000 << " (" << std::setprecision(0) << fTotalOutWorldEnergy/energyTotalGPSDouble*100 << " %)" << std::endl;
  RooConstVar* energyTotalOutWorld = new RooConstVar("energyTotalOutWorld", "Total energy escaped the world, MeV", fTotalOutWorldEnergy);
  energyTotalOutWorld->Write();

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
//void HistoManager::SetPrimaryParticle(G4double time, G4int PID, G4ThreeVector pos, G4ThreeVector mom, G4double energy){
//  fPrimaryTime = time;
//  fPrimaryPID = PID;
//  fPrimaryPos[0] = pos.x();
//  fPrimaryPos[1] = pos.y();
//  fPrimaryPos[2] = pos.z();
//  fPrimaryMom[0] = mom.x();
//  fPrimaryMom[1] = mom.y();
//  fPrimaryMom[2] = mom.z();
//  fPrimaryEnergy = energy;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void HistoManager::SetEnergy(G4int id, G4double edep, G4int sc, G4int ce, G4int opc, G4int opfc, G4int oppc){
//  fEdep[id] = edep;
//  fOP_sc[id] = sc;
//  fOP_ce[id] = ce;
//  fOP_cover[id] = opc;
//  fOP_frontcover[id] = opfc;
//  fOP_pmtcover[id] = oppc;
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// void HistoManager::SetFluxEnergy(G4int evtNb, G4int id, G4double edep, G4ThreeVector position) {
//  if (fWriteStepPoints){
//    fEvtNb = evtNb;
//
//    for (int i = 0; i < MaxNtuple; i++){
//      fFluxEne[i] = 0.; fFluxPos_X[i] = 0.; fFluxPos_Y[i] = 0.; fFluxPos_Z[i] = 0.;
//    }
//    fFluxEne[id] = edep;
//    fFluxPos_X[id] = position.x();
//    fFluxPos_Y[id] = position.y();
//    fFluxPos_Z[id] = position.z();
// }
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::setFileNamePattern(G4String fileNamePattern){
  fFileNamePattern = fileNamePattern;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String HistoManager::getFileNamePattern(){
  return fFileNamePattern;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String HistoManager::getFileName(){
  // Construct filename
  G4String crystalMaterial = G4Utils::getCrystalMaterial();
  G4int numX = G4Utils::getNCrystalsX();
  G4int numY = G4Utils::getNCrystalsY();
  G4double x = G4Utils::getCrystalX();
  G4double y = G4Utils::getCrystalY();
  G4double z = G4Utils::getCrystalZ();
  G4double gunEnergy = G4Utils::getGPSMonoEnergy();
  G4int numberOfEvents = G4Utils::getNumberOfEvents();
  TString fileName = TString::Format(fFileNamePattern.c_str(), crystalMaterial.c_str(), numX, numY, x, y, z, gunEnergy/1E3, numberOfEvents);
  TString outputFileNamePath = "./output/" + fileName;
  G4String fileNameString = G4String(outputFileNamePath.Data());
  return fileNameString;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::setWriteStepPoints(G4bool value){
  fWriteStepPoints = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool HistoManager::getWriteStepPoints(){
  return fWriteStepPoints;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::setWriteWorldEscape(G4bool value){
  fWriteWorldEscape = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool HistoManager::getWriteWorldEscape(){
  return fWriteWorldEscape;
}

G4double HistoManager::getTotalWorldOutEnergy() {
  return fTotalOutWorldEnergy;
}
