#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <tuple>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores) {
  Action accion = IDLE;


  // Decisión del agente según el nivel
  switch (sensores.nivel) {
    case 0: accion = ComportamientoTecnicoNivel_0(sensores); break;
    case 1: accion = ComportamientoTecnicoNivel_1(sensores); break;
    case 2: accion = ComportamientoTecnicoNivel_2(sensores); break;
    case 3: accion = ComportamientoTecnicoNivel_3(sensores); break;
    case 4: accion = ComportamientoTecnicoNivel_4(sensores); break;
    case 5: accion = ComportamientoTecnicoNivel_5(sensores); break;
    case 6: accion = ComportamientoTecnicoNivel_6(sensores); break;
  }

  return accion;
}

/**
 
* @brief Determina la mejor opcion entre las 3 casillas que tiene delante

* @param i terreno que hay en la posición 1 de superficie (45 izq)

* @param c terreno que hay en la posición 2 de supeficie (justo delante)

* @param d terreno que hay en la posición 3 de supeficie (45 dch)

* @return 2 si es mejor WALK, 1 para TURN_SL y 3 para TURN_SR. O no hay nada interesante.
*/

int VeoCasillaInteresanteT (char i, char c, char d,char a_c) {
if (a_c != '_') return 0;
if (c == 'U') return 2;
else if (i == 'U') return 1;
else if (d == 'U') return 3;
else if (c == 'C') return 2;
else if (i == 'C') return 1;
else if (d == 'C') return 3;
else return 0;

}


/**
* @brief Determina si casilla viable por altura.
* @param casilla tipo de terreno
* @param dif diferencia de altura entre casillas
* @param zap indica si estoy en posesión de las zapatillas
* @return 'P' si no es accesible por altura y casilla en otro caso
*/

char ViablePorAlturaT (char casilla, int dif) {
if (abs (dif) <= 1)
return casilla;
else
return 'P';
}


// Niveles del técnico
Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores) {

Action accion = IDLE;
ActualizarMapa(sensores);

// Actualizacion de variables de estado
if (sensores.superficie[0] == 'D') {
 tiene_zapatillas = true;
}

// Definicion del comportamiento
if (sensores.superficie[0] == 'U') { 
 return IDLE;
}

char i = ViablePorAlturaT (sensores.superficie[1], sensores.cota[1]-sensores.cota[0]);
char c = ViablePorAlturaT (sensores.superficie[2], sensores.cota[2]-sensores.cota[0]);
char d = ViablePorAlturaT (sensores.superficie[3], sensores.cota[3]-sensores.cota[0]);

char a_c = sensores.agentes[2];

int pos = VeoCasillaInteresanteT(i, c, d, a_c);
switch(pos)
{
  case 2:
    accion = WALK;
    break;
  case 1:
    accion = TURN_SL;
    break;
  case 3:
    accion = TURN_SR;
    break;
  default:
    accion = TURN_SL;
    break;
}


last_action = accion; 
return accion;
}


/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoTecnico::es_camino(unsigned char c) const {
  return (c == 'C' || c == 'D' || c == 'U');
}


/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */


