#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include <G4SystemOfUnits.hh>
#include <G4UserEventAction.hh>
#include <globals.hh>
#include <G4SDManager.hh>
#include "Analysis.hh"

// EventAction::EndOfEventAction, actions executed at each event

class EventAction : public G4UserEventAction
{
public:
    void EndOfEventAction(const G4Event* event) override;
	
private:
	// event-by-event scoring for test simulation
	void EndOfEventScoringTest(const G4Event* event, G4SDManager* sdm, G4HCofThisEvent* hcofEvent, G4AnalysisManager* analysis);
};

#endif
