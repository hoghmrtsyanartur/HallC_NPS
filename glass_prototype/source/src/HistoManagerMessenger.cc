/*
 * HistoManagerMessenger.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: petrstepanov
 */

#include "HistoManagerMessenger.hh"


HistoManagerMessenger::HistoManagerMessenger(HistoManager* histoManager) : G4UImessenger(),
  fHistoManager(histoManager) {
  // Instantiate command directory
  fDirectory = new G4UIdirectory("/histoManager/");
  fDirectory->SetGuidance("Histogram Manager control commands.");

  // Instantiate file name pattern command
  fileNameCommand = new G4UIcmdWithAString("/histoManager/setOutputFileNamePattern",this);
  fileNameCommand->SetGuidance("Set output file name pattern.");
  fileNameCommand->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command that controls output of the
  writeStepPointsCommand = new G4UIcmdWithAString("/histoManager/writeStepPoints",this);
  writeStepPointsCommand->SetGuidance("Control output of data related to Step Points.");
  writeStepPointsCommand->AvailableForStates(G4ApplicationState::G4State_PreInit);
}

HistoManagerMessenger::~HistoManagerMessenger() {
}

void HistoManagerMessenger::SetNewValue(G4UIcommand * command, G4String newValues){
  if(command == fileNameCommand){
    fHistoManager->setFileNamePattern(newValues);
  } else if(command == writeStepPointsCommand){
    fHistoManager->setWriteStepPoints(writeStepPointsCommand->ConvertToBool(newValues));
  }
}

G4String HistoManagerMessenger::GetCurrentValue(G4UIcommand * command){
  if (command == fileNameCommand){
    return fHistoManager->getFileNamePattern();
  } else if (command == writeStepPointsCommand){
    return writeStepPointsCommand->ConvertToString(fHistoManager->getWriteStepPoints());
  }
  return "";
}
