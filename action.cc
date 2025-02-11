#include "action.hh"

MyActionInitialization::MyActionInitialization():G4VUserActionInitialization(), fPrimaryGenerator(new MyPrimaryGenerator())
{}

MyActionInitialization::~MyActionInitialization()
{
	delete fPrimaryGenerator;
}

void MyActionInitialization::Build() const
{
	//MyPrimaryGenerator *generator = new MyPrimaryGenerator();
	//SetUserAction(generator);
	//MyPrimaryGenerator *fPrimaryGenerator = new MyPrimaryGenerator();
    SetUserAction(fPrimaryGenerator);

	MyRunAction *runAction = new MyRunAction();
	SetUserAction(runAction); 
}
