/*
 * Materials.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: petrstepanov
 */

#include "Materials.hh"
#include "G4Element.hh"
// #include "G4SIunits.hh"
#include "G4SystemOfUnits.hh"
#include "Elements.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include <stdlib.h>

Materials::Materials() : fMaterialsList{},
                         fOpticalPhotonWavelength{400., 440., 480., 520., 560., 600., 640., 680.},
                         fOpticalPhotonEnergy{},
                         fCrystalLength(0)
{
  // Create array with photon energies (for optical material properties)
  for (G4int i = 0; i < (G4int)fOpticalPhotonWavelength.size(); i++){
   G4double hc = 1239.84193;
   fOpticalPhotonEnergy.push_back(hc/fOpticalPhotonWavelength[i]*eV); // E (eV) = 1239.8 / l (nm)
  }
}

void Materials::setCrystalLength(G4double crystalLength){
  fCrystalLength = crystalLength;
  init();
}

void Materials::init(){
  fMaterialsList.clear();
  // Instantiate elements database
  Elements* elements = Elements::getInstance();

  // VM2000
  G4Material* C10H8O4 = new G4Material("C10H8O4", 1.38*g/cm3, 3);
  C10H8O4->AddElement(elements->getElement("C"), 10);
  C10H8O4->AddElement(elements->getElement("H"), 8);
  C10H8O4->AddElement(elements->getElement("O"), 4);
  fMaterialsList.push_back(C10H8O4);

  // Aluminum
  G4Material* Aluminum = new G4Material("Aluminum", 2.70*g/cm3, 1);
  Aluminum->AddElement(elements->getElement("Al"), 1);
  fMaterialsList.push_back(Aluminum);

  // Frame
  // PS: why frame is made of carbon?
  G4Material* Frame = new G4Material("Frame", 1.55*g/cm3, 1);
  Frame->AddElement(elements->getElement("C"), 1);
  fMaterialsList.push_back(Frame);

  // Mu-metal
  // Wikipedia: composition is approximately 77% nickel, 16% iron, 5% copper, and 2% chromium or molybdenum
  // http://www.mu-metal.com/technical-data.html
  G4Material* MuMetal = new G4Material("MuMetal", 8.7 * g/cm3, 4);
  MuMetal->AddElement(elements->getElement("Ni"), 77 * perCent);
  MuMetal->AddElement(elements->getElement("Fe"), 16 * perCent);
  MuMetal->AddElement(elements->getElement("Cu"), 5 * perCent);
  MuMetal->AddElement(elements->getElement("Cr"), 2 * perCent);
  fMaterialsList.push_back(MuMetal);

  // ASCII font: https://fsymbols.com/generators/carty/
  G4int n = (G4int)fOpticalPhotonWavelength.size();

  // █░█ ▄▀█ █▀▀ █░█ █░█ █▀▄▀█
  // ▀▄▀ █▀█ █▄▄ █▄█ █▄█ █░▀░█

//  G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
//
//  // Refractive Index of Vacuum
//  std::vector<G4double> refractiveIndexVacuum = { 1., 1., 1., 1., 1., 1., 1., 1.};
//  assert((G4int)refractiveIndexVacuum.size() == (G4int)fOpticalPhotonEnergy.size());
//
//  // Absorption length of Vacuum
//  std::vector<G4double> absorptionLengthVacuum  = {1000*m, 1000*m, 1000*m, 1000*m, 1000*m, 1000*m, 1000*m, 1000*m};
//  assert((G4int)absorptionLengthVacuum.size() == (G4int)fOpticalPhotonEnergy.size());
//
//  G4MaterialPropertiesTable* vacuumMPT = new G4MaterialPropertiesTable();
//  vacuumMPT->AddProperty("RINDEX", fOpticalPhotonEnergy.data(), refractiveIndexVacuum.data(), n);
//  vacuumMPT->AddProperty("ABSLENGTH", fOpticalPhotonEnergy.data(), absorptionLengthVacuum.data(), n);
//  vacuumMPT->DumpTable();
//  vacuum->SetMaterialPropertiesTable(vacuumMPT);
//
//  printMaterialProperties(vacuum);
//  fMaterialsList.push_back(vacuum);

  // ▄▀█ █ █▀█
  // █▀█ █ █▀▄

  G4Material* air = new G4Material("air", 1.205*mg/cm3, 2, kStateGas, 293*kelvin, 1*atmosphere);
  air->AddElement(elements->getElement("N"), 0.7);
  air->AddElement(elements->getElement("O"), 0.3);

  // Refractive Index of Air
  // https://refractiveindex.info/?shelf=other&book=air&page=Ciddor
  std::vector<G4double> refractiveIndexAir = { 1.00028276, 1.00028091, 1.00027954, 1.00027848, 1.00027765, 1.00027698, 1.00027644, 1.00027600};
  assert((G4int)refractiveIndexAir.size() == (G4int)fOpticalPhotonEnergy.size());

  // Absorption length of Air
  // https://web.physik.rwth-aachen.de/~hebbeker/theses/dietz-laursonn_phd.pdf
  std::vector<G4double> absorptionLengthAir  = {50*m, 50*m, 50*m, 50*m, 50*m, 50*m, 50*m, 50*m};
  assert((G4int)absorptionLengthAir.size() == (G4int)fOpticalPhotonEnergy.size());

  G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
  airMPT->AddProperty("RINDEX", fOpticalPhotonEnergy.data(), refractiveIndexAir.data(), n);
  airMPT->AddProperty("ABSLENGTH", fOpticalPhotonEnergy.data(), absorptionLengthAir.data(), n);
  airMPT->DumpTable();
  air->SetMaterialPropertiesTable(airMPT);

  printMaterialProperties(air);
  fMaterialsList.push_back(air);

  // █▀█ █▀▄ █▀▄▀█ █▀   █▀▀ █▀█ █▀▀ ▄▀█ █▀ █▀▀
  // █▀▀ █▄▀ █░▀░█ ▄█   █▄█ █▀▄ ██▄ █▀█ ▄█ ██▄

  // Some documentation on the polydimethylsiloxane grease: https://arxiv.org/pdf/1305.3010.pdf
  G4Material* grease = new G4Material("PDMS", 0.97*g/cm3, 4);
  grease->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 2);
  grease->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 6);
  grease->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 1);
  grease->AddElement(G4NistManager::Instance()->FindOrBuildElement("Si"), 1);

  // Data digitized from: https://sci-hub.do/https://doi.org/10.1116/1.5046735
  // Or here: https://refractiveindex.info/?shelf=organic&book=polydimethylsiloxane&page=Schneider-RTV615
  // Optical photon wavelength:                 {400.,   440.,   480.,   520.,   560.,   600.,   640.,   680.}
  std::vector<G4double> refractiveIndexGrease = {1.4478, 1.4420, 1.4377, 1.4344, 1.4318, 1.4297, 1.4280, 1.4266};
  assert((G4int)refractiveIndexGrease.size() == (G4int)fOpticalPhotonEnergy.size());

  // Absorption coefficient 0.15344 cm-1 => 6.517205422 cm
  // https://refractiveindex.info/?shelf=organic&book=polydimethylsiloxane&page=Querry-NIR
  std::vector<G4double> absorptionLengthGrease = {6.517*cm, 6.517*cm, 6.517*cm, 6.517*cm, 6.517*cm, 6.517*cm, 6.517*cm, 6.517*cm};
  assert((G4int)absorptionLengthGrease.size() == (G4int)fOpticalPhotonEnergy.size());

  G4MaterialPropertiesTable* greaseMPT = new G4MaterialPropertiesTable();
  greaseMPT->AddProperty("RINDEX", fOpticalPhotonEnergy.data(), refractiveIndexGrease.data(), n);
  greaseMPT->AddProperty("ABSLENGTH", fOpticalPhotonEnergy.data(), absorptionLengthGrease.data(), n);
  greaseMPT->DumpTable();
  grease->SetMaterialPropertiesTable(greaseMPT);

  printMaterialProperties(grease);
  fMaterialsList.push_back(grease);

  // █▀█ █▀▄▀█ ▀█▀   █░█░█ █ █▄░█ █▀▄ █▀█ █░█░█
  // █▀▀ █░▀░█ ░█░   ▀▄▀▄▀ █ █░▀█ █▄▀ █▄█ ▀▄▀▄▀

  // ~ 90% SiO2, 10% B2O3: https://en.wikipedia.org/wiki/Borosilicate_glass
  G4Material* SiO2 = G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4Material* B2O3 = G4NistManager::Instance()->FindOrBuildMaterial("G4_BORON_OXIDE");
  G4Material* borosilicate = new G4Material("borosilicate", 2.23*g/cm3, 2);
  borosilicate->AddMaterial(SiO2, 0.9);
  borosilicate->AddMaterial(B2O3, 0.1);

  // https://www.hamamatsu.com/resources/pdf/etd/PMT_handbook_v3aE.pdf
  // Borosilicate window with Bialkali photocathode work within 300-650 nm range, pp.35
  // https://www.hamamatsu.com/jp/en/product/optical-sensors/photodiodes/si-photodiodes/si-photodiode-faq/index.html
  std::vector<G4double> refractiveIndexBorosilicate = {1.51, 1.51, 1.51, 1.51, 1.51, 1.51, 1.51, 1.51};
  assert((G4int)refractiveIndexBorosilicate.size() == (G4int)fOpticalPhotonEnergy.size());

  // https://refractiveindex.info/?shelf=glass&book=SUMITA-BK&page=K-BK7
  // alpha = 0.0034706 cm-1 => 1/0.0034706 = 288 cm
  std::vector<G4double> absorptionLengthBorosilicate = {288.*cm, 288.*cm, 288.*cm, 288.*cm, 288.*cm, 288.*cm, 288.*cm, 288.*cm};
  assert((G4int)absorptionLengthBorosilicate.size() == (G4int)fOpticalPhotonEnergy.size());

  G4MaterialPropertiesTable* borosilicateMPT = new G4MaterialPropertiesTable();
  borosilicateMPT->AddProperty("RINDEX", fOpticalPhotonEnergy.data(), refractiveIndexBorosilicate.data(), n);
  borosilicateMPT->AddProperty("ABSLENGTH", fOpticalPhotonEnergy.data(), absorptionLengthBorosilicate.data(), n);
  borosilicateMPT->DumpTable();
  borosilicate->SetMaterialPropertiesTable(borosilicateMPT);

  printMaterialProperties(borosilicate);
  fMaterialsList.push_back(borosilicate);

  // █▀█ █▄▄ █░█░█ █▀█ █░█
  // █▀▀ █▄█ ▀▄▀▄▀ █▄█ ▀▀█

  G4Material* PbWO4 = new G4Material("PbWO4", 8.3*g/cm3, 3);
  PbWO4->AddElement(elements->getElement("Pb"), 1);
  PbWO4->AddElement(elements->getElement("W"), 1);
  PbWO4->AddElement(elements->getElement("O"), 4);

  G4MaterialPropertiesTable* PbWO4MPT = new G4MaterialPropertiesTable();

  // PbWO4 refractive index
  std::vector<G4double> refractiveIndexPbWO4 = { 2.35, 2.30, 2.27, 2.25, 2.23, 2.21, 2.201, 2.2};
  assert((G4int)refractiveIndexPbWO4.size() == (G4int)fOpticalPhotonEnergy.size());

  // PbWO4 measured longitudinal transmittance (I/I_0)
  // https://sci-hub.do/https://www.sciencedirect.com/science/article/abs/pii/0168900296002860
  std::vector<G4double> transmittancePbWO4 = { 0.33, 0.48, 0.62, 0.67, 0.68, 0.689, 0.69, 0.69};
  assert((G4int)transmittancePbWO4.size() == (G4int)fOpticalPhotonEnergy.size());

  // Absorption length PbWO4 - calculated https://sci-hub.do/https://doi.org/10.1016/0168-9002(93)91185-P
  // TODO: compare with https://sci-hub.do/https://doi.org/10.1016/S0168-9002(96)01016-9
  std::vector<G4double> absLengthPbWO4 = calcAbsorptionLength(PbWO4MPT, refractiveIndexPbWO4, transmittancePbWO4);
  assert((G4int)absLengthPbWO4.size() == (G4int)fOpticalPhotonEnergy.size());

  // Energy spectrum for the fast scintillation component PbWO4
  std::vector<G4double> scintilFastPbWO4 = {10., 25., 45., 55., 40., 35., 20., 12.};

  // Energy spectrum for the slow scintillation component PbWO4
  std::vector<G4double> scintilSlowPbWO4 = {10., 25., 45., 55., 40., 35., 20., 12.};

  PbWO4MPT->AddProperty("RINDEX",        fOpticalPhotonEnergy.data(), refractiveIndexPbWO4.data(),n);
  PbWO4MPT->AddProperty("ABSLENGTH",     fOpticalPhotonEnergy.data(), absLengthPbWO4.data(), n);
  PbWO4MPT->AddProperty("FASTCOMPONENT", fOpticalPhotonEnergy.data(), scintilFastPbWO4.data(), n);
  PbWO4MPT->AddProperty("SLOWCOMPONENT", fOpticalPhotonEnergy.data(), scintilSlowPbWO4.data(), n);

  // Scintillation light yield - number of photons per unit energy deposition ~ 300 for PbWO4
  // https://sci-hub.do/10.1016/j.phpro.2015.05.033
  PbWO4MPT->AddConstProperty("SCINTILLATIONYIELD", 300./MeV);

  // Fluctuation of mean number of optical photons produces for the step
  PbWO4MPT->AddConstProperty("RESOLUTIONSCALE", 1.0);

  PbWO4MPT->AddConstProperty("FASTTIMECONSTANT", 13.26*ns);
  PbWO4MPT->AddConstProperty("SLOWTIMECONSTANT", 412.2*ns);
  PbWO4MPT->AddConstProperty("YIELDRATIO", 0.9);
  PbWO4MPT->DumpTable();
  PbWO4->SetMaterialPropertiesTable(PbWO4MPT);

  printMaterialProperties(PbWO4);
  fMaterialsList.push_back(PbWO4);

  // █▀ █▀▀ █ █▀▀ █░░ ▄▀█ █▀ █▀
  // ▄█ █▄▄ █ █▄█ █▄▄ █▀█ ▄█ ▄█

  G4Material* BaGdSiO = new G4Material("BaGdSiO", 4.22*g/cm3, 4);
  BaGdSiO->AddElement(elements->getElement("Ba"), 0.3875);
  BaGdSiO->AddElement(elements->getElement("Gd"), 0.2146);
  BaGdSiO->AddElement(elements->getElement("Si"), 0.1369);
  BaGdSiO->AddElement(elements->getElement("O"),  0.2610);

  G4MaterialPropertiesTable* BaGdSiOMPT = new G4MaterialPropertiesTable();

  // Refractive index forSciGlass. Tanja said use 1.58 for now.
  std::vector<G4double> refractiveIndexBaGdSiO = { 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58};
  assert((G4int)refractiveIndexBaGdSiO.size() == (G4int)fOpticalPhotonEnergy.size());

  // Transmittance for SciGlass. Tanja provided with the following values.
  std::vector<G4double> transmittanceBaGdSiO = { 0.394, 0.740, 0.784, 0.798, 0.800, 0.790, 0.784, 0.762};
  assert((G4int)transmittanceBaGdSiO.size() == (G4int)fOpticalPhotonEnergy.size());

  // Absorption length for SciGlass
  std::vector<G4double> absLengthBaGdSiO = calcAbsorptionLength(BaGdSiOMPT, refractiveIndexBaGdSiO, transmittanceBaGdSiO);
  assert((G4int)absLengthBaGdSiO.size() == (G4int)fOpticalPhotonEnergy.size());

  // Need Energy spectrum for the fast scintillation component

  BaGdSiOMPT->AddProperty("RINDEX",        fOpticalPhotonEnergy.data(), refractiveIndexBaGdSiO.data(),n);
  BaGdSiOMPT->AddProperty("ABSLENGTH",     fOpticalPhotonEnergy.data(), absLengthBaGdSiO.data(), n);

  // Need Scintillation light yield

  // Need FASTTIMECONSTANT, SLOWTIMECONSTANT
  BaGdSiOMPT->DumpTable();
  BaGdSiO->SetMaterialPropertiesTable(BaGdSiOMPT);

  printMaterialProperties(BaGdSiO);
  fMaterialsList.push_back(BaGdSiO);

  // █▄▄ ▄▀█ █▀ █ ▀█ █▀█ █▀
  // █▄█ █▀█ ▄█ █ █▄ █▄█ ▄█

  G4Material* BaSi2O5 = new G4Material("BaSi2O5", 3.8*g/cm3, 3);
  BaSi2O5->AddElement(elements->getElement("Ba"), 1);
  BaSi2O5->AddElement(elements->getElement("Si"), 2);
  BaSi2O5->AddElement(elements->getElement("O"), 5);

  fMaterialsList.push_back(BaSi2O5);
}

