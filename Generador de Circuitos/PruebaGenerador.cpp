#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <set>
#include <iterator>

#define MAXCIRCUITOS 1000000
#define REPERTORIOUSADO 'A'

std::ofstream fs("ficheroSalida" + std::to_string(MAXCIRCUITOS) + REPERTORIOUSADO + ".txt");
unsigned int circuitosGenerados = 0;

// Las entradas, por comodidad tambien se han metido con las puertas
enum puerta { AND, OR, NOT, NAND, NOR, XOR, XNOR, LONLY, RONLY, NLONLY, NRONLY, X0, X1, X2, X3, X4 };


std::vector<puerta> repertorioABinario = { NAND };
std::vector<puerta> repertorioAUnario = {};

std::vector<puerta> repertorioBBinario = { AND, OR };
std::vector<puerta> repertorioBUnario = { NOT };

std::vector<puerta> repertorioCBinario = { LONLY, RONLY, NLONLY, NRONLY, NOR, XOR, NAND, AND, XNOR, OR};
std::vector<puerta> repertorioCUnario = {NOT};



struct circuito {
	unsigned int eval;
	puerta gate;
	circuito* padreIzq;
	circuito* padreDer;
	int tam;
	int prof;
	circuito(puerta gate, circuito* padreIzq, circuito* padreDer, unsigned int eval, unsigned int tam, unsigned int prof) {
		this->gate = gate;
		this->padreIzq = padreIzq;
		this->padreDer = padreDer;
		this->eval = eval;
		this->tam = tam;
		this->prof = prof;
	}
};


std::ostream& operator<<(std::ostream& o, puerta const& gate) {
	switch (gate)
	{
	case X0:
		o << "X0";
		break;
	case X1:
		o << "X1";
		break;
	case X2:
		o << "X2";
		break;
	case X3:
		o << "X3";
		break;
	case X4:
		o << "X4";
		break;
	case AND:
		o << "AND";
		break;
	case OR:
		o << "OR";
		break;
	case NOT:
		o << "NOT";
		break;
	case LONLY:
		o << "LO";
		break;
	case RONLY:
		o << "RO";
		break;
	case NLONLY:
		o << "NLO";
		break;
	case NRONLY:
		o << "NRO";
		break;
	case NOR:
		o << "NOR";
		break;
	case XOR:
		o << "XOR";
		break;
	case NAND:
		o << "NAND";
		break;
	case XNOR:
		o << "XNOR";
		break;
	default:
		break;
	}
	return o;
}


// Pinta el circuito como árbol binario
void printBT(std::ostream& o, const std::string& prefix, const circuito* circ, bool isLeft)
{
	if (circ != nullptr)
	{
		o << prefix;

		o << "|--";

		// print the value of the node
		o << circ->gate << std::endl;

		// enter the next tree level - left and right branch
		printBT(o, prefix + (isLeft ? "|   " : "    "), circ->padreIzq, true);
		printBT(o, prefix + (isLeft ? "|   " : "    "), circ->padreDer, false);
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

std::ostream& operator<<(std::ostream& o, circuito* const& circ) {
	o << "Size: " << circ->tam << " Depth: " << circ->prof << " Evaluacion: ";
	bin(o, circ->eval);
	o << "\n";
	printBT(o, "", circ, false);
	o << "\n\n";
	return o;
}

// Mapa de tamaños a circuitos de ese tamaño calculados que deben ser anotados
std::unordered_map<int, std::vector<circuito*>> espera;

// Mapa de funciones computadas a vector de circuitos que la computan (de izquierda a derecha creciente de tamaño)
std::unordered_map<unsigned int, std::vector<circuito*>> almacen;

void introduceEnSet(std::set<circuito*> &set, circuito* circ) {
	if (circ != nullptr) {
		set.insert(circ);
		introduceEnSet(set, circ->padreIzq);
		introduceEnSet(set, circ->padreDer);
	}
}

void mezclar(circuito* circuito1, circuito* circuito2, int& tam) {

	std::set<circuito*> set1, set2;
	introduceEnSet(set1, circuito1);
	introduceEnSet(set2, circuito2);

	std::set<circuito*> intersect;
	std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersect, intersect.begin()));

	for (circuito* circ : intersect) {
		if (circ->tam > 0) tam--;
	}

	/*
	if (circuito1 == nullptr || circuito2 == nullptr) return;

	if (circuito1->prof > circuito2->prof) {
		mezclar(circuito1->padreIzq, circuito2, tam);
		mezclar(circuito1->padreDer, circuito2, tam);
	}
	else if (circuito1->prof < circuito2->prof) {
		mezclar(circuito1, circuito2->padreIzq, tam);
		mezclar(circuito1, circuito2->padreDer, tam);
	}
	else if (circuito1 == circuito2) {
		tam -= circuito1->tam;
	}
	else {
		mezclar(circuito1->padreIzq, circuito2->padreIzq, tam);
		mezclar(circuito1->padreIzq, circuito2->padreDer, tam);
		mezclar(circuito1->padreDer, circuito2->padreIzq, tam);
		mezclar(circuito1->padreDer, circuito2->padreDer, tam);
	}
	*/
}

