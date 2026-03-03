#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <chrono> // Librería para medir el tiempo de ejecución

using namespace std;
using namespace std::chrono;
     
// ESTRUCTURAS Y CONSTANTES

struct Posicion {
    int fila;
    int columna;
};

const char MURO   = '#';
const char CAMINO = '.';
const char RASTRO = 'o';
const char INICIO = 'S';
const char META   = 'E';
   
// GENERACIÓN DEL LABERINTO


vector<vector<char>> generarLaberinto(int totalFilas, int totalCols) {
    
    // 1. Inicializar tablero lleno de caminos transitables (.)
    vector<vector<char>> laberinto(totalFilas, vector<char>(totalCols, CAMINO));
    vector<Posicion> caminoGarantizado;
    
    int f = 0, c = 0;
    caminoGarantizado.push_back({f, c});
    srand(time(0));

    // 2. Crear una ruta segura desde el inicio hasta el fin
    while (f < totalFilas - 1 || c < totalCols - 1) {
        if (f == totalFilas - 1) {
            c++;
        } else if (c == totalCols - 1) {
            f++;
        } else if (rand() % 2 == 0) {
            f++;
        } else {
            c++;
        }
        caminoGarantizado.push_back({f, c});
    }

    // 3. Colocar muros aleatorios evitando bloquear la ruta segura
    for (int i = 0; i < totalFilas; i++) { // recorre 
        for (int j = 0; j < totalCols; j++) {
            
            bool esCaminoBase = false;  // Inicializa una variable para saber si la celda pertenece al camino seguro
            for (auto &p : caminoGarantizado) { // Recorre todas las posiciones del camino garantizado.
                if (p.fila == i && p.columna == j) { // camino garantizado
                    esCaminoBase = true;
                    break;
                }
            }

            if (!esCaminoBase && rand() % 3 == 0) {
                laberinto[i][j] = MURO;
            }
        }
    }

    laberinto[0][0] = INICIO;
    laberinto[totalFilas - 1][totalCols - 1] = META;
    
    return laberinto;
}

     
// VISUALIZACIÓN EN TERMINAL


void imprimirLaberinto(const vector<vector<char>> &lab) {
    
    for (const auto &fila : lab) {
        for (char celda : fila) {
            cout << celda << " "; // El espacio mejora la estética cuadrada
        }
        cout << endl;
    }
}
      
// ALGORITMO DE RESOLUCIÓN (BFS)

bool resolverLaberinto(vector<vector<char>> &lab) {
    
    int filas = lab.size();
    int cols  = lab[0].size();
    
    vector<vector<bool>> visitado(filas, vector<bool>(cols, false));
    vector<vector<Posicion>> rastroPadres(filas, vector<Posicion>(cols, {-1, -1}));
    queue<Posicion> colaBusqueda;

    // Configuración inicial
    colaBusqueda.push({0, 0});
    visitado[0][0] = true;

    // Definición de movimientos (Norte, Sur, Oeste, Este)
    int dFila[] = {-1, 1, 0, 0};
    int dCol[]  = {0, 0, -1, 1};

    bool encontrado = false;

    // Bucle principal de búsqueda
    while (!colaBusqueda.empty()) { // guarda las celdas que aún necesitamos visitar.
        
        Posicion actual = colaBusqueda.front();
        colaBusqueda.pop();

        if (lab[actual.fila][actual.columna] == META) {
            encontrado = true;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nFila = actual.fila + dFila[i];
            int nCol  = actual.columna + dCol[i];

            if (nFila >= 0 && nFila < filas && nCol >= 0 && nCol < cols &&
                !visitado[nFila][nCol] && lab[nFila][nCol] != MURO) {
                
                visitado[nFila][nCol] = true;
                rastroPadres[nFila][nCol] = actual;
                colaBusqueda.push({nFila, nCol});
            }
        }
    }

    if (!encontrado) return false;

    // Reconstrucción del camino trazando la ruta de 'o'
    Posicion paso = {filas - 1, cols - 1};
    while (!(paso.fila == 0 && paso.columna == 0)) {
        if (lab[paso.fila][paso.columna] != META) {
            lab[paso.fila][paso.columna] = RASTRO;
        }
        paso = rastroPadres[paso.fila][paso.columna];
    }
    
    return true;
}


// PUNTO DE ENTRADA DEL PROGRAMA


int main(int argc, char* argv[]) {
    
    int filas = 10, columnas = 10;

    // Procesar argumentos de consola
    if (argc >= 3) {
        filas    = atoi(argv[1]);
        columnas = atoi(argv[2]);
    }

    if (filas < 2 || columnas < 2) {
        cout << "Error: Las dimensiones mínimas son 2x2." << endl;
        return 1;
    }

    // Generación inicial
    vector<vector<char>> miLaberinto = generarLaberinto(filas, columnas);

    cout << "\n--- LABERINTO ORIGINAL ---" << endl;
    imprimirLaberinto(miLaberinto);

    // Medición de rendimiento
    auto tiempoInicio = high_resolution_clock::now();

    bool exito = resolverLaberinto(miLaberinto);

    auto tiempoFin = high_resolution_clock::now();
    auto duracion  = duration_cast<microseconds>(tiempoFin - tiempoInicio);

    // Resultados finales
    cout << "\n--- LABERINTO RESUELTO ---" << endl;
    imprimirLaberinto(miLaberinto);

    if (exito) {
        cout << "\n¡Camino encontrado con éxito!" << endl;
        cout << "Tiempo de respuesta: " << duracion.count() << " microsegundos." << endl;
    } else {
        cout << "\nNo se pudo hallar una ruta de salida." << endl;
    }

    cout << "\nPresiona Enter para finalizar el proceso...";
    cin.ignore();
    cin.get();

    return 0;
}