int CasillaAExplorarT(char i, char c, char d, char a_i, char a_c, char a_d, bool i_v, bool c_v, bool d_v) {
    if (a_c != '_') return 0;

    bool c_es_via = (c == 'C' || c == 'S');
    bool i_es_via = (i == 'C' || i == 'S');
    bool d_es_via = (d == 'C' || d == 'S');

    if (c_es_via && !c_v) return 2;
    else if (i_es_via && !i_v && a_i == '_') return 1;
    else if (d_es_via && !d_v && a_d == '_') return 3;

    if (c == 'H' && !c_v) return 2;
    else if (i == 'H' && !i_v && a_i == '_') return 1;
    else if (d == 'H' && !d_v && a_d == '_') return 3;

    if (c_es_via) return 2;
    else if (i_es_via && a_i == '_') return 1;
    else if (d_es_via && a_d == '_') return 3;

    if (c == 'H') return 2;
    else if (i == 'H' && a_i == '_') return 1;
    else if (d == 'H' && a_d == '_') return 3;

    return 0;
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores) {
  Action accion = IDLE;
  ActualizarMapa(sensores);

  visitados.insert({sensores.posF, sensores.posC});

  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  char a_i = sensores.agentes[1];
  char a_c = sensores.agentes[2];
  char a_d = sensores.agentes[3];

  ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
  ubicacion c_pos = Delante(actual);
  
  ubicacion izq_orient = actual;
  // %8 porque no existe 8 en nuestra brujula
  izq_orient.brujula = (Orientacion)((izq_orient.brujula + 7) % 8); 
  ubicacion i_pos = Delante(izq_orient);
  
  ubicacion der_orient = actual;
  der_orient.brujula = (Orientacion)((der_orient.brujula + 1) % 8);
  ubicacion d_pos = Delante(der_orient);

  bool i_v = visitados.count({i_pos.f, i_pos.c}) > 0;
  bool c_v = visitados.count({c_pos.f, c_pos.c}) > 0;
  bool d_v = visitados.count({d_pos.f, d_pos.c}) > 0;

  int pos = CasillaAExplorarT(i, c, d, a_i, a_c, a_d, i_v, c_v, d_v);

  switch (pos) {
    case 2: accion = WALK; break;
    case 1: accion = TURN_SL; break;
    case 3: accion = TURN_SR; break;
    default:
      accion = TURN_SR;  // gira a la derecha pq el ingeniero va a la izquierda , y asi maximizamos el numero de casillas descubiertas
      break;
  }

  last_action = accion;
  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores) {
// El Técnico no necesita planificar nada en este nivel.
// Simplemente reutilizamos su instinto de exploración del Nivel 1 para que se aleje de la zona inicial y deje paso libre al Ingeniero. 
return ComportamientoTecnicoNivel_1(sensores);
}

// =====================================================================
// ESTRUCTURAS, BFS Y NIVEL E  (Tutorial2_p2_2025_26.pdf)
// Base para los Niveles 2 y 3 reales de la práctica.
// =====================================================================

// Siguiendo el tutorial, el estilo del profesor (Tutorial2_p2_2025_26.pdf)
// Costes energéticos exactos según Guion2_2025_26.pdf, Sección 2.3.5
// =====================================================================

// Estado del Técnico: posición + orientación + si lleva zapatillas
struct EstadoT {
    ubicacion site;
    bool zapatillas;

    bool operator==(const EstadoT &st) const {
        return site == st.site && zapatillas == st.zapatillas;
    }
    bool operator<(const EstadoT &st) const {
        if (site.f != st.site.f) return site.f < st.site.f;
        if (site.c != st.site.c) return site.c < st.site.c;
        if (site.brujula != st.site.brujula) return site.brujula < st.site.brujula;
        return zapatillas < st.zapatillas;
    }
};

// Nodo para BFS/A*: estado + secuencia de acciones + costes g y f=g+h
struct NodoT {
    EstadoT estado;
    list<Action> secuencia;
    float g;        // coste acumulado real
    float f;        // g + h (prioridad)

    bool operator==(const NodoT &node) const {
        return estado == node.estado;
    }
    // operator< para std::set<NodoT> (BFS explorados)
    bool operator<(const NodoT &node) const {
        return estado < node.estado;
    }
    // operator> para priority_queue (min-heap por f)
    bool operator>(const NodoT &node) const {
        return f > node.f;
    }
};

// Calcula la casilla a la que llegaría el Técnico al avanzar
EstadoT NextCasillaTecnico(const EstadoT &st) {
    EstadoT siguiente = st;
    switch (st.site.brujula) {
        case norte:    siguiente.site.f--;                    break;
        case noreste:  siguiente.site.f--; siguiente.site.c++; break;
        case este:                         siguiente.site.c++; break;
        case sureste:  siguiente.site.f++; siguiente.site.c++; break;
        case sur:      siguiente.site.f++;                    break;
        case suroeste: siguiente.site.f++; siguiente.site.c--; break;
        case oeste:                        siguiente.site.c--; break;
        case noroeste: siguiente.site.f--; siguiente.site.c--; break;
    }
    return siguiente;
}

