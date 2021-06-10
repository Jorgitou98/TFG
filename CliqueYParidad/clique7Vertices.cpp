#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
using namespace std;

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

vector<vector<bool>> generaMatrizAdyacencia(int repDecimal, int n) {
	vector<vector<bool>> matAdy;	
	for (int i = 0; i < n; ++i) {
		vector<bool> listaAdy;
		for (int j = 0; j < i; ++j) {
			listaAdy.push_back((repDecimal & 1) == 1);
			repDecimal >>=  1;
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

int main() {
	ofstream salidaMaxClique("salidaMaxClique.txt");
	int n = 7;
	vector<ofstream> salidasTablasVerdad;
	for (int i = 0; i < n; ++i) {
		salidasTablasVerdad.emplace_back(ofstream{ "salidaCliqueMinK" + to_string(i + 1) + ".txt" });
	}
	//Para cada posible instancia
	vector<vector<int>> subconjuntos = subconjuntosAConsiderar(n);
	for (int repDecimal = 0; repDecimal < pow(2, 21); ++repDecimal) {
		vector<vector<bool>> matAdy = generaMatrizAdyacencia(repDecimal, n);
		int k = mayorKClique(matAdy, n, subconjuntos);
		salidaMaxClique << k;
		for (int i = 0; i < salidasTablasVerdad.size(); ++i) {
			if (k >= i + 1) salidasTablasVerdad[i] << 1;
			else salidasTablasVerdad[i] << 0;
		}
	}

	return 0;
}