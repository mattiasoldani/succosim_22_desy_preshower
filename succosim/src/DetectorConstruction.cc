#include "DetectorConstruction.hh"
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
#include <CustomSD.hh>

// DetectorConstruction::Construct, i.e. where the setup geometry is implemented
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // NIST database
    G4NistManager* nist = G4NistManager::Instance();
	
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // define the world and all the setup stuff (materials, volumes) here, or...

    // --------------------------------------------------
    // ...uncomment this line for the test setup
	G4VPhysicalVolume* worldPhys = SetupTest(nist);
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
	
	// --------------------------------------------------
	// ...uncomment this line for the test sensitive detectors
	SDTest(sdm);
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

// ============================================================
// ============================================================
// DetectorConstruction::SetupTest, i.e. a sample setup for testing (here the physical objects, with world & materials included)
G4VPhysicalVolume* DetectorConstruction::SetupTest(G4NistManager* nist)
{	
	// colors
	G4VisAttributes* magenta = new G4VisAttributes(G4Colour::Magenta());
	G4VisAttributes* grey = new G4VisAttributes(G4Colour::Grey());
    G4VisAttributes* brown = new G4VisAttributes(G4Colour::Brown());
    G4VisAttributes* cyan = new G4VisAttributes(G4Colour::Cyan());

    // manual material: BC400 scintillator
    G4Element* elH = new G4Element("Hydrogen", "H", 1., 1.0079 * g/mole);
    G4Element* elC = new G4Element("Carbon", "C", 6., 12.01 * g/mole);
    G4Material* bc400 = new G4Material("BC400", 1.032*g/cm3, 2);
    bc400->AddElement(elH, 0.085);
    bc400->AddElement(elC, 0.915);

    // off-the-shelf materials (from NIST)
	G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); // air
    G4Material* lead = nist->FindOrBuildMaterial("G4_Pb");  // lead
    G4Material* silicon = nist->FindOrBuildMaterial("G4_Si");  // silicon
    G4Material* aluminum = nist->FindOrBuildMaterial("G4_Al");  // aluminum
	G4Material* iron = nist->FindOrBuildMaterial("G4_Fe"); // iron 
	G4Material* leadGlass = nist->FindOrBuildMaterial("G4_GLASS_LEAD"); // lead glass
	G4Material* tungsten = nist->FindOrBuildMaterial("G4_W"); // tungsten
	
    // world
    G4double worldSizeX = 5 * m;
    G4double worldSizeY = 1 * m;
    G4double worldSizeZ = 1 * m;
    G4VSolid* worldBox = new G4Box("World_Solid", worldSizeX / 2, worldSizeY / 2, worldSizeZ / 2);

    G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, air, "World_Logical");
    G4VisAttributes* visAttrWorld = new G4VisAttributes();
    visAttrWorld->SetVisibility(false);
    worldLog->SetVisAttributes(visAttrWorld);
	
	// detector positions, general
	G4double xTrackerSiliCentre0 = 0 * cm;  // silicon tracking module 0 longitudinal centre (4.5 cm long)
	G4double xTrackerSiliCentre1 = 120 * cm;  // silicon tracking module 1 longitudinal centre (4.5 cm long)
	G4double xTrackerSiliCentre2 = 130 * cm;  // silicon tracking module 2 longitudinal centre (4.8 cm long)
	G4double xMagnet = 180 * cm;  // bending magnet longitudinal centre (50 cm long)
	G4double xECaloFront = 230 * cm;  // electron calorimeter longitudinal front (10 cm long)
	G4double xPhCaloFront = 220 * cm;  // electron calorimeter longitudinal front (25 cm long)
	G4double xTgt = 125 * cm;  // target longitudinal centre (~3 mm long) 

    G4VPhysicalVolume* worldPhys = new G4PVPlacement(nullptr, {}, worldLog, "World", nullptr, false, 0);
	
	// target
	G4double tgtThickness = 3.504 * mm;
	G4double tgtWidth = 1 * cm;
	G4double tgtHeight = 1 * cm;
	G4VSolid* tgtBox = new G4Box("Tgt_Solid", tgtThickness / 2, tgtWidth / 2, tgtHeight / 2);
	
	G4LogicalVolume* tgtLog = new G4LogicalVolume(tgtBox, tungsten, "Tgt_Logical");
	tgtLog->SetVisAttributes(brown);
	
	new G4PVPlacement(nullptr, G4ThreeVector(xTgt, 0, 0), tgtLog, "Tgt", worldLog, false, 0);
	
	// bending magnet
	G4double magnetYokeThickness = 50 * cm;
	G4double magnetYokeWidth = 50 * cm;
	G4double magnetYokeHeight = 10 * cm;
	G4double magnetFieldHeight = 15 * cm;
	G4VSolid* magnetYokeBox = new G4Box("magnetYoke_Solid", magnetYokeThickness / 2, magnetYokeWidth / 2, magnetYokeHeight / 2);
	G4VSolid* magnetFieldBox = new G4Box("magnetField_Solid", magnetYokeThickness / 2, magnetYokeWidth / 2, magnetFieldHeight / 2);
	
	G4LogicalVolume* magnetYokeLog = new G4LogicalVolume(magnetYokeBox, iron, "magnetYoke_Logical");
    magnetYokeLog->SetVisAttributes(magenta);
	G4LogicalVolume* magnetFieldLog = new G4LogicalVolume(magnetFieldBox, air, "magnetField_Logical");
	G4VisAttributes* visAttrField = new G4VisAttributes();
    visAttrField->SetVisibility(false);
    magnetFieldLog->SetVisAttributes(visAttrField);
	
	new G4PVPlacement(nullptr, G4ThreeVector(xMagnet, 0, + (magnetFieldHeight / 2 + magnetYokeHeight / 2)), magnetYokeLog, "magnetYoke_Up", worldLog, false, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(xMagnet, 0, - (magnetFieldHeight / 2 + magnetYokeHeight / 2)), magnetYokeLog, "magnetYoke_Down", worldLog, false, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(xMagnet, 0, 0), magnetFieldLog, "magnetField", worldLog, false, 0);
	
	// silicon tracking system
	// note: it's best for tracking layers used as sensitive ones to have separate logical volumes even if identical
	G4double trackerSiliThickness = 300 * um;
	G4double trackerSiliWidth = 2 * cm;
	G4double trackerSiliHeight = 2 * cm;
	G4VSolid* trackerSiliBox = new G4Box("TrackerSili_Solid", trackerSiliThickness / 2, trackerSiliWidth / 2, trackerSiliHeight / 2);
	G4double trackerSiliBigThickness = 400 * um;
	G4double trackerSiliBigWidth = 10 * cm;
	G4double trackerSiliBigHeight = 10 * cm;
	G4VSolid* trackerSiliBigBox = new G4Box("TrackerSiliBig_Solid", trackerSiliBigThickness / 2, trackerSiliBigWidth / 2, trackerSiliBigHeight / 2);
	G4double trackerAlThickness = 10 * um;
	G4double trackerAlWidth = 20 * cm;
	G4double trackerAlHeight = 20 * cm;
	G4VSolid* trackerAlBox = new G4Box("TrackerAl_Solid", trackerAlThickness / 2, trackerAlWidth / 2, trackerAlHeight / 2);
	
	G4LogicalVolume* trackerSiliLog0 = new G4LogicalVolume(trackerSiliBox, silicon, "TrackerSili_Logical_0");  // tracker 0 is small
	G4LogicalVolume* trackerSiliLog1 = new G4LogicalVolume(trackerSiliBox, silicon, "TrackerSili_Logical_1");  // tracker 1 is small
	G4LogicalVolume* trackerSiliLog2_0 = new G4LogicalVolume(trackerSiliBigBox, silicon, "TrackerSili_Logical_2_0");  // tracker 2 is big & double -- 0th
	G4LogicalVolume* trackerSiliLog2_1 = new G4LogicalVolume(trackerSiliBigBox, silicon, "TrackerSili_Logical_2_1");  // tracker 2 is big & double -- 1st
    G4LogicalVolume* trackerAlLog = new G4LogicalVolume(trackerAlBox, aluminum, "TrackerAl_Logical");
    trackerSiliLog0->SetVisAttributes(grey);
	trackerSiliLog1->SetVisAttributes(grey);
	trackerSiliLog2_0->SetVisAttributes(grey);
	trackerSiliLog2_1->SetVisAttributes(grey);
    trackerAlLog->SetVisAttributes(grey);
	
	G4double xTrackerAlFrontCentre0 = xTrackerSiliCentre0 - (2.25*cm - trackerAlThickness / 2);
	G4double xTrackerAlBackCentre0 = xTrackerSiliCentre0 + (2.25*cm - trackerAlThickness / 2);
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerSiliCentre0, 0, 0), trackerSiliLog0, "TrackerSili_0", worldLog, false, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerAlFrontCentre0, 0, 0), trackerAlLog, "TrackerSili_0_Al_Front", worldLog, false, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerAlBackCentre0, 0, 0), trackerAlLog, "TrackerSili_0_Al_Rear", worldLog, false, 0);
	G4double xTrackerAlFrontCentre1 = xTrackerSiliCentre1 - (2.25*cm - trackerAlThickness / 2);
	G4double xTrackerAlBackCentre1 = xTrackerSiliCentre1 + (2.25*cm - trackerAlThickness / 2);
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerSiliCentre1, 0, 0), trackerSiliLog1, "TrackerSili_1", worldLog, false, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerAlFrontCentre1, 0, 0), trackerAlLog, "TrackerSili_1_Al_Front", worldLog, false, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerAlBackCentre1, 0, 0), trackerAlLog, "TrackerSili_1_Al_Rear", worldLog, false, 0);
	G4double xTrackerAlFrontCentre2 = xTrackerSiliCentre2 - (2.4*cm - trackerAlThickness / 2);
	G4double xTrackerAlBackCentre2 = xTrackerSiliCentre2 + (2.4*cm - trackerAlThickness / 2);
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerSiliCentre2-0.5*cm, 0, 0), trackerSiliLog2_0, "TrackerSili_2_0", worldLog, false, 0);  // tracker 2 sensors are separated by 1 cm
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerSiliCentre2+0.5*cm, 0, 0), trackerSiliLog2_1, "TrackerSili_2_1", worldLog, false, 0);  // tracker 2 sensors are separated by 1 cm
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerAlFrontCentre2, 0, 0), trackerAlLog, "TrackerSili_2_Al_Front", worldLog, false, 0);
	new G4PVPlacement(nullptr, G4ThreeVector(xTrackerAlBackCentre2, 0, 0), trackerAlLog, "TrackerSili_2_Al_Rear", worldLog, false, 0);

	// electron sampling (BC400 & lead) calorimeter
    G4double caloThicknessLayer = 0.5 * cm;
    G4double caloWidth = 40 * cm;
    G4double caloHeight = 10 * cm;
    G4VSolid* absBox = new G4Box("ECalTest_Abs_Solid", caloThicknessLayer / 2, caloWidth / 2, caloHeight / 2);
    G4VSolid* sciBox = new G4Box("ECalTest_Sci_Solid", caloThicknessLayer / 2, caloWidth / 2, caloHeight / 2);

    G4LogicalVolume* absLog = new G4LogicalVolume(absBox, lead, "ECalTest_Abs_Logical");
    G4LogicalVolume* sciLog = new G4LogicalVolume(sciBox, bc400, "ECalTest_Sci_Logical");
    absLog->SetVisAttributes(brown);
    sciLog->SetVisAttributes(cyan);

    G4int numberOfLayers = 10;
    G4double minX = xECaloFront + caloThicknessLayer / 2;
    for (int i = 0; i < numberOfLayers; i++)
    {
        G4ThreeVector absorberPosition = G4ThreeVector(minX + i*2 * caloThicknessLayer, 10 * cm + caloWidth / 2, 0);
        G4ThreeVector scintillatorPosition = G4ThreeVector(minX + (i * 2 + 1) * caloThicknessLayer, 10 * cm + caloWidth / 2, 0);
        ostringstream aName; aName << "ECalTest_Abs_" << i;
        new G4PVPlacement(nullptr, absorberPosition, absLog, aName.str(), worldLog, false, i);
        ostringstream sName; sName << "ECalTest_Sci_" << i;
        new G4PVPlacement(nullptr, scintillatorPosition, sciLog, sName.str(), worldLog, false, i);
    }
	
	// photon omogeneous (lead glass) calorimeter
	G4double caloPhThickness = 25 * cm;
    G4double caloPhWidth = 10 * cm;
    G4double caloPhHeight = 10 * cm;
	G4VSolid* caloPhBox = new G4Box("PhCalTest_Solid", caloPhThickness / 2, caloPhWidth / 2, caloPhHeight / 2);
	
	G4LogicalVolume* caloPhLog = new G4LogicalVolume(caloPhBox, leadGlass, "PhCalTest_Logical");
    caloPhLog->SetVisAttributes(cyan);  // color already defined above
	
	new G4PVPlacement(nullptr, G4ThreeVector(xPhCaloFront + caloPhThickness / 2, 0, 0), caloPhLog, "PhCalTest", worldLog, false, 0);

    return worldPhys;
}