// Comprueba si el Técnico puede hacer WALK desde st.
// Reglas (del profesor + guion):
//   - No puede pisar 'P' ni 'M'
//   - No puede pisar 'B' sin zapatillas
//   - Desnivel máximo 1 (zapatillas no amplían altura para el Técnico)
bool CasillaAccesibleTecnico(const EstadoT &st,
                              const vector<vector<unsigned char>> &terreno,
                              const vector<vector<unsigned char>> &altura) {
    EstadoT next = NextCasillaTecnico(st);
    // Comprobación de límites (mapa cerrado con precipicios, pero por seguridad)
    if (next.site.f < 0 || next.site.f >= (int)terreno.size() ||
        next.site.c < 0 || next.site.c >= (int)terreno[0].size())
        return false;
    char t = terreno[next.site.f][next.site.c];
    bool no_obstaculo   = (t != 'P' && t != 'M');
    bool bosque_ok      = (t != 'B' || st.zapatillas);
    bool desnivel_ok    = abs((int)altura[next.site.f][next.site.c] -
                              (int)altura[st.site.f][st.site.c]) <= 1;
    return no_obstaculo && bosque_ok && desnivel_ok;
}


// Aplica una acción al estado y devuelve el estado resultante.
// Si la acción no es aplicable (WALK bloqueado), devuelve el mismo estado.
EstadoT applyTecnico(Action accion, const EstadoT &st,
                     const vector<vector<unsigned char>> &terreno,
                     const vector<vector<unsigned char>> &altura) {
    EstadoT next = st;
    switch (accion) {
        case WALK:
            if (CasillaAccesibleTecnico(st, terreno, altura)) {
                next = NextCasillaTecnico(st);
                // Recoger zapatillas si la casilla destino es 'D'
                if (terreno[next.site.f][next.site.c] == 'D')
                    next.zapatillas = true;
            }
            break;
        case TURN_SR:
            next.site.brujula = (Orientacion)((next.site.brujula + 1) % 8);
            break;
        case TURN_SL:
            next.site.brujula = (Orientacion)((next.site.brujula + 7) % 8);
            break;
        default: break;
    }
    return next;
}

// Coste de giro (TURN_SL / TURN_SR): depende del terreno ORIGEN.
// Guion Tabla TURN: A=5, H=2, S=1, Resto=1.
float costeTurnoT(char terreno_origen) {
    switch (terreno_origen) {
        case 'A': return 5.0f;
        case 'H': return 2.0f;
        default:  return 1.0f;
    }
}

// Coste de WALK: depende del terreno ORIGEN y la diferencia de cota.
// Guion Tabla WALK:
//   A  → base 60,  ajuste +5 sube / -2 baja
//   H  → base  6,  ajuste +5 sube / -2 baja
//   S  → base  3,  ajuste +5 sube / -2 baja
//   Resto (C,D,U,B...) → base 1,  SIN ajuste de altura (+0/-0)
float costeWalkT(char terreno_origen, int cota_orig, int cota_dest) {
    float ajuste = 0.0f;
    switch (terreno_origen) {
        case 'A':
            ajuste = (cota_dest > cota_orig) ? 5.0f : (cota_dest < cota_orig ? -2.0f : 0.0f);
            return 60.0f + ajuste;
        case 'H':
            ajuste = (cota_dest > cota_orig) ? 5.0f : (cota_dest < cota_orig ? -2.0f : 0.0f);
            return 6.0f + ajuste;
        case 'S':
            ajuste = (cota_dest > cota_orig) ? 5.0f : (cota_dest < cota_orig ? -2.0f : 0.0f);
            return 3.0f + ajuste;
        default:  // C, D, U, B (con zapatillas) — sin ajuste de altura
            return 1.0f;
    }
}

