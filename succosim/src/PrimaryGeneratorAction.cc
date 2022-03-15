#include <G4SystemOfUnits.hh>
#include <G4ParticleTable.hh>
#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <Randomize.hh>
#include <math.h>
#include <G4EventManager.hh>

#include "PrimaryGeneratorAction.hh"

#include <G4GeneralParticleSource.hh>
#include <G4SPSEneDistribution.hh>
#include <G4SPSAngDistribution.hh>

#include "CustomEventInfo.hh"

using namespace std;

// PrimaryGeneratorAction::PrimaryGeneratorAction, i.e. where the particle gun is defined and the beam features that are constant throughout the run are set
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    // define the particle gun
    fGun = new G4GeneralParticleSource(); // transformation to G4GeneralParticleSource from G4ParticleGun also required editing PrimaryGeneratorAction.hh!
	
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // set the beam features that are constant throughout the run here, or...
    
    // constant features: particle type, energy, position, momentum direction
    G4ParticleDefinition* myParticle;
    myParticle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    fGun->SetParticleDefinition(myParticle);
	
    G4double zSrc = -14 * m;
    G4ThreeVector gunPosition = G4ThreeVector(0., 0., zSrc);
    fGun->SetParticlePosition(gunPosition);
	
	G4SPSEneDistribution *eneDist = fGun->GetCurrentSource()->GetEneDist() ;
// 	eneDist->SetEnergyDisType("Mono"); // monochromatic spectrum
// 	eneDist->SetMonoEnergy(5.6*GeV);
	eneDist->SetEnergyDisType("Pow"); // power-law spectrum
	eneDist->SetAlpha(-1); // set 0 for flat distribution, -1 for 1/E distribution
	eneDist->SetEmin(1*MeV);
	eneDist->SetEmax(5.6*GeV);

    
	G4SPSAngDistribution *angDist = fGun->GetCurrentSource()->GetAngDist() ;
	angDist->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.)) ;

    // --------------------------------------------------
    // ...uncomment this line for the test beam (implemented in include/TestMode.cc)
    // BeamFeaturesFxdTest(fGun);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{delete fGun;}

// PrimaryGeneratorAction::GeneratePrimaries, i.e. where the beam features that change at each event are set and the particles are shot
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // set the beam features that change at each event here, or...
    
    // randomised features: none

    // --------------------------------------------------
    // ...uncomment this line for the test beam (implemented in include/TestMode.cc)
    // BeamFeaturesRndTest(fGun);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	
    
    
    G4int nParticle = G4int(CLHEP::RandFlat::shoot(1., 5.99));
    
    
    fGun->SetNumberOfParticles(nParticle);
    
    // shoot the event primary particle
    fGun->GeneratePrimaryVertex(anEvent);
    
    
}
