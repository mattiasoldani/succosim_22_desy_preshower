#include <G4SystemOfUnits.hh>
#include <G4SDManager.hh>
#include <G4THitsMap.hh>
#include <G4Event.hh>

#include "EventAction.hh"
#include "Analysis.hh"
#include "CustomHit.hh"

using namespace std;

// EventAction::EndOfEventAction, executed at the end of each event
void EventAction::EndOfEventAction(const G4Event* event)
{
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
	G4int nModules = 2; // nr. of converter+scintillator (veto excluded) modules - TO BE DEFINED ALSO IN DetectorConstruction.cc (TWICE)!
	
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
	
    // event number to output ntuple
    analysis->FillNtupleDColumn(0, 0, event->GetEventID());

	// signal in dummy calorimeter
    if (hitCollectionDummyCalo)
    {
        G4double EDepTot_Dummy = 0.0;
        for (auto hit: *hitCollectionDummyCalo->GetVector())
        {EDepTot_Dummy += hit->GetEDep();}
        analysis->FillNtupleDColumn(0, 1, EDepTot_Dummy / MeV);
    }
    else
    {analysis->FillNtupleDColumn(0, 1, -99.);}

	// signal in charge veto
    if (hitCollectionScintiVeto)
    {
        G4double EDepTot_Veto = 0.0;
        for (auto hit: *hitCollectionScintiVeto->GetVector())
        {EDepTot_Veto += hit->GetEDep();}
        analysis->FillNtupleDColumn(0, 2, EDepTot_Veto / MeV);
    }
    else
    {analysis->FillNtupleDColumn(0, 2, -99.);}

	// signals in downstream counters
	G4double EDepTot_DC;
	for (int j = 0; j < 8; j++){
		if(j<nModules){
			if (hitCollectionScintiDC[j])
			{
				EDepTot_DC = 0.0;
				for (auto hit: *hitCollectionScintiDC[j]->GetVector())
				{EDepTot_DC += hit->GetEDep();}
				analysis->FillNtupleDColumn(0, 3+j, EDepTot_DC / MeV);
			}
			else
			{analysis->FillNtupleDColumn(0, 3+j, -99.);}
		}else{analysis->FillNtupleDColumn(0, 3+j, -99.);} // remaining columns are always filled with dummy entries
	}
	
    // add event to ntuple
    analysis->AddNtupleRow(0);

    // --------------------------------------------------
    // ...uncomment this line for the test event action (implemented in include/TestMode.cc)
    // EndOfEventScoringTest(event, sdm, hcofEvent, analysis);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}
