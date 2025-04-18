#include "generator.hh"
#include "Randomize.hh"



MyPrimaryGenerator::MyPrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1); 

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "proton";
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);

	G4double energy = 200*MeV; // Energy of the beam

    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(energy); // Mean energy; can randomise in GeneratePrimaries if needed

    // Write particle info to a file
    std::ofstream outFile("particle_info.txt");
    outFile << "Particle: " << particleName << std::endl;
    outFile << "Energy: " << energy << " MeV (mean)" << std::endl;
    outFile.close();
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
    // Half-dome (hemisphere) uniform distribution
    G4double R = -100*cm; // distance from origin
    G4double theta = std::acos(1 - G4UniformRand());  // theta from 0 to pi/2 (hemisphere)
    G4double phi = 2.0 * M_PI * G4UniformRand();

    G4double x = R * std::sin(theta) * std::cos(phi);
    G4double y = R * std::sin(theta) * std::sin(phi);
    G4double z = R * std::cos(theta);

    G4ThreeVector position(x, y, z);
    G4ThreeVector direction = -position.unit(); // Point towards origin (detector)

    fParticleGun->SetParticlePosition(position);
    fParticleGun->SetParticleMomentumDirection(direction);
    fParticleGun->GeneratePrimaryVertex(anEvent);
}


/*
MyPrimaryGenerator::MyPrimaryGenerator()
{
	fParticleGun = new G4ParticleGun(1); 
	
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName="proton";
	G4ParticleDefinition *particle = particleTable->FindParticle(particleName);
	
	G4ThreeVector mom(0.,0.,1.); 
	
	G4double energy = 700*MeV; // Energy of the beam

	fParticleGun->SetParticleMomentumDirection(mom);
	fParticleGun->SetParticleEnergy(energy); 
	fParticleGun->SetParticleDefinition(particle);

	    // Write particle info to a file
    std::ofstream outFile("particle_info.txt");
    outFile << "Particle: " << particleName << std::endl;
    outFile << "Energy: " << energy / MeV << " MeV" << std::endl; // Output in MeV
    outFile.close();
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
	delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{	
	G4double radius = 1.5*mm; //Radius beam
	
	G4double phi = 2.0*M_PI*G4UniformRand(); 
	G4double r = radius * std::sqrt(G4UniformRand());
	G4double x = r * std::cos(phi);
	G4double y = r * std::sin(phi); 


	G4ThreeVector pos(x, y, -0.5*m); //Initial position of beam
	fParticleGun->SetParticlePosition(pos);
	fParticleGun->GeneratePrimaryVertex(anEvent);
}

*/