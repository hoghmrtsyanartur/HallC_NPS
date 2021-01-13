#include "Rtypes.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TVirtualPad.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TStyle.h"
#include <TPie.h>
#include <TPieSlice.h>
#include "TMap.h"
#include <iomanip>
#include <stdalign.h>
#include <limits>  // for INT_MIN, INT_MAX
#include <stdio.h>
#include <stdlib.h>
#include <RooConstVar.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TLatex.h>
#include <TMathBase.h>
#include <TF1.h>
#include <TGFileDialog.h>
#include <TSystem.h>
#include <TGraphErrors.h>
#include <TPaletteAxis.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>



TString* removeFileExtension(const char* fileNamePath){
  TString* s = new TString(fileNamePath);

  // Test here: https://regex101.com/r/7H6We8/1
  // Don't forget to escape c++ double slashes
  TObjArray *objArray = TPRegexp("^(.*)\\.\\w*$").MatchS(*s);
  if (objArray->GetLast()+1 != 2){
    return NULL;
  }
  const TString str = ((TObjString *)objArray->At(1))->GetString();
  return new TString(str.Data());
}

TPaveStats* getPaveStats(TVirtualPad* pad){
  // Update Pad - in case the histogram was just drawn - need to update
  pad->Update();

  // pad->GetListOfPrimitives()->Print();

  // If previously renamed PaveStats and added to the pad primitives - return it
  TPaveStats *pave = (TPaveStats*)pad->GetPrimitive("mystats");
  if (pave) return pave;

  // Else remove PaveText from histogram primitives, rename it and add to the list of Pad primitives
  // pad0->getPrimitive() searches across primitives of primitives
  pave = (TPaveStats*)pad->GetPrimitive("stats");

  if (pave){
    pave->SetName("mystats");
    TH1* parentHist = (TH1*)(pave->GetParent());
    parentHist->SetStats(0);
    pad->GetListOfPrimitives()->Add(pave);
    // pad->GetListOfPrimitives()->Print();
    return pave;
  }
  // If not found search object by file type
  // for (TObject* object : *(pad->GetListOfPrimitives())){
  //  std::cout << object->GetName() << std::endl;
  //  if (object->InheritsFrom(TPaveStats::Class())){
  //    return (TPaveStats*) object;
  //  }
  // }

  return NULL;
}

enum class Align {
  kRight,
  kLeft
};

void alignStats(TVirtualPad* pad, Double_t statsWidth=0.6, Align align = Align::kRight, Double_t statsLineHeight = 0.06){
  // Retrieve the stat box
  TPaveStats *stats = getPaveStats(pad);
  if (!stats) return;

  //stats->GetX2NDC() - stats->GetX1NDC();
  Double_t statsHeight = statsLineHeight*stats->GetListOfLines()->GetSize(); // stats->GetY2NDC() - stats->GetY1NDC();

  // Move stats horizontally
  if (align == Align::kRight){
    stats->SetX2NDC(1 - pad->GetRightMargin());
    stats->SetX1NDC(stats->GetX2NDC() - statsWidth);
  } else {
    stats->SetX1NDC(pad->GetLeftMargin());
    stats->SetX2NDC(pad->GetLeftMargin() + statsWidth);
  }
  // Move stats vertically
  stats->SetY2NDC(1 - pad->GetTopMargin());
  stats->SetY1NDC(stats->GetY2NDC() - statsHeight);

  pad->Modified();
  pad->Update();
}

void addTextToStats(TVirtualPad* pad, const char* text){
  // Retrieve the stat box
  TPaveStats *stats = getPaveStats(pad);
  if (!stats) return;

  TList *listOfLines = stats->GetListOfLines();

  // Note that "=" is a control character
  TText *newLine = new TLatex(0, 0, text);
  newLine->SetTextAlign(0);
  newLine->SetTextFont(0);
  newLine ->SetTextSize(0);
  newLine->SetTextColor(0);
  listOfLines->Add(newLine);

  pad->Modified();
}

