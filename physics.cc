#include "physics.hh"

MyPhysicsList::MyPhysicsList()
{
	// Electromagnetic physics
	RegisterPhysics (new G4EmStandardPhysics()); 

	RegisterPhysics(new G4EmLivermorePhysics());            // For detailed EM processes incl. X-rays
    RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP());      // For hadronic + neutron HP models
    RegisterPhysics(new G4NeutronTrackingCut());            // Optional: neutron tracking limits
    RegisterPhysics(new G4RadioactiveDecayPhysics());       // Optional: radioactive decay
};

MyPhysicsList::~MyPhysicsList()
{};
