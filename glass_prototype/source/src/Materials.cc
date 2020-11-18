/*
 * Materials.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: petrstepanov
 */

#include "Materials.hh"
#include "G4Element.hh"
// #include "G4SIunits.hh"
#include "G4SystemOfUnits.hh"
#include "Elements.hh"
#include <stdlib.h>

Materials::Materials() : fMaterialsList{}{
  // Instantiate elements database
  Elements* elements = Elements::getInstance();

  // Air
  G4Material* Air = new G4Material("Air", 1.205*mg/cm3, 2, kStateGas, 293*kelvin, 1*atmosphere);
  Air->AddElement(elements->getElement("N"), 0.7);
  Air->AddElement(elements->getElement("O"), 0.3);
  fMaterialsList.push_back(Air);

  // Scintillation Material PbWO4
  G4Material* PbWO4 = new G4Material("PbWO4", 8.3*g/cm3, 3);
  PbWO4->AddElement(elements->getElement("Pb"), 1);
  PbWO4->AddElement(elements->getElement("W"), 1);
  PbWO4->AddElement(elements->getElement("O"), 4);
  fMaterialsList.push_back(PbWO4);

  // Scintillation Material SiO2
  G4Material* SiO2 = new G4Material("SiO2", 2.648*g/cm3, 2);
  SiO2->AddElement(elements->getElement("Si"), 1);
  SiO2->AddElement(elements->getElement("O"), 2);
  fMaterialsList.push_back(SiO2);

  // Scintillation material
  G4Material* BaSi2O5= new G4Material("BaSi2O5", 3.8*g/cm3, 3);
  BaSi2O5->AddElement(elements->getElement("Ba"), 1);
  BaSi2O5->AddElement(elements->getElement("Si"), 2);
  BaSi2O5->AddElement(elements->getElement("O"), 5);
  fMaterialsList.push_back(BaSi2O5);

  // VM2000
  G4Material* C10H8O4 = new G4Material("C10H8O4", 1.38*g/cm3, 3);
  C10H8O4->AddElement(elements->getElement("C"), 10);
  C10H8O4->AddElement(elements->getElement("H"), 8);
  C10H8O4->AddElement(elements->getElement("O"), 4);
  fMaterialsList.push_back(C10H8O4);

  // Aluminum
  G4Material* Aluminum = new G4Material("Aluminum", 2.70*g/cm3, 1);
  Aluminum->AddElement(elements->getElement("Al"), 1);
  fMaterialsList.push_back(Aluminum);

  // Frame
  // PS: why frame is made of carbon?
  G4Material* Frame = new G4Material("Frame", 1.55*g/cm3, 1);
  Frame->AddElement(elements->getElement("C"), 1);
  fMaterialsList.push_back(Frame);
}

Materials::~Materials() {
  // TODO Auto-generated destructor stub
}

Materials* Materials::instance = NULL;

G4Material* Materials::getMaterial(const char* materialId){
  for (int i=0; i<(int)fMaterialsList.size(); i++){
    if (fMaterialsList[i]->GetName() == materialId){
      return fMaterialsList[i];
    }
  }
  G4cout << "ERROR: Materials::getMaterial material " << materialId << " not found";
  exit(1);
  return NULL;
}

Materials* Materials::getInstance() {
  if (!instance){
      instance = new Materials;
  }
  return instance;
}

// PS: when having a static std::vector member
//std::vector<G4Material*> Materials::fMaterialsList = {};