// Funcion que totalmente ad-hoc pone los circuitos detamaño 0 (las entradas) con sus correspondientes evaluaciones 
void inicializa() {
	espera[0] = std::vector<circuito*>();

	circuito* circ = new circuito(X0, nullptr, nullptr, 2863311530, 0, 0);
	espera[0].push_back(circ);

	circ = new circuito(X1, nullptr, nullptr, 3435973836, 0, 0);
	espera[0].push_back(circ);

	circ = new circuito(X2, nullptr, nullptr, 4042322160, 0, 0);
	espera[0].push_back(circ);

	circ = new circuito(X3, nullptr, nullptr, 4278255360, 0, 0);
	espera[0].push_back(circ);

	circ = new circuito(X4, nullptr, nullptr, 4294901760, 0, 0);
	espera[0].push_back(circ);

}

unsigned int evaluador(puerta gate, circuito* circuito1, circuito* circuito2) {
	switch (gate)
	{
	case AND:
		return circuito1->eval & circuito2->eval;
	case OR:
		return circuito1->eval | circuito2->eval;
	case NOT:
		return ~circuito1->eval;
	case NAND:
		return ~(circuito1->eval & circuito2->eval);
	case NOR:
		return ~(circuito1->eval | circuito2->eval);
	case XOR:
		return circuito1->eval ^ circuito2->eval;
	case XNOR:
		return ~(circuito1->eval ^ circuito2->eval);
	case LONLY:
		return circuito1->eval & (~circuito2->eval);
	case RONLY:
		return (~circuito1->eval) & circuito2->eval;
	case NLONLY:
		return ~(circuito1->eval & (~circuito2->eval));
	case NRONLY:
		return ~((~circuito1->eval) & circuito2->eval);
	default:
		break;
	}
}


bool generaCircuitos(int n) {

	if (!espera.count(n)) return true;


	for (puerta gate : repertorioABinario) {
		for (size_t i = 0; i < espera[n].size(); ++i) {
			circuito* circuito1 = espera[n][i];

			for (size_t j = i + 1; j < espera[n].size(); ++j) {
				circuito* circuito2 = espera[n][j];
				int tam = circuito1->tam + circuito2->tam + 1;
				mezclar(circuito1, circuito2, tam);
				if (!espera.count(tam)) espera[tam] = std::vector<circuito*>();
				espera[tam].push_back(new circuito(gate, circuito1, circuito2, evaluador(gate, circuito1, circuito2), tam, std::max(circuito1->prof, circuito2->prof) + 1));
				circuitosGenerados++;
				if (circuitosGenerados == MAXCIRCUITOS) return false;
			}


			for (auto par2 : almacen) {
				for (circuito* circuito2 : par2.second) {
					if (circuito1 == circuito2) break;
					int tam = circuito1->tam + circuito2->tam + 1;
					mezclar(circuito1, circuito2, tam);
					if (!espera.count(tam)) espera[tam] = std::vector<circuito*>();
					espera[tam].push_back(new circuito(gate, circuito1, circuito2, evaluador(gate, circuito1, circuito2), tam, std::max(circuito1->prof, circuito2->prof) + 1));
					circuitosGenerados++;
					if (circuitosGenerados == MAXCIRCUITOS) return false;
				}
			}
		}
	}

	for (puerta gate : repertorioAUnario) {
		for (circuito* circuito1 : espera[n]) {
			int tam = circuito1->tam + 1;
			if (!espera.count(tam)) espera[tam] = std::vector<circuito*>();
			espera[tam].push_back(new circuito(gate, circuito1, nullptr, evaluador(gate, circuito1, nullptr), tam, circuito1->prof + 1));
			circuitosGenerados++;
			if (circuitosGenerados == MAXCIRCUITOS) return false;
		}
	}
	return true;
}

void anotaCircuitos(int n) {
	if (!espera.count(n)) return;
	for (circuito* circ : espera[n]) {
		if (!almacen.count(circ->eval)) {
			almacen[circ->eval] = std::vector<circuito*>();
			fs << circ;
		}
		almacen[circ->eval].push_back(circ);
	}
	std::cout << "Size " << n << " anotado\n";

}

inline void liberaCircuitos(int n) {
	espera[n].clear();
	espera.erase(n);
}

inline void anotaSobrantes() {
	std::vector<std::pair<int, std::vector<circuito*>>> sobrantes(espera.size());
	std::transform(espera.begin(), espera.end(), sobrantes.begin(), [](auto pair) {return move(pair);});
	espera.clear();
	sort(sobrantes.begin(), sobrantes.end(), [](auto pair1, auto pair2) {return pair1.first < pair2.first; });
	for (auto par : sobrantes) {
		for (circuito* circ : par.second) {
			if (!almacen.count(circ->eval)) {
				almacen[circ->eval] = std::vector<circuito*>();
				almacen[circ->eval].push_back(circ);
				fs << circ;
			}
		}
		std::cout << "Size " << par.first << " anotado\n";
	}
}

inline void muestraSobrantes() {
	for (auto par : espera) {
		std::cout << par.second.size() << " circuitos de tamano " << par.first << " por procesar\n";
	}
}

int main() {
	inicializa();
	int tamano = 0;
	while (generaCircuitos(tamano)) {
		anotaCircuitos(tamano);
		liberaCircuitos(tamano);
		tamano++;
	}
	anotaCircuitos(tamano);
	liberaCircuitos(tamano);
	std::cout << "Circuitos generados exhaustivamente hasta size " << tamano << '\n';
	muestraSobrantes();
	std::cout << "\n";
	std::cout << "Procesando... \n";
	anotaSobrantes();
	std::cout << "Total de funciones distintas computadas " << almacen.size() << '\n';
	fs << "Total de funciones distintas computadas: " << almacen.size() << '\n';
	fs.close();
	return 0;
}
