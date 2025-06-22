#include <iostream>
#include <vector>
#include <random>   // For random number generation
#include <chrono>   // For seeding the random number generator

// Define Map as a vector of vectors of integers.
using Map = std::vector<std::vector<int>>;

/**
 * @brief Prints the map (matrix) to the console.
 * @param map The map to print.
 */
void printMap(const Map& map) {
    std::cout << "--- Current Map ---" << std::endl;
    for (const auto& row : map) {
        for (int cell : row) {
            // Better visual representation
            if (cell == 0) std::cout << ". ";      // Empty space
            else if (cell == 1) std::cout << "# ";  // Wall/Room
            else if (cell == 2) std::cout << "@ ";  // Agent position
            else std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}

/**
 * @brief Function to implement the Cellular Automata logic.
 * IMPLEMENTACIÓN COMPLETA DEL AUTÓMATA CELULAR
 */
Map cellularAutomata(const Map& currentMap, int W, int H, int R, double U) {
    Map newMap = currentMap; // Initially, the new map is a copy of the current one

    // Iterate over each cell in the grid
    for (int x = 0; x < H; ++x) {
        for (int y = 0; y < W; ++y) {
            int neighborCount = 0;
            
            // Count neighbors in the square radius R
            for (int dx = -R; dx <= R; ++dx) {
                for (int dy = -R; dy <= R; ++dy) {
                    int nx = x + dx;
                    int ny = y + dy;
                    
                    // Handle borders - consider out-of-bounds as walls (1)
                    if (nx < 0 || nx >= H || ny < 0 || ny >= W) {
                        neighborCount++; // Treat borders as 1
                    } else {
                        // Only count cells with value 1 (not agent position)
                        if (currentMap[nx][ny] == 1 || currentMap[nx][ny] == 2) {
                            neighborCount++;
                        }
                    }
                }
            }
            
            // Apply cellular automata rule based on threshold
            // Preserve agent position
            if (currentMap[x][y] == 2) {
                newMap[x][y] = 2; // Keep agent
            } else {
                newMap[x][y] = (neighborCount >= U) ? 1 : 0;
            }
        }
    }

    return newMap;
}

/**
 * @brief Function to implement the Drunk Agent logic.
 * IMPLEMENTACIÓN MEJORADA Y COMPLETA DEL AGENTE BORRACHO
 */
Map drunkAgent(const Map& currentMap, int W, int H, int J, int I, int roomSizeX, int roomSizeY,
               double probGenerateRoom, double probIncreaseRoom,
               double probChangeDirection, double probIncreaseChange,
               int& agentX, int& agentY) {
    Map newMap = currentMap;

    // Random number generator setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    std::uniform_int_distribution<> dirDis(0, 3);

    // Direction vectors: North, East, South, West
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    int currentDirection = dirDis(gen);

    // Current probabilities that will change during execution
    double currentProbGenerateRoom = probGenerateRoom;
    double currentProbChangeDirection = probChangeDirection;

    std::cout << "=== DRUNK AGENT STARTING ===" << std::endl;
    std::cout << "Initial position: (" << agentX << ", " << agentY << ")" << std::endl;
    std::cout << "Parameters: J=" << J << " I=" << I << " RoomSize=" << roomSizeX << "x" << roomSizeY << std::endl;

    // Perform J movements
    for (int walk = 0; walk < J; ++walk) {
        std::cout << "\n--- Walk " << (walk + 1) << "/" << J << " ---" << std::endl;
        std::cout << "Starting at (" << agentX << ", " << agentY << ")" << std::endl;
        
        // Walk I steps in current direction
        for (int step = 0; step < I; ++step) {
            // Mark current position as corridor
            if (agentX >= 0 && agentX < H && agentY >= 0 && agentY < W) {
                newMap[agentX][agentY] = 1;
            }
            
            // Calculate next position
            int nextX = agentX + dx[currentDirection];
            int nextY = agentY + dy[currentDirection];
            
            // Check boundaries and stop if agent would go out of bounds
            if (nextX < 0 || nextX >= H || nextY < 0 || nextY >= W) {
                std::cout << "  Agent hit boundary at step " << (step + 1) << ", stopping walk" << std::endl;
                break; // Stop current walk
            }
            
            // Move agent
            agentX = nextX;
            agentY = nextY;
            
            // Mark new position
            if (agentX >= 0 && agentX < H && agentY >= 0 && agentY < W) {
                newMap[agentX][agentY] = 1;
            }
        }
        
        std::cout << "Walk ended at (" << agentX << ", " << agentY << ")" << std::endl;
        
        // Try to generate room with current probability
        if (dis(gen) < currentProbGenerateRoom) {
            std::cout << "*** GENERATING ROOM ***" << std::endl;
            
            // Generate room centered at agent position
            int roomStartX = agentX - roomSizeX / 2;
            int roomStartY = agentY - roomSizeY / 2;
            int roomEndX = roomStartX + roomSizeX;
            int roomEndY = roomStartY + roomSizeY;
            
            // Clamp room to map boundaries (ensure room is generated even near edges)
            roomStartX = std::max(0, roomStartX);
            roomStartY = std::max(0, roomStartY);
            roomEndX = std::min(H, roomEndX);
            roomEndY = std::min(W, roomEndY);
            
            // Fill room area
            for (int x = roomStartX; x < roomEndX; ++x) {
                for (int y = roomStartY; y < roomEndY; ++y) {
                    newMap[x][y] = 1;
                }
            }
            
            std::cout << "Room generated: (" << roomStartX << "," << roomStartY << ") to (" 
                      << (roomEndX-1) << "," << (roomEndY-1) << ")" << std::endl;
            
            // Reset room probability to initial value
            currentProbGenerateRoom = probGenerateRoom;
        } else {
            // Increase room probability for next attempt
            currentProbGenerateRoom = std::min(1.0, currentProbGenerateRoom + probIncreaseRoom);
            std::cout << "No room generated. Room probability increased to " 
                      << currentProbGenerateRoom << std::endl;
        }
        
        // Try to change direction with current probability
        if (dis(gen) < currentProbChangeDirection) {
            int oldDirection = currentDirection;
            currentDirection = dirDis(gen);
            std::cout << "Direction changed from " << oldDirection << " to " << currentDirection << std::endl;
            
            // Reset direction change probability
            currentProbChangeDirection = probChangeDirection;
        } else {
            // Increase direction change probability for next attempt
            currentProbChangeDirection = std::min(1.0, currentProbChangeDirection + probIncreaseChange);
            std::cout << "Direction not changed. Change probability increased to " 
                      << currentProbChangeDirection << std::endl;
        }
    }
    
    // Mark final agent position
    newMap[agentX][agentY] = 2;
    std::cout << "\nFinal agent position: (" << agentX << ", " << agentY << ")" << std::endl;
    
    return newMap;
}

/**
 * @brief Initialize map with random noise for cellular automaton
 * FUNCIÓN NUEVA PARA INICIALIZAR CON RUIDO ALEATORIO
 */
void initializeRandomMap(Map& map, int H, int W, double density = 0.45) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int x = 0; x < H; ++x) {
        for (int y = 0; y < W; ++y) {
            map[x][y] = (dis(gen) < density) ? 1 : 0;
        }
    }
    
    std::cout << "Map initialized with random noise (density: " << density << ")" << std::endl;
}

