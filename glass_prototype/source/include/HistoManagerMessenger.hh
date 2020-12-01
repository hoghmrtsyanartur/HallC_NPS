/*
 * HistoManagerMessenger.h
 *
 *  Created on: Nov 15, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_HISTOMANAGERMESSENGER_HH_
#define SRC_HISTOMANAGERMESSENGER_HH_

#include <G4UImessenger.hh>
#include <HistoManager.hh>
#include <G4UIcmdWithAString.hh>

class HistoManagerMessenger : G4UImessenger {
public:
  HistoManagerMessenger(HistoManager* histoManager);
  virtual ~HistoManagerMessenger();

  void SetNewValue(G4UIcommand * command, G4String newValues);
  G4String GetCurrentValue(G4UIcommand * command);

private:
  HistoManager* fHistoManager;

  G4UIdirectory* fDirectory;
  G4UIcmdWithAString* fileNameCommand;
  G4UIcmdWithAString* writeStepPointsCommand;
};

#endif /* SRC_HISTOMANAGERMESSENGER_HH_ */
