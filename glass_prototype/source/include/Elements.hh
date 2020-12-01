/*
 * Elements.h
 *
 *  Created on: Nov 17, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_ELEMENTS_HH_
#define SRC_ELEMENTS_HH_

#include "G4Element.hh"
#include <vector>

class Elements {
public:
  virtual ~Elements();
  static Elements* getInstance();

  G4Element* getElement(const char* symbol);

private:
  Elements();
  static Elements* instance;

  // List of defined materials
  std::vector<G4Element*> fElementsList;
  // static std::vector<G4Element*> fElementsList;
};

#endif /* SRC_ELEMENTS_HH_ */