Double_t getWeightedIntegral(TH1* hist){
  Double_t weightedIntegral = 0;
  for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++){
    weightedIntegral += hist->GetBinCenter(i)*hist->GetBinContent(i);
  }
  return weightedIntegral;
}

void addCanvasTitle(TCanvas* canvas, const char* title, const char* subtitle = 0){
  canvas->cd();
  Int_t titleHeightPx = 40;
  Int_t subtitleHeightPx = 25;

  Int_t topShiftPx = titleHeightPx + (subtitle == 0 ? 0 : subtitleHeightPx);

  Int_t canvasHeightPx = canvas->GetWh();
  Double_t topShiftNDC = (Double_t)topShiftPx/(Double_t)canvasHeightPx;

  // Resize child canvases to free space on to for title
  for (Int_t i=1; canvas->GetPad(i)!=NULL; i++){
    TVirtualPad* childPad = canvas->GetPad(i);
    Double_t yLow = childPad->GetAbsYlowNDC();
    Double_t xLow = childPad->GetAbsXlowNDC();
    Double_t height = childPad->GetAbsHNDC();
    Double_t width = childPad->GetAbsWNDC();
    childPad->SetPad(xLow, yLow*(1-topShiftNDC), xLow+width, (yLow+height)*(1-topShiftNDC));
  }

  // Add title text (fixed size in px)
  TText *t = new TText(0.5, (canvasHeightPx-(titleHeightPx+subtitleHeightPx)*0.35)/canvasHeightPx, title);
  t->SetTextFont(63);
  t->SetTextSizePixels(21);
  t->SetTextAlign(kHAlignCenter+kVAlignCenter);
  t->Draw();

  // Add subtitle text
  // Add title text (fixed size in px)
  if (subtitle == 0) return;
  TText *t2 = new TText(0.5, (canvasHeightPx-(titleHeightPx+subtitleHeightPx)*0.75)/canvasHeightPx, subtitle);
  t2->SetTextFont(63);
  t2->SetTextSizePixels(16);
  t2->SetTextAlign(kHAlignCenter+kVAlignCenter);
  t2->Draw();
}

TFile* openFile(const char* fileName){
  TFile *file = new TFile(fileName);
  if (file->IsZombie()){
    file->Close();
    std::cout << "Error opening file \"" << fileName << "\". Aborting." << std::endl;
    exit(1);
  }
  return file;
}


TTree* getTree(TFile* file, const char* treeName){
  TTree *tree = (TTree *)file->Get(treeName);
  if (!tree){
    std::cout << "Cannot find tree with name \"" << treeName << "\". Aborting." << std::endl;
    return NULL;
  }
  // std ::cout << "Number of events in the tree: " << tree->GetEntries() << std::endl;
  // tree->Print();
  return tree;
}


// ----------------------------------
// Plot energy deposition in crystals
// ----------------------------------

