#include "generator.hh"
#include "Randomize.hh"


MyPrimaryGenerator::MyPrimaryGenerator()
{
	fParticleGun = new G4ParticleGun(1); 
	
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName="e-";
	G4ParticleDefinition *particle = particleTable->FindParticle(particleName);
	
	G4ThreeVector mom(0.,0.,1.); 
	
	G4double energy = 0.09*MeV; // Energy of the beam

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




