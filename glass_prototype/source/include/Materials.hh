/*
 * Materials.h
 *
 *  Created on: Nov 16, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_MATERIALS_HH_
#define SRC_MATERIALS_HH_

#include "G4Material.hh"
#include <vector>

class Materials {
public:
  virtual ~Materials();
  static Materials* getInstance();

  G4Material* getMaterial(const char*);

  void printMaterialProperties(const char*);
  void printMaterialProperties(G4Material* material);

  void setCrystalLength(G4double crystalLength);

private:
  Materials();
  static Materials* instance;

  void init();

  std::vector<G4Material*> fMaterialsList;
  // List of defined materials
  // static std::vector<G4Material*> fMaterialsList;

  std::vector<G4double> fOpticalPhotonWavelength;
  std::vector<G4double> fOpticalPhotonEnergy;

  std::vector<G4double> calcAbsorptionLength(G4MaterialPropertiesTable* mpt, std::vector<G4double> refractiveIndex, std::vector<G4double> measuredTransmittance);

  void printLine(const char* heading, std::vector<G4double> values, G4double unit = 0.);
  void printHeader();

  const G4int TAB_COLUMN_1 = 25;
  const G4int TAB_COLUMN = 10;

  G4double fCrystalLength;
};

#endif /* SRC_MATERIALS_HH_ */
