#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <set>
#include <unordered_set>
#include <iterator>
#include <ctime>
#include <mutex>
#include <thread>
#include <map>

#define MAXCIRCUITOS 400000000

std::mutex mtxGenerador;
unsigned int circuitosGenerados = 0;
bool pararAlgoritmo = false;
unsigned int ultimoValorComputadasMillon = 0;
std::unordered_set<unsigned int> funcionesComputadas;
std::ofstream fsPorcentajes("porcentajesFuncionesDiferentes" + std::to_string(MAXCIRCUITOS / 1000000) + ".txt");

#pragma pack (2)
struct circuito {
	circuito* padreIzq;
	circuito* padreDer;
	short tam;
	circuito() {}
	circuito(circuito* padreIzq, circuito* padreDer, short tam) {
		this->padreIzq = padreIzq;
		this->padreDer = padreDer;
		this->tam = tam;
	}
};

#pragma pack (2)
struct circuitoCable : public circuito {
	unsigned int eval;
	circuitoCable(circuito* padreIzq, circuito* padreDer, unsigned int eval, short tam) {
		this->padreIzq = padreIzq;
		this->padreDer = padreDer;
		this->eval = eval;
		this->tam = tam;
	}
};

// Mapa de tamaños a circuitos de ese tamaño calculados que deben ser anotados
std::map<short, std::vector<circuito*>> espera;

// Mapa de funciones computadas a vector de circuitos que la computan (de izquierda a derecha creciente de tamaño)
std::unordered_map<unsigned int, std::vector<circuito*>> almacen;


unsigned int evaluacion(circuito* circ) {
	if (circ->padreIzq == nullptr && circ->padreDer == nullptr) {
		circuitoCable* cable = (circuitoCable*)circ;
		return cable->eval;
	}
	else return ~(evaluacion(circ->padreIzq) & evaluacion(circ->padreDer));
}


void introduceEnSet(std::set<circuito*>& set, circuito* circ) {
	if (circ != nullptr) {
		set.insert(circ);
		introduceEnSet(set, circ->padreIzq);
		introduceEnSet(set, circ->padreDer);
	}
}

void mezclar(circuito* circuito1, circuito* circuito2, short& tam) {

	std::set<circuito*> set1, set2;
	introduceEnSet(set1, circuito1);
	introduceEnSet(set2, circuito2);

	std::set<circuito*> intersect;
	std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersect, intersect.begin()));

	for (circuito* circ : intersect) {
		if (circ->tam > 0) tam--;
	}

}

// Funcion que totalmente ad-hoc pone los circuitos detamaño 0 (las entradas) con sus correspondientes evaluaciones 
void inicializa() {
	espera[0] = std::vector<circuito*>();

	circuito* circ = new circuitoCable(nullptr, nullptr, 2863311530, 0);
	espera[0].push_back(circ);

	circ = new circuitoCable(nullptr, nullptr, 3435973836, 0);
	espera[0].push_back(circ);

	circ = new circuitoCable(nullptr, nullptr, 4042322160, 0);
	espera[0].push_back(circ);

	circ = new circuitoCable(nullptr, nullptr, 4278255360, 0);
	espera[0].push_back(circ);

	circ = new circuitoCable(nullptr, nullptr, 4294901760, 0);
	espera[0].push_back(circ);

}



