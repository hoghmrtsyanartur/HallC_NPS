/*
 * MyMessenger.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: petrstepanov
 */

#include "MyMessenger.hh"
#include "G4Helper.hh"
#include "G4Utils.hh"

MyMessenger::MyMessenger() : G4UImessenger(){
  // Instantiate command directory
  fDirectory = new G4UIdirectory("/mydirectory/");
  fDirectory->SetGuidance("Test commands.");

  // Instantiate file name pattern command
  myCommand = new G4UIcommand("/mydirectory/draw",this);
  myCommand->SetGuidance("Test draw text.");
}

MyMessenger::~MyMessenger() {
}

void MyMessenger::SetNewValue(G4UIcommand * command, G4String /* newValues */){
  if(command == myCommand){
    G4Helper* helper = G4Helper::getInstance();
    helper->add2DText("Hello World");
    // G4Utils::add2DText("Hello World");
  }
}
