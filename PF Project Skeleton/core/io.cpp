#include "io.h"
#include "simulation_state.h"
#include "grid.h"
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool loadLevelFile(const char* filename) {
    ifstream file(filename);
    
    if(!file.is_open()) {
        cout << "ERROR: Could not open file " << filename << endl;
        return false;
    }
    
    cout << "Loading level file: " << filename << endl;
    
    bool readingMap = false;
    bool readingSwitches = false;
    bool readingTrains = false;
    int currentMapRow = 0;
    
    string line;
    
    while(getline(file, line)) {
        if(!readingMap) {
            line = trim(line);
            if(line.empty()) {
                continue;
            }
        }
        
        if(line == "NAME:" || trim(line) == "NAME:") {
            getline(file, line);
            continue;
        }
        else if(trim(line) == "ROWS:") {
            getline(file, line);
            rows = atoi(trim(line).c_str());
            cout << "  Rows: " << rows << endl;
        }
        else if(trim(line) == "COLS:") {
            getline(file, line);
            cols = atoi(trim(line).c_str());
            cout << "  Cols: " << cols << endl;
        }
        else if(trim(line) == "SEED:") {
            getline(file, line);
            randomSeed = atoi(trim(line).c_str());
            cout << "  Seed: " << randomSeed << endl;
        }
        else if(trim(line) == "WEATHER:") {
            getline(file, line);
            string weather = trim(line);
            
            if(weather == "NORMAL") {
                weatherMode = WEATHER_NORMAL;
            }
            else if(weather == "RAIN") {
                weatherMode = WEATHER_RAIN;
            }
            else if(weather == "FOG") {
                weatherMode = WEATHER_FOG;
            }
            cout << "  Weather: " << weather << endl;
        }
        else if(trim(line) == "MAP:") {
            readingMap = true;
            readingSwitches = false;
            readingTrains = false;
            currentMapRow = 0;
            cout << "  Reading map..." << endl;
            continue;
        }
        else if(trim(line) == "SWITCHES:") {
            readingMap = false;
            readingSwitches = true;
            readingTrains = false;
            cout << "  Reading switches..." << endl;
            continue;
        }
        else if(trim(line) == "TRAINS:") {
            readingMap = false;
            readingSwitches = false;
            readingTrains = true;
            cout << "  Reading trains..." << endl;
            continue;
        }
        else if(readingMap) {
            if(currentMapRow < rows) {
                int lineLen = line.length();
                
                for(int col = 0; col < cols; col++) {
                    if(col < lineLen) {
                        grid[currentMapRow][col] = line[col];
                    }
                    else {
                        grid[currentMapRow][col] = ' ';
                    }
                    
                    originalTiles[currentMapRow][col] = grid[currentMapRow][col];
                    char tile = grid[currentMapRow][col];
                    
                    if(tile == 'S') {
                        spawnRow[numSpawns] = currentMapRow;
                        spawnCol[numSpawns] = col;
                        numSpawns++;
                    }
                    else if(tile == 'D') {
                        destRow[numDestinations] = currentMapRow;
                        destCol[numDestinations] = col;
                        numDestinations++;
                    }
                    else if(tile >= 'A' && tile <= 'Z' && tile != 'S' && tile != 'D') {
                        int index = tile - 'A';
                        if(switchRow[index] == -1) {
                            switchRow[index] = currentMapRow;
                            switchCol[index] = col;
                            numSwitches++;
                        }
                    }
                    else if(tile == '=') {
                        currentSafetyBuffers++;
                    }
                }
                currentMapRow++;
            }
        }
        else if(readingSwitches) {
            if(line.empty()) continue;
            
            istringstream iss(line);
            char letter;
            string mode;
            int init, kUp, kRight, kDown, kLeft;
            string state0, state1;
            
            if(iss >> letter >> mode >> init >> kUp >> kRight >> kDown >> kLeft >> state0 >> state1) {
                if(letter == 'S' || letter == 'D') {
                    cout << "    WARNING: Switch '" << letter 
                         << "' conflicts with spawn/dest" << endl;
                }
                
                int index = letter - 'A';
                
                if(mode == "PER_DIR") {
                    switchMode[index] = SWITCH_PER_DIR;
                }
                else if(mode == "GLOBAL") {
                    switchMode[index] = SWITCH_GLOBAL;
                }
                
                switchState[index] = init;
                switchKValue[index][DIR_UP] = kUp;
                switchKValue[index][DIR_RIGHT] = kRight;
                switchKValue[index][DIR_DOWN] = kDown;
                switchKValue[index][DIR_LEFT] = kLeft;
                
                for(int d = 0; d < 4; d++) {
                    switchCounter[index][d] = 0;
                }
            }
        }
        else if(readingTrains) {
            if(line.empty()) continue;
            
            istringstream iss(line);
            int tick, col, row, dir, dest;
            
            if(iss >> tick >> col >> row >> dir >> dest) {
                trainSpawnTick[numTrains] = tick;
                trainDirection[numTrains] = dir;
                
                trainRow[numTrains] = row;
                trainCol[numTrains] = col;
                
                if(dest < 0 || dest >= numDestinations) {
                    dest = 0;
                }
                trainDestinationIndex[numTrains] = dest;
                
                trainState[numTrains] = TRAIN_WAITING;
                trainColor[numTrains] = numTrains % 10;
                trainDelayCounter[numTrains] = 0;
                
                numTrains++;
            }
        }
    }
    
    file.close();
    
    // Balanced spawn assignment using round-robin
    cout << "\n=== SPAWN ASSIGNMENT ===" << endl;
    cout << "Distributing " << numTrains << " trains across " 
         << numSpawns << " spawn points..." << endl;
    
    if(numSpawns > 0) {
        for(int t = 0; t < numTrains; t++) {
            int spawnIdx = t % numSpawns;
            
            trainRow[t] = spawnRow[spawnIdx];
            trainCol[t] = spawnCol[spawnIdx];
            
            cout << "  Train " << t << " -> Spawn " << spawnIdx 
                 << " at (" << trainRow[t] << "," << trainCol[t] << ")" << endl;
        }
    }
    
    cout << "\n=== LEVEL LOADED ===" << endl;
    cout << "Grid: " << rows << " x " << cols << endl;
    cout << "Trains: " << numTrains << endl;
    cout << "Spawns: " << numSpawns << endl;
    cout << "Destinations: " << numDestinations << endl;
    cout << "Switches: " << numSwitches << endl;
    cout << "Safety Buffers: " << currentSafetyBuffers << endl;
    
    cout << "\nSpawn points:" << endl;
    for(int i = 0; i < numSpawns; i++) {
        int count = 0;
        for(int t = 0; t < numTrains; t++) {
            if(trainRow[t] == spawnRow[i] && trainCol[t] == spawnCol[i]) {
                count++;
            }
        }
        cout << "  [" << i << "] row=" << spawnRow[i] 
             << " col=" << spawnCol[i] 
             << " (assigned: " << count << " trains)" << endl;
    }
    
    cout << "\nDestinations:" << endl;
    for(int i = 0; i < numDestinations; i++) {
        cout << "  [" << i << "] row=" << destRow[i] 
             << " col=" << destCol[i] << endl;
    }
    
    cout << "\nSwitches:" << endl;
    for(int i = 0; i < 26; i++) {
        if(switchRow[i] != -1) {
            char letter = 'A' + i;
            cout << "  [" << letter << "] row=" << switchRow[i] 
                 << " col=" << switchCol[i] 
                 << " mode=" << (switchMode[i] == SWITCH_PER_DIR ? "PER_DIR" : "GLOBAL")
                 << " K=" << switchKValue[i][0] << endl;
        }
    }
    
    cout << "\nFinal train assignments:" << endl;
    for(int i = 0; i < numTrains; i++) {
        int destIdx = trainDestinationIndex[i];
        cout << "  Train " << i << ": tick=" << trainSpawnTick[i]
             << " spawn=(" << trainRow[i] << "," << trainCol[i] << ")"
             << " dest=" << destIdx 
             << " at (" << destRow[destIdx] << "," << destCol[destIdx] << ")"
             << endl;
    }
    
    cout << "===================" << endl;
    
    return true;
}

