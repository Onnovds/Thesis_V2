#include "construction.hh"
#include "G4RotationMatrix.hh"

MyDetectorConstruction::MyDetectorConstruction() : logicDetector(nullptr), fPrimaryGenerator(nullptr)
{};

MyDetectorConstruction::~MyDetectorConstruction()
{};

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
	G4NistManager *nist = G4NistManager::Instance();
	
	// Materials for Model
	/*
	G4Material *Si = new G4Material("Si", 2.329*g/cm3, 1);
	Si->AddElement(nist->FindOrBuildElement("Si"), 1);
	G4Material *Al = new G4Material("Al", 2.700*g/cm3, 1);
	Al->AddElement(nist->FindOrBuildElement("Al"), 1); 
	*/
	
	// Better way of defining materials from pure elements:
	G4Material* Silicon = new G4Material("Silicon", 14., 28.0855*g/mole, 2.329*g/cm3);
	G4Material* Aluminium = new G4Material("Aluminium", 13., 26.9815385*g/mole, 2.700*g/cm3);
	
	/*
	// Materials for tutorial
	G4Material *H2O = new G4Material("H2O", 1.000*g/cm3, 2);
	H2O->AddElement(nist->FindOrBuildElement("H"), 2);
	H2O->AddElement(nist->FindOrBuildElement("O"), 1);
	
	G4Material *SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
	SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
	SiO2->AddElement(nist->FindOrBuildElement("O"), 2);
	
	G4Element *C = nist->FindOrBuildElement("C");
	
	G4Material *Aerogel = new G4Material("Aerogel", 0.200*g/cm3, 3);
	Aerogel->AddMaterial(SiO2, 62.5*perCent); 
	Aerogel->AddMaterial(H2O, 37.4*perCent);
	Aerogel->AddElement(C, 0.1*perCent); 
	
	G4double energy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
	G4double rindexAerogel[2] = {1.1, 1.1}; 
	G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
	mptAerogel->AddProperty("RINDEX", energy, rindexAerogel, 2);
	
	Aerogel->SetMaterialPropertiesTable(mptAerogel); 
	*/
		
	// Material for world 	
	G4Material *worldMat = nist->FindOrBuildMaterial("G4_Galactic"); // Vacuum (but not a perfect one, density is 1.e-25 g/cm3)




	/*
	G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
	G4double rindexWorld[2] = {1.0, 1.0};
	mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);
	
	
	worldMat->SetMaterialPropertiesTable(mptWorld);
	*/
		
	G4Box *solidWorld = new G4Box("solidWorld", 1.0*m, 1.0*m, 2.0*m);
	
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	
	G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);
	
	// Material for Model
	
	//Commented out: No Al slab, Otherwise with 
	
	
	G4double slabWidth = 10.75*mm;  // Thickness of the slab 
	G4Box *solidSlab = new G4Box("solidSlab", 0.25*m, 0.25*m, slabWidth/2); //Half-thickness should be given 
	G4LogicalVolume *logicSlab = new G4LogicalVolume(solidSlab, Aluminium, "logicSlab");
	G4VPhysicalVolume *physSlab = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.+slabWidth/2), logicSlab, "physSlab", logicWorld, false, 0, true);   //Middle of volume
	

	//--------------------------------------------


	// Define the rotation matrix for a 60-degree rotation around the Y-axis
	//G4RotationMatrix* rotationMatrix = new G4RotationMatrix();
	//rotationMatrix->rotateY(0.*deg);  // Rotate 60 degrees around the Y-axis
	
	G4double pixelWidth = 14*mm/256;
	G4double pixelHeight = 14*mm/256;
	G4double pixelDepth = 0.5*mm; 
	G4Box *solidDetector = new G4Box("solidDetector", pixelWidth/2, pixelHeight/2, pixelDepth/2);
	logicDetector = new G4LogicalVolume(solidDetector, Silicon, "logicDetector");

	G4double centerX = 0.0;  // Center X position of the array
	G4double centerY = 0.0;  // Center Y position of the array
	G4double centerZ = 0.2*m - pixelDepth/2;  // Center Z position of the array

	for(G4int i = 0; i<256; i++){
		for(G4int j = 0; j<256; j++){
			G4double posX = -7*mm + (i+0.5)*pixelWidth;
			G4double posY = -7*mm + (j+0.5)*pixelHeight;
			// Placement change of detector for testing before and after slab
			// Normal position (location of TPX sensor)
			//G4VPhysicalVolume *physDetector = new G4PVPlacement(0, G4ThreeVector(posX, posY, 0.2*m-pixelDepth/2), logicDetector, "physDetector", logicWorld, false, i*256+j, true); 
			
			// Slab at 60degree angle:
			// Rotate each pixel individually around the center  
			/*
        	G4ThreeVector position(posX - centerX, posY - centerY, 0.0);
        	position.rotateX(60.*deg);  // Rotate around the Y-axis
        
        	position += G4ThreeVector(centerX, centerY, centerZ);  // Translate back
			G4VPhysicalVolume *physDetector = new G4PVPlacement(0, position, logicDetector, "physDetector", logicWorld, false, i*100+j, true);  
			*/ 
			
			// Before slab.c
			//G4VPhysicalVolume *physDetector = new G4PVPlacement(0, G4ThreeVector(posX, posY, -0.858*m), logicDetector, "physDetector", logicWorld, false, i*256+j, true);  //14.2cm from beam generator which is at -1.0m = -0.858*m --> Slab 5 is 18.2cm away = -0.818*m   --> Slab 4 but close to detector 0.9995-0.142= 0.8575
			
			//After slab
			G4VPhysicalVolume *physDetector = new G4PVPlacement(0, G4ThreeVector(posX, posY, 0.1*mm+slabWidth+pixelDepth/2), logicDetector, "physDetector", logicWorld, false, i*256+j, true); //14.2cm from beam generator which is at -1.0m = -0.858*m + 2.04mm of detector + 0.5mm/2 (detectordepth) 
		}
		
	}
	// G4VPhysicalVolume *physDetector = new G4PVPlacement(0, G4ThreeVector(0., 0., 2.0*m), logicDetector, "physDetector", logicWorld, false, 0, true); 
	
	
	
	
	// Material for tutorial
	/*
	G4Box *solidRadiator = new G4Box("solidRadiator", 0.4*m, 0.4*m, 0.01*m); 
	
	G4LogicalVolume *logicRadiator = new G4LogicalVolume(solidRadiator, Aerogel, "logicRadiator"); 
	
	G4VPhysicalVolume *physRadiator = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.25*m), logicRadiator, "physRadiator", logicWorld, false, 0, true);
	*/
	
	
	// Return of world volume and its properties
	return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
	G4cout << "Assigning SensitiveDetector to logicDetector" << G4endl;
	if (logicDetector) { 
		if (!fPrimaryGenerator) {
            G4cerr << "Error: Generator is not set!" << G4endl;
            return;
        }
	MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector", fPrimaryGenerator);
	G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);
	logicDetector->SetSensitiveDetector(sensDet); }
	else {
	G4cerr << "Error: logicDetector not defined!" << G4endl;
	}
	
}