// DetectorConstruction::SDTest, i.e. a sample setup for testing (here the sensitive detectors & magnetic fields)
void DetectorConstruction::SDTest(G4SDManager* sdm)
{
	// bending magnet field
	G4UniformMagField* bendingField = new G4UniformMagField(G4ThreeVector(0., 0., 1.5 * tesla));
	G4LogicalVolume* fieldLog = G4LogicalVolumeStore::GetInstance()->GetVolume("magnetField_Logical");
	G4FieldManager* fieldManager = new G4FieldManager(bendingField);
	fieldLog->SetFieldManager(fieldManager, true);
	
	// SD -- electron calorimeter
	VolumeEDepSD* eCalSD = new VolumeEDepSD("ECalTest_SD");
	SetSensitiveDetector("ECalTest_Abs_Logical", eCalSD);
    SetSensitiveDetector("ECalTest_Sci_Logical", eCalSD);
    sdm->AddNewDetector(eCalSD);
	
	// SD -- photon calorimeter
	VolumeEDepSD* gammaCalSD = new VolumeEDepSD("PhCalTest_SD");
	SetSensitiveDetector("PhCalTest_Logical", gammaCalSD);
    sdm->AddNewDetector(gammaCalSD);
	
	// SD -- tracking system (1 sensitive detector per tracking plane)
	VolumeTrackingSD* trackerSD0 = new VolumeTrackingSD("Tracker_SD_0");
	SetSensitiveDetector("TrackerSili_Logical_0", trackerSD0);
	sdm->AddNewDetector(trackerSD0);
	VolumeTrackingSD* trackerSD1 = new VolumeTrackingSD("Tracker_SD_1");
	SetSensitiveDetector("TrackerSili_Logical_1", trackerSD1);
	sdm->AddNewDetector(trackerSD1);
	VolumeTrackingSD* trackerSD2_0 = new VolumeTrackingSD("Tracker_SD_2_0");
	SetSensitiveDetector("TrackerSili_Logical_2_0", trackerSD1);
	sdm->AddNewDetector(trackerSD2_0);
	VolumeTrackingSD* trackerSD2_1 = new VolumeTrackingSD("Tracker_SD_2_1");
	SetSensitiveDetector("TrackerSili_Logical_2_1", trackerSD1);
	sdm->AddNewDetector(trackerSD2_1);
}

// ============================================================
// ============================================================
// implement custom methods here

