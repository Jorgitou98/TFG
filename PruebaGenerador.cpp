#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

#define MAXCIRCUITOS 10000

std::ofstream fs("ficheroSalida" + std::to_string(MAXCIRCUITOS) + ".txt");
unsigned int circuitosGenerados = 0;

// Las entradas por comodidad tambien se han metido con las puertas
enum puerta { AND, OR, NOT, NAND, NOR, XOR, XNOR, X0, X1, X2, X3, X4 };


std::vector<puerta> repertorioA = { NAND };
std::vector<puerta> repertorioBBinario = { AND, OR };
std::vector<puerta> repertorioBUnario = { NOT };
//std::vector<char> repertorioC = { 'a', 'o', 'n' };

struct circuito {
	unsigned int eval;
	puerta gate;
	circuito* padreIzq;
	circuito* padreDer;
	unsigned int tam;
	unsigned int prof;
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
	default:
		break;
	}
	return o;
}


// Pinta el circuito como arbol binario
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
std::unordered_map<unsigned int, std::vector<circuito*>> espera;

// Mapa de funciones computadas a vector de circuitos que la computan (de izquierda a derecha creciente de tamaño)
std::unordered_map<unsigned int, std::vector<circuito*>> almacen;

void mezclar(circuito* circuito1, circuito* circuito2, unsigned int& tam) {

	if (circuito1 == nullptr || circuito2 == nullptr) return;

	if (circuito1->prof > circuito2->prof) {
		mezclar(circuito1->padreIzq, circuito2, tam);
		mezclar(circuito1->padreDer, circuito2, tam);
	}
	else if (circuito1->prof < circuito2->prof) {
		mezclar(circuito1, circuito2->padreIzq, tam);
		mezclar(circuito1, circuito2->padreDer, tam);
	}
	else if (circuito1 == circuito2) tam -= circuito1->tam;
	else {
		mezclar(circuito1->padreIzq, circuito2->padreIzq, tam);
		mezclar(circuito1->padreIzq, circuito2->padreDer, tam);
		mezclar(circuito1->padreDer, circuito2->padreIzq, tam);
		mezclar(circuito1->padreDer, circuito2->padreDer, tam);
	}
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
	if (gate == AND) {
		std::cout << "And de ";
		bin(std::cout, circuito1->eval);
		std::cout << " ";
		bin(std::cout, circuito2->eval);
		unsigned int result = circuito1->eval & circuito2->eval;
		std::cout << " resultado ";
		bin(std::cout, result);
		std::cout << "\n";
		return result;
	}
	else if (gate == OR) {
		std::cout << "Or de ";
		bin(std::cout, circuito1->eval);
		std::cout << " ";
		bin(std::cout, circuito2->eval);
		unsigned int result = circuito1->eval | circuito2->eval;
		std::cout << " resultado ";
		bin(std::cout, result);
		std::cout << "\n";
		return result;
	}
	else if (gate == NOT) {
		std::cout << "Not de ";
		bin(std::cout, circuito1->eval);
		unsigned int result = ~circuito1->eval;
		std::cout << " resultado ";
		bin(std::cout, result);
		std::cout << "\n";
		return result;
	}
}


bool generaCircuitos(unsigned int n) {

	if (!espera.count(n)) return true;


	for (puerta gate : repertorioBBinario) {
		for (int i = 0; i < espera[n].size(); ++i) {
			circuito* circuito1 = espera[n][i];

			for (int j = i + 1; j < espera[n].size(); ++j) {
				circuito* circuito2 = espera[n][j];
				unsigned int tam = circuito1->tam + circuito2->tam + 1;
				mezclar(circuito1, circuito2, tam);
				if (!espera.count(tam)) espera[tam] = std::vector<circuito*>();
				espera[tam].push_back(new circuito(gate, circuito1, circuito2, evaluador(gate, circuito1, circuito2), tam, std::max(circuito1->prof, circuito2->prof) + 1));
				circuitosGenerados++;
				if (circuitosGenerados == MAXCIRCUITOS) return false;
			}


			for (auto par2 : almacen) {
				for (circuito* circuito2 : par2.second) {
					if (circuito1 == circuito2) break;
					unsigned int tam = circuito1->tam + circuito2->tam + 1;
					mezclar(circuito1, circuito2, tam);
					if (!espera.count(tam)) espera[tam] = std::vector<circuito*>();
					espera[tam].push_back(new circuito(gate, circuito1, circuito2, evaluador(gate, circuito1, circuito2), tam, std::max(circuito1->prof, circuito2->prof) + 1));
					circuitosGenerados++;
					if (circuitosGenerados == MAXCIRCUITOS) return false;
				}
			}
		}
	}

	for (puerta gate : repertorioBUnario) {
		for (circuito* circuito1 : espera[n]) {
			unsigned int tam = circuito1->tam + 1;
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
	std::cout << "Tamano " << n << " anotado\n";

}

inline void liberaCircuitos(int n) {
	espera[n].clear();
	espera.erase(n);
}

inline void anotaSobrantes() {
	for (auto par : espera) {
		anotaCircuitos(par.first);
	}
	espera.clear();

}

inline void muestraSobrantes() {
	for (auto par : espera) {
		std::cout << par.second.size() << " circuitos de tamano " << par.first << " por procesar\n";
	}
}

int main() {
	inicializa();
	unsigned int tamano = 0;
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
	fs.close();

	return 0;
}
