/*
 * G4Utils.cc
 *
 *  Created on: Nov 29, 2020
 *      Author: petrstepanov
 */

#include "G4Utils.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SingleParticleSource.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "DetectorConstruction.hh"
#include "G4ScoringManager.hh"
#include "G4VScoringMesh.hh"
#include <regex>

#include "G4VisManager.hh"
#include "G4Point3D.hh"
#include "G4Text.hh"

G4Utils::G4Utils() {
  // TODO Auto-generated constructor stub

}

G4Utils::~G4Utils() {
  // TODO Auto-generated destructor stub
}

G4double G4Utils::getGPSMonoEnergy() {
  PrimaryGeneratorAction *primaryGeneratorAction =
      (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL)
    return 0;

  // PS: we're setting energy via "/gps/ene/mono". Therefore energy is saved
  // in G4SingleParticleSource (check in G4GeneralParticcleSourcceMessenger) which is a member ofG4GeneralParticleSource
  G4GeneralParticleSource *particleGun = primaryGeneratorAction->GetParticleGun();
  G4SingleParticleSource *particleSource = particleGun->GetCurrentSource();
  return particleSource->GetEneDist()->GetMonoEnergy();
}

G4String G4Utils::getGPSParticleName() {
  PrimaryGeneratorAction *primaryGeneratorAction =
      (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL)
    return "";

  // PS: we're setting energy via "/gps/ene/mono". Therefore energy is saved
  // in G4SingleParticleSource (check in G4GeneralParticcleSourcceMessenger) which is a member ofG4GeneralParticleSource
  G4GeneralParticleSource *particleGun = primaryGeneratorAction->GetParticleGun();
  G4SingleParticleSource *particleSource = particleGun->GetCurrentSource();
  return particleSource->GetParticleDefinition()->GetParticleName();
}

G4int G4Utils::getNumberOfEvents() {
  return (G4RunManager::GetRunManager()->GetCurrentRun())->GetNumberOfEventToBeProcessed();
}

G4String G4Utils::getCrystalMaterial() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return "";
  return detectorConstruction->GetCrystalMaterial();
}

G4ThreeVector* G4Utils::getCrystalSize() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return NULL;
  return detectorConstruction->GetCrystalSize();
}

G4double G4Utils::getCrystalX() {
  G4ThreeVector *vector = getCrystalSize();
  return vector != NULL ? vector->getX() : 0;
}

G4double G4Utils::getCrystalY() {
  G4ThreeVector *vector = getCrystalSize();
  return vector != NULL ? vector->getY() : 0;
}

G4double G4Utils::getCrystalZ() {
  G4ThreeVector *vector = getCrystalSize();
  return vector != NULL ? vector->getZ() : 0;
}

G4int G4Utils::getNCrystalsX() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return 0;
  return detectorConstruction->GetCrystalNumberX();
}

G4int G4Utils::getNCrystalsY() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return 0;
  return detectorConstruction->GetCrystalNumberY();
}

G4int G4Utils::getNCrystals() {
  return getNCrystalsX() * getNCrystalsY();
}

G4double G4Utils::getPmtLength() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return 0;
  return detectorConstruction->GetPMTLength();
}

// Petr Stepanov: Obtain total energy deposited in the Mesh
//                code copied from G4VScoreWriter.cc
G4double G4Utils::getTotalQuantityFromMesh(const char *meshName, const char *psName) {
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh *fScoringMesh = scoringManager->FindMesh(meshName);

  using MeshScoreMap = G4VScoringMesh::MeshScoreMap;

  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();

  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if (msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \"" << psName << "\"." << G4endl;
    return 0;
  }

  std::map<G4int, G4StatDouble*> *score = msMapItr->second->GetMap();

  G4double unitValue = fScoringMesh->GetPSUnitValue(psName);
  G4String unit = fScoringMesh->GetPSUnit(psName);
  G4String divisionAxisNames[3];
  fScoringMesh->GetDivisionAxisNames(divisionAxisNames);

  // Get number of the mesh segments
  G4int fNMeshSegments[3] = { 0 }; // number of segments of the mesh
  fScoringMesh->GetNumberOfSegments(fNMeshSegments);

  G4double fact = 1;
  G4double total = 0;
  G4double grandTotal = 0;
  // G4double totalVal2 = 0;
  // G4double entry = 0;

  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int y = 0; y < fNMeshSegments[1]; y++) {
      for (int z = 0; z < fNMeshSegments[2]; z++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;
        std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);

        if (value == score->end()) {
          total = 0;
          // totalVal2 = 0;
          // entry = 0;
        } else {
          total = (value->second->sum_wx())/unitValue*fact;
          // totalVal2 = (value->second->sum_wx2()) / unitValue / unitValue * fact * fact;
          // entry = value->second->n();
        }

        grandTotal += total;
      }
    }
  }
  return grandTotal;
}

