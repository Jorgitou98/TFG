#include <iostream>
#include "dirent.h"
#include <fstream>
#include <map>
#include <vector>
using namespace std;

struct tCircuito {
	string circuito;
	unsigned int tam;

	tCircuito(string circuito, unsigned int tam) {
		this->circuito = circuito;
		this->tam = tam;
	}
};

bool noEstaCircuito(string circuito, vector<tCircuito*> const& v) {
	for (tCircuito* circ : v) {
		if (circuito == circ->circuito) return false;
	}
	return true;
}

int main() {
	string ruta;
	cout << "Dame la ruta donde estan los ficheros\n";
	cin >> ruta;
	DIR* dir = opendir(ruta.c_str());
	if (dir == NULL) {
		perror("opendir");
		exit(1);
	}
	struct dirent* entrada;
	map<unsigned int, vector<tCircuito*>> mapa;
	ofstream salida("salidaMezclada.txt");
	unsigned int iteraciones = 0;
	while (entrada = readdir(dir)) {
		string stringNombre(entrada->d_name);
		// Me salto lasentradas del directorio al propio directorio y al padre
		if (stringNombre == "." || stringNombre == "..") continue;
		string rutaCompleta = ruta + "/" + stringNombre;
		ifstream archivo(rutaCompleta);
		cout << "Voy por el fichero " << rutaCompleta << '\n';
		string circuito;
		unsigned int tam, eval;
		archivo >> circuito;
		// Primera palabra por la que empieza lo que no hay que leer
		while (circuito != "Total") {
			archivo >> tam >> eval;
			if (!mapa.count(eval)) mapa[eval] = vector<tCircuito*>();
			//cout << "Guardando " << circuito << " " << tam << " " << eval << '\n';
			if (iteraciones % 100000 == 0) cout << iteraciones << '\n';
			if(noEstaCircuito(circuito, mapa[eval])) mapa[eval].push_back(new tCircuito(circuito, tam));
			archivo >> circuito;
			iteraciones++;
		}
		archivo.close();
	}
	cout << "Tras mezclar tenemos " << mapa.size() << " funciones distintas computadas\n";
	cout << "Escribiendo...\n";
	for (auto funcion : mapa) {
		for (tCircuito* elem : funcion.second) {
			salida << elem->circuito << " " << elem->tam << " " << funcion.first << '\n';
		}
	}
	salida << "Computadas " << mapa.size() << " funciones distintas\n";
	salida.close();
	closedir(dir);
}