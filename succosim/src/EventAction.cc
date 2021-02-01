#include "EventAction.hh"
#include <G4SystemOfUnits.hh>
#include <G4SDManager.hh>
#include <G4THitsMap.hh>
#include <G4Event.hh>
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
    // implement cast of the data collections and operations on them here, or... 

    // --------------------------------------------------
    // ...uncomment this line for the test event action
    EndOfEventScoringTest(event, sdm, hcofEvent, analysis);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

// ============================================================
// ============================================================
// EventAction::EndOfEventScoringTest, i.e. output data processing & ntuple filling for test setup
void EventAction::EndOfEventScoringTest(const G4Event* event, G4SDManager* sdm, G4HCofThisEvent* hcofEvent, G4AnalysisManager* analysis)
{
    // get data collections
    G4int fECalEDepId = sdm->GetCollectionID("ECalTest_SD/VolumeEDep");  // electron calorimeter
    G4int fPhCalEDepId = sdm->GetCollectionID("PhCalTest_SD/VolumeEDep");  // photon calorimeter
    G4int fTrackerId0 = sdm->GetCollectionID("Tracker_SD_0/VolumeTracking");  // tracking module 0
    G4int fTrackerId1 = sdm->GetCollectionID("Tracker_SD_1/VolumeTracking");  // tracking module 1
    G4int fTrackerId2_0 = sdm->GetCollectionID("Tracker_SD_2_0/VolumeTracking");  // tracking module 2 -- 0th
    G4int fTrackerId2_1 = sdm->GetCollectionID("Tracker_SD_2_1/VolumeTracking");  // tracking module 2 -- 1st
    VolumeEDepHitsCollection* hitCollectionECal = dynamic_cast<VolumeEDepHitsCollection*>(hcofEvent->GetHC(fECalEDepId));
    VolumeEDepHitsCollection* hitCollectionPhCal = dynamic_cast<VolumeEDepHitsCollection*>(hcofEvent->GetHC(fPhCalEDepId));
    VolumeTrackingHitsCollection* hitCollectionTracker0 = dynamic_cast<VolumeTrackingHitsCollection*>(hcofEvent->GetHC(fTrackerId0));
    VolumeTrackingHitsCollection* hitCollectionTracker1 = dynamic_cast<VolumeTrackingHitsCollection*>(hcofEvent->GetHC(fTrackerId1));
    VolumeTrackingHitsCollection* hitCollectionTracker2_0 = dynamic_cast<VolumeTrackingHitsCollection*>(hcofEvent->GetHC(fTrackerId2_0));
    VolumeTrackingHitsCollection* hitCollectionTracker2_1 = dynamic_cast<VolumeTrackingHitsCollection*>(hcofEvent->GetHC(fTrackerId2_1));
	
    // event number to output ntuple
    if ((hitCollectionTracker0) || (hitCollectionTracker1))
    {analysis->FillNtupleDColumn(0, 0, event->GetEventID());}
	
    // get tracking system data collection
    // 1 hit per particle, per step & per tracking plane
    // --> consider only hits whose energy deposit is over threshold
    // --> for hits belonging to the same particle, compute mean between all steps transverse positions
    // --> different particles are treated separately (and increase the hit counter)
    G4double thresholdTrackerEDep = 50 * keV;
	
    // --> module 0
    if (hitCollectionTracker0)
    {
        G4int lastTrackId0 = -1;
        G4int NStep0 = 1;
        G4int NHits0 = 0;
        G4double horsa0 = -9999.0*cm;
        G4double versa0 = -9999.0*cm;
        for (auto hit: *hitCollectionTracker0->GetVector())
        {
            if (hit->GetEDep()>thresholdTrackerEDep)
            {
                if(hit->GetTrackId() != lastTrackId0)
                {
                    NHits0+=1;
                    NStep0=1;
                    horsa0=hit->GetX()[0];
                    versa0=hit->GetX()[1];
                }
                else
                {
                    NStep0+=1;
                    horsa0+=hit->GetX()[0];
                    versa0+=hit->GetX()[1];
                }
                lastTrackId0 = hit->GetTrackId();
            }
        }
        horsa0 = horsa0 / NStep0;
        versa0 = versa0 / NStep0;
        analysis->FillNtupleDColumn(0, 1, NHits0);
        analysis->FillNtupleDColumn(0, 2, NHits0);  // columns 1 & 2 are identical -- same silicon layer
        analysis->FillNtupleDColumn(0, 7, horsa0 / cm);
        analysis->FillNtupleDColumn(0, 8, versa0 / cm);
    }
    else
    {
        analysis->FillNtupleDColumn(0, 1, 0);
        analysis->FillNtupleDColumn(0, 2, 0);
        analysis->FillNtupleDColumn(0, 7, -9999.0 / cm);
        analysis->FillNtupleDColumn(0, 8, -9999.0 / cm);
    }
	
    // --> module 1
    if (hitCollectionTracker1)
    {
        G4int lastTrackId1 = -1;
        G4int NStep1 = 1;
        G4int NHits1 = 0;
        G4double horsa1 = -9999.0*cm;
        G4double versa1 = -9999.0*cm;
        for (auto hit: *hitCollectionTracker1->GetVector())
        {
            if (hit->GetEDep()>thresholdTrackerEDep)
            {
                if(hit->GetTrackId() != lastTrackId1)
                    {
                    NHits1+=1;
                    NStep1=1;
                    horsa1=hit->GetX()[0];
                    versa1=hit->GetX()[1];
                }
                else
                {
                    NStep1+=1;
                    horsa1+=hit->GetX()[0];
                    versa1+=hit->GetX()[1];
                }
                lastTrackId1 = hit->GetTrackId();
            }
        }
        horsa1 = horsa1 / NStep1;
        versa1 = versa1 / NStep1;
        analysis->FillNtupleDColumn(0, 3, NHits1);
        analysis->FillNtupleDColumn(0, 4, NHits1);  // columns 3 & 4 are identical -- same silicon layer
        analysis->FillNtupleDColumn(0, 9, horsa1 / cm);
        analysis->FillNtupleDColumn(0, 10, versa1 / cm);
    }
    else
    {
        analysis->FillNtupleDColumn(0, 3, 0);
        analysis->FillNtupleDColumn(0, 4, 0);
        analysis->FillNtupleDColumn(0, 9, -9999.0 / cm);
        analysis->FillNtupleDColumn(0, 10, -9999.0 / cm);
    }
	
    // --> module 2 -- 0th
    if (hitCollectionTracker2_0)
    {
        G4int lastTrackId2_0 = -1;
        G4int NStep2_0 = 1;
        G4int NHits2_0 = 0;
        G4double horsa2 = -9999.0*cm;
        for (auto hit: *hitCollectionTracker2_0->GetVector())
        {
            if (hit->GetEDep()>thresholdTrackerEDep)
            {
                if(hit->GetTrackId() != lastTrackId2_0)
                {
                    NHits2_0+=1;
                    NStep2_0=1;
                    horsa2=hit->GetX()[0];
                }
                else
                {
                    NStep2_0+=1;
                    horsa2+=hit->GetX()[0];
                }
                lastTrackId2_0 = hit->GetTrackId();
            }
        }
        horsa2 = horsa2 / NStep2_0;
        analysis->FillNtupleDColumn(0, 5, NHits2_0);
        analysis->FillNtupleDColumn(0, 11, horsa2 / cm);
    }
    else
    {
    analysis->FillNtupleDColumn(0, 5, 0);
    analysis->FillNtupleDColumn(0, 11, -9999.0 / cm);
    }
	
    // --> module 2 -- 1st
    if (hitCollectionTracker2_1)
    {
        G4int lastTrackId2_1 = -1;
        G4int NStep2_1 = 1;
        G4int NHits2_1 = 0;
        G4double versa2 = -9999.0*cm;
        for (auto hit: *hitCollectionTracker2_1->GetVector())
        {
            if (hit->GetEDep()>thresholdTrackerEDep)
            {
                if(hit->GetTrackId() != lastTrackId2_1)
                {
                    NHits2_1+=1;
                    NStep2_1=1;
                    versa2=hit->GetX()[1];
                }
                else
                {
                    NStep2_1+=1;
                    versa2+=hit->GetX()[1];
                }
                lastTrackId2_1 = hit->GetTrackId();
            }
        }
        versa2 = versa2 / NStep2_1;
        analysis->FillNtupleDColumn(0, 6, NHits2_1);
        analysis->FillNtupleDColumn(0, 12, versa2 / cm);
    }
    else
    {
        analysis->FillNtupleDColumn(0, 6, 0);
        analysis->FillNtupleDColumn(0, 12, -9999.0 / cm);
    }
	
    // get electron calorimeter data collection
    // 1 hit per particle, per step & per detector layer (both scintillator & absorber) --> sum everything for the current event
    if (hitCollectionECal)
    {
        G4double ECalEDepTot = 0.0;
        for (auto hit: *hitCollectionECal->GetVector())
        {ECalEDepTot += hit->GetEDep();}
        analysis->FillNtupleDColumn(0, 13, ECalEDepTot / MeV);
    }
    else
    {analysis->FillNtupleDColumn(0, 13, 0.0);}
	
    // get photon calorimeter data collection
    // 1 hit per particle & per step --> sum everything for the current event
    if (hitCollectionPhCal)
    {
        G4double PhCalEDepTot = 0.0;
        for (auto hit: *hitCollectionPhCal->GetVector())
        {PhCalEDepTot += hit->GetEDep();}
        analysis->FillNtupleDColumn(0, 14, PhCalEDepTot / MeV);
    }
    else
    {analysis->FillNtupleDColumn(0, 14, 0.0);}
	
    // add event to ntuple
    if ((hitCollectionTracker0) || (hitCollectionTracker1))
    {analysis->AddNtupleRow(0);}
}
