#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_set>
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
	sort(subconjuntos.begin(), subconjuntos.end(), [](vector<int> const& v1, vector<int> const& v2) {return v1.size() < v2.size(); });
	return subconjuntos;
}

vector<vector<int>> conjuntoRep(int n, int conj, int elems) {
	vector<vector<int>> rep;
	for (int i = 0; i < conj; ++i) {
		rep.push_back(vector<int>());
		for (int j = 0; j < elems; ++j) {
			rep[i].push_back(n % 2);
			n /= 2;
		}
	}
	return rep;
}

bool recubre(vector<vector<int>> const& rep, vector<int> const& subconjuntos, int elems) {
	unordered_set<int> encontrados;
	for (int subconjunto : subconjuntos) {
		for (int j = 0; j < elems; ++j) {
			if (rep[subconjunto][j] == 1) encontrados.insert(j);
		}
	}
	return encontrados.size() == elems;
}

int minSubconjuntosCover(int n, vector<vector<int>> const& subconjuntosPosibles, int conj, int elems) {
	vector<vector<int>> rep = conjuntoRep(n, conj, elems);
	for (auto subconjuntos : subconjuntosPosibles) {
		if (recubre(rep, subconjuntos, elems)) return subconjuntos.size();
	}
	return -1;
}

int main() {
	int conj = 4;
	int elems = 5;
	ofstream salidaMinSetCover("salidaMinSetCover.txt");
	vector<ofstream> salidasTablasVerdad;
	for (int i = 0; i < conj; ++i) {
		salidasTablasVerdad.emplace_back(ofstream{ "salidaSetCoverMin" + to_string(i + 1) + ".txt" });
	}
	// Guardamos las posibilidades de subconjuntos a escoger de mayor a menor cardinal
	vector<vector<int>> subconjuntosPosibles = subconjuntosAConsiderar(conj);
	for (int i = 0; i < pow(2, 21); ++i) {
		int minSub = minSubconjuntosCover(i, subconjuntosPosibles, conj, elems);
		salidaMinSetCover << minSub;
		for (int i = 0; i < salidasTablasVerdad.size(); ++i) {
			if(minSub == -1 | minSub > i + 1) salidasTablasVerdad[i] << 0;
			else salidasTablasVerdad[i] << 1;
		}
	}
}