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

#define MAXCIRCUITOS 500000000

std::mutex mtxGenerador;
std::ofstream fs("ficheroSalida" + std::to_string(MAXCIRCUITOS) + "NuevaRep.txt");
unsigned int circuitosGenerados = 0;
bool pararAlgoritmo = false;


#pragma pack (2)
struct circuito {
	circuito* padreIzq;
	circuito* padreDer;
	short tam;
	circuito(){}
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

// Pinta el circuito como árbol binario
unsigned int printBT(std::ostream& o, const std::string& prefix, circuito const* circ, bool isRight)
{
		o << prefix;

		o << "|--";

		// Mostramos el cable o la puerta que es
		if (circ->padreIzq == nullptr && circ->padreDer == nullptr) {
			circuitoCable* cable = (circuitoCable*)circ;
			if (cable->eval == 2863311530) o << "X0\n";
			else if (cable->eval == 3435973836) o << "X1\n";
			else if (cable->eval == 4042322160) o << "X2\n";
			else if (cable->eval == 4278255360) o << "X3\n";
			else if (cable->eval == 4294901760) o << "X4\n";
			return cable->eval;
		}
		else {
			o << "NAND\n";
			unsigned int evalDer = printBT(o, prefix + (isRight ? "|   " : "    "), circ->padreDer, true);
			unsigned int evalIzq = printBT(o, prefix + (isRight ? "|   " : "    "), circ->padreIzq, false);
			return ~(evalIzq & evalDer);
		}

}

void bin(std::ostream& o, unsigned int n)
{
	unsigned int num = n;
	for (int i = 0; i < 32; ++i) {
		o << (num & 1);
		num = num >> 1;
	}
}

void muestraCircuitoDetalle(std::ostream& o, circuito* const& circ) {
	o << "Size: " << circ->tam;
	o << "\n";
	unsigned int eval = printBT(o, "", circ, false);
	o << " Evaluacion: ";
	bin(o, evaluacion(circ));
	o << "\n\n";
}

unsigned int preordenRecursivo(std::ostream& o, circuito* const& circ) {
	if (circ->padreIzq == nullptr && circ->padreDer == nullptr) {
		circuitoCable* cable = (circuitoCable*) circ;
		if (cable->eval == 2863311530) o << '0';
		else if (cable->eval == 3435973836) o << '1';
		else if (cable->eval == 4042322160) o << '2';
		else if (cable->eval == 4278255360) o << '3';
		else if (cable->eval == 4294901760) o << '4';
		return cable->eval;
	}
	else {
		o << 'n';
		unsigned int evalIzq = preordenRecursivo(o, circ->padreIzq);
		unsigned int evalDer = preordenRecursivo(o, circ->padreDer);
		return ~(evalIzq & evalDer);
	}
}


std::ostream& operator<<(std::ostream& o, circuito* circ) {
	unsigned int eval = preordenRecursivo(o, circ);
	o << " " << circ->tam << " " << eval << '\n';
	return o;
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
			espera[tam].push_back(new circuito(circuito1, circuito2, tam));
			circuitosGenerados++;
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
				std::cout << circuitosGenerados << '\n';
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
				espera[tam].push_back(new circuito(circuito1, circuito2, tam));
				circuitosGenerados++;
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
					std::cout << circuitosGenerados << '\n';
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
			fs << circ;
		}
		almacen[eval].push_back(circ);
	}
	std::cout << "Size " << n << " anotado\n";

}

inline void quitaCircuitosEspera(short n) {
	espera[n].clear();
	espera.erase(n);
}


int anotaSobrantes() {
	// Cuando anotemos los sobrante aqui tendremos las nuevas funciones computadas.
	// Permite no escribir en el almacen lo que resultaría más caro
	std::unordered_set<unsigned int> computadasSobrantes;
	for (auto par : espera) {
		for (circuito* circ : par.second) {
			unsigned int eval = evaluacion(circ);
			if (!almacen.count(eval) && !computadasSobrantes.count(eval)) {
				computadasSobrantes.insert(eval);
				fs << circ;
			}
			delete circ;
		}
		std::cout << "Size " << par.first << " anotado\n";
	}
	return computadasSobrantes.size();
}


inline void muestraSobrantes() {
	for (auto par : espera) {
		std::cout << par.second.size() << " circuitos de tamano " << par.first << " por procesar\n";
	}
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
	std::cout << "Circuitos generados exhaustivamente hasta size " << tamano - 1 << '\n';
	muestraSobrantes();
	std::cout << "\n";
	std::cout << "Procesando... \n";
	int computadasSobrantes = anotaSobrantes();
	std::cout << "Total de funciones distintas computadas " << almacen.size() + computadasSobrantes << '\n';
	fs << "Total de funciones distintas computadas: " << almacen.size() + computadasSobrantes << '\n';
	liberaAlmacen();
	t1 = clock();
	double time = (double(t1 - t0) / CLOCKS_PER_SEC);
	fs << "Tiempo de ejecucion: " << time << "\n";
	fs.close();
	return 0;
}