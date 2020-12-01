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

private:
  Materials();
  static Materials* instance;

  std::vector<G4Material*> fMaterialsList;
  // List of defined materials
  // static std::vector<G4Material*> fMaterialsList;
};

#endif /* SRC_MATERIALS_HH_ */
