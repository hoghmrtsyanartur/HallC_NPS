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

#include <MyVisCommands.hh>
#include "G4Timer.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"

#include "HistoManager.hh"
//#include "DetectorConstructionAna02.hh"
#include "ActionInitialization.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
//#include "G4UIExecutive.hh"

//#include "G4PhysListFactory.hh"

// PS: add optical photon physics
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
//#include "QGSP_BERT.hh"
//#include "G4EmStandardPhysics_option4.hh"

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

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

	// Create an instance of the G4RunManager class
  // It controls the flow of the program and manages the event loop(s) within a run
	G4RunManager *runManager = new G4RunManager();

	// Contruct the detector
	// TODO: use TextGeometry to define the detector geometry instead?
	// PS: try using the detector construction from AnaEx02
	DetectorConstruction *detector = new DetectorConstruction();
	runManager->SetUserInitialization(detector);

	// PS: Instantiate local physics list and define physics
	// http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/GettingStarted/physicsDef.html?highlight=g4vuserphysicslists#how-to-specify-physics-processes
  // PhysicsList *physicsList = new PhysicsList();

  // PS: 5. Replace Physics
  // G4VModularPhysicsList* physicsList = new FTFP_BERT();
  // runManager->SetUserInitialization(physicsList);

  // PS: Try adding optical physics (works)
	// http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/TrackingAndPhysics/physicsProcess.html?highlight=ftfp_bert#g4opticalphysics-constructor
  // G4VModularPhysicsList* physicsList = new QGSP_BERT();
  G4VModularPhysicsList* physicsList = new FTFP_BERT();
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);

  // PS: Try set up physics using factory - like in Gears App
	// G4PhysListFactory factory = new G4PhysListFactory();
	// G4VModularPhysicsList* physlist = factory->GetReferencePhysList("FTFP_BERT_EMV");
	// physlist.SetVerboseLevel(verbose);
	// runManager->SetUserInitialization(physlist);

  runManager->SetUserInitialization(physicsList);

	// PS: Object that outputs the ROOT file

	// Instantiate General Particle Source (gps)
	// Save primary particle information in Histogram Manager

  ActionInitialization* actionInitialization = new ActionInitialization(detector);
  runManager->SetUserInitialization(actionInitialization);

  // PS: why we not initialize the Run Manager here?
  // runManager->Initialize();

  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();

  // Get the pointer to the User Interface manager (created by runManager)
  // In order for the user to issue commands to the program
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (!ui){
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode
    // Instantiate Misc Messenger that saves image
    new MyVisCommands();

    // UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();

    delete ui;
  }

	// Print CPU time
	timer->Stop();
	G4cout << "Elapsed time: " << timer->GetRealElapsed() << G4endl;

  delete visManager;
  delete runManager;

	return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
