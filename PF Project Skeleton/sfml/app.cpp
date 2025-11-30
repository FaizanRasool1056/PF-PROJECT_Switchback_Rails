#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/grid.h"
#include "../core/switches.h"
#include "../core/io.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <cstring>

using namespace std;

static sf::RenderWindow* g_window = nullptr;
static sf::Font g_font;
static bool g_fontLoaded = false;
static sf::View g_camera;
static bool g_isPaused = true;
static bool g_isStepMode = false;
static float g_cellSize = 32.0f;
static float g_gridOffsetX = 10.0f;
static float g_gridOffsetY = 120.0f;
static sf::Clock g_tickClock;
static float g_tickInterval = 0.3f;

void drawGridLines() {
    float gridWidth = cols * g_cellSize;
    float gridHeight = rows * g_cellSize;
    
    sf::Color gridColor(40, 40, 40);
    
    for(int c = 0; c <= cols; c++) {
        float x = g_gridOffsetX + c * g_cellSize;
        sf::RectangleShape line(sf::Vector2f(1, gridHeight));
        line.setPosition(x, g_gridOffsetY);
        line.setFillColor(gridColor);
        g_window->draw(line);
    }
    
    for(int r = 0; r <= rows; r++) {
        float y = g_gridOffsetY + r * g_cellSize;
        sf::RectangleShape line(sf::Vector2f(gridWidth, 1));
        line.setPosition(g_gridOffsetX, y);
        line.setFillColor(gridColor);
        g_window->draw(line);
    }
}

void drawGridCell(int row, int col) {
    float x = g_gridOffsetX + col * g_cellSize;
    float y = g_gridOffsetY + row * g_cellSize;
    
    char tile = grid[row][col];
    if(tile == ' ' || tile == '.') return;
    
    sf::RectangleShape cell(sf::Vector2f(g_cellSize, g_cellSize));
    cell.setPosition(x, y);
    
    if(tile == '-' || tile == '|') {
        cell.setFillColor(sf::Color(35, 35, 35));
    }
    else if(tile == '/' || tile == '\\') {
        cell.setFillColor(sf::Color(45, 45, 45));
    }
    else if(tile == '+') {
        cell.setFillColor(sf::Color(55, 55, 60));
    }
    else if(tile == 'S') {
        cell.setFillColor(sf::Color(20, 70, 20));
    }
    else if(tile == 'D') {
        cell.setFillColor(sf::Color(20, 50, 90));
    }
    else if(tile == '=') {
        cell.setFillColor(sf::Color(70, 40, 10));
    }
    else if(tile >= 'A' && tile <= 'Z') {
        int idx = tile - 'A';
        if(switchState[idx] == 0) {
            cell.setFillColor(sf::Color(60, 40, 10));
        } else {
            cell.setFillColor(sf::Color(60, 10, 60));
        }
    }
    else {
        cell.setFillColor(sf::Color(20, 20, 20));
    }
    
    cell.setOutlineThickness(0);
    g_window->draw(cell);
    
    if(g_fontLoaded && tile != ' ') {
        sf::Text label;
        label.setFont(g_font);
        label.setString(tile);
        label.setCharacterSize(18);
        label.setFillColor(sf::Color(180, 180, 180));
        
        sf::FloatRect bounds = label.getLocalBounds();
        label.setPosition(
            x + g_cellSize/2 - bounds.width/2,
            y + g_cellSize/2 - bounds.height/2 - 2
        );
        
        g_window->draw(label);
    }
}

void drawTrain(int trainIndex) {
    int state = trainState[trainIndex];
    int row = trainRow[trainIndex];
    int col = trainCol[trainIndex];
    
    if(state != TRAIN_MOVING) return;
    if(row < 0 || row >= rows || col < 0 || col >= cols) return;
    
    float cx = g_gridOffsetX + col * g_cellSize + g_cellSize / 2.0f;
    float cy = g_gridOffsetY + row * g_cellSize + g_cellSize / 2.0f;
    
    float radius = g_cellSize * 0.35f;
    if(radius < 8) radius = 8;
    if(radius > 12) radius = 12;
    
    sf::CircleShape dot(radius);
    dot.setPosition(cx - radius, cy - radius);
    
    sf::Color colors[10] = {
        sf::Color(255, 60, 60),
        sf::Color(60, 255, 60),
        sf::Color(255, 255, 60),
        sf::Color(60, 200, 255),
        sf::Color(255, 60, 255),
        sf::Color(255, 150, 60),
        sf::Color(255, 255, 255),
        sf::Color(200, 255, 60),
        sf::Color(255, 100, 200),
        sf::Color(100, 255, 255)
    };
    
    dot.setFillColor(colors[trainIndex % 10]);
    dot.setOutlineThickness(2);
    dot.setOutlineColor(sf::Color::Black);
    
    g_window->draw(dot);
    
    if(g_fontLoaded && radius >= 8) {
        sf::Text num;
        num.setFont(g_font);
        char buf[5];
        sprintf(buf, "%d", trainIndex);
        num.setString(buf);
        num.setCharacterSize(11);
        num.setFillColor(sf::Color::Black);
        num.setStyle(sf::Text::Bold);
        
        sf::FloatRect b = num.getLocalBounds();
        num.setPosition(cx - b.width/2, cy - b.height/2 - 1);
        g_window->draw(num);
    }
}

