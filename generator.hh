#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
	MyPrimaryGenerator();
	~MyPrimaryGenerator();

	G4String GetParticleName() const {return fParticleGun->GetParticleDefinition()->GetParticleName();}
	G4double GetParticleEnergy() const {return fParticleGun->GetParticleEnergy();}
	
	virtual void GeneratePrimaries(G4Event*);
private:
	G4ParticleGun *fParticleGun;
};

#endif
