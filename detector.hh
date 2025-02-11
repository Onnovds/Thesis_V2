#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh" 
#include "generator.hh"

class MySensitiveDetector : public G4VSensitiveDetector
{
public: 
	MySensitiveDetector(G4String name, MyPrimaryGenerator* generator);
	~MySensitiveDetector();
	
	G4int GetTotalCount() const{return totalCount ;}
	G4int GetProtonCount() const{return protonCount ;}
	G4int GetElectronCount() const{return electronCount ;}
	G4int GetGammaCount() const{return gammaCount ;}
	G4int GetNeutronCount() const{return neutronCount ;}
	
	void ResetCounts();
private:
	virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
	
	G4int totalCount;
	G4int protonCount;
	G4int electronCount;
	G4int gammaCount;
	G4int neutronCount;
	MyPrimaryGenerator* fPrimaryGenerator;
};

#endif