void tareaGeneraCircuitos(short n, size_t ini, size_t fin) {
	for (size_t i = ini; i < fin; ++i) {
		circuito* circuito1 = espera[n][i];

		// Cruzo el circuito con todos los de su tamaño con los que no se haya cruzado ya
		for (size_t j = i + 1; j < espera[n].size(); ++j) {
			circuito* circuito2 = espera[n][j];
			short tam = circuito1->tam + circuito2->tam + 1;
			mezclar(circuito1, circuito2, tam);
			mtxGenerador.lock();
			if (!espera.count(tam)) espera[tam] = std::vector<circuito*>();
			circuito* circ = new circuito(circuito1, circuito2, tam);
			espera[tam].push_back(circ);
			circuitosGenerados++;
			unsigned int eval = evaluacion(circ);
			if (!funcionesComputadas.count(eval)) {
				funcionesComputadas.insert(eval);
			}
			if (pararAlgoritmo) {
				mtxGenerador.unlock();
				return;
			}
			else if (circuitosGenerados == MAXCIRCUITOS) {
				pararAlgoritmo = true;
				mtxGenerador.unlock();
				return;
			}
			else if (circuitosGenerados % 1000000 == 0) {
				std::cout << circuitosGenerados / 1000000 << '\n';
				unsigned int computadasNuevas = funcionesComputadas.size() - ultimoValorComputadasMillon;
				ultimoValorComputadasMillon = funcionesComputadas.size();
				std::cout << computadasNuevas << '\n';
				fsPorcentajes << computadasNuevas << '\n';
				mtxGenerador.unlock();
			}
			else mtxGenerador.unlock();
		}

		// Cruzo el circuito con todos los de tamaños menores
		for (auto par2 : almacen) {
			for (circuito* circuito2 : par2.second) {
				if (circuito1 == circuito2) break;
				short tam = circuito1->tam + circuito2->tam + 1;
				mezclar(circuito1, circuito2, tam);
				mtxGenerador.lock();
				if (!espera.count(tam)) espera[tam] = std::vector<circuito*>();
				circuito* circ = new circuito(circuito1, circuito2, tam);
				espera[tam].push_back(circ);
				circuitosGenerados++;
				unsigned int eval = evaluacion(circ);
				if (!funcionesComputadas.count(eval)) {
					funcionesComputadas.insert(eval);
				}
				if (pararAlgoritmo) {
					mtxGenerador.unlock();
					return;
				}
				else if (circuitosGenerados == MAXCIRCUITOS) {
					pararAlgoritmo = true;
					mtxGenerador.unlock();
					return;
				}
				else if (circuitosGenerados % 1000000 == 0) {
					std::cout << circuitosGenerados / 1000000 << '\n';
					float porcentaje = (float)funcionesComputadas.size() / (float)circuitosGenerados * 100;
					std::cout << porcentaje << '\n';
					fsPorcentajes << porcentaje << '\n';
					mtxGenerador.unlock();
				}
				else mtxGenerador.unlock();
			}
		}
	}
}


void generaCircuitos(short n) {
	if (!espera.count(n)) return;
	size_t numProcesadores = std::thread::hardware_concurrency();
	size_t tamProceso = (espera[n].size() / numProcesadores);
	std::vector<std::thread> hilos(numProcesadores);
	for (size_t i = 0; i < numProcesadores; ++i) {
		if (i < numProcesadores - 1) hilos[i] = std::thread(tareaGeneraCircuitos, n, i * tamProceso, (i + 1) * tamProceso);
		else hilos[i] = std::thread(tareaGeneraCircuitos, n, i * tamProceso, espera[n].size());
	}

	for (size_t i = 0; i < numProcesadores; ++i) {
		hilos[i].join();
	}

}

void anotaCircuitos(short n) {
	if (!espera.count(n)) return;
	for (circuito* circ : espera[n]) {
		unsigned int eval = evaluacion(circ);
		if (!almacen.count(eval)) {
			almacen[eval] = std::vector<circuito*>();
		}
		almacen[eval].push_back(circ);
	}
	std::cout << "Size " << n << " anotado\n";
}

inline void quitaCircuitosEspera(short n) {
	espera[n].clear();
	espera.erase(n);
}


inline void liberaAlmacen() {
	for (auto par : almacen) {
		for (circuito* circ : par.second) {
			delete circ;
		}
		par.second.clear();
	}
	almacen.clear();
}

inline void liberaEspera() {
	for (auto par : espera) {
		for (circuito* circ : par.second) {
			delete circ;
		}
	}
	espera.clear();
}


int main() {
	unsigned t0, t1;
	t0 = clock();
	inicializa();
	int tamano = 0;
	while (!pararAlgoritmo) {
		generaCircuitos(tamano);
		anotaCircuitos(tamano);
		quitaCircuitosEspera(tamano);
		tamano++;
	}
	//liberaEspera();
	//liberaAlmacen();
	t1 = clock();
	double time = (double(t1 - t0) / CLOCKS_PER_SEC);
	fsPorcentajes << "Tiempo de ejecucion: " << time << "\n";
	fsPorcentajes.close();
	return 0;
}