Materials::~Materials() {
  // TODO Auto-generated destructor stub
}

Materials* Materials::instance = NULL;

G4Material* Materials::getMaterial(const char* materialId){
  for (int i=0; i<(int)fMaterialsList.size(); i++){
    if (fMaterialsList[i]->GetName() == materialId){
      return fMaterialsList[i];
    }
  }
  G4cout << "ERROR: Materials::getMaterial material " << materialId << " not found";
  exit(1);
  return NULL;
}

std::vector<G4double> Materials::calcAbsorptionLength(G4MaterialPropertiesTable* mpt, std::vector<G4double> refractiveIndex, std::vector<G4double> measuredTransmittance){
  // Reflectivity. To be filled from down below
  std::vector<G4double> reflectivity = {};

  // Ideal Transmittance. To be filled from down below
  std::vector<G4double> idealTransmittance = {};

  // Theoretical Transmittance. To be filled from down below
  // std::vector<G4double> realTransmittance = {};

  // Attenuation length
  std::vector<G4double> attenuationLength = {};

  G4double refractiveIndexAir = 1;

  for (G4int i = 0; i < (G4int)refractiveIndex.size(); i++){
    // All formulas below taken from: https://sci-hub.do/https://doi.org/10.1016/0168-9002(93)91185-P

    // Reflectivity
    G4double R = pow((refractiveIndex[i] - refractiveIndexAir)/(refractiveIndex[i] + refractiveIndexAir), 2.);
    reflectivity.push_back(R);

    // Ideal transmittance with infinite light attenuation length and two parallel end surfaces
    G4double Ts = (1 - R)/(1 + R);
    idealTransmittance.push_back(Ts);

    // Real transmittance can also be estimated but we're using the measured value
    // G4double tReal = crystalLength*pow(1-r,2)/(1-pow(l*r,2));

    // Absorption (light attenuation length)
    G4double T = measuredTransmittance[i];
    G4double lambda = fCrystalLength/log(T*pow(1-Ts,2)/(sqrt(4*pow(Ts,4) + pow(T,2)*pow(1-pow(Ts,2),2))-2*pow(Ts,2)));
    attenuationLength.push_back(lambda);
  }

  // Add custom values to the material properties table (for debugging output)
  mpt->AddProperty("MY_REFLECTIVITY", fOpticalPhotonEnergy.data(), reflectivity.data(), (G4int)fOpticalPhotonEnergy.size());
  mpt->AddProperty("MY_MEAS_TRANSM", fOpticalPhotonEnergy.data(), measuredTransmittance.data(), (G4int)fOpticalPhotonEnergy.size());
  mpt->AddProperty("MY_IDEAL_TRANSM", fOpticalPhotonEnergy.data(), idealTransmittance.data(), (G4int)fOpticalPhotonEnergy.size());

  // Return
  return attenuationLength;
}

