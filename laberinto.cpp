#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <chrono> // Para medición de tiempos (Requisito del challenge)

using namespace std;

// Estructura para coordenadas
struct Pos {
    int x, y;
};

// --- MODULO DE GENERACIÓN ---
vector<vector<char>> generarLaberinto(int filas, int cols) {
    // Inicializar todo como camino '*'
    vector<vector<char>> lab(filas, vector<char>(cols, '*'));
    srand(time(0));

    // Crear camino garantizado para asegurar que sea resoluble
    vector<Pos> caminoBase;
    int x = 0, y = 0;
    caminoBase.push_back({x, y});

    while (x < filas - 1 || y < cols - 1) {
        if (x == filas - 1) y++;
        else if (y == cols - 1) x++;
        else if (rand() % 2) x++;
        else y++;
        caminoBase.push_back({x, y});
    }

    // Agregar muros '#' aleatorios (30% de probabilidad) fuera del camino base
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < cols; j++) {
            bool enCamino = false;
            for (auto &p : caminoBase) {
                if (p.x == i && p.y == j) { enCamino = true; break; }
            }
            if (!enCamino && rand() % 100 < 30) {
                lab[i][j] = '#';
            }
        }
    }

    // Requisito: E en el inicio, S en el final
    lab[0][0] = 'E'; 
    lab[filas - 1][cols - 1] = 'S'; 

    return lab;
}

// --- MODULO DE VISUALIZACIÓN ---
void imprimir(const vector<vector<char>> &lab) {
    for (auto &fila : lab) {
        for (char c : fila) cout << c << " ";
        cout << "\n";
    }
}

// --- MODULO DE RESOLUCIÓN (BFS) ---
bool resolverBFS(vector<vector<char>> &lab) {
    int filas = lab.size();
    int cols = lab[0].size();
    vector<vector<bool>> visitado(filas, vector<bool>(cols, false));
    vector<vector<Pos>> padre(filas, vector<Pos>(cols, {-1, -1}));

    queue<Pos> q;
    q.push({0, 0});
    visitado[0][0] = true;

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    bool encontrado = false;

    while (!q.empty()) {
        Pos p = q.front(); q.pop();
        
        if (lab[p.x][p.y] == 'S') {
            encontrado = true;
            break;
        }

        for (int d = 0; d < 4; d++) {
            int nx = p.x + dx[d], ny = p.y + dy[d];
            if (nx >= 0 && nx < filas && ny >= 0 && ny < cols &&
                !visitado[nx][ny] && lab[nx][ny] != '#') {
                visitado[nx][ny] = true;
                padre[nx][ny] = p;
                q.push({nx, ny});
            }
        }
    }

    if (!encontrado) return false;

    // Marcar camino recorrido con 'o'
    Pos p = {filas - 1, cols - 1};
    while (!(p.x == 0 && p.y == 0)) {
        if (lab[p.x][p.y] != 'S' && lab[p.x][p.y] != 'E') lab[p.x][p.y] = 'o';
        p = padre[p.x][p.y];
    }
    return true;
}

// --- MAIN (Punto de entrada configurable) ---
int main(int argc, char* argv[]) {
    // 1. Tamaño por defecto 10x10
    int filas = 10;
    int cols = 10;

    // 2. Parámetros externos (Dinámico)
    // Si el usuario pasa argumentos por consola, se reasignan
    if (argc >= 3) {
        filas = atoi(argv[1]);
        cols = atoi(argv[2]);
    }

    if (filas < 2 || cols < 2) {
        cout << "Error: Tamaño minimo de 2x2 requerido.\n";
        return 1;
    }

    // Generación
    vector<vector<char>> laberinto = generarLaberinto(filas, cols);
    cout << "\n=== LABERINTO ORIGINAL (" << filas << "x" << cols << ") ===\n";
    imprimir(laberinto);

    // Resolución y Medición
    auto inicio = chrono::high_resolution_clock::now();
    bool exito = resolverBFS(laberinto);
    auto fin = chrono::high_resolution_clock::now();
    
    chrono::duration<double, milli> tiempo = fin - inicio;

    cout << "\n=== LABERINTO RESUELTO ===\n";
    imprimir(laberinto);

    if (exito) cout << "\n¡Escape exitoso!\n";
    else cout << "\nNo se encontro salida.\n";

    cout << "Tiempo de ejecucion: " << tiempo.count() << " ms\n";

    return 0;
}
