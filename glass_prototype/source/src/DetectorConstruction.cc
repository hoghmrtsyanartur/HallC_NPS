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
/// \file hadronic/Hadr03/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
// $Id: DetectorConstruction.cc 70755 2013-06-05 12:17:48Z ihrivnac $
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Box.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4SDManager.hh"
#include "G4AutoDelete.hh"

#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4UnitsTable.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4PVReplica.hh"
#include "G4VisAttributes.hh"//coloring

#include "G4SubtractionSolid.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSEnergyDeposit3D.hh"

#include "Materials.hh"

#include "G4ScoringManager.hh"
#include "G4VScoringMesh.hh"
#include "G4ScoringBox.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  :G4VUserDetectorConstruction(),
   fVacuumMater(0),
   fDetectorMater(0), fLogicDetector(0),
   fLogicCrystal(0),
   fLogicPMT(0), fLogicWrap(0), fLogicPMTcover(0), fLogicWrapFront(0),
   fPMTmater(0), fWrapMater(0), fPMTcoverMater(0), 
   fMom_X(0), fMom_Y(0),
   fWorldMater(0), fPhysiWorld(0), fEDepScorerCrystal(0)
{
  // Default size of crystals (can be overrided in DetectorMessenger)
  fCrystal_X = 20*mm;
  fCrystal_Y = 20*mm;
  fCrystal_Z = 200*mm;

  // Default number of crystals in the assembly
  fCrystalNumX = 3;
  fCrystalNumY = 3;

  // Thickness of the wrap materials
  fWrapThickness = 65*micrometer; // 10*mm;

  // Gap between crystals
  fGap = 0.5*mm;
  fFrame_length = 20*mm; // ? need

  // PMT tube dimensions
  fPMT_radius = 18/2*mm;
  fPMT_length = 88*mm;

  // World size - length depends on the crystal length and PMT length
  fWorld_X = 0.75*meter; // 4.5*m;
  fWorld_Y = fCrystal_Z + fPMT_length + 100*mm; // 4.5*m;
  fWorld_Z = 1.5*meter;   // 10*m;


  DefineMaterials();

  // Custom UImessenger for Detector geometry modification
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructVolumes();

  InitVisScoringManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  G4NistManager* nist = G4NistManager::Instance();
  fVacuumMater = nist->FindOrBuildMaterial("G4_Galactic");

  Materials* materials = Materials::getInstance();
  fWorldMater = materials->getMaterial("Air");
  fPMTmater = materials->getMaterial("SiO2");
  fDetectorMater = materials->getMaterial("BaSi2O5");
  fWrapMater = materials->getMaterial("C10H8O4"); // VM2000
  fPMTcoverMater = materials->getMaterial("MuMetal");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes()
{
  //optical photons properties
  // 
  G4double hc = 1.23984193;
  
  // Petr Stepanov: question what is this 8 value?
  const G4int n = 8;
  G4double OpticalPhotonWavelength[n] = //in micro meters
    { 0.400, 0.440, 0.480, 0.520, 0.560, 0.600, 0.640, 0.680};
  G4double OpticalPhotonEnergy[n] = {0.};
  for (G4int i = 0 ; i < n ; i++){
    OpticalPhotonEnergy[i] = (hc/OpticalPhotonWavelength[i])*eV;
  }

  //Refractive Index of Crystal
  G4double RefractiveIndexCrystal[n] = 
    { 2.35, 2.30, 2.27, 2.25, 2.23, 2.21, 2.201, 2.2}; 

  //Refractive Index of Air
  G4double RefractiveIndexAir[n] = 
    { 1., 1., 1., 1., 1., 1., 1., 1.};

  //Reflectivity. To be filled from down below
  G4double R[n] = {0.};

  //Theoretical Transmittance. To be filled from down below
  G4double Ts[n] = {0.};

  //Measured Transmittance(longitudinal)
  G4double T[n] = 
    { 0.33, 0.48, 0.62, 0.67, 0.68, 0.689, 0.69, 0.69};

  //Attenuation length
  G4double LAL[n] = {0.};


  for (G4int i = 0 ; i < n ; i++){
    R[i] = ((RefractiveIndexCrystal[i] - RefractiveIndexAir[i])*(RefractiveIndexCrystal[i] - RefractiveIndexAir[i]))/((RefractiveIndexCrystal[i] + RefractiveIndexAir[i])*(RefractiveIndexCrystal[i] + RefractiveIndexAir[i]));
    Ts[i] = (1 - R[i])/(1 + R[i]); 
    LAL[i] = fCrystal_Z/(log((T[i]*(1-Ts[i])*(1-Ts[i]))/(sqrt(4*Ts[i]*Ts[i]*Ts[i]*Ts[i] + T[i]*T[i]*(1-Ts[i]*Ts[i])*(1-Ts[i]*Ts[i]))-2*Ts[i]*Ts[i])));
    G4cout<<"n[i]"<<RefractiveIndexCrystal[i]<<"["<<i<<"], "<<"Wavelength[i] : "<<OpticalPhotonWavelength[i]<<"["<<i<<"], "<<"R[i] : "<<R[i]<<"["<<i<<"], "<<"T[i] : "<<T[i]<<"["<<i<<"], "<<"LAL[i] : "<<LAL[i]<<"["<<i<<"]"<<G4endl;  
  }

  G4double ScintilFast[n] =
    {10., 25., 45., 55., 40., 35., 20., 12.};

  G4MaterialPropertiesTable* CrystalOP = new G4MaterialPropertiesTable();
  CrystalOP->AddProperty("RINDEX",       OpticalPhotonEnergy, RefractiveIndexCrystal,n);
  CrystalOP->AddProperty("ABSLENGTH",    OpticalPhotonEnergy, LAL,     n);
  CrystalOP->AddProperty("FASTCOMPONENT",OpticalPhotonEnergy, ScintilFast,     n);
  CrystalOP->AddProperty("SLOWCOMPONENT",OpticalPhotonEnergy, ScintilFast,     n);

  CrystalOP->AddConstProperty("SCINTILLATIONYIELD",0.455*0.5*3.34*15/MeV);//to make 15/MeV arrive at the PMT.
  CrystalOP->AddConstProperty("RESOLUTIONSCALE",1.0);  
  CrystalOP->AddConstProperty("FASTTIMECONSTANT", 13.26*ns);
  CrystalOP->AddConstProperty("SLOWTIMECONSTANT", 412.2*ns);
  CrystalOP->AddConstProperty("YIELDRATIO",0.85);
  CrystalOP->DumpTable();

  fDetectorMater->SetMaterialPropertiesTable(CrystalOP);

  G4double RefractiveIndexPMT[n] =
    {1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50};

  G4double AbsorptionPMT[n] =
    { 1.*m, 1.*m, 1.*m, 1.*m, 1.*m, 1.*m, 1.*m, 1.*m};
  
  G4MaterialPropertiesTable* pmtOP = new G4MaterialPropertiesTable();
  pmtOP->AddProperty("RINDEX",       OpticalPhotonEnergy, RefractiveIndexPMT,n);
  pmtOP->AddProperty("ABSLENGTH",    OpticalPhotonEnergy, AbsorptionPMT,     n);
  pmtOP->DumpTable();

  fPMTmater->SetMaterialPropertiesTable(pmtOP);

  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  
  // START DEFINING THE GEOMETRY
  fCheckOverlaps = true; // activate checking overlaps

  // GEOMETRY: World volume
  G4Box* sWorld = new G4Box("World_sol",0.5*fWorld_X, 0.5*fWorld_Y, 0.5*fWorld_Z); //its size
  G4LogicalVolume* lWorld = new G4LogicalVolume(sWorld,       // its solid
                                                fWorldMater,  // its material
                                                "World_log"); //its name
  fPhysiWorld = new G4PVPlacement(0,                     // no rotation
                                  G4ThreeVector(),       // at (0,0,0)
                                  lWorld,                // its logical volume
                                  "World_pos",           // its name
                                  0,                     // its mother  volume
                                  false,                 // no boolean operation
                                  0,                     // copy number
                                  fCheckOverlaps);       // checking overlaps

  // GEOMETRY: World volume -> Mother volume
  // Refer to picture
  G4double single_X = fCrystal_X + 2*fWrapThickness + fGap;
  G4double single_Y = fCrystal_Y + 2*fWrapThickness + fGap;
  G4double single_Z = fWrapThickness + fCrystal_Z + fPMT_length;

  fMom_X = fCrystalNumX*single_X;  // Petr Stepanov: added variable crystal number
  fMom_Y = fCrystalNumY*single_Y;  // Petr Stepanov: added variable crystal number
  G4double mom_Z = single_Z;

  G4double mom_pos_Z = 0.5*single_Z;

  G4Box* sMother = new G4Box("Mother_sol", 0.5*fMom_X, 0.5*fMom_Y, 0.5*mom_Z);
  G4LogicalVolume* lMother = new G4LogicalVolume(sMother,       // solid
                                                 fWorldMater,   // material
                                                 "Mother_log"); // name
  /*G4VPhysicalVolume* pMother =*/ new G4PVPlacement(0,                // no rotation
                               G4ThreeVector(0, 0, mom_pos_Z),     // translation position (world center)
                               lMother,                            // its logical volume
                               "Mother_pos",                       // its name
                               lWorld,                             // its mother logical volume
                               false,                              // no boolean operation
                               0,                                  // copy number
                               fCheckOverlaps);                    // checking overlaps

  // GEOMETRY: World volume -> Mother volume -> Temperature control box
  G4Box* sTemp = new G4Box("Temp_sol", 0.5*fMom_X, 0.5*fMom_Y, 0.5*mom_Z);
  G4LogicalVolume* lTemp = new G4LogicalVolume(sTemp,       // solid
                                               fWorldMater, // material
                                               "Temp_log"); // name
  /*G4VPhysicalVolume* pTemp =*/ new G4PVPlacement(0,                  // no rotation
                                               G4ThreeVector(),    // same as Mother volume
                                               lTemp,              // its logical volume
                                               "Temp_pos",         // its name
                                               lMother,            // its mother  volume
                                               false,              // no boolean operation
                                               0,                  // copy number
                                               fCheckOverlaps);    // checking overlaps

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume
  G4Box* sSingle = new G4Box("Single_sol", 0.5*single_X, 0.5*single_Y, 0.5*single_Z);
  G4LogicalVolume* lSingle = new G4LogicalVolume(sSingle,       // solid
                                                 fWorldMater,   // material
                                                 "Single_log"); // name
  G4double totalLengthX = fCrystalNumX*single_X;
  G4double minX = -totalLengthX/2 + single_X/2;
  G4double totalLengthY = fCrystalNumY*single_Y;
  G4double minY = -totalLengthY/2 + single_Y/2;
  G4int replicaNumber = 0;
  std::cout << "Single physical volume replicas:" << std::endl;
  for(G4int ly = 0 ; ly < fCrystalNumY ; ly++){
    for(G4int lx = 0 ; lx < fCrystalNumX ; lx++){
      // Physical volumes of same type can share a logical volume
      G4VPhysicalVolume* pSingle =
          new G4PVPlacement(0,                                  // no rotation
                            G4ThreeVector(minX + lx*single_X,   // translation position
                                          minY + ly*single_Y,
                                          0),
                            lSingle,                            // translation position
                            "Single",
                            lTemp, //place the each Singles(which will be containing the cyrstal) inside the Temp control box
                            false,
                            replicaNumber,
                            fCheckOverlaps);
      replicaNumber++;
      std::cout << "  Y: " << ly + 1 << ", X " << lx + 1 << ": " << pSingle << std::endl;
      std::cout << "  copy number: " << pSingle->GetCopyNo() << std::endl;
    }
  }

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume ->
  //           Crystal Volume
  G4Box* sCrystal = new G4Box("Crystal_sol", 0.5*fCrystal_X, 0.5*fCrystal_Y, 0.5*fCrystal_Z);
  fLogicCrystal = new G4LogicalVolume(sCrystal,       // its solid
                                      fDetectorMater, // its material
                                      "Crystal_log"); // its name
  // Float crystals to the face of the Single container
  G4double crystal_pos_Z = -(0.5*single_Z - 0.5*fCrystal_Z - fWrapThickness);
  G4VPhysicalVolume* crystalPos = new G4PVPlacement(0,   //no rotation
                                                    G4ThreeVector(0, 0, crystal_pos_Z), // translation
                                                    fLogicCrystal,           // its logical volume
                                                    "Crystal",               // its name
                                                    lSingle,                 // its parent volume
                                                    false,                   // no boolean operation
                                                    0,                       // copy number
                                                    fCheckOverlaps);         // checking overlaps

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> Crystal wrap
  // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Detector/Geometry/geomSolids.html?highlight=g4subtractionsolid#solids-made-by-boolean-operations
  G4Box* sWrapOuter = new G4Box("Wrap_outer", 0.5*(fCrystal_X + 2*fWrapThickness), 0.5*(fCrystal_Y + 2*fWrapThickness), 0.5*(fCrystal_Z + fWrapThickness));
  G4Box* sWrapInner = new G4Box("Wrap_inner", 0.5*fCrystal_X, 0.5*fCrystal_Y, 0.5*(fCrystal_Z + fWrapThickness));
  G4RotationMatrix* wrapRot = new G4RotationMatrix();
  G4ThreeVector wrapTrans(0, 0, fWrapThickness);
  // sWrap_end_sol = sWrap - sWrap_end
  G4SubtractionSolid* sWrap = new G4SubtractionSolid("Wrap_sol", sWrapOuter, sWrapInner, wrapRot, wrapTrans);
  fLogicWrap = new G4LogicalVolume(sWrap, fWrapMater, "Wrap_log");
  G4double wrap_pos_Z = -(0.5*single_Z - 0.5*(fCrystal_Z + fWrapThickness));
  G4VPhysicalVolume* fWrapPos = new G4PVPlacement(0,
                                G4ThreeVector(0, 0, wrap_pos_Z),
                fLogicWrap,
                "Wrap_pos",
                lSingle,
                false,
                0,
                fCheckOverlaps);

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT
  G4Tubs* sPMT = new G4Tubs("PMT_sol", 0, fPMT_radius, fPMT_length/2, 0, twopi);

  fLogicPMT = new G4LogicalVolume(sPMT, fPMTmater, "PMT_log");
  G4double PMT_pos_Z = 0.5*single_Z - 0.5*fPMT_length;
  G4VPhysicalVolume* fPMTpos = new G4PVPlacement(0,
                                             G4ThreeVector(0, 0, PMT_pos_Z),
                                             fLogicPMT,
                                             "PMT_pos",
                                             lSingle,
                                             false,
                                             0,
                                             fCheckOverlaps);

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT Cover
  G4Box* sPMTcoverOuter = new G4Box("PMTcover_outer", 0.5*(single_X), 0.5*(single_Y), 0.5*(fPMT_length));
  G4Tubs* sPMTcoverInner = new G4Tubs("PMTcover_inner", 0, fPMT_radius, fPMT_length/2 + 1, 0, twopi); // + 1 just in case for better subtraction

  G4RotationMatrix* coverRot = new G4RotationMatrix();
  G4ThreeVector coverTrans(0, 0, 0);
  G4SubtractionSolid* sPMTcover = new G4SubtractionSolid("PMTcover_sol", sPMTcoverOuter, sPMTcoverInner, coverRot, coverTrans);

  fLogicPMTcover = new G4LogicalVolume(sPMTcover, fPMTcoverMater, "PMTcover_log");

  G4VPhysicalVolume* fPMTcoverPos = new G4PVPlacement(0,
                                                      G4ThreeVector(0, 0, PMT_pos_Z),
                                                      fLogicPMTcover,
                                                      "PMTcover_pos",
                                                      lSingle,
                                                      false,
                                                      0,
                                                      fCheckOverlaps);

/*
  G4Box* sFrame_outer = new G4Box("Frame_outer_sol", 0.5*single_X, 0.5*single_Y, 0.5*fFrame_length);
  G4Box* sFrame_inner = new G4Box("Frame_inner_sol", 0.5*(single_X - fGap), 0.5*(single_Y - fGap), 0.5*fFrame_length);
  G4SubtractionSolid* sFrame = new G4SubtractionSolid("Frame_sol", sFrame_outer, sFrame_inner);

  G4LogicalVolume* lFrame = new G4LogicalVolume(sFrame,
            fFrameMater,
            "Frame_log");

  new G4PVPlacement(0,
        G4ThreeVector(0., 0., -(0.5*single_Z - (0.5*fFrame_length + fWrapThickness))),
        lFrame,
        "Frame_pos1",
        lSingle,
        false,
        0,
        fCheckOverlaps);

  new G4PVPlacement(0,
        G4ThreeVector(0., 0., 0.5*single_Z - (0.5*fFrame_length + fPMTcoverThickness + fPMT_length)),
        lFrame,
        "Frame_pos2",
        lSingle,
        false,
        0,
        fCheckOverlaps);
*/

  //
  //opticalsurface
  //
  G4OpticalSurface* opWrapperSurface = new G4OpticalSurface("WrapperSurface");
  opWrapperSurface->SetType(dielectric_LUT);
  opWrapperSurface->SetFinish(polishedvm2000air);
  opWrapperSurface->SetModel(LUT);
  new G4LogicalBorderSurface("WrapperSurface", crystalPos,fWrapPos,opWrapperSurface);
  G4MaterialPropertiesTable* opWS = new G4MaterialPropertiesTable();
  opWS->DumpTable();
  opWrapperSurface->SetMaterialPropertiesTable(opWS);

//  G4OpticalSurface* opWrapperFrontSurface = new G4OpticalSurface("WrapperFrontSurface");
//  opWrapperFrontSurface->SetType(dielectric_LUT);
//  opWrapperFrontSurface->SetFinish(polishedvm2000air);
//  opWrapperFrontSurface->SetModel(LUT);
//  new G4LogicalBorderSurface("WrapperFrontSurface", crystalPos, fWrapFrontPos, opWrapperFrontSurface);
//  G4MaterialPropertiesTable* opWFS = new G4MaterialPropertiesTable();
//  opWFS->DumpTable();
//  opWrapperFrontSurface->SetMaterialPropertiesTable(opWFS);

  G4OpticalSurface* opPMTSurface = new G4OpticalSurface("PMTSurface");
  opPMTSurface->SetType(dielectric_dielectric);
  opPMTSurface->SetFinish(polished);
  opPMTSurface->SetModel(unified);
  new G4LogicalBorderSurface("PMTSurface", crystalPos,fPMTpos,opPMTSurface);
  G4MaterialPropertiesTable* opPS = new G4MaterialPropertiesTable();
  opPS->DumpTable();
  opPMTSurface->SetMaterialPropertiesTable(opPS);

  G4OpticalSurface* opPMTcoverSurface = new G4OpticalSurface("PMTcoverSurface");
  opPMTcoverSurface->SetType(dielectric_metal);
  opPMTcoverSurface->SetFinish(polished);
  opPMTcoverSurface->SetModel(unified);
  const  G4int nEntriesPMTcover = 2;
  G4double PhotonEnergyPMTcover[nEntriesPMTcover] = { 2.30*eV, 3.26*eV}; 
  G4double reflectivityPMTcover[nEntriesPMTcover] = {0., 0.};
  G4double efficiencyPMTcover[nEntriesPMTcover] = {1., 1.};
  new G4LogicalBorderSurface("PMTcoverSurface", fPMTpos,fPMTcoverPos,opPMTcoverSurface);
  G4MaterialPropertiesTable* opPcS = new G4MaterialPropertiesTable();
  opPcS -> AddProperty("REFLECTIVITY",PhotonEnergyPMTcover,reflectivityPMTcover,nEntriesPMTcover);
  opPcS -> AddProperty("EFFICIENCY",PhotonEnergyPMTcover,efficiencyPMTcover,nEntriesPMTcover);  
  opPcS->DumpTable();
  opPMTcoverSurface->SetMaterialPropertiesTable(opPcS);

  // COLORS
  G4VisAttributes* invisible = new G4VisAttributes(false);
  lWorld->SetVisAttributes(invisible);
  lMother->SetVisAttributes(invisible);
  lTemp->SetVisAttributes(invisible);
  lSingle->SetVisAttributes(invisible);

  G4VisAttributes* CrystalVisAtt = new G4VisAttributes(G4Colour::Cyan());
  fLogicCrystal->SetVisAttributes(CrystalVisAtt);

  G4VisAttributes* WrapVisAtt = new G4VisAttributes(G4Colour::White());
  fLogicWrap->SetVisAttributes(WrapVisAtt);

  G4VisAttributes* PMTVisAtt = new G4VisAttributes(G4Colour::Blue());
  fLogicPMT->SetVisAttributes(PMTVisAtt);

  G4VisAttributes* PMTcoverVisAtt = new G4VisAttributes(G4Colour::Green());
  fLogicPMTcover->SetVisAttributes(PMTcoverVisAtt);

  // Initialize the scoring mesh for visualization
  InitVisScoringManager();

  //always return the root volume
  return fPhysiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::ConstructSDandField()
{
//  // sensitive detectors -----------------------------------------------------
//  G4SDManager* SDman = G4SDManager::GetSDMpointer();
//  G4VSensitiveDetector* PbWO4 = new MyB5HadCalorimeterSD("/HadCalorimeter");
//  SDman->AddNewDetector(PbWO4);
//  fLogicCrystal->SetSensitiveDetector(PbWO4);

//  G4VSensitiveDetector* CrystalCover = new CrystalCoverSD("/CrystalCover");
//  SDman->AddNewDetector(CrystalCover);
//  fLogicWrap->SetSensitiveDetector(CrystalCover);

//  G4VSensitiveDetector* CrystalFrontCover
//    = new CrystalFrontCoverSD(SDname="/CrystalFrontCover");
//  SDman->AddNewDetector(CrystalFrontCover);
//  fLogicWrapFront->SetSensitiveDetector(CrystalFrontCover);
//
//  G4VSensitiveDetector* PMTcover = new PMTcoverSD("/PMTcover");
//  SDman->AddNewDetector(PMTcover);
//  fLogicPMTcover->SetSensitiveDetector(PMTcover);

  // Primitive Scorer for Crystal
  G4MultiFunctionalDetector* detectorCrystal = new G4MultiFunctionalDetector("crystal");
  fEDepScorerCrystal = new G4PSEnergyDeposit("edep", 1);
  detectorCrystal->RegisterPrimitive(fEDepScorerCrystal);
  fLogicCrystal->SetSensitiveDetector(detectorCrystal);
  G4SDManager::GetSDMpointer()->AddNewDetector(detectorCrystal);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetDetectorGap(G4double value){
  fGap = value*mm;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double DetectorConstruction::GetDetectorGap(){
  return fGap;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCrystalSize(G4ThreeVector vector){
  fCrystal_X = vector.getX();
  fCrystal_Y = vector.getY();
  fCrystal_Z = vector.getZ();
  std::cout << "DetectorConstruction::SetCrystalSize()" << std::endl;
  std::cout << "- setting crystal size to " << vector.getX() << "x" << vector.getY() << "x" << vector.getZ() << std::endl;
  G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector* DetectorConstruction::GetCrystalSize(){
  return new G4ThreeVector(fCrystal_X,fCrystal_Y,fCrystal_Z);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCrystalMaterial(const char* material){
  Materials* materials = Materials::getInstance();
  fDetectorMater = materials->getMaterial(material);
  // Do we need that?
  // G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String DetectorConstruction::GetCrystalMaterial(){
  return fDetectorMater->GetName();
}

void DetectorConstruction::SetCrystalNumberX(G4int num){
  fCrystalNumX = num;
  G4cout << "DetectorConstruction::SetCrystalNumberX()" << G4endl;
  G4cout << "- setting crystal number along X axis to " << num << G4endl;
  G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}

G4int DetectorConstruction::GetCrystalNumberX(){
  return fCrystalNumX;
}

void DetectorConstruction::SetCrystalNumberY(G4int num){
  fCrystalNumY = num;
  G4cout << "DetectorConstruction::SetCrystalNumberY()" << G4endl;
  G4cout << "- setting crystal number along Y axis to " << num << G4endl;
  G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}

G4int DetectorConstruction::GetCrystalNumberY(){
  return fCrystalNumY;
}

G4VPrimitiveScorer* DetectorConstruction::GetEdepScorer(){
  return fEDepScorerCrystal;
}

//G4TwoVector* DetectorConstruction::GetMomXY(){
//  return new G4TwoVector(fMom_X, fMom_Y);
//}
//
//G4double DetectorConstruction::GetWrapThickness(){
//  return fWrapThickness;
//}

// Petr Stepanov: initialize scoring manager for Interactive visualization
//                of the energy deposition in the crystal and PMT assembly

void DetectorConstruction::InitVisScoringManager(){
  // Instantiate Scoring Manager for visualization
  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();


  // CRYSTALS MESH

  // Define the box Mesh with name "crystalsMesh": /score/create/boxMesh crystalsMesh
  G4VScoringMesh* mesh = scoringManager->FindMesh("crystalsMesh");
  if (!mesh ){
    mesh = new G4ScoringBox("crystalsMesh");
    scoringManager->RegisterScoringMesh(mesh);
  }

  // Set mesh size to wrap around all crystal volumes (and wrap) in XY plane and have length of a crystal in Z
  // Example in macro file: /score/mesh/boxSize 31. 31. 100. mm
  G4double size[3];
  size[0] = (fCrystalNumX*(fCrystal_X + 2*fWrapThickness) + (fCrystalNumX-1)*fGap)/2;
  size[1] = (fCrystalNumY*(fCrystal_Y + 2*fWrapThickness) + (fCrystalNumY-1)*fGap)/2;
  size[2] = (fCrystal_Z + fWrapThickness)/2;
  mesh->SetSize(size);

  // Set number of bins in the Mesh: /score/mesh/nBin 30 30 100
  G4double segmentWidth = 3.6; // mm
  G4int segments[3];
  segments[0] = fCrystalNumX*((fCrystal_X + 2*fWrapThickness + fGap)/segmentWidth);
  segments[1] = fCrystalNumY*((fCrystal_Y + 2*fWrapThickness + fGap)/segmentWidth);
  segments[2] = (fCrystal_Z+fWrapThickness)/segmentWidth;
  mesh->SetNumberOfSegments(segments);

  // Set Mesh position: /score/mesh/translate/xyz 0. 0. 2100. mm
  G4double centerPosition[3];
  centerPosition[0] = 0;
  centerPosition[1] = 0;
  centerPosition[2] = (fWrapThickness + fCrystal_Z)/2;
  mesh->SetCenterPosition(centerPosition);

  // Set Mesh primitive scorer: /score/quantity/energyDeposit eneDep
  G4PSEnergyDeposit* edepScorer = new G4PSEnergyDeposit3D("eneDepCrystal");
  mesh->SetPrimitiveScorer(edepScorer);

  // Close the Mesh: /score/close
  scoringManager->CloseCurrentMesh();


  // PMT MESH

  // Define the box Mesh with name "pmtMesh"
  mesh = scoringManager->FindMesh("pmtsMesh");
  if (!mesh ){
    mesh = new G4ScoringBox("pmtsMesh");
    scoringManager->RegisterScoringMesh(mesh);
  }

  // Set mesh size to wrap around all single volumes in XY plane and have length of a PMT in Z
  G4double size2[3];
  size2[0] = fMom_X/2;
  size2[1] = fMom_Y/2;
  size2[2] = fPMT_length/2;
  mesh->SetSize(size2);

  // Set number of bins in the Mesh: /score/mesh/nBin 30 30 100
  G4int segments2[3];
  segments2[0] = fMom_X/segmentWidth;
  segments2[1] = fMom_Y/segmentWidth;
  segments2[2] = fPMT_length/segmentWidth;
  mesh->SetNumberOfSegments(segments2);

  // Set Mesh position: /score/mesh/translate/xyz 0. 0. 2100. mm
  G4double centerPosition2[3];
  centerPosition2[0] = 0;
  centerPosition2[1] = 0;
  centerPosition2[2] = fWrapThickness + fCrystal_Z + fPMT_length/2;
  mesh->SetCenterPosition(centerPosition2);

  // Set Mesh primitive scorer: /score/quantity/energyDeposit eneDep
  G4PSEnergyDeposit* pmtEdepScorer = new G4PSEnergyDeposit3D("eneDepPMT");
  mesh->SetPrimitiveScorer(pmtEdepScorer);

  // Close the Mesh: /score/close
  scoringManager->CloseCurrentMesh();
}

G4double DetectorConstruction::GetPMTLength() {
  return fPMT_length;
}
