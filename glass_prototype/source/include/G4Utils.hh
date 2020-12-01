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

  static G4ThreeVector* getCrystalSize();
  static G4double getCrystalX();
  static G4double getCrystalY();
  static G4double getCrystalZ();
};

#endif /* SRC_G4UTILS_HH_ */