int plotCrystalEdep(const char *fileName){
  TFile *file = openFile(fileName);

  // Obtain number of crystals from file
  TVector2* nCrystals = (TVector2*) file->Get("crystalsNumber");
  const Int_t nHists = nCrystals->X() * nCrystals->Y();
  std::cout << "Crystal assembly size: " <<  nCrystals->X() << "x" << nCrystals->Y() << std::endl;

  // Obtain incident particle energy
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();

  // Instantiate histograms to be saved in ROOT file
  TH1D **edepHist = new TH1D*[nHists];
  for (UInt_t i = 0; i < nHists; i++){
    TString edepHistName = TString::Format("edepHist_%d", i);
    TString edepHistTitle = TString::Format("Energy Deposition in Crystal %d", i+1);
    edepHist[i] = new TH1D(edepHistName.Data(), edepHistTitle.Data(), 100, 0, particleEnergy);
    edepHist[i]->GetXaxis()->SetTitle("Deposited energy, MeV");
    edepHist[i]->GetYaxis()->SetTitle("Counts");
  }

  TTree* tree = getTree(file, "tree_crystals");
  if (tree == NULL) return 0;

  // Do not process files smaller that 100 events
  if ((Int_t)tree->GetEntries() < 100) return 0;

  // Try opening branches
  // TBranch* branch = tree->GetBranch("edep");
  // if (!branch){
  //   std::cout << "Branch \"edep\" could not be initialized. Skipping." << std::endl;
  //   return 0;
  // }

  // Every branch contains an array with values for each PMT event
  Double_t* edep = new Double_t[nHists];
  Double_t* totalEdep = new Double_t[nHists];
  for(Int_t j = 0 ; j < nHists ; j++){
    totalEdep[j] = 0;
  }

  tree->SetBranchAddress("edep", edep);
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++){
    tree->GetEntry(i);
    for(Int_t j = 0 ; j < nHists ; j++){
      edepHist[j]->Fill(edep[j]);
      totalEdep[j] += edep[j];
    }
  }

  // Calculate total deposited energy
  // TODO: check if correct
  Double_t totalDepositedEnergy = 0;
  for (Int_t i = 0; i < nHists; i++){
    std::cout << "Crystal " << i << ": " << totalEdep[i] << " MeV" << std::endl;
    totalDepositedEnergy += totalEdep[i];
  }

  // Get number of events, GPS particle energy and particle name
  Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal();
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector2* crystalsNumber = (TVector2*)file->Get("crystalsNumber");
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  Double_t totalGPSEnergy = numberOfEvents*particleEnergy;

  // Instantiate the Canvas
  std::stringstream buffer;
  buffer << "Energy deposition in " << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() <<
      ", " << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << " mm " <<  crystalMaterial <<
      "crystal assembly per event, " << particleEnergy/1E3 << " GeV " << particleName << ".";
  TString canvasTitle = buffer.str().c_str();
  TString canvasSubtitle = TString::Format(
      "Injected Kinetic Energy %.1f GeV. Deposited in Crystals %.1f GeV (%.1f %%).",
      totalGPSEnergy/1E3, totalDepositedEnergy/1E3, totalDepositedEnergy/totalGPSEnergy*100);

  TCanvas* canvas = new TCanvas("crystalEdepCanvas", canvasTitle.Data(), 1024, 800);
  canvas->Divide(nCrystals->X(),nCrystals->Y());

  gStyle->SetPalette(1);

  for (Int_t i = 0; i < nHists; i++){
    TVirtualPad* pad = canvas->cd(i+1);
    edepHist[i]->Draw();
    edepHist[i]->SetFillColor(kCyan);
    alignStats(pad);
    // TString text1 = TString::Format("%.1f MeV deposited", totaledep[i]);
    Double_t percentDepositedEnergy = totalEdep[i]/totalDepositedEnergy*100.;
    TString text2 = TString::Format("E deposited = %.0f MeV, %.1f %%", totalEdep[i], percentDepositedEnergy);
    addTextToStats(pad, text2.Data());
    alignStats(pad);
  }


  // Write global title for first Canvas
  addCanvasTitle(canvas, canvasTitle.Data(), canvasSubtitle.Data());

  // Save canvas to file
  TString* fileNameOnly = removeFileExtension(fileName);
  canvas->SaveAs((*fileNameOnly+"-edep.png").Data());

  return 0;
}


// ---------------------------------
// Plot particles escaping the world
// ---------------------------------

