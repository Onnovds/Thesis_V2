#include "run.hh"

MyRunAction::MyRunAction()
{	
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	/*
	man->CreateNtuple("Hits", "Hits"); 
	man->CreateNtupleIColumn("fEvent");
	man->CreateNtupleDColumn("fX");
	man->CreateNtupleDColumn("fY");
	man->CreateNtupleDColumn("fZ");
	man->FinishNtuple(0); */
	
	man->CreateNtuple("DetectorData", "Energy Data");
	man->CreateNtupleIColumn("fEvent");
	man->CreateNtupleDColumn("fX");
	man->CreateNtupleDColumn("fY");
	man->CreateNtupleDColumn("fZ");
	man->CreateNtupleDColumn("KineticEnergy");
	man->CreateNtupleDColumn("DepositedEnergy");
	man->CreateNtupleDColumn("LET");
	man->CreateNtupleSColumn("ParticleType");
	man->CreateNtupleIColumn("ParentID");
	man->CreateNtupleIColumn("isPrimary");
	man->CreateNtupleDColumn("GeneratorEnergy");
	man->FinishNtuple();
}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	G4int runID = run->GetRunID();
	
	std::stringstream strRunID;
	strRunID << runID;
	
	G4cout << "Opening file..." << G4endl;

	man->OpenFile("/home/onno/satellite_test/build/output"+strRunID.str()+".root");
	
	// Reset particle counters at the start of each run
    	auto sdManager = G4SDManager::GetSDMpointer();
    	auto myDetector = dynamic_cast<MySensitiveDetector*>(sdManager->FindSensitiveDetector("SensitiveDetector"));
    	if (myDetector) {
        	myDetector->ResetCounts();
    	}	

}

void MyRunAction::EndOfRunAction(const G4Run*)
{
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	man->Write();
	G4cout << "Closing file..." << G4endl;
	man->CloseFile();
	
	// Output particle counts at the end of each run
    	auto sdManager = G4SDManager::GetSDMpointer();
    	auto myDetector = dynamic_cast<MySensitiveDetector*>(sdManager->FindSensitiveDetector("SensitiveDetector"));
    	if (myDetector) {
    		G4cout << "Particles counted: " << myDetector->GetTotalCount() << G4endl;
        	G4cout << "Protons counted: " << myDetector->GetProtonCount() << G4endl;
        	G4cout << "Electrons counted: " << myDetector->GetElectronCount() << G4endl;
        	G4cout << "Gamma counted: " <<myDetector->GetGammaCount() <<G4endl;
			G4cout << "Neutrons counted: " <<myDetector->GetNeutronCount() <<G4endl;
    	}
}
