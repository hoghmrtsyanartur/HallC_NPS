//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "PMTcoverSD.hh"
#include "PMTcoverHit.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PMTcoverSD::PMTcoverSD(G4String name)
: G4VSensitiveDetector(name), fHitsCollection(0), fHCID(-1)
{
    collectionName.insert("PMTcoverColl");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PMTcoverSD::~PMTcoverSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PMTcoverSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection 
    = new PMTcoverHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0)
    { fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection); }
  hce->AddHitsCollection(fHCID,fHitsCollection);

  // fill calorimeter hits with zero energy deposition
  for (G4int iColumn=0 ; iColumn<36 ; iColumn++)
    for (G4int iRow=0 ; iRow<30 ; iRow++)
      {
	PMTcoverHit* hit = new PMTcoverHit();
	fHitsCollection->insert(hit);
      }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PMTcoverSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4Track* track = step->GetTrack();
  G4String ParticleName = track->GetDynamicParticle()->GetParticleDefinition()->GetParticleName();
  G4int op = 0;
  if (ParticleName!="opticalphoton") return false;
  else if (ParticleName=="opticalphoton") op = 1;

  G4TouchableHistory* touchable
    = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

  //changed crystal numbering. column by column!
  G4VPhysicalVolume* cellPhysical = touchable->GetVolume(2);
  G4int rowNo = cellPhysical->GetCopyNo();//0~29(30) in total
  G4VPhysicalVolume* columnPhysical = touchable->GetVolume(1);
  G4int columnNo = columnPhysical->GetCopyNo();//0~35(36) in total
  G4int hitID = columnNo+36*rowNo;//0~1079(1080)

  PMTcoverHit* hit = (*fHitsCollection)[hitID];

  // check if it is first touch
  if (hit->GetColumnID()<0)
    {
      hit->SetColumnID(columnNo);
      hit->SetRowID(rowNo);
      G4int depth = touchable->GetHistory()->GetDepth();
      G4AffineTransform transform 
	= touchable->GetHistory()->GetTransform(depth-2);
      transform.Invert();
      hit->SetRot(transform.NetRotation());
      hit->SetPos(transform.NetTranslation());
    }

  hit->AddOPInt(op);

  return true;
}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