void plotEscapeParticles(const char* fileName){
  TFile* file = openFile(fileName);
  TTree *tree = getTree(file, "tree_escape");
  if (tree == NULL) return;

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();
  Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal();
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector2* crystalsNumber = (TVector2*)file->Get("crystalsNumber");
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  Double_t totalGPSEnergy = numberOfEvents*particleEnergy;

  // Set Marker Style
  TCanvas* canvas = new TCanvas("escapeCanvas", "", 1280, 520);
  canvas->Divide(3, 1);

  // PAD 1: Draw world escape locations
  tree->SetMarkerStyle(6);
  TVirtualPad* pad1 = canvas->cd(1);
  // Int_t maxEntries = TMath::Min((Double_t)tree->GetEntries(), 1.E4);
  // tree->Draw("x:y:z:energy", "", "COLZ", maxEntries);
  pad1->SetMargin(0.15, 0.2, 0.15, 0.15);
  gStyle->SetPalette(kTemperatureMap);
  tree->Draw("x:y:z:energy", "", "COLZ");
  TH1* htemp1 = (TH1*) pad1->GetListOfPrimitives()->FindObject("htemp");
  // ROOT bug:
  htemp1->SetTitle("Escape Locations;Z, mm; Y, mm; Energy, MeV");
  pad1->Update();
  TPaletteAxis *palette = (TPaletteAxis*)htemp1->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.85);
  palette->SetX2NDC(0.9);
  palette->SetTitleOffset(-1.5);
  htemp1->GetXaxis()->SetTitleOffset(2);
  htemp1->GetYaxis()->SetTitleOffset(2);
  htemp1->GetZaxis()->SetTitleOffset(2);
  htemp1->GetZaxis()->SetTitle("X, mm");

  // PAD 2: Draw Energy distribution
  TVirtualPad* pad2 = canvas->cd(2);
  pad2->SetLogy();
  gStyle->SetOptStat(10); // https://root.cern.ch/doc/master/classTPaveStats.html
  tree->Draw("energy");
  TH1* htemp2 = (TH1*) pad2->GetListOfPrimitives()->FindObject("htemp");
  htemp2->SetTitle("Kinetic Energy Distribution;Kinetic energy, MeV;Events");
  htemp2->SetFillColor(kCyan);
  alignStats(pad2, 0.4, Align::kRight, 0.09);

  Double_t escapeEnergy = 0; // getWeightedIntegral(htemp2);
  Double_t energy;
  tree->SetBranchAddress("energy",&energy);
  for (Int_t i = 0; i<(Int_t)tree->GetEntries(); i++) {
    tree->GetEntry(i);
    escapeEnergy+= energy;
  }
  std::stringstream buffer;
  buffer << "Particles escaped the simulation for " << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() <<
      ", " << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << " mm " <<  crystalMaterial <<
      "crystal assembly per event, " << particleEnergy/1E3 << " GeV " << particleName << ".";

  TString escapeEnergyString = TString::Format("Total E = %.0f MeV", escapeEnergy);
  addTextToStats(pad2, escapeEnergyString.Data());
  TString canvasTitle = buffer.str().c_str();
  canvas->SetTitle(canvasTitle.Data());

  // PAD 3: Draw PDG
  TVirtualPad* pad3 = canvas->cd(3);
  Int_t pdgMax =  std::numeric_limits<int>::min();
  Int_t pdgMin = std::numeric_limits<int>::max();
  Int_t pdg;
  tree->SetBranchAddress("pdg",&pdg);
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++) {
    tree->GetEntry(i);
    if (pdg > pdgMax) pdgMax = pdg;
    if (pdg < pdgMin) pdgMin = pdg;
  }
  TH1* pdgHist = new TH1D("pdgHist", "Particle Type (PDG)", pdgMax - pdgMin + 1, pdgMin - 0.5, pdgMax + 0.5);
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++) {
    tree->GetEntry(i);
    pdgHist->Fill(pdg);
  }
  // Make pie chart
  TMap* pdgNameMap = (TMap*)file->Get("pdgNameMap");
  pdgNameMap->Print();
  std::vector<const char*> particleNames = {};
  std::vector<double> binValues = {};
  for (Int_t i = 1; i <= pdgHist->GetXaxis()->GetNbins(); i++){
    if (pdgHist->GetBinContent(i)!=0){
      TString* label = new TString();
      label->Form("%d", (int)pdgHist->GetBinCenter(i));
      TObjString* objString = (TObjString*)pdgNameMap->GetValue(label->Data());
      particleNames.push_back(objString->GetName());
      std::cout << label->Data() << std::endl;
      binValues.push_back(pdgHist->GetBinContent(i));
    }
  }
  Int_t colors[] = {kGray, kOrange + 1, kSpring - 5, kAzure + 8, kPink + 1, kViolet - 4, kRed - 7, kViolet + 6};
  TPie *pie4 = new TPie("pie4", "Particle IDs", (int)binValues.size(), &binValues[0], colors, &particleNames[0]);
  pie4->SetRadius(.2);
  // pie4->SetLabelFormat("%txt: %val (%perc)");
  pie4->SetLabelFormat("%txt");
  pie4->SetTextSize(0.04);

  pie4->Draw("3dnol");
  TLegend* legend = new TLegend(0.5,0.5,0.9,0.9,"");
  for (Int_t i = 0; i < pie4->GetEntries(); ++i) {
    Double_t pieValue = pie4->GetSlice(i)->GetValue();
    TString text = TString::Format("%s (%d particles, %.1f %%)", pie4->GetSlice(i)->GetTitle(), (int)pieValue, pieValue/tree->GetEntries()*100);
    legend->AddEntry(pie4->GetSlice(i),text.Data(),"f");
  }
  legend->SetX1(0.1);
  Double_t legendBottom = 0.9-0.06*legend->GetNRows();
  legend->SetY1(legendBottom);
  legend->SetX2(.9);
  legend->SetY2(.9);
  legend->Draw();
  pie4->SetY((1-legendBottom)*3/4);
  pie4->SetTitle("Particle Types and Ratios");

  // Add title to canvas2
  TString subtitle = TString::Format("Injected Kinetic Energy %.1f GeV. Escaped Kinetic Energy %.1f GeV (%.1f %%)", totalGPSEnergy/1E3, escapeEnergy/1E3, escapeEnergy/totalGPSEnergy*100);
  addCanvasTitle(canvas, canvas->GetTitle(), subtitle.Data());

  // Save canvas
  TString* fileNameOnly = removeFileExtension(fileName);
  canvas->SaveAs((*fileNameOnly+"-eesc.png").Data());
}


