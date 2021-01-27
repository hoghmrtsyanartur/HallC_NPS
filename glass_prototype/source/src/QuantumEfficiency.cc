/*
 * QuantumEfficiency.cpp
 *
 *  Created on: Jan 23, 2021
 *      Author: petrstepanov
 */

#include <QuantumEfficiency.hh>


QuantumEfficiency::QuantumEfficiency(PhotoCathode photoCatode) {
  if (photoCatode == PhotoCathode::Bialkali){
    currentPoints.assign(pointsBialkali.begin(), pointsBialkali.end());
  }
}

QuantumEfficiency::~QuantumEfficiency() {
  // TODO Auto-generated destructor stub
}

G4double QuantumEfficiency::getEfficiency(G4double wavelength){
  if (wavelength < pointsBialkali[0]) return 0;
  if (wavelength >= pointsBialkali[pointsBialkali.size()-2]) return 0;

  for (G4int i = 0; i <= (G4int)currentPoints.size()-4; i+=2){
    if (wavelength >= currentPoints[i] && wavelength < currentPoints[i+2]){
      G4double dLeft = wavelength - currentPoints[i];
      G4double dRight = currentPoints[i+2] - wavelength;
      return currentPoints[i+1] + (currentPoints[i+3] - currentPoints[i+1]) * dLeft / (dLeft+dRight);
    }
  }

  return 0;
}
