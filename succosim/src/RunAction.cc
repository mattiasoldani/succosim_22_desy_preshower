#include <G4SystemOfUnits.hh>
#include <G4String.hh>

#include "RunAction.hh"
#include "Analysis.hh"

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
	
    analysis->CreateNtupleDColumn("NEvent");  // 0
	analysis->CreateNtupleDColumn("NGenParticles");  // 1
	analysis->CreateNtupleDColumn("EDep_Dummy");  // 2
    analysis->CreateNtupleDColumn("EDep_Veto");  // 3
	for (int j = 0; j < 8; j++){
		ostringstream columnName; columnName << "EDep_DC" << j;
		analysis->CreateNtupleDColumn(columnName.str());  // 4, 5, ... - up to 8 active converter layers are supported
	}
  
    // --------------------------------------------------
    // ...uncomment this line for the test ntuple columns (implemented in include/TestMode.cc)
    // OutputNtupleTest(analysis);
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