// Petr Stepanov: Obtain total energy deposited in the Mesh
//                code copied from G4VScoreWriter.cc
G4double G4Utils::getProjectionZMaximumQuantityFromMesh(const char *meshName, const char *psName) {
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh *fScoringMesh = scoringManager->FindMesh(meshName);

  using MeshScoreMap = G4VScoringMesh::MeshScoreMap;

  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();

  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if (msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \"" << psName << "\"." << G4endl;
    return 0;
  }

  std::map<G4int, G4StatDouble*> *score = msMapItr->second->GetMap();

  G4double unitValue = fScoringMesh->GetPSUnitValue(psName);
  G4String unit = fScoringMesh->GetPSUnit(psName);
  G4String divisionAxisNames[3];
  fScoringMesh->GetDivisionAxisNames(divisionAxisNames);

  // Get number of the mesh segments
  G4int fNMeshSegments[3] = { 0 }; // number of segments of the mesh
  fScoringMesh->GetNumberOfSegments(fNMeshSegments);

  G4double fact = 1;
  G4double total = 0;
  // G4double grandMax = 0;
  // G4double totalVal2 = 0;
  // G4double entry = 0;

  size_t segmentsX = fNMeshSegments[0];
  size_t segmentsY = fNMeshSegments[1];
  size_t segmentsZ = fNMeshSegments[2];

  // XY projection Maximum
  std::vector<std::vector<G4double>> xy (segmentsX, std::vector<G4double> (segmentsY, 0));
  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int y = 0; y < fNMeshSegments[1]; y++) {
      xy[x][y] = 0;
      for (int z = 0; z < fNMeshSegments[2]; z++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;
        std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);

        if (value == score->end()) {
          total = 0;
        } else {
          total = (value->second->sum_wx())/unitValue*fact;
        }

        xy[x][y] += total;
      }
    }
  }
  Double_t xyMax = 0;
  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int y = 0; y < fNMeshSegments[1]; y++) {
      if (xy[x][y] > xyMax) xyMax = xy[x][y];
    }
  }
  std::cout << "xyMax = " << xyMax << std::endl;

  // YZ projection Maximum
  std::vector<std::vector<G4double>> yz (segmentsY, std::vector<G4double> (segmentsZ, 0));
  for (int y = 0; y < fNMeshSegments[1]; y++) {
    for (int z = 0; z < fNMeshSegments[2]; z++) {
      yz[y][z] = 0;
      for (int x = 0; x < fNMeshSegments[0]; x++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;
        std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);

        if (value == score->end()) {
          total = 0;
        } else {
          total = (value->second->sum_wx())/unitValue*fact;
        }

        yz[y][z] += total;
      }
    }
  }
  Double_t yzMax = 0;
  for (int y = 0; y < fNMeshSegments[1]; y++) {
    for (int z = 0; z < fNMeshSegments[2]; z++) {
      if (yz[y][z] > yzMax) yzMax = yz[y][z];
    }
  }
  std::cout << "yzMax = " << yzMax << std::endl;

  // XZ projection Maximum
  std::vector<std::vector<G4double>> xz (segmentsX, std::vector<G4double> (segmentsZ, 0));
  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int z = 0; z < fNMeshSegments[2]; z++) {
      xz[x][z] = 0;
      for (int y = 0; y < fNMeshSegments[1]; y++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;
        std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);

        if (value == score->end()) {
          total = 0;
        } else {
          total = (value->second->sum_wx())/unitValue*fact;
        }

        xz[x][z] += total;
      }
    }
  }
  Double_t xzMax = 0;
  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int z = 0; z < fNMeshSegments[2]; z++) {
      if (xz[x][z] > xzMax) xzMax = xz[x][z];
    }
  }
  std::cout << "zzMax = " << xzMax << std::endl;

  return std::max(xzMax, yzMax);
}

G4double G4Utils::getMaximumQuantityFromMesh(const char *meshName, const char *psName) {
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh *fScoringMesh = scoringManager->FindMesh(meshName);

  using MeshScoreMap = G4VScoringMesh::MeshScoreMap;

  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();

  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if (msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \"" << psName << "\"." << G4endl;
    return 0;
  }

  G4double unitValue = fScoringMesh->GetPSUnitValue(psName);
  Double_t fact = 1;
  std::map<G4int, G4StatDouble*> *score = msMapItr->second->GetMap();


  G4double max = 0;
  std::map<G4int, G4StatDouble*>::iterator it;

  for (it = score->begin(); it != score->end(); it++){
    Double_t total = (it->second->sum_wx())/unitValue*fact;
    if (total > max) max = total;
  }

  return max;
}

bool G4Utils::replaceSubstring(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
