/*
 * VisHelperMessenger.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: petrstepanov
 */

#include <VisHelperMessenger.hh>
#include <VisHelper.hh>
#include "VisHelper.hh"

VisHelperMessenger::VisHelperMessenger() : G4UImessenger(){
  // Instantiate command directory
  fDirectory = new G4UIdirectory("/mydirectory/");
  fDirectory->SetGuidance("Test commands.");

  // Instantiate file name pattern command
  draw2dTextCommand = new G4UIcmdWithAString("/visHelper/drawText",this);
  draw2dTextCommand->SetGuidance("Draw 2D text");

  // Instantiate command for drawing 2D statistics
  drawStatsCommand = new G4UIcommand("/visHelper/drawStats",this);
  drawStatsCommand->SetGuidance("Draw statistics");
}

VisHelperMessenger::~VisHelperMessenger() {
}

void VisHelperMessenger::SetNewValue(G4UIcommand * command, G4String newValue){
  VisHelper* helper = VisHelper::getInstance();

  if(command == draw2dTextCommand){
    helper->draw2DText(newValue.c_str());
  } else if (command == drawStatsCommand){
    helper->drawSatistics();
  }
}
