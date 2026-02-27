#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>

using namespace std;

// -----------------------
// Posición en la grilla
struct Pos {
    int x, y;
};

// -----------------------
// Generar laberinto aleatorio con camino garantizado
vector<vector<char>> generarLaberinto(int filas, int cols) {
    vector<vector<char>> lab(filas, vector<char>(cols, '*'));

    // Crear camino base garantizado: de S(0,0) a E(filas-1, cols-1)
    vector<Pos> caminoBase;
    int x = 0, y = 0;
    caminoBase.push_back({x,y});
    srand(time(0));

    while (x < filas - 1 || y < cols - 1) {
        if (x == filas - 1) y++;
        else if (y == cols - 1) x++;
        else if (rand() % 2) x++;
        else y++;
        caminoBase.push_back({x,y});
    }

    // Agregar muros aleatorios solo fuera del camino base
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < cols; j++) {
            bool enCamino = false;
            for (auto &p : caminoBase)
                if (p.x == i && p.y == j) { enCamino = true; break; }

            if (!enCamino && rand() % 3 == 0) {
                lab[i][j] = '#';
            }
        }
    }

    // Marcar entrada y salida definitivas
    lab[0][0] = 'S';
    lab[filas-1][cols-1] = 'E';

    return lab;
}

// -----------------------
// Imprimir laberinto en consola
void imprimir(const vector<vector<char>> &lab) {
    for (auto &fila : lab) {
        for (char c : fila) cout << c << " ";
        cout << "\n";
    }
}

// -----------------------
// Resolver con BFS y marcar camino
bool resolverBFS(vector<vector<char>> &lab) {
    int filas = lab.size();
    int cols = lab[0].size();
    vector<vector<bool>> visitado(filas, vector<bool>(cols, false));
    vector<vector<Pos>> padre(filas, vector<Pos>(cols, {-1,-1}));

    queue<Pos> q;
    q.push({0,0});
    visitado[0][0] = true;

    int dx[] = {-1,1,0,0};
    int dy[] = {0,0,-1,1};

    bool encontrado = false;

    while (!q.empty()) {
        Pos p = q.front(); q.pop();
        if (lab[p.x][p.y] == 'E') {
            encontrado = true;
            break;
        }

        for (int d=0; d<4; d++) {
            int nx = p.x + dx[d];
            int ny = p.y + dy[d];
            if (nx>=0 && nx<filas && ny>=0 && ny<cols &&
                !visitado[nx][ny] && lab[nx][ny]!='#') {
                visitado[nx][ny] = true;
                padre[nx][ny] = p;
                q.push({nx,ny});
            }
        }
    }

    if (!encontrado) return false;

    // Marcar camino desde E a S
    Pos p = {filas-1, cols-1};
    while (!(p.x==0 && p.y==0)) {
        if (lab[p.x][p.y] != 'E') lab[p.x][p.y] = 'o';
        p = padre[p.x][p.y];
    }
    return true;
}

// -----------------------
// MAIN
int main(int argc, char* argv[]) {

    int filas = 10;
    int cols  = 10;

    if (argc >= 3) {
        filas = atoi(argv[1]);
        cols  = atoi(argv[2]);
    }

    if (filas < 2 || cols < 2) {
        cout << "Error: tamaño mínimo 2x2.\n";
        return 1;
    }

    vector<vector<char>> laberinto = generarLaberinto(filas, cols);

    cout << "\n=== LABERINTO ORIGINAL ===\n";
    imprimir(laberinto);

    bool solucion = resolverBFS(laberinto);

    cout << "\n=== LABERINTO RESUELTO ===\n";
    imprimir(laberinto);

    if (solucion) cout << "\nCamino encontrado! (marcado con 'o')\n";
    else cout << "\nSin solucion.\n";

    // Pausa para ver resultados en Windows si se ejecuta con doble click
    cout << "\nPresiona Enter para salir...";
    cin.ignore();
    cin.get();

    return 0;
}