void initializeLogFiles() {
    ofstream traceFile("out/trace.csv");
    if(traceFile.is_open()) {
        traceFile << "Tick,TrainID,Row,Col,Direction,State\n";
        traceFile.close();
    }
    
    ofstream switchFile("out/switches.csv");
    if(switchFile.is_open()) {
        switchFile << "Tick,Switch,Mode,State,Counter\n";
        switchFile.close();
    }
    
    ofstream signalFile("out/signals.csv");
    if(signalFile.is_open()) {
        signalFile << "Tick,Switch,Signal\n";
        signalFile.close();
    }
}

void logTrainTrace() {
    ofstream file("out/trace.csv", ios::app);
    if(!file.is_open()) return;
    
    for(int i = 0; i < numTrains; i++) {
        file << currentTick << ","
             << i << ","
             << trainRow[i] << ","
             << trainCol[i] << ","
             << trainDirection[i] << ","
             << trainState[i] << "\n";
    }
    
    file.close();
}

void logSwitchState() {
    ofstream file("out/switches.csv", ios::app);
    if(!file.is_open()) return;
    
    for(int i = 0; i < 26; i++) {
        if(switchRow[i] != -1) {
            char letter = 'A' + i;
            file << currentTick << ","
                 << letter << ","
                 << switchMode[i] << ","
                 << switchState[i] << ","
                 << switchCounter[i][0] << "\n";
        }
    }
    
    file.close();
}

