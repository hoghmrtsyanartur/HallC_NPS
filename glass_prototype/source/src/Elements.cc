/*
 * Elements.cpp
 *
 *  Created on: Nov 17, 2020
 *      Author: petrstepanov
 */

#include "Elements.hh"
// #include "G4SIunits.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <stdlib.h>

Elements::Elements() : fElementsList{}{
  // Nitrogen
  G4Element* N = new G4Element("Nitrogen", "N", 7, 14.01*g/mole);
  fElementsList.push_back(N);
  // Oxygen
  G4Element* O = new G4Element("Oxygen", "O", 8, 16.00*g/mole);
  fElementsList.push_back(O);

  // Lead
  G4Isotope* Pb204 = new G4Isotope("204Pb", 82, 204, 203.97*g/mole);
  G4Isotope* Pb206 = new G4Isotope("204Pb", 82, 206, 205.97*g/mole);
  G4Isotope* Pb207 = new G4Isotope("204Pb", 82, 207, 206.98*g/mole);
  G4Isotope* Pb208 = new G4Isotope("204Pb", 82, 208, 207.98*g/mole);
  G4Element* Pb = new G4Element("Lead","Pb", 4); // nIsotopes
  Pb->AddIsotope(Pb204, 1.4000*perCent);
  Pb->AddIsotope(Pb206, 24.100*perCent);
  Pb->AddIsotope(Pb207, 22.100*perCent);
  Pb->AddIsotope(Pb208, 52.400*perCent);
  fElementsList.push_back(Pb);

  // Tungsten
  G4Isotope *W180 = new G4Isotope("180W", 74, 180, 179.95*g/mole);
  G4Isotope *W182 = new G4Isotope("182W", 74, 182, 181.95*g/mole);
  G4Isotope *W183 = new G4Isotope("183W", 74, 183, 182.95*g/mole);
  G4Isotope *W184 = new G4Isotope("184W", 74, 184, 183.95*g/mole);
  G4Isotope *W186 = new G4Isotope("186W", 74, 186, 185.95*g/mole);
  G4Element* W  = new G4Element("Tungsten","W" , 5); //Introducimos el Wolframio
  W->AddIsotope(W180, 0.1200*perCent);
  W->AddIsotope(W182, 26.500*perCent);
  W->AddIsotope(W183, 14.310*perCent);
  W->AddIsotope(W184, 30.640*perCent);
  W->AddIsotope(W186, 28.430*perCent);
  fElementsList.push_back(W);

  // Aluminum
  G4Element* Al = new G4Element("Aluminium", "Al", 13, 26.98*g/mole);
  fElementsList.push_back(Al);

  // Silicon
  G4Element* Si = new G4Element("Silicon", "Si", 14, 28.09*g/mole);
  fElementsList.push_back(Si);

  // Barium
  G4Element* Ba = new G4Element("Barium", "Ba", 56, 137.33*g/mole);
  fElementsList.push_back(Ba);

  // Carbon
  G4Element* C  = new G4Element("Carbon", "C", 6, 12.01*g/mole);
  fElementsList.push_back(C);

  // Hydrogen
  G4Element* H  = new G4Element("Hydrogen", "H", 1, 1.008*g/mole);
  fElementsList.push_back(H);
}

Elements::~Elements() {
}

Elements* Elements::instance = NULL;

G4Element* Elements::getElement(const char* symbol){
  for (G4int i=0; i<(int)fElementsList.size(); i++){
    if (fElementsList[i]->GetSymbol() == symbol){
      return fElementsList[i];
    }
  }
  G4cout << "ERROR: Elements::getElement element \"" << symbol << "\" not found";
  exit(1);
  return NULL;
}

Elements* Elements::getInstance() {
  if (!instance){
      instance = new Elements;
  }
  return instance;
}

// PS: when having a static std::vector member
// std::vector<G4Element*> Elements::fElementsList = {};
