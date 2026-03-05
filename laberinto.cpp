#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

/* ESTRUCTURA DE POSICIÓN
   Define un objeto simple para almacenar coordenadas (fila y columna).
   Se utiliza para identificar celdas en la matriz y rastrear el camino.
*/
struct Posicion {
    int fila;
    int columna;
};

/* CONSTANTES GLOBALES
   Definen los caracteres que se mostrarán en la terminal. 
   Centralizar esto permite cambiar el diseño visual del laberinto 
   modificando un solo lugar en el código.
*/
const char MURO   = '#';
const char CAMINO = '.';
const char RASTRO = 'o';
const char INICIO = 'S';
const char META   = 'E';

/* BLOQUE: GENERACIÓN PROCEDIMENTAL DEL LABERINTO
   Esta función crea el tablero. Primero garantiza que exista una solución 
   trazando una ruta aleatoria (derecha/abajo) y luego puebla el resto 
   del mapa con muros aleatorios sin obstruir dicha ruta.
*/
vector<vector<char>> generarLaberinto(int totalFilas, int totalCols) {
    
    vector<vector<char>> laberinto(totalFilas, vector<char>(totalCols, CAMINO));
    vector<Posicion> caminoGarantizado;
    
    int f = 0; 
    int c = 0;
    
    caminoGarantizado.push_back({f, c});
    srand(time(0));

    // Generación del camino seguro (Arquitecto)
    while (f < totalFilas - 1 || c < totalCols - 1) {
        if (f == totalFilas - 1) {
            c++;
        } 
        else if (c == totalCols - 1) {
            f++;
        } 
        else if (rand() % 2 == 0) {
            f++;
        } 
        else {
            c++;
        }
        caminoGarantizado.push_back({f, c});
    }

    // Colocación de obstáculos aleatorios (33% de probabilidad)
    for (int i = 0; i < totalFilas; i++) {
        for (int j = 0; j < totalCols; j++) {
            
            bool esCaminoBase = false;
            for (auto &p : caminoGarantizado) {
                if (p.fila == i && p.columna == j) {
                    esCaminoBase = true;
                    break;
                }
            }

            if (esCaminoBase == false && rand() % 3 == 0) {
                laberinto[i][j] = MURO;
            }
        }
    }

    laberinto[0][0] = INICIO;
    laberinto[totalFilas - 1][totalCols - 1] = META;
    
    return laberinto;
}

/* BLOQUE: RENDERIZADO VISUAL
   Se encarga de recorrer la matriz bidimensional e imprimir cada carácter.
   Añade un espacio entre celdas para mantener la proporción cuadrada en la consola.
*/
void imprimirLaberinto(const vector<vector<char>> &lab) {
    for (const auto &fila : lab) {
        for (char celda : fila) {
            cout << celda << " "; 
        }
        cout << endl;
    }
}

/* BLOQUE: ALGORITMO DE RESOLUCIÓN (BFS)
   Utiliza una búsqueda en anchura para encontrar la ruta más corta. 
   Explora el laberinto por "capas" usando una cola (queue). 
   Mantiene un registro de 'visitados' para evitar ciclos y una matriz 
   de 'padres' para poder reconstruir el camino una vez hallada la meta.
*/
bool resolverLaberinto(vector<vector<char>> &lab) {
    
    int filas = lab.size();
    int cols  = lab[0].size();
    
    vector<vector<bool>> visitado(filas, vector<bool>(cols, false));
    vector<vector<Posicion>> rastroPadres(filas, vector<Posicion>(cols, {-1, -1}));
    queue<Posicion> colaBusqueda;

    colaBusqueda.push({0, 0});
    visitado[0][0] = true;

    int dFila[] = {-1, 1, 0, 0};
    int dCol[]  = {0, 0, -1, 1};

    bool encontrado = false;

    // Proceso de exploración
    while (colaBusqueda.empty() == false) {
        
        Posicion actual = colaBusqueda.front();
        colaBusqueda.pop();

        if (lab[actual.fila][actual.columna] == META) {
            encontrado = true;
            break;
        }

        for (int i = 0; i < 4; i++) { // direcciones de busqueda 
            int nFila = actual.fila + dFila[i];
            int nCol  = actual.columna + dCol[i];

            if (nFila >= 0 && nFila < filas && nCol >= 0 && nCol < cols && // filtro
                visitado[nFila][nCol] == false && lab[nFila][nCol] != MURO) {
                
                visitado[nFila][nCol] = true; // marca 
                rastroPadres[nFila][nCol] = actual;
                colaBusqueda.push({nFila, nCol});// agenda 
            }
        }
    }

    if (encontrado == false) return false;

    // Bloque de reconstrucción del camino (Retroceso)
    Posicion paso = {filas - 1, cols - 1};
    while (paso.fila != 0 || paso.columna != 0) {
        if (lab[paso.fila][paso.columna] != META) {
            lab[paso.fila][paso.columna] = RASTRO;
        }
        paso = rastroPadres[paso.fila][paso.columna];
    }
    
    return true;
}

/* BLOQUE PRINCIPAL (MAIN)
   Punto de entrada del programa. Gestiona la lógica de ejecución:
   1. Configura dimensiones.
   2. Genera y muestra el laberinto original.
   3. Activa el cronómetro de alta resolución.
   4. Resuelve el laberinto y muestra el tiempo final en microsegundos.
*/
int main(int argc, char* argv[]) {
    
    int filas = 10;
    int columnas = 10;

    if (argc >= 3) {
        filas    = atoi(argv[1]);
        columnas = atoi(argv[2]);
    }

    if (filas < 2 || columnas < 2) {
        cout << "Error: Dimensiones mínimas 2x2." << endl;
        return 1;
    }

    vector<vector<char>> miLaberinto = generarLaberinto(filas, columnas);

    cout << "\n--- LABERINTO GENERADO ---\n" << endl;
    imprimirLaberinto(miLaberinto);

    auto tiempoInicio = high_resolution_clock::now();

    bool exito = resolverLaberinto(miLaberinto);

    auto tiempoFin = high_resolution_clock::now();
    auto duracion  = duration_cast<microseconds>(tiempoFin - tiempoInicio);

    cout << "\n--- LABERINTO RESUELTO ---\n" << endl;
    imprimirLaberinto(miLaberinto);

    if (exito == true) {
        cout << "Resultado: Camino encontrado con éxito." << endl;
        cout << "Rendimiento: " << duracion.count() << " microsegundos." << endl;
    } 
    else {
        cout << "Resultado: No se encontró solución." << endl;
    }

    cout << "\nPresiona Enter para cerrar...";
    cin.ignore();
    cin.get();

    return 0;
}

    return 0;
}
