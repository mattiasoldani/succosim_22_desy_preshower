#include "PrimaryGeneratorAction.hh"
#include <G4SystemOfUnits.hh>
#include <G4ParticleTable.hh>
#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <Randomize.hh>
#include <math.h>

using namespace std;

// PrimaryGeneratorAction::PrimaryGeneratorAction, i.e. where the particle gun is defined and the beam features that are constant throughout the run are set
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
	// define the particle gun
	fGun = new G4ParticleGun();
	
	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// set the beam features that are constant throughout the run here, or...

	// --------------------------------------------------
	// ...uncomment this line for the test beam
	BeamFeaturesFxdTest(fGun);
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{delete fGun;}

// PrimaryGeneratorAction::GeneratePrimaries, i.e. where the beam features that change at each event are set and the particles are shot
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// set the beam features that change at each event here, or...

	// --------------------------------------------------
	// ...uncomment this line for the test beam
	BeamFeaturesRndTest(fGun);
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	
	// shot the event primary particle
	fGun->GeneratePrimaryVertex(anEvent);
}

// ============================================================
// ============================================================
// PrimaryGeneratorAction::BeamFeaturesFxdTest, i.e. definition of a sample beam for testing (constant features here)
void PrimaryGeneratorAction::BeamFeaturesFxdTest(G4ParticleGun* fGunTest)
{	
	// constant features: particle type, energy
    G4ParticleDefinition* myParticle;
    myParticle = G4ParticleTable::GetParticleTable()->FindParticle("e-");
    fGunTest->SetParticleDefinition(myParticle);
    G4double energy = 1 * GeV;
    fGunTest->SetParticleEnergy(energy);
}

// PrimaryGeneratorAction::BeamFeaturesRndTest, i.e. definition of a sample beam for testing (randomised features here)
void PrimaryGeneratorAction::BeamFeaturesRndTest(G4ParticleGun* fGunTest)
{
	// randomised features: source position (transverse, uniform in a square, whereas longitudinal is fixed), momentum direction
    G4double xFixed = -3 * cm;
    G4double yRnd = (5*mm) * (G4UniformRand()-0.5);
    G4double zRnd = (5*mm) * (G4UniformRand()-0.5);
    G4ThreeVector gunPosition = G4ThreeVector(xFixed, yRnd, zRnd);
    fGunTest->SetParticlePosition(gunPosition);
	G4double pi = 3.1415926535;
	G4double thRnd = G4RandGauss::shoot(0, 0.001);
	G4double phiRnd = 2 * pi * G4UniformRand();
    G4ThreeVector momentumDirection = G4ThreeVector(cos(thRnd), sin(thRnd)*cos(phiRnd), sin(thRnd)*sin(phiRnd));
    fGunTest->SetParticleMomentumDirection(momentumDirection);
}