#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <iterator>
using namespace std;

// Se asume que elementos tiene tama�o mayor o igual que k
vector<int> eligeKDeN(int k, vector<int> const& elementos) {
	vector<int> seleccionados;
	vector<int> elementosCopia = elementos;
	std::random_device dev;
	std::mt19937 rng(dev());
	for (int i = 0; i < k; ++i) {
		std::uniform_int_distribution<std::mt19937::result_type> gen(0, elementosCopia.size() - 1);
		int pos = gen(rng);
		seleccionados.push_back(elementosCopia[pos]);
		elementosCopia.erase(elementosCopia.begin() + pos);
	}
	sort(seleccionados.begin(), seleccionados.end());
	return seleccionados;
}

int valorEntorno(int n, vector<int> const& seleccionados) {
	int valor = 0;
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> gen(0, 1);
	int j = 0;
	for (int i = 0; i < n; ++i) {
		if (j >= seleccionados.size() || seleccionados[j] != i) {
			int bit = gen(rng);
			if (bit == 1) valor += pow(2, i);
		}
		else j++;
	}
	return valor;
}

int valorSeleccionados(vector<int> const& seleccionados, int num) {
	int valor = 0;
	for (int pot : seleccionados) {
		if (num % 2 == 1) valor += pow(2, pot);
		num /= 2;
	}
	return valor;
}

vector<int> posicionesInstancia(vector<int> const& seleccionados, int valorEnt) {
	vector<int> pos;
	for (int i = 0; i < pow(2, seleccionados.size()); ++i) {
		int valorSelec = valorSeleccionados(seleccionados, i);
		pos.push_back(valorSelec + valorEnt);
	}
	return pos;
}

bool balanceada(string cadena) {
	int unos = 0;
	int ceros = 0;
	for (char c : cadena) {
		if (c == '0') ceros++;
		else unos++;
	}
	return ((double)abs(unos - ceros) / (double)cadena.size()) <= (1.0 / 16.0);
}

void copiaSalidas(vector<int> const& posInst, ifstream& entradasTablasVerdad, ofstream& salidasMuestras, ofstream& salidasMuestrasDificiles) {
	string cadena = "";
	string cadenaCSV = "";
	for (int j = 0; j < posInst.size(); ++j) {
		entradasTablasVerdad.seekg(posInst[j], ios::beg);
		char c;
		entradasTablasVerdad >> c;
		cadena += c;
		cadenaCSV += c;
		if (j < posInst.size() - 1) cadenaCSV += ',';
	}
	salidasMuestras << cadenaCSV << '\n';
	if (balanceada(cadena)) salidasMuestrasDificiles << cadenaCSV << '\n';


}

int main() {
	int n = 21;
	int k = 5;
	string cabecera = "";
	int pot = pow(2, k);
	for (int i = 0; i < pot; ++i) {
		if (i < pot - 1) cabecera += "Bit " + to_string(i) + ",";
		else cabecera += "Bit " + to_string(i);
	}
	vector<int> elementos;
	for (int i = 0; i < n; ++i) elementos.push_back(i);
	string nombreFich;
	cout << "Nombre del fichero .txt sin la extension\n";
	cin >> nombreFich;
	ifstream entrada(nombreFich + ".txt");
	ofstream salida("muestras" + nombreFich + ".csv");
	ofstream salidaDificil("muestrasDificil" + nombreFich + ".csv");
	salida << cabecera << '\n';
	salidaDificil << cabecera << '\n';
	int numMuestras;
	cout << "Numero de muestras\n";
	cin >> numMuestras;
	for (int i = 0; i < numMuestras; ++i) {
		vector<int> seleccionados = eligeKDeN(k, elementos);
		int valorEnt = valorEntorno(n, seleccionados);
		vector<int> posInst = posicionesInstancia(seleccionados, valorEnt);
		copiaSalidas(posInst, entrada, salida, salidaDificil);
	}

	return 0;
}