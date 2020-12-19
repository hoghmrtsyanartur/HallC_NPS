/*
 * MyMessenger.h
 *
 *  Created on: Nov 15, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_MyMessenger_HH_
#define SRC_MyMessenger_HH_

#include <G4UImessenger.hh>
#include <G4UIcommand.hh>

class MyMessenger : G4UImessenger {
public:
  MyMessenger();
  virtual ~MyMessenger();

  void SetNewValue(G4UIcommand * command, G4String newValues);

private:

  G4UIdirectory* fDirectory;
  G4UIcommand* myCommand;
};

#endif /* SRC_MyMessenger_HH_ */
