#include "physics.hh"

MyPhysicsList::MyPhysicsList()
{
	// Electromagnetic physics
	RegisterPhysics (new G4EmStandardPhysics()); 

	// Decays
    RegisterPhysics(new G4DecayPhysics());

    // Hadronic interactions (optional for nuclear effects)
    RegisterPhysics(new G4HadronElasticPhysics());
};

MyPhysicsList::~MyPhysicsList()
{};
