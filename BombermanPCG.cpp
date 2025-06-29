#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;

const int MAP_SIZE = 15;
const vector<string> POWER_UPS = { "#", "$", "@", "&" };
const vector<string> ENEMIES = { "B", "O", "D", "M" };


struct Map {
    vector<vector<string>> grid;

    Map() : grid(MAP_SIZE, vector<string>(MAP_SIZE, "-")) {}

    void print() {
        for (auto& row : grid) {
            for (auto& cell : row) {
                cout << setw(3) << cell;
            }
            cout << endl;
        }
    }
};

class MapGenerator {
public:
    Map generate(float p_md, float p_power, float p_enemy) {
        Map map;

        // Crear bordes y X intercaladas
        for (int i = 0; i < MAP_SIZE; ++i) {
            for (int j = 0; j < MAP_SIZE; ++j) {
                if (i == 0 || i == MAP_SIZE - 1 || j == 0 || j == MAP_SIZE - 1 ||
                    (i % 2 == 0 && j % 2 == 0)) {
                    map.grid[i][j] = "X";
                }
            }
        }

        

        // Obtener posiciones disponibles para colocar contenido
        vector<pair<int, int>> free_positions;
        for (int i = 1; i < MAP_SIZE - 1; ++i) {
            for (int j = 1; j < MAP_SIZE - 1; ++j) {
                if (map.grid[i][j] == "-") {
                    free_positions.emplace_back(i, j);
                }
            }
        }

        return map;
    }

// Esta parte elige de manera aleatoria uno de los power-ups o enemigos de la lista
private:
    default_random_engine random_engine{ random_device{}() };

    string getRandomPowerUp() {
        return POWER_UPS[random_engine() % POWER_UPS.size()];
    }

    string getRandomEnemy() {
        return ENEMIES[random_engine() % ENEMIES.size()];
    }
};

int main() {
    MapGenerator generator;
    Map map = generator.generate(0.4f, 0.1f, 0.05f);
    map.print();
    return 0;
}