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

#include "G4LogicalSkinSurface.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  :G4VUserDetectorConstruction(),
   // Materials
   fWorldMater(0),
   fDetectorMater(0),
   fGreaseMater(0),
   fPMTCaseMater(0),
   fPMTWindowMater(0),
   fPMTVacuumMater(0),
   fPMTCathodeMater(0),
   fWrapMater(0),
   fPMTcoverMater(0),
   fPMTreflectorMater(0),

   fMPPCMater(0),
   fMPPCCaseMater(0),


   // Physical Volumes
   fPhysiWorld(0),

   // Logical Volumes
   fLogicDetector(0),
   fLogicCrystal(0),
   fLogicGrease(0),
   fLogicPMTCase(0),
   fLogicPMTWindow(0),
   fLogicPMTVacuum(0),
   fLogicPMTCathode(0),
   fLogicWrap(0),
   fLogicPMTcover(0),
   fLogicPMTReflector(0),

   fLogicMPPC(0),
   fLogicMPPCCase(0),

   fMom_X(0), fMom_Y(0),

   fEDepScorerCrystal(0)
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

  // Optical grease
  fGreaseThickness = 0.2*mm; //0.2*mm;

  // PMT tube dimensions
  fPMT_window_radius = 9*mm;
  fPMT_window_thickness = 1.5*mm;
  fPMT_reflector_thickness = 100*micrometer;
  fPMT_case_thickness = 1.5*mm; // https://www.hamamatsu.com/resources/pdf/etd/High_energy_PMT_TPMZ0003E.pdf
  fPMT_cathode_radius = fPMT_window_radius-fPMT_reflector_thickness;
  fPMT_cathode_distance = 1.5*mm;
  fPMT_cathode_thickness = 20*mm;
  fPMT_length = 88*mm;

  // MPPC dimensions
  fMPPC_size = 6*mm;
  fMPPC_case_thickness = 7*mm;

  // World size - length depends on the crystal length and PMT length
  fWorld_X = 0.75*meter; // 4.5*m;
  fWorld_Y = fCrystal_Z + fPMT_length + 100*mm; // 4.5*m;
  fWorld_Z = 1.5*meter;   // 10*m;

  fCheckOverlaps = true;

  // Default detector type:
  detectorType = DetectorType::PMT;

  // Custom UImessenger for Detector geometry modification
  fDetectorMessenger = new DetectorMessenger(this);

  // Instantiate the materials databse
  Materials* materials = Materials::getInstance();
  materials->setCrystalLength(fCrystal_Z);
  DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction() {
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct() {
  ConstructVolumes();
  return fPhysiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // NIST material database:
  // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html
  fWorldMater = Materials::getInstance()->getMaterial("air");

  fDetectorMater = Materials::getInstance()->getMaterial("BaSi2O5"); // By default
  fWrapMater = Materials::getInstance()->getMaterial("C10H8O4"); // VM2000

  // PMT Cover
  fPMTcoverMater = Materials::getInstance()->getMaterial("MuMetal");

  // Optical grease
  fGreaseMater = Materials::getInstance()->getMaterial("PDMS");

  // PMT case
  fPMTCaseMater = Materials::getInstance()->getMaterial("borosilicate"); // G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");

  // PMT window
  fPMTWindowMater = Materials::getInstance()->getMaterial("borosilicate");

  // PMT vacuum
  fPMTVacuumMater = Materials::getInstance()->getMaterial("vacuum"); // G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

  // PMT Bialkali photocathode - stainless stell substrate https://sci-hub.do/https://doi.org/10.1016/S0168-9002(96)00809-1
  fPMTCathodeMater = Materials::getInstance()->getMaterial("vacuum"); // G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

  // PMT Reflective mirror from UltraDetectorConstruction.cc example
  fPMTreflectorMater = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

  // MPPC material
  fMPPCMater = Materials::getInstance()->getMaterial("vacuum");
  fMPPCCaseMater = G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void  DetectorConstruction::ConstructVolumes() {
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  
  // START DEFINING THE GEOMETRY

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
  G4double single_Z = fWrapThickness + fCrystal_Z + fGreaseThickness + fPMT_length;

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

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> Crystal Volume
  G4Box* sCrystal = new G4Box("Crystal_sol", 0.5*fCrystal_X, 0.5*fCrystal_Y, 0.5*fCrystal_Z);
  fLogicCrystal = new G4LogicalVolume(sCrystal,       // its solid
                                      fDetectorMater, // its material
                                      "Crystal_log"); // its name
  // Float crystals to the face of the Single container
  G4double crystal_pos_Z = -0.5*single_Z + 0.5*fCrystal_Z + fWrapThickness;
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

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> Optical grease
  if (fGreaseThickness > 0){
    G4Tubs* greaseSolid = new G4Tubs("greaseSolid", 0, fPMT_window_radius + fPMT_case_thickness, fGreaseThickness/2, 0, twopi);

    fLogicGrease = new G4LogicalVolume(greaseSolid, fGreaseMater, "greaseLog");
    G4double greasePosZ = 0.5*single_Z - 0.5*fGreaseThickness - fPMT_length;
    /* G4VPhysicalVolume* greasePos = */ new G4PVPlacement(0,
                                  G4ThreeVector(0, 0, greasePosZ),
                                  fLogicGrease,
                                  "greasePhys",
                                  lSingle,
                                  false,
                                  0,
                                  fCheckOverlaps);
  }

  if (detectorType == DetectorType::PMT){

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT
    G4Tubs* sPMTCase = new G4Tubs("PMT_sol", 0, fPMT_window_radius + fPMT_case_thickness, fPMT_length/2, 0, twopi);

    fLogicPMTCase = new G4LogicalVolume(sPMTCase, fPMTCaseMater, "PMT_log");
    G4double PMT_pos_Z = 0.5*single_Z - 0.5*fPMT_length; // Float PMTs to far end
    /* G4VPhysicalVolume* fPMTpos = */ new G4PVPlacement(0,
                                               G4ThreeVector(0, 0, PMT_pos_Z),
                                               fLogicPMTCase,
                                               "PMT_pos",
                                               lSingle,
                                               false,
                                               0,
                                               fCheckOverlaps);

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT -> Window
    G4Tubs* pmtWindowSolid = new G4Tubs("pmtWindowSolid", 0, fPMT_window_radius, fPMT_window_thickness/2, 0, twopi);
    fLogicPMTWindow = new G4LogicalVolume(pmtWindowSolid, fPMTWindowMater, "pmtWindowLog");
    G4double pmtWindowPosZ = - 0.5*fPMT_length + fPMT_window_thickness/2;
    /* G4VPhysicalVolume* fPMTWindowPos = */ new G4PVPlacement(0,
                                                         G4ThreeVector(0, 0, pmtWindowPosZ),
                                                         fLogicPMTWindow,
                                                         "pmtWindowPhys",
                                                         fLogicPMTCase,
                                                         false,
                                                         0,
                                                         fCheckOverlaps);

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT -> Vacuum
    G4double pmtVacuumLength = fPMT_length - fPMT_window_thickness - fPMT_case_thickness;
    G4Tubs* pmtVacuumSolid = new G4Tubs("pmtVacuumSolid", 0, fPMT_window_radius, pmtVacuumLength/2, 0, twopi);
    fLogicPMTVacuum = new G4LogicalVolume(pmtVacuumSolid, fPMTVacuumMater, "pmtVacuumLog");
    G4double pmtVacuumPosZ = - fPMT_length/2 + pmtVacuumLength/2 + fPMT_window_thickness;
    /* G4VPhysicalVolume* fPMTVacuumPos = */ new G4PVPlacement(0,
                                                         G4ThreeVector(0, 0, pmtVacuumPosZ),
                                                         fLogicPMTVacuum,
                                                         "pmtVacuumPhys",
                                                         fLogicPMTCase,
                                                         false,
                                                         0,
                                                         fCheckOverlaps);

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT -> Vacuum -> Reflector
    G4double pmtReflectorLength = fPMT_cathode_distance + fPMT_cathode_thickness;
    G4Tubs* pmtReflectorSolid = new G4Tubs("pmtReflectorSolid", fPMT_window_radius - fPMT_reflector_thickness, fPMT_window_radius, pmtReflectorLength/2, 0, twopi);
    G4double pmtReflectorPosZ = - pmtVacuumLength/2 + pmtReflectorLength/2;

    fLogicPMTReflector = new G4LogicalVolume(pmtReflectorSolid, fPMTreflectorMater, "pmtReflectorLog");
    /* G4VPhysicalVolume* fPMTReflectorPos = */ new G4PVPlacement(0,
                                                         G4ThreeVector(0, 0, pmtReflectorPosZ),
                                                         fLogicPMTReflector,
                                                         "pmtReflectorPhys",
                                                         fLogicPMTVacuum,
                                                         false,
                                                         0,
                                                         fCheckOverlaps);

    // Reflector surface - copied from UltraDetectotConstruction.cc
    G4OpticalSurface* pmtReflectorOpticalSurface = new G4OpticalSurface("ReflectorOpticalSurface");
    pmtReflectorOpticalSurface->SetType(dielectric_metal);
    pmtReflectorOpticalSurface->SetFinish(polished);
    pmtReflectorOpticalSurface->SetModel(unified);

    //    const G4int NUM = 2;
//    G4double lambda_min = 200*nm ;
//    G4double lambda_max = 700*nm ;
//    G4double XX[NUM] = {h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min} ;
//    G4double ICEREFLECTIVITY[NUM] = { 0.95, 0.95 };

    // G4MaterialPropertiesTable *VacuumMirrorMPT = new G4MaterialPropertiesTable();
//    VacuumMirrorMPT->AddProperty("REFLECTIVITY", XX, ICEREFLECTIVITY,NUM);
    // pmtReflectorOpticalSurface->SetMaterialPropertiesTable(VacuumMirrorMPT);

    new G4LogicalSkinSurface("pmtReflectorSkinSurface", fLogicPMTReflector, pmtReflectorOpticalSurface);


    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT -> Vacuum -> Photocathode
    G4Tubs* pmtCathodeSolid = new G4Tubs("pmtCathodeSolid", 0, fPMT_cathode_radius, fPMT_cathode_thickness/2, 0, twopi);;
    fLogicPMTCathode = new G4LogicalVolume(pmtCathodeSolid, fPMTCathodeMater, "pmtCathodeLog");
    G4double pmtCathodePosZ = - pmtVacuumLength/2 + fPMT_cathode_thickness/2 + fPMT_cathode_distance;
    /* G4VPhysicalVolume* fPMTCathodePos = */ new G4PVPlacement(0,
                                                         G4ThreeVector(0, 0, pmtCathodePosZ),
                                                         fLogicPMTCathode,
                                                         "pmtCathodePhys",
                                                         fLogicPMTVacuum,
                                                         false,
                                                         0,
                                                         fCheckOverlaps);

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT Cover
    G4Box* sPMTcoverOuter = new G4Box("PMTcover_outer", 0.5*(single_X), 0.5*(single_Y), 0.5*(fPMT_length));
    G4Tubs* sPMTcoverInner = new G4Tubs("PMTcover_inner", 0, fPMT_window_radius + fPMT_case_thickness, fPMT_length/2 + 1, 0, twopi); // + 1 just in case for better subtraction

    G4RotationMatrix* coverRot = new G4RotationMatrix();
    G4ThreeVector coverTrans(0, 0, 0);
    G4SubtractionSolid* sPMTcover = new G4SubtractionSolid("PMTcover_sol", sPMTcoverOuter, sPMTcoverInner, coverRot, coverTrans);

    fLogicPMTcover = new G4LogicalVolume(sPMTcover, fPMTcoverMater, "PMTcover_log");

    /* G4VPhysicalVolume* fPMTcoverPos = */ new G4PVPlacement(0,
                                                        G4ThreeVector(0, 0, PMT_pos_Z),
                                                        fLogicPMTcover,
                                                        "PMTcover_pos",
                                                        lSingle,
                                                        false,
                                                        0,
                                                        fCheckOverlaps);
  }
  else if (detectorType == DetectorType::MPPC){
    G4Box* mppcCaseSolid = new G4Box("MPPCCase_sol", 0.5*(fMPPC_size + fMPPC_case_thickness*2), 0.5*(fMPPC_size + fMPPC_case_thickness*2), 0.5*(fMPPC_size + fMPPC_case_thickness));
    fLogicMPPCCase = new G4LogicalVolume(mppcCaseSolid, fMPPCCaseMater, "mppcCaseLog");
    G4double mppcCasePosZ = 0.5*single_Z + 0.5*(fMPPC_size + fMPPC_case_thickness) - fPMT_length;
    /* G4VPhysicalVolume* fMPPCPos = */ new G4PVPlacement(0,
                                                         G4ThreeVector(0, 0, mppcCasePosZ),
                                                         fLogicMPPCCase,
                                                         "mppcCasePhys",
                                                         lSingle,
                                                         false,
                                                         0,
                                                         fCheckOverlaps);

    G4Box* mppcSolid = new G4Box("MPPC_sol", 0.5*fMPPC_size, 0.5*fMPPC_size, 0.5*fMPPC_size);
    fLogicMPPC = new G4LogicalVolume(mppcSolid, fMPPCMater, "mppcLog");
    G4double mppcPosZ = -fMPPC_case_thickness/2;
    /* G4VPhysicalVolume* fMPPCPos = */ new G4PVPlacement(0,
                                                         G4ThreeVector(0, 0, mppcPosZ),
                                                         fLogicMPPC,
                                                         "mppcPhys",
                                                         fLogicMPPCCase,
                                                         false,
                                                         0,
                                                         fCheckOverlaps);
  }
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

  // Optical Surface for Crystal and Wrap

  G4OpticalSurface* opWrapperSurface = new G4OpticalSurface("WrapperSurface");
  opWrapperSurface->SetType(dielectric_LUT);
  opWrapperSurface->SetFinish(polishedvm2000air);
  opWrapperSurface->SetModel(LUT);

  new G4LogicalBorderSurface("WrapperSurface", crystalPos, fWrapPos, opWrapperSurface);

  // G4MaterialPropertiesTable* opWS = new G4MaterialPropertiesTable();
  // opWrapperSurface->SetMaterialPropertiesTable(opWS);

  // Optical Surface for Crystal and Grease

//  G4OpticalSurface* opWrapperFrontSurface = new G4OpticalSurface("WrapperFrontSurface");
//  opWrapperFrontSurface->SetType(dielectric_LUT);
//  opWrapperFrontSurface->SetFinish(polishedvm2000air);
//  opWrapperFrontSurface->SetModel(LUT);
//  new G4LogicalBorderSurface("WrapperFrontSurface", crystalPos, fWrapFrontPos, opWrapperFrontSurface);
//  G4MaterialPropertiesTable* opWFS = new G4MaterialPropertiesTable();
//  opWFS->DumpTable();
//  opWrapperFrontSurface->SetMaterialPropertiesTable(opWFS);

//  G4OpticalSurface* opPMTSurface = new G4OpticalSurface("PMTSurface");
//  opPMTSurface->SetType(dielectric_dielectric);
//  opPMTSurface->SetFinish(polished);
//  opPMTSurface->SetModel(unified);
//  new G4LogicalBorderSurface("PMTSurface", crystalPos,fPMTpos,opPMTSurface);
//  G4MaterialPropertiesTable* opPS = new G4MaterialPropertiesTable();
//  opPS->DumpTable();
//  opPMTSurface->SetMaterialPropertiesTable(opPS);

//  G4OpticalSurface* opPMTcoverSurface = new G4OpticalSurface("PMTcoverSurface");
//  opPMTcoverSurface->SetType(dielectric_metal);
//  opPMTcoverSurface->SetFinish(polished);
//  opPMTcoverSurface->SetModel(unified);
//  const  G4int nEntriesPMTcover = 2;
//  G4double PhotonEnergyPMTcover[nEntriesPMTcover] = { 2.30*eV, 3.26*eV};
//  G4double reflectivityPMTcover[nEntriesPMTcover] = {0., 0.};
//  G4double efficiencyPMTcover[nEntriesPMTcover] = {1., 1.};
//  new G4LogicalBorderSurface("PMTcoverSurface", fPMTpos,fPMTcoverPos,opPMTcoverSurface);
//  G4MaterialPropertiesTable* opPcS = new G4MaterialPropertiesTable();
//  opPcS -> AddProperty("REFLECTIVITY",PhotonEnergyPMTcover,reflectivityPMTcover,nEntriesPMTcover);
//  opPcS -> AddProperty("EFFICIENCY",PhotonEnergyPMTcover,efficiencyPMTcover,nEntriesPMTcover);
//  opPcS->DumpTable();
//  opPMTcoverSurface->SetMaterialPropertiesTable(opPcS);

  // COLORS
  G4VisAttributes* invisible = new G4VisAttributes(false);
  lWorld->SetVisAttributes(invisible);
  lMother->SetVisAttributes(invisible);
  lTemp->SetVisAttributes(invisible);
  lSingle->SetVisAttributes(invisible);

  fLogicCrystal->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));

  fLogicWrap->SetVisAttributes(new G4VisAttributes(G4Colour::White()));

  if (fLogicPMTCase) fLogicPMTCase->SetVisAttributes(new G4VisAttributes(G4Colour::Brown()));
  if (fLogicPMTWindow) fLogicPMTWindow->SetVisAttributes(new G4VisAttributes(G4Colour::Yellow()));
  if (fLogicPMTVacuum) fLogicPMTVacuum->SetVisAttributes(new G4VisAttributes(G4Colour::White()));
  if (fLogicPMTCathode) fLogicPMTCathode->SetVisAttributes(new G4VisAttributes(G4Colour::Magenta()));
  if (fLogicPMTcover) fLogicPMTcover->SetVisAttributes(new G4VisAttributes(G4Colour::Gray()));
  if (fLogicPMTReflector) fLogicPMTReflector->SetVisAttributes(new G4VisAttributes(G4Colour::Cyan()));

  if (fLogicMPPC) fLogicMPPC->SetVisAttributes(new G4VisAttributes(G4Colour::Magenta()));
  if (fLogicMPPCCase) fLogicMPPCCase->SetVisAttributes(new G4VisAttributes(G4Colour::Brown()));
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

  // Scoring mesh for energy deposition visualization
  InitVisScoringManager();
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
  if (vector.z() != fCrystal_Z){
    fCrystal_Z = vector.getZ();
    Materials::getInstance()->setCrystalLength(fCrystal_Z);
    DefineMaterials();
  }
  std::cout << "DetectorConstruction::SetCrystalSize()" << std::endl;
  std::cout << "- setting crystal size to " << vector.getX() << "x" << vector.getY() << "x" << vector.getZ() << std::endl;

  // TODO: check if Materials were re-initialized with correct crystal length!
  G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector* DetectorConstruction::GetCrystalSize(){
  return new G4ThreeVector(fCrystal_X,fCrystal_Y,fCrystal_Z);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCrystalMaterial(const char* material){
  fDetectorMater = Materials::getInstance()->getMaterial(material);
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

  // TODO: check if geometry is reinitialized
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

void DetectorConstruction::SetDetectorType(G4String dType) {
  if (dType == "PMT"){
    detectorType = DetectorType::PMT;
  }
  else if (dType == "MPPC"){
    detectorType = DetectorType::MPPC;
  }
}

G4String DetectorConstruction::GetDetectorType() {
  if (detectorType == DetectorType::PMT){
    return "PMT";
  }
  else if (detectorType == DetectorType::MPPC){
    return "MPPC";
  }
  return "";
}

void DetectorConstruction::SetPmtDiameter(G4double diameter){
	fPMT_window_radius = diameter/2;
	fPMT_cathode_radius = fPMT_window_radius-fPMT_reflector_thickness;
}
G4double DetectorConstruction::GetPmtDiameter(){
	return fPMT_window_radius*2;
}

void DetectorConstruction::SetGreaseThickness(G4double thickness){
	fGreaseThickness = thickness;
}
G4double DetectorConstruction::GetGreaseThickness(){
	return fGreaseThickness;
}
