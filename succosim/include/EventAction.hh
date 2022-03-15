#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include <G4SystemOfUnits.hh>
#include <G4UserEventAction.hh>
#include <G4SDManager.hh>
#include <globals.hh>

#include "Analysis.hh"

// EventAction::EndOfEventAction, actions executed at each event

class EventAction : public G4UserEventAction
{
public:
    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;
	
private:
    // event-by-event scoring for test simulation (implemented in include/TestMode.cc)
    void EndOfEventScoringTest(const G4Event* event, G4SDManager* sdm, G4HCofThisEvent* hcofEvent, G4AnalysisManager* analysis);
};

#endif
