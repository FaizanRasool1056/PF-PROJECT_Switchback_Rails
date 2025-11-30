#ifndef SIMULATION_STATE_H
#define SIMULATION_STATE_H

// ============================================================================
// SIMULATION_STATE.H - Global constants and state
// ============================================================================
// This file declares all the global variables used throughout the simulation
// Think of it as the "shared memory" that all parts of the program can access
// ============================================================================

// ----------------------------------------------------------------------------
// GRID CONSTANTS
// ----------------------------------------------------------------------------
// These define the maximum size our grid can be
#define MAX_GRID_SIZE 100      // Grid can be at most 100x100

// ----------------------------------------------------------------------------
// TRAIN CONSTANTS  
// ----------------------------------------------------------------------------
// Direction constants - easier to read than numbers
#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3

// Train state constants
#define TRAIN_WAITING 0        // Train hasn't spawned yet
#define TRAIN_MOVING 1         // Train is actively moving
#define TRAIN_CRASHED 2        // Train had a collision
#define TRAIN_DELIVERED 3      // Train reached its destination

#define MAX_TRAINS 100         // Maximum number of trains

// ----------------------------------------------------------------------------
// SWITCH CONSTANTS
// ----------------------------------------------------------------------------
#define MAX_SWITCHES 26        // A-Z = 26 possible switches

// Switch mode constants
#define SWITCH_PER_DIR 0       // Each direction has its own counter
#define SWITCH_GLOBAL 1        // All directions share one counter

// ----------------------------------------------------------------------------
// WEATHER CONSTANTS
// ----------------------------------------------------------------------------
#define WEATHER_NORMAL 0
#define WEATHER_RAIN 1
#define WEATHER_FOG 2

// ----------------------------------------------------------------------------
// SIGNAL CONSTANTS
// ----------------------------------------------------------------------------
#define SIGNAL_GREEN 0         // Path is clear
#define SIGNAL_YELLOW 1        // Warning - train nearby
#define SIGNAL_RED 2           // Blocked - don't proceed

// ----------------------------------------------------------------------------
// GLOBAL STATE: GRID
// ----------------------------------------------------------------------------
extern char grid[100][100];           // The main game grid
extern char originalTiles[100][100];  // Backup for safety tile toggle
extern int rows;                      // Number of rows in current level
extern int cols;                      // Number of columns in current level

// ----------------------------------------------------------------------------
// GLOBAL STATE: TRAINS
// ----------------------------------------------------------------------------
extern int trainRow[100];             // Current row of each train
extern int trainCol[100];             // Current column of each train
extern int trainDirection[100];       // Current direction (0-3)
extern int trainState[100];           // Current state (WAITING, MOVING, etc.)
extern int trainSpawnTick[100];       // When this train should spawn
extern int trainDestinationIndex[100];// Which destination this train goes to
extern int trainColor[100];           // Color for display (0-9)
extern int trainDelayCounter[100];    // Ticks remaining in delay
extern int numTrains;                 // Total number of trains

// ----------------------------------------------------------------------------
// GLOBAL STATE: SWITCHES (A-Z mapped to 0-25)
// ----------------------------------------------------------------------------
extern int switchMode[26];            // PER_DIR or GLOBAL
extern int switchState[26];           // Current state (0 or 1)
extern int switchKValue[26][4];       // K-values [switch][direction]
extern int switchCounter[26][4];      // Current counters [switch][direction]
extern int switchRow[26];             // Position of switch on grid
extern int switchCol[26];
extern bool switchFlipQueued[26];    // Should flip after movement?
extern int switchSignal[26];          // Current signal color
extern int numSwitches;               // Number of switches in level

// ----------------------------------------------------------------------------
// GLOBAL STATE: SPAWN POINTS
// ----------------------------------------------------------------------------
extern int spawnRow[50];              // Row of each spawn point 'S'
extern int spawnCol[50];              // Column of each spawn point 'S'
extern int numSpawns;                 // Total spawn points

// ----------------------------------------------------------------------------
// GLOBAL STATE: DESTINATION POINTS
// ----------------------------------------------------------------------------
extern int destRow[50];               // Row of each destination 'D'
extern int destCol[50];               // Column of each destination 'D'
extern int numDestinations;           // Total destination points

// ----------------------------------------------------------------------------
// GLOBAL STATE: SIMULATION PARAMETERS
// ----------------------------------------------------------------------------
extern int currentTick;               // Current simulation time
extern int randomSeed;                // For deterministic randomness
extern int weatherMode;               // NORMAL, RAIN, or FOG
extern int maxSafetyBuffers;          // Max safety tiles allowed
extern int currentSafetyBuffers;      // Currently placed safety tiles

// ----------------------------------------------------------------------------
// GLOBAL STATE: METRICS
// ----------------------------------------------------------------------------
extern int trainsDelivered;           // Successfully reached destination
extern int trainsCrashed;             // Collisions or invalid moves
extern int totalWaitTicks;            // Sum of all waiting time
extern int totalSwitchFlips;          // Total state changes
extern int signalViolations;          // Entries against red signal

// ----------------------------------------------------------------------------
// GLOBAL STATE: EMERGENCY HALT
// ----------------------------------------------------------------------------
extern bool emergencyHaltActive;      // Is halt currently active?
extern int emergencyHaltTicks;        // Ticks remaining in halt
extern int emergencyHaltRow;          // Center row of 3x3 halt zone
extern int emergencyHaltCol;          // Center column of 3x3 halt zone

// ----------------------------------------------------------------------------
// INITIALIZATION FUNCTION
// ----------------------------------------------------------------------------
// Call this before loading a new level to reset everything
void initializeSimulationState();

#endif