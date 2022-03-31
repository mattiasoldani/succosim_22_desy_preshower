#include <G4SystemOfUnits.hh>
#include <G4SDManager.hh>
#include <G4RunManager.hh>
#include <G4THitsMap.hh>
#include <G4Event.hh>
#include <G4EventManager.hh>

#include "EventAction.hh"
#include "Analysis.hh"
#include "CustomHit.hh"

#include "CustomEventInfo.hh"

using namespace std;

void EventAction::BeginOfEventAction(const G4Event* event)
{
    G4int count_particles = 0;
    for ( G4int ivtx = 0; ivtx < event->GetNumberOfPrimaryVertex(); ivtx++ ) {
        for ( G4int ipp = 0; ipp < event->GetPrimaryVertex( ivtx )->GetNumberOfParticle(); ipp++ ) {
        
            count_particles++;              
        }  
    }
    G4EventManager::GetEventManager()->SetUserInformation(new CustomEventInfo(count_particles));
    
}




// EventAction::EndOfEventAction, executed at the end of each event
void EventAction::EndOfEventAction(const G4Event* event)
{
    
    // load the gun
//     G4GeneralParticleSource* mygun = G4GeneralParticleSource::GetSDMpointer();
    
    // load the sensitive detector manager (set verbosity in DetectorConstruction.cc)
    G4SDManager* sdm = G4SDManager::GetSDMpointer();
	
    // load the analysis manager for data output (set verbosity in RunAction.cc)
    G4AnalysisManager* analysis = G4AnalysisManager::Instance();

    // get the set of all the data collections for the current event
    G4HCofThisEvent* hcofEvent = event->GetHCofThisEvent();
    if(!hcofEvent) return;
	
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // implement cast of the data collections, operations on them and ntuple filling here, or... 
	
	// detector features
	G4int nModules = 1; // nr. of converter+scintillator (veto excluded) modules - TO BE DEFINED ALSO IN DetectorConstruction.cc (TWICE)!
	
	// get data collections
    G4int fEDepVetoId = sdm->GetCollectionID("scintiVetoSD/VolumeEDep");
	VolumeEDepHitsCollection* hitCollectionScintiVeto = dynamic_cast<VolumeEDepHitsCollection*>(hcofEvent->GetHC(fEDepVetoId));
	
    G4int fEDepDummyId = sdm->GetCollectionID("dummyCaloSD/VolumeEDep");
	VolumeEDepHitsCollection* hitCollectionDummyCalo = dynamic_cast<VolumeEDepHitsCollection*>(hcofEvent->GetHC(fEDepDummyId));
	
	vector<G4int> fEDepDCId;
	vector<VolumeEDepHitsCollection*> hitCollectionScintiDC;
	for (int j = 0; j < nModules; j++){
		ostringstream collectionName; collectionName << "scintiDCSD_" << j << "/VolumeEDep";
		fEDepDCId.push_back(sdm->GetCollectionID(collectionName.str()));
		hitCollectionScintiDC.push_back(dynamic_cast<VolumeEDepHitsCollection*>(hcofEvent->GetHC(fEDepDCId[j])));
	}
	
	G4int eid=event->GetEventID();
    G4int evtot=G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed();
    
    // event number to output ntuple
    analysis->FillNtupleDColumn(0, 0, eid);
    
    //number of particles generated
    CustomEventInfo*  ei( static_cast< CustomEventInfo * >(event->GetUserInformation() ) );
    G4int nGenPart=ei->GetNParticles();
    analysis->FillNtupleDColumn(0, 1, nGenPart);

	// signal in dummy calorimeter
    if (hitCollectionDummyCalo)
    {
        G4double EDepTot_Dummy = 0.0;
        for (auto hit: *hitCollectionDummyCalo->GetVector())
        {EDepTot_Dummy += hit->GetEDep();}
        analysis->FillNtupleDColumn(0, 2, EDepTot_Dummy / MeV);
    }
    else
    {analysis->FillNtupleDColumn(0, 2, -99.);}

	// signal in charge veto
    if (hitCollectionScintiVeto)
    {
        G4double EDepTot_Veto = 0.0;
        for (auto hit: *hitCollectionScintiVeto->GetVector())
        {EDepTot_Veto += hit->GetEDep();}
        analysis->FillNtupleDColumn(0, 3, EDepTot_Veto / MeV);
    }
    else
    {analysis->FillNtupleDColumn(0, 3, -99.);}

	// signals in downstream counters
	G4double EDepTot_DC;
	for (int j = 0; j < 8; j++){
		if(j<nModules){
			if (hitCollectionScintiDC[j])
			{
				EDepTot_DC = 0.0;
				for (auto hit: *hitCollectionScintiDC[j]->GetVector())
				{EDepTot_DC += hit->GetEDep();}
				analysis->FillNtupleDColumn(0, 4+j, EDepTot_DC / MeV);
			}
			else
			{analysis->FillNtupleDColumn(0, 4+j, -99.);}
		}else{analysis->FillNtupleDColumn(0, 4+j, -99.);} // remaining columns are always filled with dummy entries
	}
	
    // add event to ntuple
    analysis->AddNtupleRow(0);
    
    if(eid%500==0){G4cout<<"Event "<<eid<<"... done!\t( "<<100.*((float)eid)/evtot<<" % )      \r"<<flush;}

    // --------------------------------------------------
    // ...uncomment this line for the test event action (implemented in include/TestMode.cc)
    // EndOfEventScoringTest(event, sdm, hcofEvent, analysis);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}