// ---------------------------------
// Plot energy resolution
// ---------------------------------

class CrystalBallFunctionObject {
  public:
    Double_t operator() (double *_x, double *par) {
      LongDouble_t x = _x[0];
      LongDouble_t a = par[0];
      LongDouble_t n = par[1];
      LongDouble_t mean = par[2];
      LongDouble_t sigma = par[3];
      LongDouble_t norm = par[4];

      LongDouble_t pi = TMath::Pi();

      LongDouble_t A = pow(n/abs(a), n)*exp(-a*a/2);
      LongDouble_t B = n/abs(a) - abs(a);
      LongDouble_t C = n/abs(a)/(n-1)*exp(-abs(a)*abs(a)/2.);
      LongDouble_t D = sqrt(pi/2)*(1+erf(abs(a)/sqrt(2)));
      LongDouble_t N = 1/sigma/(C+D);

      if ((x-mean)/sigma > -a){
        return norm*N*exp(-(x-mean)*(x-mean)/2/sigma/sigma);
      }

      return norm*N*A*pow(B-(x-mean)/sigma, -n);
    }
};

class GaussFunctionObject {
  public:
    GaussFunctionObject(TH1* hist){
      // if option "width" is specified, the integral is the sum of
      // the bin contents multiplied by the bin width in x.
      this->normalization = hist->Integral("width");
    }

    Double_t operator() (double *_x, double *par) {
      Double_t x = _x[0];
      Double_t mean = par[0];
      Double_t sigma = par[1];

      return normalization*1/sigma/TMath::Sqrt(2*TMath::Pi())*TMath::Exp(-(x-mean)*(x-mean)/2/sigma/sigma);
    }

  private:
    Double_t normalization;
};