void drawUI() {
    if(!g_fontLoaded) return;
    
    float panelX = 10;
    float panelY = 10;
    float panelWidth = 200;
    float panelHeight = 100;
    
    sf::RectangleShape bg(sf::Vector2f(panelWidth, panelHeight));
    bg.setPosition(panelX, panelY);
    bg.setFillColor(sf::Color(0, 0, 0, 240));
    bg.setOutlineThickness(2);
    bg.setOutlineColor(sf::Color(0, 180, 180));
    g_window->draw(bg);
    
    sf::Text title;
    title.setFont(g_font);
    title.setCharacterSize(12);
    title.setFillColor(sf::Color(0, 255, 255));
    title.setStyle(sf::Text::Bold);
    
    char titleText[50];
    sprintf(titleText, "SWITCHBACK RAILS %s", g_isPaused ? "PAUSED" : "");
    title.setString(titleText);
    title.setPosition(panelX + 5, panelY + 5);
    g_window->draw(title);
    
    int moving = 0, waiting = 0, delivered = 0, crashed = 0;
    for(int i = 0; i < numTrains; i++) {
        if(trainState[i] == TRAIN_MOVING) moving++;
        else if(trainState[i] == TRAIN_WAITING) waiting++;
        else if(trainState[i] == TRAIN_DELIVERED) delivered++;
        else if(trainState[i] == TRAIN_CRASHED) crashed++;
    }
    
    char buf[300];
    sprintf(buf,
        "Tick:%d | Trains:%d\n"
        "Active:%d Delivered:%d\n"
        "Crashed:%d Flips:%d\n"
        "Weather:NORMAL\n"
        "SPACE:Pause .:Step\n"
        "ESC:Exit",
        currentTick, numTrains,
        moving, delivered,
        crashed, totalSwitchFlips
    );
    
    sf::Text info;
    info.setFont(g_font);
    info.setString(buf);
    info.setCharacterSize(10);
    info.setFillColor(sf::Color::White);
    info.setLineSpacing(1.1);
    info.setPosition(panelX + 5, panelY + 22);
    g_window->draw(info);
}

bool initializeApp() {
    cout << "========================================" << endl;
    cout << "INITIALIZING SFML" << endl;
    cout << "========================================" << endl;
    
    if(rows <= 0 || cols <= 0) {
        cout << "ERROR: Invalid grid: " << rows << "x" << cols << endl;
        return false;
    }
    
    int w = 10 + cols * 32 + 20;
    int h = 120 + rows * 32 + 20;
    
    if(w < 800) w = 800;
    if(h < 600) h = 600;
    if(w > 1200) w = 1200;
    if(h > 800) h = 800;
    
    g_window = new sf::RenderWindow(sf::VideoMode(w, h), "Switchback Rails");
    
    if(!g_window) {
        cout << "ERROR: Failed to create window!" << endl;
        return false;
    }
    
    g_window->setFramerateLimit(60);
    
    const char* fonts[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:\\Windows\\Fonts\\arial.ttf"
    };
    
    g_fontLoaded = false;
    for(int i = 0; i < 5; i++) {
        if(g_font.loadFromFile(fonts[i])) {
            g_fontLoaded = true;
            break;
        }
    }
    
    g_camera = g_window->getDefaultView();
    g_window->setView(g_camera);
    
    cout << "Window size: " << w << "x" << h << " pixels" << endl;
    cout << "Grid size: " << rows << "x" << cols << " cells" << endl;
    cout << "Ready!" << endl;
    cout << "========================================" << endl;
    
    return true;
}

void runApp() {
    cout << "========================================" << endl;
    cout << "SIMULATION READY" << endl;
    cout << "========================================" << endl;
    
    g_tickClock.restart();
    
    while(g_window->isOpen()) {
        
        sf::Event event;
        while(g_window->pollEvent(event)) {
            
            if(event.type == sf::Event::Closed) {
                g_window->close();
            }
            
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Space) {
                    g_isPaused = !g_isPaused;
                    cout << (g_isPaused ? "PAUSED" : "RUNNING") << endl;
                    if(!g_isPaused) {
                        g_tickClock.restart();
                    }
                }
                else if(event.key.code == sf::Keyboard::Period) {
                    g_isStepMode = true;
                    g_isPaused = true;
                }
                else if(event.key.code == sf::Keyboard::Escape) {
                    cout << "Exiting..." << endl;
                    g_window->close();
                }
            }
        }
        
        if(isSimulationComplete()) {
            sf::sleep(sf::seconds(3));
            g_window->close();
            continue;
        }
        
        bool tick = false;
        if(g_isStepMode) {
            tick = true;
            g_isStepMode = false;
        }
        else if(!g_isPaused && g_tickClock.getElapsedTime().asSeconds() >= g_tickInterval) {
            tick = true;
            g_tickClock.restart();
        }
        
        if(tick) {
            simulateOneTick();
        }
        
        g_window->clear(sf::Color(15, 15, 15));
        
        drawGridLines();
        
        for(int r = 0; r < rows; r++) {
            for(int c = 0; c < cols; c++) {
                drawGridCell(r, c);
            }
        }
        
        for(int i = 0; i < numTrains; i++) {
            drawTrain(i);
        }
        
        drawUI();
        
        g_window->display();
        
        sf::sleep(sf::milliseconds(16));
    }
}

void cleanupApp() {
    if(g_window) {
        delete g_window;
        g_window = nullptr;
    }
}