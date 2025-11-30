#include "grid.h"
#include "simulation.h"
#include "simulation_state.h"
#include "trains.h"
#include "switches.h"
#include "io.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

#define MAX_TICKS 1000

void initializeSimulation() {
    srand(randomSeed);
    initializeLogFiles();
    
    cout << "==================================================" << endl;
    cout << "SIMULATION INITIALIZED" << endl;
    cout << "==================================================" << endl;
    cout << "Random Seed: " << randomSeed << endl;
    cout << "Total Trains: " << numTrains << endl;
    cout << "Safety Buffers: " << currentSafetyBuffers << endl;
    cout << "Max Ticks: " << MAX_TICKS << endl;
    cout << "==================================================" << endl;
    cout << endl;
}

void simulateOneTick() {
    
    cout << endl;
    cout << "========================================" << endl;
    cout << "TICK: " << currentTick << endl;
    cout << "========================================" << endl;
    
    // Phase 1: Spawn trains
    cout << "[Phase 1] Spawning trains..." << endl;
    spawnTrainsForTick();
    
    // Phase 2: Determine routes
    cout << "[Phase 2] Determining routes..." << endl;
    determineAllRoutes();
    
    // Phase 3: Update switch counters
    cout << "[Phase 3] Updating switch counters..." << endl;
    updateSwitchCounters();
    
    // Phase 4: Queue switch flips
    cout << "[Phase 4] Checking for switch flips..." << endl;
    queueSwitchFlips();
    
    // Phase 5: Move trains
    cout << "[Phase 5] Moving trains..." << endl;
    moveAllTrains();
    
    // Phase 6: Apply deferred flips
    cout << "[Phase 6] Applying deferred flips..." << endl;
    applyDeferredFlips();
    
    // Phase 7: Check arrivals and log
    cout << "[Phase 7] Checking arrivals and logging..." << endl;
    checkArrivals();
    updateSignalLights();
    updateEmergencyHalt();
    
    logTrainTrace();
    logSwitchState();
    logSignalState();
    
    // Print grid state
    cout << endl;
    printGrid();
    
    // Print train status
    cout << endl;
    cout << "Train Status:" << endl;
    for(int i = 0; i < numTrains; i++) {
        if(trainState[i] == TRAIN_MOVING) {
            cout << "  Train " << i << " at (" << trainRow[i] << "," 
                 << trainCol[i] << ") moving ";
            
            if(trainDirection[i] == DIR_UP) cout << "UP";
            else if(trainDirection[i] == DIR_RIGHT) cout << "RIGHT";
            else if(trainDirection[i] == DIR_DOWN) cout << "DOWN";
            else if(trainDirection[i] == DIR_LEFT) cout << "LEFT";
            
            cout << " -> Dest " << trainDestinationIndex[i] 
                 << " at (" << destRow[trainDestinationIndex[i]] << "," 
                 << destCol[trainDestinationIndex[i]] << ")";
            
            if(trainDelayCounter[i] > 0) {
                cout << " [DELAYED: " << trainDelayCounter[i] << " ticks]";
            }
            
            cout << endl;
        }
        else if(trainState[i] == TRAIN_DELIVERED) {
            cout << "  Train " << i << " DELIVERED to destination " 
                 << trainDestinationIndex[i] << endl;
        }
        else if(trainState[i] == TRAIN_CRASHED) {
            cout << "  Train " << i << " CRASHED" << endl;
        }
        else if(trainState[i] == TRAIN_WAITING) {
            cout << "  Train " << i << " WAITING (spawn tick: " 
                 << trainSpawnTick[i] << ")" << endl;
        }
    }
    
    // Print metrics
    cout << endl;
    cout << "Current Metrics:" << endl;
    cout << "  Trains Delivered: " << trainsDelivered << endl;
    cout << "  Trains Crashed: " << trainsCrashed << endl;
    cout << "  Total Switch Flips: " << totalSwitchFlips << endl;
    cout << "  Safety Buffers Active: " << currentSafetyBuffers << endl;
    cout << "  Signal Violations: " << signalViolations << endl;
    
    // Print switch status
    cout << endl;
    cout << "Switch Status:" << endl;
    for(int i = 0; i < 26; i++) {
        if(switchRow[i] != -1) {
            char letter = 'A' + i;
            char mode = (switchMode[i] == SWITCH_PER_DIR) ? 'P' : 'G';
            
            cout << "  Switch " << letter << " [" << mode << "] ";
            cout << "State:" << switchState[i] << " ";
            cout << "Counter:" << switchCounter[i][0] << "/" << switchKValue[i][0];
            
            if(switchFlipQueued[i]) {
                cout << " [FLIP QUEUED]";
            }
            
            cout << " Signal:";
            if(switchSignal[i] == SIGNAL_GREEN) cout << "GREEN";
            else if(switchSignal[i] == SIGNAL_YELLOW) cout << "YELLOW";
            else cout << "RED";
            
            cout << endl;
        }
    }
    
    currentTick++;
}

bool isSimulationComplete() {
    
    int activeTrains = 0;
    
    for(int i = 0; i < numTrains; i++) {
        if(trainState[i] == TRAIN_WAITING || trainState[i] == TRAIN_MOVING) {
            activeTrains++;
        }
    }
    
    // Check timeout
    if(currentTick >= MAX_TICKS) {
        cout << endl;
        cout << "==================================================" << endl;
        cout << "SIMULATION TIMEOUT!" << endl;
        cout << "==================================================" << endl;
        cout << "Reached maximum tick limit: " << MAX_TICKS << endl;
        cout << "Stopping simulation to prevent infinite loop." << endl;
        cout << "Active trains remaining: " << activeTrains << endl;
        cout << "Trains delivered: " << trainsDelivered << endl;
        cout << "Trains crashed: " << trainsCrashed << endl;
        cout << "==================================================" << endl;
        
        // Mark remaining trains as crashed
        for(int i = 0; i < numTrains; i++) {
            if(trainState[i] == TRAIN_WAITING || trainState[i] == TRAIN_MOVING) {
                trainState[i] = TRAIN_CRASHED;
                trainsCrashed++;
            }
        }
        
        writeMetrics();
        return true;
    }
    
    // Check normal completion
    if(activeTrains == 0) {
        cout << endl;
        cout << "==================================================" << endl;
        cout << "SIMULATION COMPLETE!" << endl;
        cout << "==================================================" << endl;
        cout << "Total ticks: " << currentTick << endl;
        cout << "Trains delivered: " << trainsDelivered << endl;
        cout << "Trains crashed: " << trainsCrashed << endl;
        cout << "Total switch flips: " << totalSwitchFlips << endl;
        cout << "Signal violations: " << signalViolations << endl;
        
        if(numTrains > 0) {
            float successRate = (float)trainsDelivered * 100.0 / numTrains;
            cout << "Success rate: " << successRate << "%" << endl;
        }
        
        cout << "==================================================" << endl;
        
        writeMetrics();
        
        return true;
    }
    
    return false;
}