void logSignalState() {
    ofstream file("out/signals.csv", ios::app);
    if(!file.is_open()) return;
    
    for(int i = 0; i < 26; i++) {
        if(switchRow[i] != -1) {
            char letter = 'A' + i;
            
            const char* signalName;
            if(switchSignal[i] == SIGNAL_GREEN) {
                signalName = "GREEN";
            }
            else if(switchSignal[i] == SIGNAL_YELLOW) {
                signalName = "YELLOW";
            }
            else {
                signalName = "RED";
            }
            
            file << currentTick << ","
                 << letter << ","
                 << signalName << "\n";
        }
    }
    
    file.close();
}

void writeMetrics() {
    ofstream file("out/metrics.txt");
    if(!file.is_open()) return;
    
    file << "===================================\n";
    file << "SIMULATION METRICS\n";
    file << "===================================\n\n";
    
    file << "Grid Size: " << rows << " x " << cols << "\n";
    file << "Total Trains: " << numTrains << "\n";
    file << "Trains Delivered: " << trainsDelivered << "\n";
    file << "Trains Crashed: " << trainsCrashed << "\n";
    file << "Total Ticks: " << currentTick << "\n\n";
    
    if(numTrains > 0) {
        float avgWait = (float)totalWaitTicks / numTrains;
        file << "Average Wait Time: " << avgWait << " ticks\n";
    }
    
    file << "Total Switch Flips: " << totalSwitchFlips << "\n";
    file << "Signal Violations: " << signalViolations << "\n";
    file << "Safety Buffers: " << currentSafetyBuffers << "\n\n";
    
    if(currentTick > 0) {
        float throughput = (float)trainsDelivered * 100.0 / currentTick;
        file << "Throughput: " << throughput << " trains per 100 ticks\n";
    }
    
    if(numTrains > 0) {
        float successRate = (float)trainsDelivered * 100.0 / numTrains;
        file << "Success Rate: " << successRate << "%\n";
    }
    
    file << "===================================\n";
    file.close();
}