int main() {
    std::cout << "=== CELLULAR AUTOMATA AND DRUNK AGENT SIMULATION ===" << std::endl;

    // --- Initial Map Configuration ---
    int mapRows = 25;
    int mapCols = 40;
    Map myMap(mapRows, std::vector<int>(mapCols, 0)); // Map initialized with zeros

    std::cout << "\n=== TESTING DRUNK AGENT ===" << std::endl;
    
    // Initialize map for drunk agent (empty)
    std::fill(myMap.begin(), myMap.end(), std::vector<int>(mapCols, 0));
    
    // Drunk Agent's initial random position
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDis(0, mapRows - 1);
    std::uniform_int_distribution<> yDis(0, mapCols - 1);
    
    int drunkAgentX = xDis(gen);
    int drunkAgentY = yDis(gen);
    myMap[drunkAgentX][drunkAgentY] = 2; // Mark agent position

    std::cout << "Initial map state (Drunk Agent):" << std::endl;
    printMap(myMap);

    // Drunk Agent Parameters
    int da_W = mapCols;
    int da_H = mapRows;
    int da_J = 8;      // Number of "walks"
    int da_I = 12;     // Steps per walk
    int da_roomSizeX = 5;
    int da_roomSizeY = 4;
    double da_probGenerateRoom = 0.3;
    double da_probIncreaseRoom = 0.1;
    double da_probChangeDirection = 0.25;
    double da_probIncreaseChange = 0.15;

    // Run drunk agent
    myMap = drunkAgent(myMap, da_W, da_H, da_J, da_I, da_roomSizeX, da_roomSizeY,
                       da_probGenerateRoom, da_probIncreaseRoom,
                       da_probChangeDirection, da_probIncreaseChange,
                       drunkAgentX, drunkAgentY);

    std::cout << "\nFinal map after Drunk Agent:" << std::endl;
    printMap(myMap);

    // Count filled cells
    int filledCells = 0;
    for (const auto& row : myMap) {
        for (int cell : row) {
            if (cell == 1 || cell == 2) filledCells++;
        }
    }
    std::cout << "Filled cells: " << filledCells << "/" << (mapRows * mapCols) << std::endl;

    std::cout << "\n=== TESTING CELLULAR AUTOMATON ===" << std::endl;
    
    // Initialize map with random noise for cellular automaton
    Map caMap(mapRows, std::vector<int>(mapCols, 0));
    initializeRandomMap(caMap, mapRows, mapCols, 0.45);
    
    std::cout << "Initial random map:" << std::endl;
    printMap(caMap);

    // Cellular Automata Parameters
    int ca_W = mapCols;
    int ca_H = mapRows;
    int ca_R = 1;      // Radius of neighbor window
    int ca_U = 5;      // Threshold (for 3x3 neighborhood, this is reasonable)
    int numIterations = 3;

    // Run cellular automaton iterations
    for (int iteration = 0; iteration < numIterations; ++iteration) {
        std::cout << "\n--- Cellular Automaton Iteration " << (iteration + 1) << "/" << numIterations << " ---" << std::endl;
        
        caMap = cellularAutomata(caMap, ca_W, ca_H, ca_R, ca_U);
        printMap(caMap);
        
        // Count filled cells
        int filled = 0;
        for (const auto& row : caMap) {
            for (int cell : row) {
                if (cell == 1) filled++;
            }
        }
        std::cout << "Filled cells after iteration " << (iteration + 1) << ": " << filled << "/" << (mapRows * mapCols) << std::endl;
    }

    std::cout << "\n=== SIMULATION FINISHED ===" << std::endl;
    return 0;
}