void Materials::printLine(const char* heading, std::vector<G4double> values, G4double unit){
  // Print photon wavelength
  std::cout << std::endl << std::left << std::setw(TAB_COLUMN_1) << heading;
  for (G4int i = 0; i < (G4int)values.size(); i++){
    G4double value = (unit == 0.) ? values[i] : values[i]/unit;
    std::cout << std::left << std::setw(TAB_COLUMN) << value;
  }
}


void Materials::printHeader(){
  printLine("Photon wavelength, nm", fOpticalPhotonWavelength);
  printLine("Photon energy, eV", fOpticalPhotonEnergy, eV);
}

typedef std::map< G4int, G4MaterialPropertyVector*,
                  std::less<G4int> >::const_iterator MPiterator;

void Materials::printMaterialProperties(G4Material* material){
  G4MaterialPropertiesTable* mpt = material->GetMaterialPropertiesTable();
  if (!mpt) return;

  std::map< G4String, G4MaterialPropertyVector*, std::less<G4String> >* pMap = mpt->GetPropertiesMap();

  std::cout << "\nMaterial name: " << material->GetName() << std::endl;

  // Print photon wavelengths
  std::cout << std::left << std::setw(TAB_COLUMN_1) << "PHOTON_WAVELEGTH";
  for (G4int i = 0; i < (G4int)fOpticalPhotonWavelength.size(); i++){
    std::cout << std::left << std::setw(TAB_COLUMN) << fOpticalPhotonWavelength[i];
  }
  std::cout << std::endl;

  // Print material properties
  for(auto it = pMap->begin(); it!=pMap->end(); ++it){
    std::cout << std::left << std::setw(TAB_COLUMN_1) << it->first;

    for (G4int i = 0; i < (G4int)fOpticalPhotonEnergy.size(); i++){
      G4bool b = true;
      G4double value = it->second->GetValue(fOpticalPhotonEnergy[i], b);
      std::cout << std::left << std::setw(TAB_COLUMN) << value;
    }
    std::cout << std::endl;
  }
}

void Materials::printMaterialProperties(const char* materialId){
  G4Material* material = getMaterial(materialId);
  if (material == NULL) return;
  printMaterialProperties(material);
}

Materials* Materials::getInstance() {
  if (!instance){
      instance = new Materials;
  }
  return instance;
}

// PS: when having a static std::vector member
//std::vector<G4Material*> Materials::fMaterialsList = {};
