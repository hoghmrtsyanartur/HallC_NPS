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
#include <TVector3.h>
#include <TLatex.h>

const Double_t statsLineHeight = 0.06;

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

void alignStats(TVirtualPad* pad, Double_t statsWidth=0.6){
  // Retrieve the stat box
  TPaveStats *stats = getPaveStats(pad);
  if (!stats) return;

  //stats->GetX2NDC() - stats->GetX1NDC();
  Double_t statsHeight = statsLineHeight*stats->GetListOfLines()->GetSize(); // stats->GetY2NDC() - stats->GetY1NDC();
  // Move stats horizontally
  stats->SetX2NDC(1 - pad->GetRightMargin());
  stats->SetX1NDC(stats->GetX2NDC() - statsWidth);
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

  alignStats(pad);

  pad->Modified();
}

Double_t getWeightedIntegral(TH1* hist){
  Double_t weightedIntegral = 0;
  for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++){
    weightedIntegral += hist->GetBinCenter(i)*hist->GetBinContent(i);
  }
  return weightedIntegral;
}

void addCanvasTitle(TCanvas* canvas, const char* title){
  canvas->cd();
  Int_t headingHeightPx = 40;
  Int_t canvasHeightPx = canvas->GetWh();
  Double_t titleHeightNDC = (Double_t)headingHeightPx/(Double_t)canvasHeightPx;
  // Resize child canvases to free space on to for title
  for (Int_t i=1; canvas->GetPad(i)!=NULL; i++){
    TVirtualPad* childPad = canvas->GetPad(i);
    Double_t yLow = childPad->GetAbsYlowNDC();
    Double_t xLow = childPad->GetAbsXlowNDC();
    Double_t height = childPad->GetAbsHNDC();
    Double_t width = childPad->GetAbsWNDC();
    childPad->SetPad(xLow, yLow*(1-titleHeightNDC), xLow+width, (yLow+height)*(1-titleHeightNDC));
  }

  // Add title text (fixed size in px)
  TText *t = new TText(0.5, (canvasHeightPx-headingHeightPx/2.)/canvasHeightPx, title);
  t->SetTextFont(63);
  t->SetTextSizePixels(16);
  t->SetTextAlign(kHAlignCenter+kVAlignCenter);
  t->Draw();
}



