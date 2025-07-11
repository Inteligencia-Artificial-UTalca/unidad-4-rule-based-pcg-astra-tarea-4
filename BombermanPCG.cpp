#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iomanip>
#include <chrono>


using namespace std;
using namespace chrono;

const int MAP_SIZE = 15;
const int TOTAL_CELLS = MAP_SIZE * MAP_SIZE;
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

        // Calcular cuántos muros destructibles, power ups y enemigos colocar
        int md_total = free_positions.size() * p_md;
        int power_total = md_total * p_power;
        int enemy_total = (free_positions.size() - md_total) * p_enemy;

        shuffle(free_positions.begin(), free_positions.end(), random_engine);

        // Colocar muros destructibles y power ups
        for (auto& [i, j] : free_positions) {
            if (md_total <= 0) break;

            if (power_total > 0) {
                map.grid[i][j] = getRandomPowerUp();
                power_total--;
            } else {
                map.grid[i][j] = "*";
            }
            md_total--;
        }

        // Colocar una única salida "S" en un muro destructible "*"
        shuffle(free_positions.begin(), free_positions.end(), random_engine);
        for (auto& [i, j] : free_positions) {
            if (map.grid[i][j] == "*") {
                map.grid[i][j] = "S";
                break;
            }
        }

        // Colocar enemigos en celdas vacías
        shuffle(free_positions.begin(), free_positions.end(), random_engine);
        for (auto& [i, j] : free_positions) {
            if (enemy_total <= 0) break;
            if (map.grid[i][j] == "-") {
                map.grid[i][j] = getRandomEnemy();
                enemy_total--;
            }
        }

        // Asegurar que las 4 esquinas interiores estén vacías
        vector<pair<int, int>> corners = {
            {1, 1}, {1, MAP_SIZE - 2},
            {MAP_SIZE - 2, 1}, {MAP_SIZE - 2, MAP_SIZE - 2}
        };
        for (auto& [i, j] : corners) {
            map.grid[i][j] = "-";
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

    float p_md = 0.4f;  
    float p_power = 0.1f; 
    float p_enemy = 0.05f;
    float p_ev = 0.00000001f;
    int ev_count = 0;

    auto start = high_resolution_clock::now();

    MapGenerator generator;
    Map map = generator.generate(p_md, p_power, p_enemy);
    map.print();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    for (auto& row : map.grid) {
        for (auto& cell : row) {
            if (cell == "-") ev_count++;
        }
    }

    p_ev = 100 * ev_count / TOTAL_CELLS;

    cout << "\nTiempo de ejecucion: " << duration.count() << " µs" << endl;
    cout << "Parametros usados: p_md = " << p_md * 100 << "%, p_power = " << p_power * 100 << "%, p_enemy = " << p_enemy * 100 << "%, p_ev = " << p_ev << "%" << endl;

    return 0;
}