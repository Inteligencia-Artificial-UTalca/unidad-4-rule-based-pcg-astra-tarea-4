#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <tuple>
#include <iomanip>
#include <algorithm>

using namespace std;

const int MAP_SIZE = 15;
const int TOTAL_CELLS = MAP_SIZE * MAP_SIZE;

// Strings que representan cada tipo de celda
const vector<string> POWER_UPS = { "#", "$", "@", "&" };
const vector<string> ENEMIES = { "B", "O", "D", "M" };

struct Map {
    vector<vector<string>> grid;
    float score = 0.0;

    Map() : grid(MAP_SIZE, vector<string>(MAP_SIZE, "-")) {}

    void print() {
        for (auto& row : grid) {
            for (auto& cell : row) {
                cout << setw(4) << cell;
            }
            cout << endl;
        }
    }
};

class MapGenerator {
public:
    Map generate(float p_md, float p_power, float p_enemy) {
    Map map;

    // Poner bordes y estructura intercalada con muros indestructibles ("X")
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            if (i == 0 || i == MAP_SIZE - 1 || j == 0 || j == MAP_SIZE - 1 ||
                (i % 2 == 0 && j % 2 == 0)) {
                map.grid[i][j] = "X";
            }
        }
    }

    // Contar cuántas celdas están disponibles (siguen como "-")
    int available_cells = 0;
    vector<pair<int, int>> positions;
    for (int i = 1; i < MAP_SIZE - 1; ++i) {
        for (int j = 1; j < MAP_SIZE - 1; ++j) {
            if (map.grid[i][j] == "-") {
                positions.emplace_back(i, j);
                available_cells++;
            }
        }
    }

    // Calcular cuántos bloques colocar
    int md_count = available_cells * p_md;
    int power_count = md_count * p_power;
    int enemy_count = (available_cells - md_count) * p_enemy;

    // Colocar bloques destructibles con o sin power-ups
    shuffle(positions.begin(), positions.end(), random_engine);
    for (auto& [i, j] : positions) {
        if (md_count > 0) {
            if (power_count > 0) {
                map.grid[i][j] = getRandomPowerUp();
                power_count--;
            } else {
                map.grid[i][j] = "*";
            }
            md_count--;
        }
    }

    // Colocar salida "S" en un muro destructible aleatorio
    shuffle(positions.begin(), positions.end(), random_engine);
    for (auto& [i, j] : positions) {
    if (map.grid[i][j] == "*") {
        map.grid[i][j] = "S";
        break;
    }
}


    // Colocar enemigos en espacios vacíos restantes
    shuffle(positions.begin(), positions.end(), random_engine);
    for (auto& [i, j] : positions) {
        if (enemy_count > 0 && map.grid[i][j] == "-") {
            map.grid[i][j] = getRandomEnemy();
            enemy_count--;
        }
    }

    return map;
}


private:
    default_random_engine random_engine{ random_device{}() };

    string getRandomPowerUp() {
        return POWER_UPS[random_engine() % POWER_UPS.size()];
    }

    string getRandomEnemy() {
        return ENEMIES[random_engine() % ENEMIES.size()];
    }
};

float evaluateMap(const Map& map) {
    int ev_count = 0;
    int enemy_count = 0;
    int power_up_count = 0;
    int power_up_score = 0;
    int md_count = 0;

    for (auto& row : map.grid) {
        for (auto& cell : row) {
            if (cell == "-") ev_count++;
            if (find(ENEMIES.begin(), ENEMIES.end(), cell) != ENEMIES.end()) enemy_count++;
            if (find(POWER_UPS.begin(), POWER_UPS.end(), cell) != POWER_UPS.end()) power_up_count++;
            if (cell == "*") md_count++;
        }
    }

    if(power_up_count > 3) {
        power_up_score = 1.5f * power_up_count; 
    }

    if(power_up_count > 8) {
        power_up_score = 0.5f * power_up_count; 
    }

    float score = 0.5f * ev_count     // Menos peso a espacios vacíos
                - 1.5f * enemy_count  // Penalización por enemigos
                + power_up_score      // Score sumado por power-ups
                + 3.0f * md_count;    // Mas score mientras hayan mas muros destructibles

    return score;
}


int main() {
vector<float> md_vals = { 0.2f, 0.25f, 0.3f, 0.35f,  0.4f };
vector<float> power_vals = { 0.1f, 0.15f,  0.2f, 0.25f, 0.3f };
vector<float> enemy_vals = { 0.05f, 0.55f, 0.1f, 0.15f, 0.2f };

    MapGenerator generator;

    float best_score = -1e9;
    tuple<float, float, float, float> best_params;
    Map best_map;

    for (float p_md : md_vals) {
    for (float p_power : power_vals) {
        for (float p_enemy : enemy_vals) {
            Map map = generator.generate(p_md, p_power, p_enemy);
            float score = evaluateMap(map);
            if (score > best_score) {
                best_score = score;
                best_params = make_tuple(0.0f, p_md, p_power, p_enemy);
                best_map = map;
                best_map.score = score;
            }
        }
    }
}

    auto [mi, md, power, enemy] = best_params;
    cout << "Mejores parámetros encontrados:" << endl;
    cout << "Muros destructibles:   " << md << endl;
    cout << "Power ups:             " << power << endl;
    cout << "Enemigos:              " << enemy << endl;
    cout << "Score:                 " << best_map.score << endl << endl;

    best_map.print();
    return 0;
}