TVector3* plotEnergyResolution(const char* fileName){
  TFile* file = openFile(fileName);
  TTree *tree = getTree(file, "tree_crystals");
  if (tree == NULL) return 0;

  // Do not process files smaller that 100 events
  if ((Int_t)tree->GetEntries() < 100) return 0;

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();
  Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal();
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  TVector2* crystalsNumber =  (TVector2*)file->Get("crystalsNumber");
  Double_t totalGPSEnergy = numberOfEvents*particleEnergy;

  // Estimate energy histogram upper range
  Int_t nCrystals = crystalsNumber->X()*crystalsNumber->Y();
  Double_t edep[nCrystals];
  tree->SetBranchAddress("edep", edep);
  Double_t minEdep = particleEnergy;
  Double_t maxEdep = 0;
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++){
    // Get tree entry
    tree->GetEntry(i);
    // Each event we calculate the total energy deposited in all crystals
    Double_t totalEdep = 0;
    for(Int_t j = 0 ; j < nCrystals ; j++){
      totalEdep += edep[j];
    }
    if (totalEdep > maxEdep) maxEdep = totalEdep;
    if (totalEdep < minEdep) minEdep = totalEdep;
  }

  // Instantiate histogram for total energy deposition
  std::stringstream buffer;
  buffer << "Total energy deposition in " << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() <<
      ", " << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << " mm " <<  crystalMaterial <<
      " crystal assembly per event, " << particleEnergy/1E3 << " GeV " << particleName << ".";

  // TString title = TString::Format("Total energy deposition in %dx%d, %dx%dx%d %s crystal assembly per event, %.0f GeV %s.", (Int_t)crystalsNumber->X(), (Int_t)crystalsNumber->Y(), (Int_t)crystalSize->X(), (Int_t)crystalSize->Y(), (Int_t)crystalSize->Z(), crystalMaterial, particleEnergy/1E3, particleName);
  TH1D* edepHist = new TH1D("totalEdepHist", buffer.str().c_str(), 250, minEdep, maxEdep*1.25);
  edepHist->GetYaxis()->SetTitle("Counts");
  edepHist->GetXaxis()->SetTitle("Total deposited energy, MeV");

  // Fill histogram from the tree
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++){
    // Get tree entry
    tree->GetEntry(i);
    // Each event we calculate the total energy deposited in all crystals
    Double_t totalEdep = 0;
    for(Int_t j = 0 ; j < nCrystals ; j++){
      totalEdep += edep[j];
    }
    edepHist->Fill(totalEdep);
  }

  TCanvas* canvas = new TCanvas("eResCanvas", buffer.str().c_str());
  canvas->SetGrid();
  // Fit with ROOT gaus
  // edepHist->Fit("gaus"); // "0" - Fit with gaus, do not plot the result of the fit
  // Double_t m = edepHist->GetFunction("gaus")->GetParameter(1); // mean
  // Double_t Dm = edepHist->GetFunction("gaus")->GetParError(1); // mean error
  // Double_t s = edepHist->GetFunction("gaus")->GetParameter(2); // sigma
  // Double_t Ds = edepHist->GetFunction("gaus")->GetParError(2); // sigma error

  // Fit with my gauss
  // GaussFunctionObject* fGaussFunctionObject = new GaussFunctionObject(edepHist);
  // TF1* fGauss = new TF1("fGauss", fGaussFunctionObject, edepHist->GetXaxis()->GetXmin(), edepHist->GetXaxis()->GetXmax(), 2);    // create TF1 class.
  // fGauss->SetParName(0, "mean");
  // fGauss->SetParameter(0, 3000);
  // fGauss->SetParLimits(0, 1000, 5000);
  // fGauss->SetParName(1, "sigma");
  // fGauss->SetParameter(1, 200);
  // fGauss->SetParLimits(1, 0, 1000);
  // edepHist->Fit(fGauss, "W"); // ignore bin uncertanties?
  // Double_t m = edepHist->GetFunction("fGauss")->GetParameter(0); // mean
  // Double_t Dm = edepHist->GetFunction("fGauss")->GetParError(0); // mean error
  // Double_t s = edepHist->GetFunction("fGauss")->GetParameter(1); // sigma
  // Double_t Ds = edepHist->GetFunction("fGauss")->GetParError(1); // sigma error

  // Fit with crystal ball
  CrystalBallFunctionObject* fGaussFunctionObject = new CrystalBallFunctionObject();
  TF1* fBall = new TF1("fBall", *fGaussFunctionObject, edepHist->GetXaxis()->GetXmin(), edepHist->GetXaxis()->GetXmax(), 5);    // create TF1 class.
  fBall->SetNpx(1000);

  fBall->SetParName(0, "a");
  fBall->SetParameter(0, 1);
  fBall->SetParLimits(0, 0, 1E2);

  fBall->SetParName(1, "n");
  fBall->SetParameter(1, 10);
  fBall->SetParLimits(1, 1+1E-3, 1E2);

  fBall->SetParName(2, "mean");
  fBall->SetParameter(2, edepHist->GetBinCenter(edepHist->GetMaximumBin()));
  fBall->SetParLimits(2, edepHist->GetXaxis()->GetXmin(), edepHist->GetXaxis()->GetXmax());

  fBall->SetParName(3, "sigma");
  fBall->SetParameter(3, 200);
  fBall->SetParLimits(3, 0, 1000);

  fBall->SetParName(4, "norm");
  fBall->SetParameter(4, edepHist->Integral("width"));
  fBall->SetParLimits(4, edepHist->Integral("width")/2, edepHist->Integral("width")*2);

  // Abnormal termination of minimization.
  // Solution: increase the fitting range. Specify function range (-10000, 10000) and pass "R" parameter

  edepHist->Fit(fBall, "W");

  Double_t m = edepHist->GetFunction("fBall")->GetParameter(2); // mean
  Double_t Dm = edepHist->GetFunction("fBall")->GetParError(2); // mean error
  Double_t s = edepHist->GetFunction("fBall")->GetParameter(3); // sigma
  Double_t Ds = edepHist->GetFunction("fBall")->GetParError(3); // sigma error

  Double_t r = s/m*100;  // Resolution
  Double_t Dr = 100*TMath::Sqrt(TMath::Power(1/m*Ds,2)+TMath::Power(s/m/m*Dm,2)); // Indirect error

  // Plot histogram and fit
  gStyle->SetOptStat(10);
  edepHist->Draw();
  edepHist->SetFillColor(kCyan);
  TString meanString = TString::Format("Mean, MeV = %.1f #pm %.1f", m, Dm);
  addTextToStats(canvas, meanString.Data());
  TString sigmaString = TString::Format("Sigma, MeV = %.1f #pm %.1f", s, Ds);
  addTextToStats(canvas, sigmaString.Data());
  TString resolutionString = TString::Format("Resolution, %% = %.1f #pm %.1f", r, Dr);
  addTextToStats(canvas, resolutionString.Data());
  alignStats(canvas, 0.35, Align::kLeft);

  // Save canvas
  TString* fileNameOnly = removeFileExtension(fileName);
  canvas->SaveAs((*fileNameOnly+"-eres.png").Data());

  return new TVector3(particleEnergy/1000., r, Dr);

}


