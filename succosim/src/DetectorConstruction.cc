#include <G4SystemOfUnits.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4UniformMagField.hh>
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>
#include <G4ChordFinder.hh>
#include <G4MultiFunctionalDetector.hh>
#include <G4Box.hh>

#include "DetectorConstruction.hh"
#include "CustomSD.hh"

#include <vector>
#include <string>

// DetectorConstruction::Construct, i.e. where the setup geometry is implemented
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // NIST database
    G4NistManager* nist = G4NistManager::Instance();
	
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // define the world and all the setup stuff (materials, volumes) here, or...
	
    // colors (boolean toggles visibility)
    G4VisAttributes* col_scinti = new G4VisAttributes(true, G4Colour::Cyan());
    G4VisAttributes* col_conv = new G4VisAttributes(true, G4Colour::Brown());
	G4VisAttributes* col_dummy = new G4VisAttributes(false, G4Colour::Red());

    // off-the-shelf materials (from NIST)
    G4Material* mat_world = nist->FindOrBuildMaterial("G4_Galactic"); // world material - G4_Galactic, G4_AIR
    G4Material* mat_scinti = nist->FindOrBuildMaterial("G4_POLYSTYRENE"); // plastic scintillator - G4_POLYSTYRENE
    G4Material* mat_conv = nist->FindOrBuildMaterial("G4_Cu"); // converter - G4_Cu, G4_Al
	G4Material* mat_dummy = nist->FindOrBuildMaterial("G4_W"); // dummy "calorimeter" - G4_W
	
	// detector features
    
    //read the nr. of scinti tiles from the .conf file
    int nveto(1),nDC(1);
    ifstream conf_in("construction.conf"); string line;
    if(conf_in.is_open()){
        getline(conf_in,line); nveto=stoi(line);
        getline(conf_in,line); nDC=stoi(line);
        conf_in.close();
    }
    
	G4int nModules = 1; // nr. of converter+scintillator (veto excluded) modules - TO BE DEFINED ALSO IN DetectorConstruction::ConstructSDandField (THIS FILE) & IN EventAction.cc!
	
	G4double gapZLayers = 2 * cm; // edge-edge space between different layers (i.e. between scintillator and converter layers)
	G4double gapZScinti = 2 * mm; // in case multiple scintillating tiles are used within single scintillator layers, space between them
	G4double sideScinti = 25 * cm; // transverse size of scintillating tiles
	G4double thicknessScinti = 4 * mm; // thickness of scintillating tiles
	G4double sideConv = 10 * cm; // transverse size of converter tiles
	G4double thicknessConv = 0.2 * mat_conv->GetRadlen(); // total thickness of converter tiles (as a fraction of X_0) - if multiple modules are selected, value is splitted evenly between modules
	G4int nTilesVeto = nveto; // nr. of scintillating tiles in the charge veto
	G4int nTilesDC = nDC; // nr. of scintillating tiles in each downstream scintillator
	
	
	G4double thicknessVeto = nTilesVeto*thicknessScinti + (nTilesVeto-1)*gapZScinti;
	G4double thicknessConvSingle = thicknessConv/nModules;
	G4double thicknessDC = nTilesDC*thicknessScinti + (nTilesDC-1)*gapZScinti;
	
    // world
    G4double worldSizeX = 0.5 * m;
    G4double worldSizeY = 0.5 * m;
    G4double worldSizeZ = 30 * m; // note: also check beam source position in PrimaryGeneratorAction.cc
    G4VSolid* worldBox = new G4Box("World_Solid", worldSizeX/2, worldSizeY/2, worldSizeZ/2);

    G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, mat_world, "World_Logical");
    G4VisAttributes* visAttrWorld = new G4VisAttributes();
    visAttrWorld->SetVisibility(false);
    worldLog->SetVisAttributes(visAttrWorld);

    G4VPhysicalVolume* worldPhys = new G4PVPlacement(nullptr, {}, worldLog, "World", nullptr, false, 0);	
	
	// solid tiles
	G4VSolid* solid_scinti = new G4Box("scintillatingTileSolid", sideScinti / 2, sideScinti / 2, thicknessScinti / 2); // scintillating tiles
	G4VSolid* solid_conv = new G4Box("converterTileSolid", sideConv / 2, sideConv / 2, thicknessConvSingle / 2); // converter tiles
	G4VSolid* solid_dummy = new G4Box("dummyCaloSolid", worldSizeX/2, worldSizeY/2, 1*m); // dummy "calorimeter"
	
	
	// detector individual parts - charge veto
	G4LogicalVolume* log_scintiVeto = new G4LogicalVolume(solid_scinti, mat_scinti, "scintillatingTileVetoLog");
	log_scintiVeto->SetVisAttributes(col_scinti);
    for (int i = 0; i < nTilesVeto; i++)
    {
        G4ThreeVector zTilesVeto = G4ThreeVector(0., 0., thicknessScinti/2 + i*(thicknessScinti+gapZScinti));
        ostringstream tilesVetoName; tilesVetoName << "scintillatingTileVeto_" << i;
        new G4PVPlacement(nullptr, zTilesVeto, log_scintiVeto, tilesVetoName.str(), worldLog, false, i);
    }
	
	// detector individual parts - converter layers
	G4LogicalVolume* log_conv = new G4LogicalVolume(solid_conv, mat_conv, "converterTileLog");
	log_conv->SetVisAttributes(col_conv);
    for (int i = 0; i < nModules; i++)
    {
        G4ThreeVector zConv = G4ThreeVector(0., 0., thicknessVeto+gapZLayers+thicknessConvSingle/2 + i*(thicknessConvSingle+2*gapZLayers+thicknessDC));
        ostringstream convName; convName << "converterTile_" << i;
        new G4PVPlacement(nullptr, zConv, log_conv, convName.str(), worldLog, false, i);
    }
	
	// detector invidivual parts - downstream counters
	vector<G4LogicalVolume*> log_scintiDC;
	for (int j = 0; j < nModules; j++){
		ostringstream logScintiDCName; logScintiDCName << "scintillatingTileDCLog_" << j;
		log_scintiDC.push_back(new G4LogicalVolume(solid_scinti, mat_scinti, logScintiDCName.str()));
		log_scintiDC[j]->SetVisAttributes(col_scinti);
		for (int i = 0; i < nTilesDC; i++)
		{
			G4ThreeVector zTilesDC = G4ThreeVector(0., 0., thicknessVeto+2*gapZLayers+thicknessConvSingle+thicknessScinti/2 + j*(thicknessDC+2*gapZLayers+thicknessConvSingle) + i*(thicknessScinti+gapZScinti));
			ostringstream tilesDCName; tilesDCName << "scintillatingTileDC_" << j << "_" << i;
			new G4PVPlacement(nullptr, zTilesDC, log_scintiDC[j], tilesDCName.str(), worldLog, false, i);
		}
	}
	
	// dummy "calorimeter" to debug beam energy
	G4LogicalVolume* log_dummy = new G4LogicalVolume(solid_dummy, mat_dummy, "dummyCaloLog");
	log_dummy->SetVisAttributes(col_dummy);
	new G4PVPlacement(nullptr, G4ThreeVector(0., 0., worldSizeZ/2-0.5*m), log_dummy, "dummyCalo", worldLog, false, 0);

    // --------------------------------------------------
    // ...uncomment this line for the test setup (implemented in include/TestMode.cc) 
    // G4VPhysicalVolume* worldPhys = SetupTest(nist);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	
    // print list of defined material
    G4cout << "-----" << G4endl;
    G4cout << "| DetectorConstruction.cc: material list" << G4endl;
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
    G4cout << "-----" << G4endl;
	
    return worldPhys;
}