// Heurística admisible: distancia Chebyshev × coste mínimo por paso (1).
// Todos los costes son >= 1 (mínimo: terreno Resto=1, S bajando = 3-2 = 1).
float heuristicaT(const EstadoT &st, const EstadoT &fin) {
    return (float)max(abs(st.site.f - fin.site.f),
                      abs(st.site.c - fin.site.c));
}

// ── BFS en Anchura V2 (Nivel E) ──────────────────────────────────────────────
// Búsqueda en anchura optimizada: frontier como list<NodoT> (FIFO),
// explored como set<NodoT> (búsqueda en O(log n)).
// Minimiza número de acciones (instantes), no energía.
list<Action> B_Anchura_V2(const EstadoT &inicio, const EstadoT &fin,
                           const vector<vector<unsigned char>> &terreno,
                           const vector<vector<unsigned char>> &altura) {

    NodoT current_node;
    current_node.estado    = inicio;
    current_node.secuencia = {};
    current_node.g         = 0.0f;
    current_node.f         = 0.0f;

    list<NodoT> frontier;
    set<NodoT>  explored;

    bool SolutionFound = (inicio.site.f == fin.site.f &&
                          inicio.site.c == fin.site.c);

    frontier.push_back(current_node);

    while (!frontier.empty() && !SolutionFound) {

        frontier.pop_front();
        explored.insert(current_node);

        // Hijo WALK — único que puede alcanzar la solución
        if (!SolutionFound) {
            NodoT child_Walk = current_node;
            EstadoT next = applyTecnico(WALK, current_node.estado, terreno, altura);
            if (!(next == current_node.estado)) {         // WALK fue aplicable
                child_Walk.estado = next;
                if (child_Walk.estado.site.f == fin.site.f &&
                    child_Walk.estado.site.c == fin.site.c) {
                    SolutionFound = true;
                    child_Walk.secuencia.push_back(WALK);
                    current_node = child_Walk;
                } else if (explored.find(child_Walk) == explored.end()) {
                    child_Walk.secuencia.push_back(WALK);
                    frontier.push_back(child_Walk);
                }
            }
        }

        // Hijo TURN_SR
        if (!SolutionFound) {
            NodoT child_SR = current_node;
            child_SR.estado = applyTecnico(TURN_SR, current_node.estado, terreno, altura);
            if (explored.find(child_SR) == explored.end()) {
                child_SR.secuencia.push_back(TURN_SR);
                frontier.push_back(child_SR);
            }
        }

        // Hijo TURN_SL
        if (!SolutionFound) {
            NodoT child_SL = current_node;
            child_SL.estado = applyTecnico(TURN_SL, current_node.estado, terreno, altura);
            if (explored.find(child_SL) == explored.end()) {
                child_SL.secuencia.push_back(TURN_SL);
                frontier.push_back(child_SL);
            }
        }

        // Tomar siguiente nodo no explorado de frontier
        if (!SolutionFound && !frontier.empty()) {
            current_node = frontier.front();
            while (!frontier.empty() &&
                   explored.find(current_node) != explored.end()) {
                frontier.pop_front();
                if (!frontier.empty())
                    current_node = frontier.front();
            }
        }
    }

    if (SolutionFound)
        return current_node.secuencia;
    return list<Action>();
}

