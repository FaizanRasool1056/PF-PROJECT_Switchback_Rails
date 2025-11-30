#include "simulation_state.h"
#include <cstring>

// Grid data
char grid[100][100];
char originalTiles[100][100];
int rows = 0;
int cols = 0;

// Train data - using parallel arrays
int trainRow[100];
int trainCol[100];
int trainDirection[100];
int trainState[100];
int trainSpawnTick[100];
int trainDestinationIndex[100];
int trainColor[100];
int trainDelayCounter[100];
int numTrains = 0;

// Switch data - A-Z mapped to 0-25
int switchMode[26];
int switchState[26];
int switchKValue[26][4];
int switchCounter[26][4];
int switchRow[26];
int switchCol[26];
bool switchFlipQueued[26];
int switchSignal[26];
int numSwitches = 0;

// Spawn and destination points
int spawnRow[50];
int spawnCol[50];
int numSpawns = 0;

int destRow[50];
int destCol[50];
int numDestinations = 0;

// Simulation parameters
int currentTick = 0;
int randomSeed = 0;
int weatherMode = 0;
int maxSafetyBuffers = 10;
int currentSafetyBuffers = 0;

// Metrics
int trainsDelivered = 0;
int trainsCrashed = 0;
int totalWaitTicks = 0;
int totalSwitchFlips = 0;
int signalViolations = 0;

// Emergency halt
bool emergencyHaltActive = false;
int emergencyHaltTicks = 0;
int emergencyHaltRow = -1;
int emergencyHaltCol = -1;

void initializeSimulationState() {
    
    // Reset grid
    for(int i = 0; i < 100; i++) {
        for(int j = 0; j < 100; j++) {
            grid[i][j] = ' ';
            originalTiles[i][j] = ' ';
        }
    }
    rows = 0;
    cols = 0;
    
    // Reset trains
    for(int i = 0; i < 100; i++) {
        trainRow[i] = -1;
        trainCol[i] = -1;
        trainDirection[i] = 0;
        trainState[i] = 0;
        trainSpawnTick[i] = 0;
        trainDestinationIndex[i] = 0;
        trainColor[i] = 0;
        trainDelayCounter[i] = 0;
    }
    numTrains = 0;
    
    // Reset switches
    for(int i = 0; i < 26; i++) {
        switchMode[i] = 0;
        switchState[i] = 0;
        switchRow[i] = -1;
        switchCol[i] = -1;
        switchFlipQueued[i] = false;
        switchSignal[i] = 0;
        
        for(int d = 0; d < 4; d++) {
            switchKValue[i][d] = 0;
            switchCounter[i][d] = 0;
        }
    }
    numSwitches = 0;
    
    // Reset spawn points
    for(int i = 0; i < 50; i++) {
        spawnRow[i] = -1;
        spawnCol[i] = -1;
    }
    numSpawns = 0;
    
    // Reset destinations
    for(int i = 0; i < 50; i++) {
        destRow[i] = -1;
        destCol[i] = -1;
    }
    numDestinations = 0;
    
    // Reset simulation parameters
    currentTick = 0;
    randomSeed = 0;
    weatherMode = 0;
    maxSafetyBuffers = 10;
    currentSafetyBuffers = 0;
    
    // Reset metrics
    trainsDelivered = 0;
    trainsCrashed = 0;
    totalWaitTicks = 0;
    totalSwitchFlips = 0;
    signalViolations = 0;
    
    // Reset emergency halt
    emergencyHaltActive = false;
    emergencyHaltTicks = 0;
    emergencyHaltRow = -1;
    emergencyHaltCol = -1;
}