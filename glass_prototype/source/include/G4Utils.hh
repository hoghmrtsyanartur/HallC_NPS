/*
 * G4Utils.hh
 *
 *  Created on: Nov 29, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_G4UTILS_HH_
#define SRC_G4UTILS_HH_

#include "globals.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"

class G4Utils {
public:
  G4Utils();
  virtual ~G4Utils();

  static G4double getGPSMonoEnergy();
  static G4String getGPSParticleName();
  static G4int getNumberOfEvents();
  static G4String getCrystalMaterial();
  static G4double getPmtLength();

  static G4ThreeVector* getCrystalSize();
  static G4double getCrystalX();
  static G4double getCrystalY();
  static G4double getCrystalZ();

  static G4int getNCrystalsX();
  static G4int getNCrystalsY();
  static G4int getNCrystals();

  static G4double getTotalQuantityFromMesh(const char *meshName, const char *psName);
  static G4double getMaximumQuantityFromMesh(const char *meshName, const char *psName);
  static G4double getProjectionZMaximumQuantityFromMesh(const char *meshName, const char *psName);
  static void setMaximumMeshQuantity(const char *meshName, const char *psName, G4double val);

  static void add2DText(const char*);
};

#endif /* SRC_G4UTILS_HH_ */
