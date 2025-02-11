#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"

#include "generator.hh"
#include "run.hh"

class MyActionInitialization : public G4VUserActionInitialization
{
public: 
	MyActionInitialization();
	virtual ~MyActionInitialization();
	
	MyPrimaryGenerator* GetGenerator() const {return fPrimaryGenerator;}
	
	virtual void Build() const override;

private:
	MyPrimaryGenerator *fPrimaryGenerator;
};



#endif
