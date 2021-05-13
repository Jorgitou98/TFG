#include <iostream>
#include <algorithm> 
#include <vector>
#include <unordered_map>
#include <bitset>
#include <math.h>
#include <fstream>
#include <unordered_set>
using namespace std;

struct ordenValores{
	vector<unsigned int> orden;
	vector<unsigned int> valores;
};

ostream &operator << (ostream & out, vector<unsigned int> const& v) {
	for (unsigned int elem : v) {
		out << elem << " ";
	}
	out << '\n';
	return out;
}

vector<vector<unsigned int>> permutaciones(vector<unsigned int>& ordenPermIni) {
	vector<vector<unsigned int>> permutaciones = { ordenPermIni };
	while (next_permutation(ordenPermIni.begin(), ordenPermIni.end())) {
		permutaciones.push_back(ordenPermIni);
	}
	return permutaciones;
}

void muestraOrdenes(vector<vector<unsigned int>> const& ordenesEntradas) {
	for (unsigned int i = 0; i < ordenesEntradas.size(); ++i) {
		cout << "Orden " << i+1 << '\n';
		for (unsigned int elem : ordenesEntradas[i]) {
			cout << elem << " ";
		}
		cout << "\n\n";
	}
}

vector<unsigned int> binario32Bits(unsigned int num) {
	vector<unsigned int> binario;
	for (unsigned int i = 0; i < 32; ++i) {
		binario.push_back(num & 1);
		num = num >> 1;
	}
	return binario;
}

unordered_map<unsigned int, vector<unsigned int>> traduceABinarios(unordered_map<unsigned int, unsigned int> const& entradas){
	unordered_map<unsigned int, vector<unsigned int>> traduccion;
	for (auto par: entradas) {
		traduccion[par.first] = binario32Bits(par.second);
	}
	return traduccion;
}
void muestraOrdenesAValores(vector<ordenValores> const& ordenAValorBits) {
	for (ordenValores elem : ordenAValorBits) {
		cout << "Orden: " << elem.orden << "Valores: " << elem.valores << '\n';
	}
}

vector<unsigned int> valorBitsOrden(unordered_map<unsigned int, vector<unsigned int>> & funEntradaBinario, vector<unsigned int> const& orden) {
	vector<unsigned int> valoresBits;
	for (unsigned int i = 0; i < 32; ++i) {
		unsigned int valorBit = 0;
		for (unsigned int j = 0; j < orden.size(); ++j) {
			if (funEntradaBinario[orden[j]][i] == 1) valorBit = valorBit + pow(2,j);
		}
		valoresBits.push_back(valorBit);
	}
	return valoresBits;
}

vector<ordenValores> traduceOrdenesAValorBits(unordered_map<unsigned int, vector<unsigned int>>& funEntradaBinario, vector<vector<unsigned int>> const& ordenesEntradas) {
	vector<ordenValores> ordenAValorBits;
	for (vector<unsigned int> orden : ordenesEntradas) {
		ordenAValorBits.push_back({ orden, valorBitsOrden(funEntradaBinario, orden) });
	}
	return ordenAValorBits;
}

unsigned int funcionSegunValores(vector<unsigned int> funcion, vector<unsigned int> valores) {
	unsigned int valor = 0;
	for (unsigned int i = 0; i < valores.size(); ++i) {
		if (funcion[i] == 1) valor += pow(2, valores[i]);
	}
	return valor;
}

void sacaFuncionesClase(string fich, vector<ordenValores> const& ordenAValorBits) {
	unordered_set<unsigned int> funEncontradas;
	ifstream entrada(fich);
	ofstream salida("dataSetEnClases.txt");
	unsigned int numClases = 0;
	unsigned int funcion;
	string saltar;
	while (entrada >> funcion && entrada >> saltar) {
		if (!funEncontradas.count(funcion)) {
			numClases++;
			salida << "Clase " << numClases << '\n';
			salida << "[";
			for (ordenValores parOrdenValor : ordenAValorBits) {
				unsigned int funcEncontrada = funcionSegunValores(binario32Bits(funcion), parOrdenValor.valores);
				if (!funEncontradas.count(funcEncontrada)) {
					salida << funcEncontrada << " ";
					funEncontradas.insert(funcEncontrada);
				}
			}
			salida << "]\n";
		}
	}
	salida << "Ultima funcion " << funcion<< '\n';
	salida << "Funciones encontradas " << funEncontradas.size();
	entrada.close();
	salida.close();
}

int main() {
	vector<unsigned int> ordenIni = { 0,1,2,3,4 };
	vector<vector<unsigned int>> ordenesEntradas = permutaciones(ordenIni);
	muestraOrdenes(ordenesEntradas);
	unordered_map<unsigned int, unsigned int> entradas = { {0, 2863311530},  {1, 3435973836}, {2, 4042322160}, {3, 4278255360}, {4, 4294901760} };
	unordered_map<unsigned int, vector<unsigned int>> funEntradaBinario = traduceABinarios(entradas);
	vector<ordenValores> ordenAValorBits = traduceOrdenesAValorBits(funEntradaBinario, ordenesEntradas);
	muestraOrdenesAValores(ordenAValorBits);
	sacaFuncionesClase("funcionesSinRepeticion.txt", ordenAValorBits);
	return 0;
}