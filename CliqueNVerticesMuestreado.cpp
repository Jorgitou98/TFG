#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include<list>
#include <random>
using namespace std;


void subsetTamK(vector<int> const& arr, int size, int left, int index, vector<int>& l, vector<vector<int>> & subsets) {
	if (left == 0) {
		subsets.push_back(l);
	}
	for (int i = index; i < size; i++) {
		l.push_back(arr[i]);
		subsetTamK(arr, size, left - 1, i + 1, l, subsets);
		l.pop_back();
	}

}

vector<vector<int>> subconjuntosAConsiderar(int n) {
	vector<int> todos;
	for (int i = 0; i < n; ++i) todos.push_back(i);
	vector<vector<int>> subconjuntos;
	for (int i = 0; i < pow(2, n); i++) {
		int t = i;
		vector<int> v;
		for (int j = 0; j < n; j++) {
			if (t & 1)
				v.push_back(todos[j]);
			t >>= 1;
		}
		sort(v.begin(), v.end(), greater<>());
		subconjuntos.push_back(v);
	}
	sort(subconjuntos.begin(), subconjuntos.end(), [](vector<int> const& v1, vector<int> const& v2) {return v1.size() > v2.size(); });
	return subconjuntos;
}

vector<vector<bool>> generaMatrizAdyacencia(unsigned long long repDecimal, int n) {
	vector<vector<bool>> matAdy;
	for (int i = 0; i < n; ++i) {
		vector<bool> listaAdy;
		for (int j = 0; j < i; ++j) {
			listaAdy.push_back((repDecimal & 1) == 1);
			repDecimal >>= 1;
		}
		matAdy.push_back(listaAdy);
	}
	return matAdy;
}

bool esClique(vector<vector<bool>> const& matAdy, vector<int> const& subconjunto) {
	for (int i = 0; i < subconjunto.size(); ++i) {
		for (int j = i + 1; j < subconjunto.size(); ++j) {
			if (!matAdy[subconjunto[i]][subconjunto[j]]) return false;
		}
	}
	return true;
}

int mayorKClique(vector<vector<bool>> const& matAdy, int n, vector<vector<int>> const& subconjuntos) {
	for (vector <int> subconjunto : subconjuntos) {
		if (esClique(matAdy, subconjunto)) return subconjunto.size();
	}
	return 0;
}

int valorDecimal(vector<int> valores, vector<int> const& potencias) {
	int decimal = 0;
	for (int i = 0; i < valores.size(); ++i) {
		if (valores[i] == 1) decimal += pow(2, potencias[i]);
	}
	return decimal;
}

vector<int> binario(int n, int numBits) {
	vector<int> binario(numBits);
	for (int i = 1; i <= numBits; ++i){
		binario[numBits - i] = n % 2;
		n = n / 2;
	}
	return binario;
}

vector<vector<int>> binariosHastaN(int n, int numBits) {
	vector<vector<int>> bins;
	for (int i = 0; i < n; ++i) bins.push_back(binario(i, numBits));
	return bins;
}

void resuelveInstancias(int nVertices, vector<vector<int>> const& bitsElegidos) {
	ofstream salidaMaxClique("salidaMaxClique.txt");
	vector<ofstream> salidasTablasVerdad;
	vector<pair<int, int>> cuentaCerosUnos;
	for (int i = 0; i < n; ++i) {
		salidasTablasVerdad.emplace_back(ofstream{ "salidaCliqueMinK" + to_string(i + 1) + ".txt" });
	}
	vector<vector<int>> subconjuntos = subconjuntosAConsiderar(nVertices);
	vector<vector<int>> bins32 = binariosHastaN(32, 5);
	for (vector<int> bits : bitsElegidos) {
		// Resolvemos la instancia
		for (auto bin32 : bins32) {
			int repDecimal = valorDecimal(bin32, bits);
			vector<vector<bool>> matAdy = generaMatrizAdyacencia(repDecimal, nVertices);
			int k = mayorKClique(matAdy, nVertices, subconjuntos);
			salidaMaxClique << k;
			for (int i = 0; i < salidasTablasVerdad.size(); ++i) {
				if (k >= i + 1) {
					salidasTablasVerdad[i] << 1;
					cuentaCerosUnos[i].second++;
				}
				else {
					salidasTablasVerdad[i] << 0;
					cuentaCerosUnos[i].first++;
				}
			}
		}
		salidaMaxClique << '\n';
		for (int i = 0; i < salidasTablasVerdad.size(); ++i) {
			salidasTablasVerdad[i] << '\n';
		}
	}
	for (int i = 0; i < salidasTablasVerdad.size(); ++i) {
		salidasTablasVerdad[i] << "Numero de ceros: " << cuentaCerosUnos[i].first << '\n';
		salidasTablasVerdad[i] << "Numero de unos: " << cuentaCerosUnos[i].second << '\n';
		double proporcion = (double)min(cuentaCerosUnos[i].first, cuentaCerosUnos[i].second) / (double)max(cuentaCerosUnos[i].first, cuentaCerosUnos[i].second);
		salidasTablasVerdad[i] << "Proporcion: " << proporcion << '\n';
	}
}

int main() {
	int n = 21;
	vector<int> arr;
	for (int i = 0; i < n; ++i) arr.push_back(i);
	vector<int> l;
	vector<vector<int>> bitsElegidos;
	subsetTamK(arr, arr.size(), 5, 0, l, bitsElegidos);
	resuelveInstancias(7, bitsElegidos);
	/*int n;
	cout << "Numero de vertices\n";
	cin >> n;
	int numMuestras;
	cout << "Numero de muestras\n";
	cin >> numMuestras;
	resuelveInstancias(n, numMuestras);*/
	return 0;
}