int energyDeposition(const char *fileName){
  // Open input ROOT file
  std::cout << fileName << std::endl;
  TFile *file = new TFile(fileName);
  if (file->IsZombie()){
    file->Close();
    std::cout << "Error opening file \"" << fileName << "\". Skipping." << std::endl;
    return 0;
  }

  TString* fileNameOnly = removeFileExtension(fileName);

  // Test File Baskets
  // https://root-forum.cern.ch/t/error-in-tbasket-readbasketbuffers/8932/3
  // file->Map();

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();
  Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal();
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  Double_t totalGPSEnergy = numberOfEvents*particleEnergy;

  // Obtain the Tree named "T" from file
  TTree *tree = (TTree *)file->Get("t");
  if (!tree){
    std::cout << "Cannot find object with name \"T\". Skipping." << std::endl;
    return 0;
  }
  std ::cout << "Number of events in the tree: " << tree->GetEntries() << std::endl;

  tree->Print();

  // Instantiate histograms to be saved in ROOT file
  const Int_t nHists = 9;
  TH1D **edepHist = new TH1D*[nHists];
  for (UInt_t i = 0; i < nHists; i++){
    TString edepHistName = TString::Format("edepHist_%d", i);
    TString edepHistTitle = TString::Format("Energy Deposition in Crystal %d", i+1);
    edepHist[i] = new TH1D(edepHistName.Data(), edepHistTitle.Data(), 100, 0, 8000);
    edepHist[i]->GetXaxis()->SetTitle("Deposited energy, MeV");
    edepHist[i]->GetYaxis()->SetTitle("Counts");
  }

  // Try opening branches
  // TBranch* branch = tree->GetBranch("edep");
  // if (!branch){
  //   std::cout << "Branch \"edep\" could not be initialized. Skipping." << std::endl;
  //   return 0;
  // }

  // Every branch contains an array with values for each PMT event
  Double_t edep[nHists];
  Double_t totalEdep[nHists] = {0};
  tree->SetBranchAddress("edep", edep);
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++){
    tree->GetEntry(i);
    for(Int_t j = 0 ; j < nHists ; j++){
      edepHist[j]->Fill(edep[j]);
      totalEdep[j] += edep[j];
      // std::cout << "edep[" << j << "] = " << edep[j] << std::endl;
    }
  }

  // Calculate total deposited energy
  // TODO: check if correct
  Double_t totalDepositedEnergy = 0;
  for (Int_t i = 0; i < nHists; i++){
    totalDepositedEnergy += totalEdep[i];
  }

  // Instantiate the Canvas
  TString canvas1Title = TString::Format(
      "Injected Kinetic Energy %.1f GeV (%s) in %dx%dx%d mm %s. Deposited in Crystals %.1f GeV (%.1f %%)",
      totalGPSEnergy/1E3, particleName, (int)crystalSize->x(), (int)crystalSize->y(), (int)crystalSize->z(),
      crystalMaterial, totalDepositedEnergy/1E3,
      totalDepositedEnergy/totalGPSEnergy*100);
  TCanvas* canvas1 = new TCanvas("canvas1", canvas1Title.Data(), 1024, 800);
  canvas1->Divide(3,3);

  gStyle->SetPalette(1);

  for (Int_t i = 0; i < nHists; i++){
    TVirtualPad* pad = canvas1->cd(i+1);
    edepHist[i]->Draw();
    edepHist[i]->SetFillColor(kCyan);
    alignStats(pad);
    // TString text1 = TString::Format("%.1f MeV deposited", totaledep[i]);
    Double_t percentDepositedEnergy = totalEdep[i]/totalDepositedEnergy*100.;
    TString text2 = TString::Format("E deposited = %.0f MeV, %.1f %%", totalEdep[i], percentDepositedEnergy);
    addTextToStats(pad, text2.Data());
  }

  // Write global title for first Canvas
  addCanvasTitle(canvas1, canvas1->GetTitle());
  // Save canvas to file
  canvas1->SaveAs((*fileNameOnly+"-edep.png").Data());

  // ---------------------------------
  // Plot particles escaping the world
  // ---------------------------------

  TTree *escapeTree = (TTree *)file->Get("t_escape");
  if (!escapeTree){
    std::cout << "Cannot find object with name \"T\". Skipping." << std::endl;
    return 0;
  }
  std ::cout << "Number of events in the tree: " << tree->GetEntries() << std::endl;

  // Set Marker Style
  TCanvas* escapeCanvas = new TCanvas("escapeCanvas", "Particles escaping the World", 1024, 400);
  escapeCanvas->Divide(3);

  // PAD 1: Draw world escape locations
  escapeTree->SetMarkerStyle(6);
  TVirtualPad* pad1 = escapeCanvas->cd(1);
  escapeTree->Draw("x:y:z:energy", "", "COLZ");
  TH1* htemp1 = (TH1*) pad1->GetListOfPrimitives()->FindObject("htemp");
  htemp1->SetTitle("Escape Locations");

  // PAD 2: Draw Energy distribution
  TVirtualPad* pad2 = escapeCanvas->cd(2);
  pad2->SetLogy();
  // gStyle->SetOptStat(1001111); // with integral https://root.cern.ch/doc/master/classTPaveStats.html
  escapeTree->Draw("energy");
  TH1* htemp2 = (TH1*) pad2->GetListOfPrimitives()->FindObject("htemp");
  htemp2->SetTitle("Kinetic Energy Distribution");
  htemp2->SetFillColor(kCyan);
  alignStats(pad2);
  Double_t escapeEnergy = 0; // getWeightedIntegral(htemp2);
  Double_t energy;
  escapeTree->SetBranchAddress("energy",&energy);
  for (Int_t i = 0; i<(Int_t)escapeTree->GetEntries(); i++) {
    escapeTree->GetEntry(i);
    escapeEnergy+= energy;
  }
  TString escapeEnergyString = TString::Format("Total E = %.0f MeV", escapeEnergy);
  addTextToStats(pad2, escapeEnergyString.Data());
  TString canvasTitle2 = TString::Format("Injected Kinetic Energy %.1f GeV (%s) in %s. Escaped Kinetic Energy %.1f GeV (%.1f %%)", totalGPSEnergy/1E3, particleName, crystalMaterial, escapeEnergy/1E3, escapeEnergy/totalGPSEnergy*100);
  escapeCanvas->SetTitle(canvasTitle2.Data());

  // PAD 3: Draw PDG
  TVirtualPad* pad3 = escapeCanvas->cd(3);
  Int_t pdgMax =  std::numeric_limits<int>::min();
  Int_t pdgMin = std::numeric_limits<int>::max();
  Int_t pdg;
  escapeTree->SetBranchAddress("pdg",&pdg);
  for (Int_t i = 0; i < (Int_t)escapeTree->GetEntries(); i++) {
    escapeTree->GetEntry(i);
    if (pdg > pdgMax) pdgMax = pdg;
    if (pdg < pdgMin) pdgMin = pdg;
  }
  TH1* pdgHist = new TH1D("pdgHist", "Particle Type (PDG)", pdgMax - pdgMin + 1, pdgMin - 0.5, pdgMax + 0.5);
  for (Int_t i = 0; i < (Int_t)escapeTree->GetEntries(); i++) {
    escapeTree->GetEntry(i);
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
    TString text = TString::Format("%s (%d particles, %.1f %%)", pie4->GetSlice(i)->GetTitle(), (int)pieValue, pieValue/escapeTree->GetEntries()*100);
    legend->AddEntry(pie4->GetSlice(i),text.Data(),"f");
  }
  legend->SetX1(0.1);
  Double_t legendBottom = 0.9-statsLineHeight*legend->GetNRows();
  legend->SetY1(legendBottom);
  legend->SetX2(.9);
  legend->SetY2(.9);
  legend->Draw();
  pie4->SetY((1-legendBottom)/2);
  pie4->SetTitle("Particle Types and Ratios");

  // Add title to canvas2
  addCanvasTitle(escapeCanvas, escapeCanvas->GetTitle());
  // Save canvas
  escapeCanvas->SaveAs((*fileNameOnly+"-eesc.png").Data());

  // Return success
  return 0;
}