// DetectorConstruction::ConstructSDandField, i.e. where the sensitive detectors and magnetic fields are implemented
void DetectorConstruction::ConstructSDandField()
{
    // load the sensitive detector manager
    G4SDManager* sdm = G4SDManager::GetSDMpointer();
    sdm->SetVerboseLevel(1);  // set sensitive detector manager verbosity here

    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // create the sensitive detectors and bin them to the logical volumes here, or...
	
	// detector features
	G4int nModules = 2; // nr. of converter+scintillator (veto excluded) modules - TO BE DEFINED ALSO IN DetectorConstruction::Construct (THIS FILE) & IN EventAction.cc!
	
    // veto
    VolumeEDepSD* sd_scintiVeto = new VolumeEDepSD("scintiVetoSD");
    SetSensitiveDetector("scintillatingTileVetoLog", sd_scintiVeto);
    sdm->AddNewDetector(sd_scintiVeto);
	
	// downstream counters
	vector<VolumeEDepSD*> sd_scintiDC;
	for (int j = 0; j < nModules; j++){
		ostringstream sdScintiDCName; sdScintiDCName << "scintiDCSD_" << j;
		ostringstream logScintiDCName; logScintiDCName << "scintillatingTileDCLog_" << j;
		sd_scintiDC.push_back(new VolumeEDepSD(sdScintiDCName.str()));
		SetSensitiveDetector(logScintiDCName.str(), sd_scintiDC[j]);
		sdm->AddNewDetector(sd_scintiDC[j]);
	}
	
	// dummy "calorimeter" to debug beam energy
    VolumeEDepSD* sd_dummy = new VolumeEDepSD("dummyCaloSD");
    SetSensitiveDetector("dummyCaloLog", sd_dummy);
    sdm->AddNewDetector(sd_dummy);
	
    // --------------------------------------------------
    // ...uncomment this line for the test sensitive detectors (implemented in include/TestMode.cc)
    // SDTest(sdm);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

// ============================================================
// ============================================================
// implement custom methods here