/**
 * @brief Nivel E (ficticio del tutorial): BFS en anchura para llevar al
 * Técnico a la Belkanita minimizando instantes de simulación.
 * Sirve de base y demostración para los Niveles 2 y 3 reales.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_E(Sensores sensores) {
    Action accion = IDLE;

    if (!hayPlan) {
        EstadoT inicio, fin;
        inicio.site.f      = sensores.posF;
        inicio.site.c      = sensores.posC;
        inicio.site.brujula = sensores.rumbo;
        inicio.zapatillas  = tiene_zapatillas;

        fin.site.f       = sensores.BelPosF;
        fin.site.c       = sensores.BelPosC;
        fin.site.brujula = norte;   // irrelevante para condición de parada
        fin.zapatillas   = false;

        plan = B_Anchura_V2(inicio, fin, mapaResultado, mapaCotas);
        VisualizaPlan(inicio.site, plan);
        hayPlan = (plan.size() != 0);
    }

    if (hayPlan && plan.size() > 0) {
        accion = plan.front();
        plan.pop_front();
    }
    if (plan.size() == 0) {
        hayPlan = false;
    }

    return accion;
}

// Algoritmo A* para el Técnico (minimiza energía).
// Usa priority_queue (min-heap por f=g+h) como frontera y
// set<EstadoT> como explorados — almacena solo estados, no secuencias,
// para evitar consumo masivo de memoria en mapas grandes.
// El mapa es completamente conocido (Nivel 3).
// ── ESTRUCTURA OPTIMIZADA PARA A* (Evita límite de tiempo y empates) ──────────────
struct NodoAEstrella {
    EstadoT estado;
    float g;
    float f;

    bool operator>(const NodoAEstrella &node) const {
        if (f == node.f) {
            // TIE-BREAKER: Si hay empate en 'f', preferimos el nodo con mayor 'g'.
            // Mayor 'g' significa que hemos avanzado más pasos (menor 'h') y estamos más cerca de la meta.
            return g < node.g; 
        }
        return f > node.f; // min-heap para priority_queue
    }
};

// Algoritmo A* para el Técnico optimizado (minimiza energía).
// Utiliza matrices 1D aplanadas en lugar de 4D para máxima velocidad de caché.
list<Action> AEstrella_Nivel3(const EstadoT &inicio, const EstadoT &fin,
                              const vector<vector<unsigned char>> &terreno,
                              const vector<vector<unsigned char>> &altura) {

    int filas = terreno.size();
    int columnas = terreno[0].size();

    // Aplanamiento: Fila * Columna * Orientacion(8) * Zapatillas(2)
    int MAX_STATES = filas * columnas * 8 * 2;
    
    // Una única reserva de memoria ultra-rápida (en lugar de miles de sub-vectores)
    vector<float> g_score(MAX_STATES, 1e9f);
    vector<pair<EstadoT, Action>> came_from(MAX_STATES);

    // Función lambda para obtener el índice lineal 1D
    auto get_index = [&](int f, int c, int b, int z) {
        return (((f * columnas) + c) * 8 + b) * 2 + z;
    };

    priority_queue<NodoAEstrella, vector<NodoAEstrella>, greater<NodoAEstrella>> frontera;

    NodoAEstrella nodo_inicio;
    nodo_inicio.estado = inicio;
    nodo_inicio.g = 0.0f;
    nodo_inicio.f = heuristicaT(inicio, fin);
    
    frontera.push(nodo_inicio);
    
    int start_idx = get_index(inicio.site.f, inicio.site.c, inicio.site.brujula, inicio.zapatillas ? 1 : 0);
    g_score[start_idx] = 0.0f;

    while (!frontera.empty()) {
        NodoAEstrella current = frontera.top();
        frontera.pop();

        int f = current.estado.site.f;
        int c = current.estado.site.c;
        int b = current.estado.site.brujula;
        int z = current.estado.zapatillas ? 1 : 0;
        int curr_idx = get_index(f, c, b, z);

        // Si ya habíamos encontrado una ruta más barata hacia este estado, descartamos rama
        if (current.g > g_score[curr_idx])
            continue;

        // Condición de éxito: Llegamos a la Belkanita
        if (f == fin.site.f && c == fin.site.c) {
            list<Action> path;
            EstadoT curr_state = current.estado;
            
            // Reconstruimos el camino desde el final hasta el inicio
            while (!(curr_state == inicio)) {
                int cf = curr_state.site.f;
                int cc = curr_state.site.c;
                int cb = curr_state.site.brujula;
                int cz = curr_state.zapatillas ? 1 : 0;
                int c_idx = get_index(cf, cc, cb, cz);

                auto prev = came_from[c_idx];
                path.push_front(prev.second);
                curr_state = prev.first;
            }
            return path;
        }

        char t_orig = terreno[f][c];
        int h_orig = (int)altura[f][c];

        // Expansión de vecinos
        Action acciones[] = {TURN_SL, TURN_SR, WALK};
        for (Action act : acciones) {
            EstadoT next_state = current.estado;
            float transition_cost = 0.0f;

            if (act == TURN_SL || act == TURN_SR) {
                next_state = applyTecnico(act, current.estado, terreno, altura);
                transition_cost = costeTurnoT(t_orig);
            } else if (act == WALK) {
                if (CasillaAccesibleTecnico(current.estado, terreno, altura)) {
                    next_state = NextCasillaTecnico(current.estado);
                    // Regla: si pisa la casilla 'D', adquiere zapatillas
                    if (terreno[next_state.site.f][next_state.site.c] == 'D') {
                        next_state.zapatillas = true;
                    }
                    int h_dest = (int)altura[next_state.site.f][next_state.site.c];
                    
                    // Solo aplica la lógica de altura (aumento o penalización) a la base del coste 
                    transition_cost = costeWalkT(t_orig, h_orig, h_dest); 
                } else {
                    continue; // Obstáculo, desnivel > 1 o precipicio
                }
            }

            float tentative_g = current.g + transition_cost;

            int nf = next_state.site.f;
            int nc = next_state.site.c;
            int nb = next_state.site.brujula;
            int nz = next_state.zapatillas ? 1 : 0;
            int next_idx = get_index(nf, nc, nb, nz);

            // Si descubrimos una ruta más barata al estado siguiente, la actualizamos
            if (tentative_g < g_score[next_idx]) {
                g_score[next_idx] = tentative_g;
                came_from[next_idx] = {current.estado, act};

                NodoAEstrella child;
                child.estado = next_state;
                child.g = tentative_g;
                child.f = tentative_g + heuristicaT(next_state, fin);
                frontera.push(child);
            }
        }
    }

    return list<Action>(); // Sin solución (A* exploró todo y no encontró la meta)
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * El Técnico es el agente deliberativo: usa A* para llegar a la Belkanita
 * minimizando el consumo de energía según los costes reales del guion.
 * El mapa es completamente conocido (constructor con mapa).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
    Action accion = IDLE;

    // Actualizar zapatillas si el agente acaba de pisarlas
    if (sensores.superficie[0] == 'D')
        tiene_zapatillas = true;

    // Detectar desincronización: si la posición real no coincide con la esperada
    // (ocurre cuando otro agente bloqueó nuestro WALK el turno anterior)
    if (hayPlan && checkPos) {
        if (sensores.posF != posEsperada.f ||
            sensores.posC != posEsperada.c ||
            sensores.rumbo != posEsperada.brujula) {
            hayPlan = false;  // replanificar desde la posición real
        }
    }
    checkPos = false;

    if (!hayPlan) {
        EstadoT inicio, fin;
        inicio.site.f       = sensores.posF;
        inicio.site.c       = sensores.posC;
        inicio.site.brujula = sensores.rumbo;
        inicio.zapatillas   = tiene_zapatillas;

        fin.site.f       = sensores.BelPosF;
        fin.site.c       = sensores.BelPosC;
        fin.site.brujula = norte;
        fin.zapatillas   = false;

        plan = AEstrella_Nivel3(inicio, fin, mapaResultado, mapaCotas);
        VisualizaPlan(inicio.site, plan);
        hayPlan = (plan.size() != 0);
    }

    if (hayPlan && plan.size() > 0) {
        accion = plan.front();

        // Si la próxima acción es WALK pero hay un agente directamente delante,
        // esperamos (IDLE) sin consumir la acción del plan.
        // El Ingeniero (reactivo) eventualmente se moverá y podremos avanzar.
        if (accion == WALK && sensores.agentes[2] != '_') {
            accion = IDLE;
            checkPos = false;
            last_action = accion;
            return accion;
        }
  
        if (accion == WALK && sensores.agentes[2] != '_') {
            accion = IDLE;
            checkPos = false; // Detenemos la alerta temporalmente para NO replanificar.
            last_action = accion;
            return accion;
        }
        plan.pop_front();


        // Calcular posición esperada tras ejecutar esta acción
        ubicacion cur = {sensores.posF, sensores.posC, sensores.rumbo};
        checkPos = true;
        switch (accion) {
            case TURN_SL:
                posEsperada = {cur.f, cur.c, (Orientacion)((cur.brujula + 7) % 8)};
                break;
            case TURN_SR:
                posEsperada = {cur.f, cur.c, (Orientacion)((cur.brujula + 1) % 8)};
                break;
            case WALK:
                posEsperada = Delante(cur);
                break;
            default:
                checkPos = false;
                break;
        }
    }
    if (plan.size() == 0)
        hayPlan = false;

    last_action = accion;
    return accion;
}



/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}

int ObtenerRumboT(int f_act, int c_act, int f_obj, int c_obj) {
    if (f_act == f_obj && c_obj > c_act) return 2; 
    if (f_act == f_obj && c_obj < c_act) return 6; 
    if (f_obj > f_act && c_act == c_obj) return 4; 
    if (f_obj < f_act && c_act == c_obj) return 0; 
    return -1; 
}

Action GirarHaciaT(int rumbo_actual, int rumbo_objetivo) {
    int diff = (rumbo_objetivo - rumbo_actual + 8) % 8;
    if (diff == 0) return IDLE;
    if (diff <= 4) return TURN_SR;
    return TURN_SL;
}

// Clonamos las estructuras de tubería para el Técnico
struct NodoTuberiaT {
    int f, c, altura_actual;
    list<Paso> red;
};

struct EstadoTuberiaT {
    int f, c, alteracion;
    bool operator<(const EstadoTuberiaT& otro) const {
        return tie(f, c, alteracion) < tie(otro.f, otro.c, otro.alteracion);
    }
};

// Generador del plano (BFS idéntico al del Ingeniero)
list<Paso> ProyectarRedTuberiasTecnico(int inicio_f, int inicio_c, 
                                const vector<vector<unsigned char>>& mapaR, 
                                const vector<vector<unsigned char>>& mapaC) {
    queue<NodoTuberiaT> frontera;
    set<EstadoTuberiaT> visitados;
    int altura_inicial = mapaC[inicio_f][inicio_c];
    int ops[] = {0, 1, -1};
    
    for (int op : ops) {
        if (op == -1 && altura_inicial <= 1) continue;
        if (op == 1 && altura_inicial >= 9) continue;
        if (op != 0 && mapaR[inicio_f][inicio_c] == 'A') continue;
        
        NodoTuberiaT inicial = {inicio_f, inicio_c, altura_inicial + op, {{inicio_f, inicio_c, op}}};
        frontera.push(inicial);
        visitados.insert({inicio_f, inicio_c, op});
    }

    int df[] = {-1, 1, 0, 0}, dc[] = {0, 0, 1, -1};

    while (!frontera.empty()) {
        NodoTuberiaT actual = frontera.front();
        frontera.pop();

        if (mapaR[actual.f][actual.c] == 'U') return actual.red;

        for (int i = 0; i < 4; ++i) {
            int nf = actual.f + df[i], nc = actual.c + dc[i];
            if (nf >= 0 && nf < mapaR.size() && nc >= 0 && nc < mapaR[0].size()) {
                char t = mapaR[nf][nc];
                if (t != 'M' && t != 'P') {
                    int c_orig = mapaC[nf][nc];
                    for (int op : ops) {
                        if (op == -1 && c_orig <= 1) continue;
                        if (op == 1 && c_orig >= 9) continue;
                        if (op != 0 && t == 'A') continue;
                        
                        int c_final = c_orig + op;
                        int caida = actual.altura_actual - c_final;
                        if (caida == 0 || caida == 1) {
                            EstadoTuberiaT n_st = {nf, nc, op};
                            if (visitados.find(n_st) == visitados.end()) {
                                visitados.insert(n_st);
                                NodoTuberiaT vecino = {nf, nc, c_final, actual.red};
                                vecino.red.push_back({nf, nc, op});
                                frontera.push(vecino);
                            }
                        }
                    }
                }
            }
        }
    }
    return list<Paso>(); 
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores)
{
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (!hayPlan) {
        list<Paso> plan_lista = ProyectarRedTuberiasTecnico(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas);
        red_tuberias.assign(plan_lista.begin(), plan_lista.end());
        tramo_actual = 0;
        hayPlan = true;
        plan.clear();
    }

    if (tramo_actual >= (int)red_tuberias.size() - 1) return IDLE;

    // Objetivo del Técnico: Ocupar el inicio del tramo (nodo i)
    int target_f = red_tuberias[tramo_actual].fil;
    int target_c = red_tuberias[tramo_actual].col;

    // ¿ESTAMOS EN POSICIÓN?
    if (sensores.posF == target_f && sensores.posC == target_c) {
        plan.clear();
        
        // A. Rotar para encarar al Ingeniero (que debe estar en el nodo i + 1)
        int f_ing = red_tuberias[tramo_actual + 1].fil;
        int c_ing = red_tuberias[tramo_actual + 1].col;
        int rumbo_obj = ObtenerRumboT(sensores.posF, sensores.posC, f_ing, c_ing);
        
        if (sensores.rumbo != rumbo_obj) {
            return GirarHaciaT(sensores.rumbo, rumbo_obj);
        }

        // B. Sincronización
        // Si el Ingeniero llegó, hizo sus ajustes de tierra y nos mira, ejecutamos a la vez.
        if (sensores.enfrente) {
            tramo_actual++; 
            return INSTALL;
        }
        
        return IDLE;
    } 
    else {
        // NAVEGACIÓN AL OBJETIVO
        if (plan.empty() || sensores.choque) {
            EstadoT inicio = { {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo}, tiene_zapatillas };
            EstadoT fin = { {target_f, target_c, norte}, false }; 
            plan = AEstrella_Nivel3(inicio, fin, mapaResultado, mapaCotas);
        }

        if (!plan.empty()) {
            Action accion = plan.front();
            // Escudo anti-atascos
            if (accion == WALK && sensores.agentes[2] != '_') return IDLE;
            plan.pop_front();
            return accion;
        }
    }
    return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores) {
  return IDLE;
}




// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores) {
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo) {
    case norte:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
          mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
        }
      break;
    case noreste:
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
      mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
      mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
      mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
      mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
      mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
      mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
      mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
      mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
      break;
    case este:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
          mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
        }
      break;
    case sureste:
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
      mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
      mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
      mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
      mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
      mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
      mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
      mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
      mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
      mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
      mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
      break;
    case sur:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
          mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
        }
      break;
    case suroeste:
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
      mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
      mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
      mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
      mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
      mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
      mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
      mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
      mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
      break;
    case oeste:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
          mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
        }
      break;
    case noroeste:
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
      mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
      mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
      mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
      mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
      mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
      mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
      mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
      mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
      mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
      mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
      break;
  }
}



/**
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas) {
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
  return es_camino(mapaResultado[f][c]);  // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual) {
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size()) return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1) return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const {
  ubicacion delante = actual;
  switch (actual.brujula) {
    case 0: delante.f--; break;                        // norte
    case 1: delante.f--; delante.c++; break;     // noreste
    case 2: delante.c++; break;                     // este
    case 3: delante.f++; delante.c++; break;     // sureste
    case 4: delante.f++; break;                        // sur
    case 5: delante.f++; delante.c--; break;     // suroeste
    case 6: delante.c--; break;                     // oeste
    case 7: delante.f--; delante.c--; break;     // noroeste
  }
  return delante;
}


/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}



/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
   listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}


