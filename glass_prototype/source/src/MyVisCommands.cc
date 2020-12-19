/*
 * MyVisMessenger.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: petrstepanov
 */

#include "MyVisCommands.hh"
#include "G4Utils.hh"

#include <CLHEP/Vector/ThreeVector.h>
#include <TString.h>
#include <G4Square.hh>
#include <G4Point3D.hh>
using CLHEP::Hep3Vector;

// /myvis/centerviewer

MyVisCommands::MyVisCommands() : G4VVisCommand() {
  // Instantiate command for setting the gap between crystals
  fCenterViewCommand = new G4UIcommand("/myvis/centerViewer", this);
  fCenterViewCommand->SetGuidance("Center the camera on the crystals.");

  // Instantiate command for drawing 2D text
  fDrawText2DCommand = new G4UIcmdWithAString("/myvis/drawText2D", this);
  fDrawText2DCommand->SetGuidance("Draw 2D text over the visualization.");

  // Instantiate command for drawing 2D text
  fDrawStatsCommand = new G4UIcommand("/myvis/drawStats", this);
  fDrawStatsCommand->SetGuidance("Draw energy deposition statistics.");
}

MyVisCommands::~MyVisCommands() {
  delete fCenterViewCommand;
}

void MyVisCommands::SetNewValue(G4UIcommand* cmd, G4String string) {
  if (cmd==fCenterViewCommand){
    G4VViewer* currentViewer = fpVisManager->GetCurrentViewer();
    G4ViewParameters vp = currentViewer->GetViewParameters();

    if (!currentViewer) return;
    // G4OpenGLViewer* pOGLViewer = dynamic_cast<G4OpenGLViewer*>(currentViewer);
    // if (!pOGLViewer) return;

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

    SetViewParameters(currentViewer,vp);
  } else if (cmd==fDrawText2DCommand){
    drawText2D(string);
  } else if (cmd==fDrawStatsCommand){
    // Write material
    TString sMat = TString::Format("Crystal material:  %s", G4Utils::getCrystalMaterial().c_str());
    drawText2D(sMat.Data());

    // Write crystal size
    TString sSiz = TString::Format("Crystal size:      %.1f x %.1f x %.1f mm", G4Utils::getCrystalX(), G4Utils::getCrystalY(), G4Utils::getCrystalZ());
    drawText2D(sSiz.Data());

    // Write incident particle
    TString sPar = TString::Format("Incident particle: %s, %.0f MeV, %d events", G4Utils::getGPSParticleName().c_str(), G4Utils::getGPSMonoEnergy(), G4Utils::getNumberOfEvents());
    drawText2D(sPar.Data());

    // Total deposited energy
    G4double energyTotalGPSDouble = G4Utils::getGPSMonoEnergy()*G4Utils::getNumberOfEvents();
    TString s1 = TString::Format("Total incident energy, GeV:     %.1f", energyTotalGPSDouble/1000);
    drawText2D(s1.Data());

    // Write total energy in crystals from mesh
    G4double energyTotalCrystalsMeshDouble = G4Utils::getTotalQuantityFromMesh("crystalsMesh", "eneDepCrystal");
    TString s2 = TString::Format("Deposited in crystals, GeV:     %.1f (%.1f %%)", energyTotalCrystalsMeshDouble/1000, energyTotalCrystalsMeshDouble/energyTotalGPSDouble*100);
    drawText2D(s2.Data());

    // Write total energy in pmts from mesh
    G4double energyTotalPMTMeshDouble = G4Utils::getTotalQuantityFromMesh("pmtsMesh", "eneDepPMT");
    TString s3 = TString::Format("Deposited in PMT assembly, GeV: %.1f (%.1f %%)", energyTotalPMTMeshDouble/1000, energyTotalPMTMeshDouble/energyTotalGPSDouble*100);
    drawText2D(s3.Data());

    // Write total energy escaped the world
    TString s4 = TString::Format("Escaped the world, GeV:         %.1f (%.1f %%)", energyTotalCrystalsMeshDouble/1000, energyTotalCrystalsMeshDouble/energyTotalGPSDouble*100);
    drawText2D(s4.Data());
  }
}

void MyVisCommands::drawText2D(G4String text){
  G4VVisManager* fVisManager = G4VVisManager::GetConcreteInstance();
  if(!fVisManager) {
    G4cerr << "G4VScoringMesh::DrawColorChart(): no visualization system" << G4endl;
    return;
  }


  static G4int lineNumber = 0;

  // Draw text shadow (black)
  // G4double shadowOffset = 0.0035;

//  G4int visWidth =
  G4int textSize = 14;

  G4double lineHeight = textSize/200.;
  G4double textY = 0.9-lineNumber*lineHeight;

  fVisManager->BeginDraw2D();

  for(size_t i = 0; i <= text.length(); i++){
    G4Square* shadow = new G4Square(G4Point3D(-0.9 + i*lineHeight*textSize/33., textY+lineHeight/3, 0));
    shadow->SetScreenSize(textSize*1.5);
    shadow->SetFillStyle(G4VMarker::FillStyle::filled);
    G4VisAttributes att(G4Colour::Red());
    shadow->SetVisAttributes(&att);
    fVisManager->Draw2D(*shadow);
  }

  // Draw text itself (white)
  G4Text t(text, G4Point3D(-0.9, textY, 0.1));
  t.SetScreenSize(textSize);
  G4VisAttributes attText(G4Colour::White());
  t.SetVisAttributes(&attText);
  fVisManager->Draw2D(t);

  fVisManager->EndDraw2D();

  lineNumber++;
}
