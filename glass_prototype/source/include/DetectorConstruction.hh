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
/// \file radioactivedecay/rdecay02/include/DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
// $Id: DetectorConstruction.hh 66586 2012-12-21 10:48:39Z ihrivnac $
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "DetectorMessenger.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4TwoVector.hh"
//class G4LogicalVolume;
//class G4Material;
//class DetectorMessenger;
//class SensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  ~DetectorConstruction();

public:
  
  // Mandatory virtual functions
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  // Messenger setters and getters
  void SetDetectorGap(G4double value);
  G4double GetDetectorGap();

  void SetCrystalSize(G4ThreeVector vector);
  G4ThreeVector* GetCrystalSize();

  void SetCrystalMaterial(const char* material);
  G4String GetCrystalMaterial();

  void SetCrystalNumberX(G4int num);
  G4int GetCrystalNumberX();

  void SetCrystalNumberY(G4int num);
  G4int GetCrystalNumberY();
 
  G4double GetPMTLength();

  // Energy deposit getter
  G4VPrimitiveScorer* GetEdepScorer();

private:

  const G4double     inch = 2.54*CLHEP::cm;

  // Materials
  G4Material*          fWorldMater;
  G4Material*          fDetectorMater;
  G4Material*          fGreaseMater;
  G4Material*          fPMTCaseMater;
  G4Material*          fPMTWindowMater;
  G4Material*          fPMTVacuumMater;
  G4Material*          fPMTCathodeMater;
  G4Material*          fWrapMater;
  G4Material*          fPMTcoverMater;

  // Physical Volumes
  G4VPhysicalVolume*   fPhysiWorld;

  // Logical Volumes
  G4LogicalVolume*     fLogicDetector;
  G4LogicalVolume*     fLogicCrystal; //in order to call from ConstructSDandField
  G4LogicalVolume*     fLogicGrease;
  G4LogicalVolume*     fLogicPMTCase;
  G4LogicalVolume*     fLogicPMTWindow;
  G4LogicalVolume*     fLogicPMTVacuum;
  G4LogicalVolume*     fLogicPMTCathode;
  G4LogicalVolume*     fLogicWrap;
  G4LogicalVolume*     fLogicPMTcover;
//  G4LogicalVolume*     fLogicWrapFront;


  G4double             fWorld_X;
  G4double             fWorld_Y;
  G4double             fWorld_Z;

  G4double             fGap;

  G4double             fFrame_length;

  G4double             fCrystal_X;
  G4double             fCrystal_Y;
  G4double             fCrystal_Z;

  G4double             fMom_X;
  G4double             fMom_Y;

  G4int                fCrystalNumX;
  G4int                fCrystalNumY;

  G4double             fWrapThickness;

  G4double             fGreaseThickness;
  G4double             fPMT_window_radius;
  G4double             fPMT_window_thickness;
  G4double             fPMT_case_thickness;
  G4double             fPMT_cathode_radius;
  G4double             fPMT_cathode_thickness;
  G4double             fPMT_cathode_distance;

  G4double             fPMT_length;
                
  DetectorMessenger*   fDetectorMessenger;
  G4VPrimitiveScorer*  fEDepScorerCrystal;

private:
    
  void DefineMaterials();
  void ConstructVolumes();
  void InitVisScoringManager();

  G4bool fCheckOverlaps;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif

