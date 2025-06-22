#include <iostream>
#include <vector>
#include <random>   // For random number generation
#include <chrono>   // For seeding the random number generator
#include <algorithm> // For std::max, std::min

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
 * @brief Function to implement the Cellular Automata logic (double-buffer version).
 *
 * @param currentMap The current grid (read-only).
 * @param W Width of the grid (number of columns).
 * @param H Height of the grid (number of rows).
 * @param R Radius of neighbor window.
 * @param U Threshold: if neighborCount >= U then cell = 1, else 0.
 * @return A new Map after applying one iteration of the automaton.
 *
 * Bordes: se tratan como 1 (pared) cuando se cuenta vecino fuera de rango.
 */
Map cellularAutomata(const Map& currentMap, int W, int H, int R, int U) {
    Map newMap = currentMap; // Initially, new map is a copy of the current one

    // Iterate over each cell in the grid
    for (int x = 0; x < H; ++x) {
        for (int y = 0; y < W; ++y) {
            int neighborCount = 0;
            
            // Count neighbors in the square radius R (incluyendo la propia celda)
            for (int dx = -R; dx <= R; ++dx) {
                for (int dy = -R; dy <= R; ++dy) {
                    int nx = x + dx;
                    int ny = y + dy;
                    
                    // Handle borders - consider out-of-bounds as walls (1)
                    if (nx < 0 || nx >= H || ny < 0 || ny >= W) {
                        neighborCount++;
                    } else {
                        // Count cells with value 1 or 2 as occupied
                        if (currentMap[nx][ny] == 1 || currentMap[nx][ny] == 2) {
                            neighborCount++;
                        }
                    }
                }
            }
            
            // Preserve agent position if exists
            if (currentMap[x][y] == 2) {
                newMap[x][y] = 2;
            } else {
                newMap[x][y] = (neighborCount >= U) ? 1 : 0;
            }
        }
    }

    return newMap;
}

/**
 * @brief Function to implement the Cellular Automata logic in-place (sin segunda grilla).
 *
 * @param map Reference to the grid to update directly.
 * @param W Width of the grid (number of columns).
 * @param H Height of the grid (number of rows).
 * @param R Radius of neighbor window.
 * @param U Threshold: if neighborCount >= U then cell = 1, else 0.
 *
 * Nota: al modificar la misma grilla, cambios en celdas ya procesadas afectarán celdas posteriores.
 * Bordes se tratan como 1 (pared).
 */
void cellularAutomataInPlace(Map& map, int W, int H, int R, int U) {
    // Recorremos cada celda y la actualizamos directamente
    for (int x = 0; x < H; ++x) {
        for (int y = 0; y < W; ++y) {
            int neighborCount = 0;
            for (int dx = -R; dx <= R; ++dx) {
                for (int dy = -R; dy <= R; ++dy) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx < 0 || nx >= H || ny < 0 || ny >= W) {
                        neighborCount++;
                    } else {
                        if (map[nx][ny] == 1 || map[nx][ny] == 2) {
                            neighborCount++;
                        }
                    }
                }
            }
            // Si se quiere preservar agente (valor 2), se omite actualización
            if (map[x][y] == 2) {
                continue;
            }
            map[x][y] = (neighborCount >= U) ? 1 : 0;
        }
    }
}

/**
 * @brief Function to implement the Drunk Agent logic.
 * 
 * @param currentMap The current grid (with possible initial agent mark).
 * @param W Width (columns).
 * @param H Height (rows).
 * @param J Number of "walks".
 * @param I Steps per walk.
 * @param roomSizeX Width of room to generate.
 * @param roomSizeY Height of room to generate.
 * @param probGenerateRoom Initial probability to generate a room.
 * @param probIncreaseRoom Increment to probability if no room generated.
 * @param probChangeDirection Initial probability to change direction.
 * @param probIncreaseChange Increment to probability if no change happened.
 * @param agentX Reference to agent's X (row) position; se actualizará.
 * @param agentY Reference to agent's Y (column) position; se actualizará.
 * @return New Map with corridors (1), rooms (1) y posición final del agente marcada con 2.
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
    int dx_arr[] = {-1, 0, 1, 0};
    int dy_arr[] = {0, 1, 0, -1};

    int currentDirection = dirDis(gen);

    // Current probabilities that will change during execution
    double currentProbGenerateRoom = probGenerateRoom;
    double currentProbChangeDirection = probChangeDirection;

    std::cout << "=== DRUNK AGENT STARTING ===" << std::endl;
    std::cout << "Initial position: (" << agentX << ", " << agentY << ")" << std::endl;
    std::cout << "Parameters: J=" << J << " I=" << I 
              << " RoomSize=" << roomSizeX << "x" << roomSizeY << std::endl;

    // Perform J movements
    for (int walk = 0; walk < J; ++walk) {
        std::cout << "\n--- Walk " << (walk + 1) << "/" << J << " ---" << std::endl;
        std::cout << "Starting at (" << agentX << ", " << agentY << ")" << std::endl;
        
        // Walk I steps in current direction
        for (int step = 0; step < I; ++step) {
            // Mark current position as corridor (1) if está en rango
            if (agentX >= 0 && agentX < H && agentY >= 0 && agentY < W) {
                newMap[agentX][agentY] = 1;
            }
            
            // Calculate next position
            int nextX = agentX + dx_arr[currentDirection];
            int nextY = agentY + dy_arr[currentDirection];
            
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
            
            // Clamp room to map boundaries (ensure generation near edges)
            roomStartX = std::max(0, roomStartX);
            roomStartY = std::max(0, roomStartY);
            roomEndX   = std::min(H, roomEndX);
            roomEndY   = std::min(W, roomEndY);
            
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
    if (agentX >= 0 && agentX < H && agentY >= 0 && agentY < W) {
        newMap[agentX][agentY] = 2;
    }
    std::cout << "\nFinal agent position: (" << agentX << ", " << agentY << ")" << std::endl;
    
    return newMap;
}

/**
 * @brief Initialize map with random noise for cellular automaton
 * @param map Reference to the Map to initialize.
 * @param H Number of rows.
 * @param W Number of columns.
 * @param density Probability that a cell is 1 initially.
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

    // --- TESTING DRUNK AGENT ---
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
    // Mark agent position temporarily
    myMap[drunkAgentX][drunkAgentY] = 2;

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
    {
        int filledCells = 0;
        for (const auto& row : myMap) {
            for (int cell : row) {
                if (cell == 1 || cell == 2) filledCells++;
            }
        }
        std::cout << "Filled cells: " << filledCells << "/" << (mapRows * mapCols) << std::endl;
    }

    // --- CELLULAR AUTOMATA VERSIÓN IN-PLACE ---
    std::cout << "\n=== TESTING CELLULAR AUTOMATON (IN-PLACE) ===" << std::endl;
    Map caMapInPlace(mapRows, std::vector<int>(mapCols, 0));
    initializeRandomMap(caMapInPlace, mapRows, mapCols, 0.45);
    std::cout << "Initial random map (in-place CA):" << std::endl;
    printMap(caMapInPlace);
    int ca_R = 1; 
    int ca_U = 5;      // Threshold (for 3x3 neighborhood, incluir propia celda)
    int numIterationsInPlace = 3;
    for (int iteration = 0; iteration < numIterationsInPlace; ++iteration) {
        std::cout << "\n--- In-Place CA Iteration " << (iteration + 1) << "/" << numIterationsInPlace << " ---" << std::endl;
        cellularAutomataInPlace(caMapInPlace, mapCols, mapRows, ca_R, ca_U);
        printMap(caMapInPlace);
        int filled = 0;
        for (const auto& row : caMapInPlace) {
            for (int cell : row) {
                if (cell == 1) filled++;
            }
        }
        std::cout << "Filled cells after in-place iter " << (iteration + 1) << ": " 
                  << filled << "/" << (mapRows * mapCols) << std::endl;
    }

    // --- CELLULAR AUTOMATA VERSIÓN DOBLE BUFFER (no sobrescribe original en cada iter) ---
    std::cout << "\n=== TESTING CELLULAR AUTOMATON (DOUBLE BUFFER) ===" << std::endl;
    Map caMap(mapRows, std::vector<int>(mapCols, 0));
    initializeRandomMap(caMap, mapRows, mapCols, 0.45);
    std::cout << "Initial random map (double-buffer CA):" << std::endl;
    printMap(caMap);
    int numIterations = 3;
    for (int iteration = 0; iteration < numIterations; ++iteration) {
        std::cout << "\n--- Double-Buffer CA Iteration " << (iteration + 1) << "/" << numIterations << " ---" << std::endl;
        caMap = cellularAutomata(caMap, mapCols, mapRows, ca_R, ca_U);
        printMap(caMap);
        int filled = 0;
        for (const auto& row : caMap) {
            for (int cell : row) {
                if (cell == 1) filled++;
            }
        }
        std::cout << "Filled cells after double-buffer iter " << (iteration + 1) << ": " 
                  << filled << "/" << (mapRows * mapCols) << std::endl;
    }

    std::cout << "\n=== SIMULATION FINISHED ===" << std::endl;
    return 0;
}
