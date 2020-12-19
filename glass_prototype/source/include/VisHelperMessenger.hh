/*
 * VisHelperMessenger.h
 *
 *  Created on: Nov 15, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_VisHelperMessenger_HH_
#define SRC_VisHelperMessenger_HH_

#include <G4UImessenger.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcommand.hh>

class VisHelperMessenger : G4UImessenger {
public:
  VisHelperMessenger();
  virtual ~VisHelperMessenger();

  void SetNewValue(G4UIcommand * command, G4String newValues);

private:

  G4UIdirectory* fDirectory;
  G4UIcommand* draw2dTextCommand;
  G4UIcommand* drawStatsCommand;
};

#endif /* SRC_VisHelperMessenger_HH_ */
