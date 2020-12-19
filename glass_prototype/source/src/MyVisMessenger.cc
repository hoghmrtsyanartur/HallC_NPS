/*
 * MyVisMessenger.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: petrstepanov
 */

#include <CLHEP/Vector/ThreeVector.h>
#include <MyVisMessenger.hh>
#include <VisHelper.hh>
#include "G4Utils.hh"

using CLHEP::Hep3Vector;

MyVisMessenger::MyVisMessenger() : G4VVisCommand() {
  // Instantiate command for setting the gap between crystals
  fSaveImageCommand = new G4UIcommand("/vis/viewer/set/centerOnCrystals", this);
  fSaveImageCommand->SetGuidance("Set the gap between the crystals.");
}

MyVisMessenger::~MyVisMessenger() {
  // TODO Auto-generated destructor stub
}

void MyVisMessenger::SetNewValue(G4UIcommand* cmd, G4String /* g4String */) {
  G4VViewer* currentViewer = fpVisManager->GetCurrentViewer();
  G4ViewParameters vp = currentViewer->GetViewParameters();

  if (cmd==fSaveImageCommand){
    if (!currentViewer) return;
  //  G4OpenGLViewer* pOGLViewer = dynamic_cast<G4OpenGLViewer*>(currentViewer);
  //  if (!pOGLViewer) return;

    // Set pivot point
    // Command: /vis/viewer/set/targetPoint 0 0 145 mm
    G4ThreeVector targetPoint(0, 0, (G4Utils::getCrystalZ() + G4Utils::getPmtLength())/2.);
    const G4Point3D& standardTargetPoint = currentViewer->GetSceneHandler()->GetScene()->GetStandardTargetPoint();
    vp.SetCurrentTargetPoint(targetPoint - standardTargetPoint);

    // Set viewing angle
    // Command: /vis/viewer/set/viewpointVector -1 1 -1
    G4ThreeVector viewVector(-1, 1, -1);
    Hep3Vector viewpointVector = viewVector.unit();
    vp.SetViewAndLights(viewpointVector);

    // G4Helper* helper = G4Helper::getInstance();
    // helper->add2DText("asdasd");
  }

  SetViewParameters(currentViewer,vp);
}
