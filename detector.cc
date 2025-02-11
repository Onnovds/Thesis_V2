#include "detector.hh"
#include "generator.hh"

MySensitiveDetector::MySensitiveDetector(G4String name, MyPrimaryGenerator* generator) : G4VSensitiveDetector(name), totalCount(0), protonCount(0), electronCount(0), gammaCount(0), neutronCount(0), fPrimaryGenerator(generator)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

void MySensitiveDetector::ResetCounts() {
	totalCount = 0;
	protonCount = 0;
	electronCount = 0;
	gammaCount = 0;
    neutronCount = 0;
}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
    G4Track *track = aStep->GetTrack();
    
    //Particle types and counters
    G4String particleType = track->GetDefinition()->GetParticleName();
    
    if (particleType == "proton"){
    	protonCount++;
    } else if (particleType == "e-"){
    	electronCount++;
    } else if (particleType == "gamma"){
    	gammaCount++;
    } else if (particleType == "neutron"){
        neutronCount++;
    }
    totalCount++;
    
    // Makes proton stop when it hits detector
    track->SetTrackStatus(fStopAndKill);
	
    // Get the kinetic energy at the start of the step
    G4double kineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

    // Get the energy deposited in this step
    G4double depositedEnergy = aStep->GetTotalEnergyDeposit();

    // Access the position where the hit occurred
    G4ThreeVector hitPosition = aStep->GetPreStepPoint()->GetPosition();
    
    // Get the event ID number
    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    // Get the parent ID number (primary = 0, otherwise secondary)
    
    G4int parentID = track->GetParentID();
    
    bool isPrimary = (parentID == 0) ? 1 : 0;

    // Get the Generator energy
    G4double GeneratorEnergy = fPrimaryGenerator->GetParticleEnergy();
    
    // Outputs when running to check if it outputs
    G4cout << "EventID:" << evt 
           << " Kinetic Energy: " << kineticEnergy / keV << " keV, "
           << "Deposited Energy: " << depositedEnergy / keV << " keV, "
           << "Particle Type: " << particleType
           << " Position: " << hitPosition
           << " Generator Energy: " << GeneratorEnergy /MeV << " MeV"<<  G4endl;

    // Store values in Ntuple
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleIColumn(0, evt);
    analysisManager->FillNtupleDColumn(1, hitPosition.x());
    analysisManager->FillNtupleDColumn(2, hitPosition.y());
    analysisManager->FillNtupleDColumn(3, hitPosition.z());
    analysisManager->FillNtupleDColumn(4, kineticEnergy);
    analysisManager->FillNtupleDColumn(5, depositedEnergy);
    
    
    // Only consider charged particles
    if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0) {
        G4double dE = aStep->GetTotalEnergyDeposit();
        G4double dx = aStep->GetStepLength();

        if (dx > 0) {
            G4double LET = dE / dx;

            LET /= (keV/um);  //Convert to keV/um

            G4cout << "LET: " << LET << " keV/um, " << G4endl; 

            // Store or output LET
            analysisManager->FillNtupleDColumn(6, LET);
        }
    }
    analysisManager->FillNtupleSColumn(7, particleType);
    analysisManager->FillNtupleIColumn(8, parentID);
    analysisManager->FillNtupleIColumn(9, isPrimary); 
    analysisManager->FillNtupleDColumn(10, GeneratorEnergy);
    analysisManager->AddNtupleRow();



	return true;
}

