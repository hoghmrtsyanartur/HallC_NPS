//
// ************************************************************************
// * License and Disclaimer                                               *
// *                                                                      *
// * The	Geant4 software	is	copyright of the Copyright Holders	of    *
// * the Geant4 Collaboration.	It is provided	under	the terms	and   *
// * conditions of the Geant4 Software License,	included in the file      *
// * LICENSE and available at	http://cern.ch/geant4/license .	These     *
// * include a list of copyright holders.                                 *
// *                                                                      *
// * Neither the authors of this software system, nor their employing     *
// * institutes,nor the agencies providing financial support for this     *
// * work	make	any representation or	warranty, express or implied, *
// * regarding	this	software system or assume any liability for its   *
// * use.	Please see the license in the file	LICENSE	and URL above     *
// * for the full disclaimer and the limitation of liability.             *
// *                                                                      *
// * This	code	implementation is the result of	the	scientific and    *
// * technical work of the GEANT4 collaboration.                          *
// * By using,	copying,	modifying or	distributing the software (or *
// * any work based	on the software)	you	agree	to acknowledge its    *
// * use	in	resulting	scientific	publications,	and indicate your *
// * acceptance of all terms of the Geant4 Software license.              *
// ************************************************************************
//
// The simulation is based on AnaEx02
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4UIExecutive.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "PhysicsList.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "HistoManager.hh"

#include "G4Timer.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char **argv) {
	// Start Timer
	G4Timer *timer = new G4Timer();
	timer->Start();

	// Detect interactive mode (if no arguments) and define UI session
	G4UIExecutive *ui = nullptr;
	if (argc == 1) {
		ui = new G4UIExecutive(argc, argv);
	}

	//Construct the default run manager
	G4RunManager *runManager = new G4RunManager;

	// Contruct the detector
	// TODO: use TextGeometry to define the detector geometry instead?
	DetectorConstruction *detector = new DetectorConstruction();
	runManager->SetUserInitialization(detector);

	// Instantiate local physics list and define physics
	PhysicsList *phys = new PhysicsList();
	runManager->SetUserInitialization(phys);

	// What is this?
	HistoManager *histoManager = new HistoManager();

	// Choose the Random engine
	G4Random::setTheEngine(new CLHEP::RanecuEngine);

	PrimaryGeneratorAction *gen_action = new PrimaryGeneratorAction(histoManager);
	runManager->SetUserAction(gen_action);

	// What is this action for?
	G4String fileNamee = "output_file.txt";
	RunAction *run_action = new RunAction(histoManager, fileNamee);
	runManager->SetUserAction(run_action);

	EventAction *event_action = new EventAction(histoManager);
	runManager->SetUserAction(event_action);

	SteppingAction *stepping_action = new SteppingAction(detector, event_action, histoManager);
	runManager->SetUserAction(stepping_action);

	if (ui) {
		// Interactive mode
		G4VisManager* visManager = new G4VisExecutive();
		visManager->Initialize();
		ui->SessionStart();
		delete ui;
	} else {
		// Batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[1];

		// Initialize visualization
		G4UImanager *uIManager = G4UImanager::GetUIpointer();
		uIManager->ApplyCommand(command+fileName);
	}

	// Print CPU time
	timer->Stop();
	G4cout << "Elapsed time: " << timer->GetRealElapsed() << G4endl;

	return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
