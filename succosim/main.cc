#ifdef G4MULTITHREADED
    #include <G4MTRunManager.hh>
    using RunManager = G4MTRunManager;
#else
    #include <G4RunManager.hh>
    using RunManager = G4RunManager;
#endif

#ifdef G4VIS_USE
    #include <G4VisExecutive.hh>
#endif
#ifdef G4UI_USE
    #include <G4UIExecutive.hh>
#endif

#include <vector>
#include <G4String.hh>
#include <G4UImanager.hh>
#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "Analysis.hh"
#include "PhysicsList.hh"
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// add new physics lists here
// e.g. #include "FTFP_BERT.hh"
#include "FTFP_BERT.hh"
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

using namespace std;

int main(int argc, char** argv)
{
    cout << "-----" << endl;
    cout << "| main.cc: let's start!" << endl;
    cout << "-----" << endl;

    // find execution type (interactive or not) and detect all the macros given as executable arguments
    vector<G4String> macros;
    bool interactive = false;
    if  (argc == 1)
    {
        interactive = true;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            G4String arg = argv[i];
            if (arg == "-i" || arg == "--interactive")
            {
                interactive = true;
                continue;
            }
            else
            {
                macros.push_back(arg);
            }
        }
    }

	// load run manager
    auto runManager = new RunManager();
    runManager->SetVerboseLevel(0);  // <<< set run manager verbosity here

    // if in graphical mode, start visualisation
    #ifdef G4VIS_USE
        G4VisManager* visManager = new G4VisExecutive("Quiet");  // <<< set visualisation manager verbosity here
        visManager->SetVerboseLevel(0);  // <<< set visualisation manager verbosity here
        visManager->Initialize();
    #endif

    // load physics list, detector construction and action initialisation
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // set physics list name here: (new ...()), set PhysicsList for the custom list
    // e.g. runManager->SetUserInitialization(new FTFP_BERT());
    runManager->SetUserInitialization(new FTFP_BERT());
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new ActionInitialization());

    // load user interface manager and program executor
    #ifdef G4UI_USE
        G4UIExecutive* ui = nullptr;
        if (interactive)
        {
            ui = new G4UIExecutive(argc, argv);
        }
    #endif
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // execute all the macros given as executable arguments
    for (auto macro : macros)
    {
        G4String command = "/control/execute ";
        UImanager->ApplyCommand(command + macro);
    }

    // if in interactive mode, open it; moreover, if in graphical mode, execute the default macro(s) for graphical mode startup
    #ifdef G4UI_USE
        if (interactive)
        {
            if (ui->IsGUI())
            {
                // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
                // select macro to be executed at graphical mode startup
                // e.g. UImanager->ApplyCommand("/control/execute macros/gui.mac");
                UImanager->ApplyCommand("/control/execute macros/gui.mac");
                // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            }
            else
            {
                UImanager->ApplyCommand("/run/initialize");
            }
            ui->SessionStart();
            delete ui;
        }
    #endif

    // at the end of the session, delete the run manager
    delete runManager;
	
    cout << "-----" << endl;
    cout << "| main.cc: done, goodbye!" << endl;
    cout << "-----" << endl;

    return 0;
}
