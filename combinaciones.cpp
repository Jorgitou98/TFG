#include <iostream>
#include <vector>
#include <fstream>
using namespace std;


void suman32(vector<vector<int>> & soluciones, int sumaActual, vector <int> const& conjuntoCons, vector<int> const& conjunto) {
	if (sumaActual == 32) soluciones.push_back(conjuntoCons);
	else if (sumaActual > 32) return;

	for (int elem : conjunto) {
		vector<int> copia = conjuntoCons;
		copia.push_back(elem);
		suman32(soluciones, sumaActual + elem, copia, conjunto);
	}
}


int main() {
	ofstream salida("salidaCombinaciones32.txt");
	vector<vector<int>> soluciones;
	suman32(soluciones, 0, {}, { 4,8,16,32 });
	cout << soluciones.size();
	for (auto v : soluciones) {
		cout << "Solucion:\n";
			for (int elem : v) {
				cout << elem << " ";
				salida << elem << " ";
		}
			cout << '\n';
			salida << '\n';
	}
	
	return 0;
}