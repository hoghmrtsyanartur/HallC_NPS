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

G4Utils::G4Utils() {
  // TODO Auto-generated constructor stub

}

G4Utils::~G4Utils() {
  // TODO Auto-generated destructor stub
}

G4double G4Utils::getGPSMonoEnergy(){
  PrimaryGeneratorAction* primaryGeneratorAction = (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL) return 0;

  // PS: we're setting energy via "/gps/ene/mono". Therefore energy is saved
  // in G4SingleParticleSource (check in G4GeneralParticcleSourcceMessenger) which is a member ofG4GeneralParticleSource
  G4GeneralParticleSource* particleGun = primaryGeneratorAction->GetParticleGun();
  G4SingleParticleSource* particleSource = particleGun->GetCurrentSource();
  return particleSource->GetEneDist()->GetMonoEnergy();
}

G4String G4Utils::getGPSParticleName(){
  PrimaryGeneratorAction* primaryGeneratorAction = (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL) return "";

  // PS: we're setting energy via "/gps/ene/mono". Therefore energy is saved
  // in G4SingleParticleSource (check in G4GeneralParticcleSourcceMessenger) which is a member ofG4GeneralParticleSource
  G4GeneralParticleSource* particleGun = primaryGeneratorAction->GetParticleGun();
  G4SingleParticleSource* particleSource = particleGun->GetCurrentSource();
  return particleSource->GetParticleDefinition()->GetParticleName();
}

G4int G4Utils::getNumberOfEvents(){
  return (G4RunManager::GetRunManager()->GetCurrentRun())->GetNumberOfEventToBeProcessed();
}

G4String G4Utils::getCrystalMaterial(){
  DetectorConstruction* detectorConstruction = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL) return "";
  return detectorConstruction->GetCrystalMaterial();
}

G4ThreeVector* G4Utils::getCrystalSize(){
  DetectorConstruction* detectorConstruction = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL) return NULL;
  return detectorConstruction->GetCrystalSize();
}

G4double G4Utils::getCrystalX(){
  G4ThreeVector* vector = getCrystalSize();
  return vector != NULL ? vector->getX() : 0;
}

G4double G4Utils::getCrystalY(){
  G4ThreeVector* vector = getCrystalSize();
  return vector != NULL ? vector->getY() : 0;
}

G4double G4Utils::getCrystalZ(){
  G4ThreeVector* vector = getCrystalSize();
  return vector != NULL ? vector->getZ() : 0;
}