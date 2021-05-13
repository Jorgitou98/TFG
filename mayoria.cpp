#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

int parUnos(int num, int n) {
	int unos = 0;
	for (int i = 0; i < n; ++i) {
		if (num % 2 == 1) unos++;
		num /= 2;
	}
	return unos > n/2 == 0;
}
int main() {
	ofstream salidaMayoria("salidaMayoria.txt");
	int n = 21;
	for (int num = 0; num < pow(2, n); ++num) {
		if (parUnos(num, n)) salidaMayoria << '1';
		else salidaMayoria << '0';
	}
	return 0;
}