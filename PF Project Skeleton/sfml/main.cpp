#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/io.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    
    cout << "=====================================" << endl;
    cout << "   SWITCHBACK RAILS SIMULATOR" << endl;
    cout << "=====================================" << endl;
    cout << endl;
    
    // Check command line arguments
    if(argc < 2) {
        cout << "ERROR: No level file specified!" << endl;
        cout << "Usage: ./switchback_rails <level_file>" << endl;
        cout << "Example: ./switchback_rails data/levels/easy_level.lvl" << endl;
        return 1;
    }
    
    const char* levelFile = argv[1];
    cout << "Level file: " << levelFile << endl;
    cout << endl;
    
    // Initialize simulation state
    cout << "Initializing simulation state..." << endl;
    initializeSimulationState();
    cout << endl;
    
    // Load level file
    cout << "Loading level file..." << endl;
    if(!loadLevelFile(levelFile)) {
        cout << "ERROR: Failed to load level file!" << endl;
        return 1;
    }
    cout << endl;
    
    // Initialize simulation
    cout << "Initializing simulation..." << endl;
    initializeSimulation();
    cout << endl;
    
    // Initialize SFML application
    cout << "Initializing SFML application..." << endl;
    if(!initializeApp()) {
        cout << "ERROR: Failed to initialize SFML application!" << endl;
        return 1;
    }
    cout << endl;
    
    // Print controls
    cout << "=====================================" << endl;
    cout << "         CONTROLS" << endl;
    cout << "=====================================" << endl;
    cout << "SPACE     = Pause/Resume simulation" << endl;
    cout << ". (period) = Step one tick forward" << endl;
    cout << "ESC       = Exit and save metrics" << endl;
    cout << "=====================================" << endl;
    cout << endl;
    
    // Run main loop
    cout << "Starting simulation..." << endl;
    cout << endl;
    runApp();
    
    // Cleanup
    cout << endl;
    cout << "Cleaning up..." << endl;
    cleanupApp();
    
    // Print final statistics
    cout << endl;
    cout << "=====================================" << endl;
    cout << "      SIMULATION COMPLETE" << endl;
    cout << "=====================================" << endl;
    cout << "Total ticks: " << currentTick << endl;
    cout << "Trains delivered: " << trainsDelivered << endl;
    cout << "Trains crashed: " << trainsCrashed << endl;
    cout << "Total switch flips: " << totalSwitchFlips << endl;
    cout << "=====================================" << endl;
    cout << endl;
    cout << "Check out/ directory for detailed logs:" << endl;
    cout << "  - trace.csv (train movements)" << endl;
    cout << "  - switches.csv (switch states)" << endl;
    cout << "  - signals.csv (signal colors)" << endl;
    cout << "  - metrics.txt (final statistics)" << endl;
    cout << endl;
    
    return 0;
}