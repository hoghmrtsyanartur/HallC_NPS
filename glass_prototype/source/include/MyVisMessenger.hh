/*
 * MyVisMessenger.h
 *
 *  Created on: Dec 17, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_MyVisMessenger_H_
#define SRC_MyVisMessenger_H_

#include "G4VVisCommand.hh"
#include "G4UIcmdWithAString.hh"

// Class inheritance and commands copied from source G4VisCommandsViewerSet.cc

class MyVisMessenger: public G4VVisCommand {
public:
  MyVisMessenger();
  virtual ~MyVisMessenger();

  virtual void SetNewValue(G4UIcommand*, G4String);

private:
  G4UIcommand* fSaveImageCommand;
};

#endif /* SRC_MyVisMessenger_H_ */
