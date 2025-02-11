#include <iostream> 

#include "G4MTRunManager.hh" 
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

#include "construction.hh"
#include "physics.hh"
#include "action.hh"

int main(int argc, char** argv)
{
	G4MTRunManager *runManager = new G4MTRunManager();

	runManager->SetNumberOfThreads(4);
	runManager->SetUserInitialization(new MyPhysicsList());

	MyDetectorConstruction* detector = new MyDetectorConstruction();
    MyActionInitialization* actions = new MyActionInitialization();

    // Set up the generator
    detector->SetGenerator(actions->GetGenerator());

	runManager->SetUserInitialization(detector);
	runManager->SetUserInitialization(actions);
	runManager->Initialize();
	
	
	G4UIExecutive *ui = 0;
	
	if (argc==1)
	{
		ui = new G4UIExecutive(argc, argv);
	}	
	
	G4VisManager *visManager = new G4VisExecutive();
	visManager->Initialize();
	
	G4UImanager *UImanager = G4UImanager::GetUIpointer();
	
	if(ui)
	{
		UImanager->ApplyCommand("/control/execute vis.mac");
		ui->SessionStart();
	}
	else
	{
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand(command+fileName);
	}
	
	
	
	delete visManager;
	delete ui;
	delete runManager;
	
	return 0;
}
