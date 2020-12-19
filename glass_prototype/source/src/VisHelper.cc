/*
 * VisHelper.cc
 *
 *  Created on: Dec 18, 2020
 *      Author: petrstepanov
 */

#include "VisHelper.hh"

#include "G4VisManager.hh"
#include "G4Point3D.hh"
#include "G4Text.hh"
#include "G4Square.hh"
#include "TString.h"
#include "G4Utils.hh"
#include <ostream>

VisHelper* VisHelper::instance = NULL;

VisHelper* VisHelper::getInstance(){
    if (!instance){
        instance = new VisHelper;
    }
    return instance;
}

VisHelper::VisHelper() : lineNumber(0) {
  // TODO Auto-generated constructor stub

}

VisHelper::~VisHelper() {
  // TODO Auto-generated destructor stub
}

void VisHelper::draw2DText(const char *text) {
  G4VVisManager* fVisManager = G4VVisManager::GetConcreteInstance();
  if(!fVisManager) {
    G4cerr << "G4VScoringMesh::DrawColorChart(): no visualization system" << G4endl;
    return;
  }

  fVisManager->BeginDraw2D();


  // Draw text shadow (black)
  // G4double shadowOffset = 0.0035;
  G4int textSize = 14;
  G4double lineHeight = textSize/200.;
  G4double textY = 0.9-lineNumber*lineHeight;

  for(size_t i = 0; i <= strlen(text); i++){
    G4Square* shadow = new G4Square(G4Point3D(-0.9 + i*lineHeight*textSize/33., textY+lineHeight/3, 0));
    shadow->SetScreenSize(textSize*1.5);
    shadow->SetFillStyle(G4VMarker::FillStyle::filled);
    G4VisAttributes att(G4Colour::Black());
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

void VisHelper::drawSatistics(){
  // Write material
  TString sMat = TString::Format("Crystal material:  %s", G4Utils::getCrystalMaterial().c_str());
  draw2DText(sMat.Data());

  // Write crystal size
  TString sSiz = TString::Format("Crystal size:      %.1f x %.1f x %.1f mm", G4Utils::getCrystalX(), G4Utils::getCrystalY(), G4Utils::getCrystalZ());
  draw2DText(sSiz.Data());

  // Write incident particle
  TString sPar = TString::Format("Incident particle: %s, %.0f MeV, %d events", G4Utils::getGPSParticleName().c_str(), G4Utils::getGPSMonoEnergy(), G4Utils::getNumberOfEvents());
  draw2DText(sPar.Data());

  // Total deposited energy
  G4double energyTotalGPSDouble = G4Utils::getGPSMonoEnergy()*G4Utils::getNumberOfEvents();
  TString s1 = TString::Format("Total incident energy, GeV:     %.1f", energyTotalGPSDouble/1000);
  draw2DText(s1.Data());

  // Write total energy in crystals from mesh
  G4double energyTotalCrystalsMeshDouble = G4Utils::getTotalQuantityFromMesh("crystalsMesh", "eneDepCrystal");
  TString s2 = TString::Format("Deposited in crystals, GeV:     %.1f (%.1f %%)", energyTotalCrystalsMeshDouble/1000, energyTotalCrystalsMeshDouble/energyTotalGPSDouble*100);
  draw2DText(s2.Data());

  // Write total energy in pmts from mesh
  G4double energyTotalPMTMeshDouble = G4Utils::getTotalQuantityFromMesh("pmtsMesh", "eneDepPMT");
  TString s3 = TString::Format("Deposited in PMT assembly, GeV: %.1f (%.1f %%)", energyTotalPMTMeshDouble/1000, energyTotalPMTMeshDouble/energyTotalGPSDouble*100);
  draw2DText(s3.Data());

  // Write total energy escaped the world
  TString s4 = TString::Format("Escaped the world, GeV:         %.1f (%.1f %%)", energyTotalCrystalsMeshDouble/1000, energyTotalCrystalsMeshDouble/energyTotalGPSDouble*100);
  draw2DText(s4.Data());
}
