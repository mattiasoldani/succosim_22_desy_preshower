#include "RunAction.hh"
#include <G4SystemOfUnits.hh>
#include "Analysis.hh"
#include <G4String.hh>

using namespace std;

// RunAction, class created at run start, with RunAction::EndOfRunAction executed at the end of the run

RunAction::RunAction() :  G4UserRunAction()
{
	// load the analysis manager for data output
	G4AnalysisManager* analysis = G4AnalysisManager::Instance();
	analysis->SetVerboseLevel(1);  // set analysis manager verbosity here
  
	// create output ntuple
	analysis->SetFirstNtupleId(0);
	analysis->CreateNtuple("outData", "output data");
	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// create the ntuple columns (remember the order, it is needed to fill them) here, or...
	// e.g. analysis->CreateNtupleDColumn("NEvent");
  
	// --------------------------------------------------
	// ...uncomment this line for the test ntuple columns
	OutputNtupleTest(analysis);
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	analysis->FinishNtuple(0);
	
	// open output file
	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// choose output file name here --> file will have extension .root and and will be in ./out_data/
	G4String outFileName = "OutData";
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	analysis->OpenFile("./out_data/"+outFileName+".root");
}

RunAction::~RunAction()
{
    // write output file & close it
    G4AnalysisManager* analysis = G4AnalysisManager::Instance();
    analysis->Write();
	analysis->CloseFile();
}

void RunAction::EndOfRunAction(const G4Run* run)
{
	// retrieve the number of events produced in the run
	G4int nofEvents = run->GetNumberOfEvent();
	if (nofEvents == 0) return;

	if (IsMaster())
	{
		G4cout << "-----" << G4endl;
		G4cout << "| RunAction.cc: end of run --> generated events: " << nofEvents << G4endl;
		G4cout << "-----" << G4endl;
	}
}

// ============================================================
// ============================================================
// RunAction::OutputNtupleTest, i.e. ntuple structure for the test setup
void RunAction::OutputNtupleTest(G4AnalysisManager* analysis)
{
	analysis->CreateNtupleDColumn("NEvent");  // 0
	analysis->CreateNtupleDColumn("Tracker_NHit_X_0");  // 1
	analysis->CreateNtupleDColumn("Tracker_NHit_Y_0");  // 2 (this is gonna be identical to the previous -- same silicon layer)
	analysis->CreateNtupleDColumn("Tracker_NHit_X_1");  // 3
	analysis->CreateNtupleDColumn("Tracker_NHit_Y_1");  // 4 (this is gonna be identical to the previous -- same silicon layer)
	analysis->CreateNtupleDColumn("Tracker_NHit_X_2");  // 5
	analysis->CreateNtupleDColumn("Tracker_NHit_Y_2");  // 6
	analysis->CreateNtupleDColumn("Tracker_X_0");  // 7
	analysis->CreateNtupleDColumn("Tracker_Y_0");  // 8
	analysis->CreateNtupleDColumn("Tracker_X_1");  // 9
	analysis->CreateNtupleDColumn("Tracker_Y_1");  // 10
	analysis->CreateNtupleDColumn("Tracker_X_2");  // 11
	analysis->CreateNtupleDColumn("Tracker_Y_2");  // 12
	analysis->CreateNtupleDColumn("ECal_EDep");  // 13
	analysis->CreateNtupleDColumn("GammaCal_EDep");  // 14
}