// ---------------------------------
// Main function
// ---------------------------------

TVector3* energyResolution(const char *fileName){
  plotCrystalEdep(fileName);
  plotEscapeParticles(fileName);
  return plotEnergyResolution(fileName);
}

int energyResolution(){
  const char *filetypes[] = { "All files",     "*",
                              "ROOT files",    "*.root",
                              0,               0 };
  TGFileInfo fi;
  fi.fFileTypes = filetypes;
  new TGFileDialog(gClient->GetRoot(), 0, kFDOpen, &fi);

  if (fi.fMultipleSelection && fi.fFileNamesList) {
    TObjString *el;
    TIter next(fi.fFileNamesList);

    std::vector<Double_t> energy = {};
    std::vector<Double_t> resolution = {};
    std::vector<Double_t> resolutionErr = {};

    while ((el = (TObjString *) next())) {
      TVector3* v = energyResolution(el->GetString().Data());
      if (v != NULL){
        energy.push_back(v->X());
        resolution.push_back(v->Y());
        resolutionErr.push_back(v->Z());
      }
    }

    // Plot graph with energy resolution
    const char* firstFilename = ((TObjString *)fi.fFileNamesList->At(0))->GetString().Data();
    TFile* file = openFile(firstFilename);
    // Get number of events, GPS particle energy and particle name
    const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
    TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
    TVector2* crystalsNumber =  (TVector2*)file->Get("crystalsNumber");

    // If given more than one energy point
    if (energy.size() > 1){
      // Plot graph with energy resolution graph
      std::stringstream buffer;
      buffer << "Energy resolution of the " << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() <<
          ", " << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << " mm " <<  crystalMaterial <<
          " crystal assembly;";
      buffer << "Incident particle energy, GeV;";
      buffer << "Energy resolution, %;";

      TCanvas* canvas = new TCanvas("energyResolution", buffer.str().c_str());
      canvas->SetGrid();
      TGraphErrors* gr = new TGraphErrors((int)energy.size(), &energy[0], &resolution[0], 0, &resolutionErr[0]); // convert vector to array

      gr->SetTitle(buffer.str().c_str());
      // gr->SetMarkerColor(4);
      gr->SetMarkerStyle(21);
      gr->Sort(&TGraph::CompareX);
      gr->Draw("ALP");
      canvas->Update();
      TString fileNameNoExtension = TString::Format("resolution-%s-%dx%d-%dx%dx%d", crystalMaterial, (Int_t)crystalsNumber->X(), (Int_t)crystalsNumber->Y(), (Int_t)crystalSize->X(), (Int_t)crystalSize->Y(), (Int_t)crystalSize->Y());
      TString resolutionPngFileName = fileNameNoExtension + ".png";
      canvas->SaveAs(resolutionPngFileName.Data());

      // Export graph with energy resolution data
      // Save data to ASCII file
      TString resolutionFileName = fileNameNoExtension + ".txt";
      std::ofstream outfile;
      outfile << std::left; // left align
      const unsigned int tab = 20;
      outfile.open(resolutionFileName.Data()); // overwrite
      outfile << "# " << std::setw(tab) << "Energy, GeV" << std::setw(tab) << "Resolution, %" << "Resolution Error, %" << std::endl;
      for (int i = 0; i < gr->GetN(); i++){
        outfile << "  " << std::setw(tab) << gr->GetPointX(i) << std::setw(tab) << gr->GetPointY(i) << gr->GetErrorY(i) << std::endl;
      }
      outfile.close();

      // Generate gnuplot file
      // TString gnuplotFileName =  fileNameNoExtension + ".gp";
      // TString gnuplotPngFileName =  fileNameNoExtension + ".png";
      // outfile.open(gnuplotFileName.Data()); // overwrite
      // outfile << "set output '" << gnuplotPngFileName.Data() << "'" << std::endl;
      // outfile << "set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 1 pointtype 7" << std::endl;
      // outfile << "unset key" << std::endl;
      //
      // outfile << "set title \"Energy resolution for 3x3, 20x20x400 mm Ga assembly\"" << std::endl;
      // outfile << "set xlabel \"Particle energy, GeV\"" << std::endl;
      // outfile << "set ylabel \"Energy resolution, %\"" << std::endl;
      //
      // outfile << "plot '< sort -nk1 resolution-BaGdSiO-5x5-20x20x20.txt' using 1:2:3 with yerrorbars linestyle 1, \\" << std::endl;
      // outfile << "     ''                                                            with lines linestyle 1" << std::endl;
      // outfile.close();
    }
  } else if (fi.fFilename) {
    // If selected single file
    energyResolution(fi.fFilename);
  }
  return 1;
}
