/*
 * G4Helper.cc
 *
 *  Created on: Dec 18, 2020
 *      Author: petrstepanov
 */

#include "G4Helper.hh"

#include "G4VisManager.hh"
#include "G4Point3D.hh"
#include "G4Text.hh"

#include <ostream>

G4Helper* G4Helper::instance = NULL;

G4Helper* G4Helper::getInstance(){
    if (!instance){
        instance = new G4Helper;
    }
    return instance;
}

G4Helper::G4Helper() : lineNumber(0) {
  // TODO Auto-generated constructor stub

}

G4Helper::~G4Helper() {
  // TODO Auto-generated destructor stub
}


void G4Helper::add2DText(const char *text) {
  G4VVisManager* fVisManager = G4VVisManager::GetConcreteInstance();
  if(!fVisManager) {
    G4cerr << "G4VScoringMesh::DrawColorChart(): no visualization system" << G4endl;
    return;
  }

  fVisManager->BeginDraw2D();

  G4Text t(text, G4Point3D(-0.9, 0.9 -(lineNumber++)*0.02, 0.1));
  t.SetScreenSize(12);
  G4VisAttributes att(G4Colour::White());
  t.SetVisAttributes(&att);
  fVisManager->Draw2D(t);

  fVisManager->EndDraw2D();
}
