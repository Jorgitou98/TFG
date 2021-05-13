#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
using namespace std;

const int pasTriMax = pow(2,21)-1;

vector<unsigned long> pasTri(pasTriMax+1);

void pascalTriangle(unsigned long n)
// Calculate the n'th line 0.. middle
{
    unsigned long j, k;

    pasTri[0] = 1;
    j = 1;
    while (j <= n)
    {
        j++;
        k = j / 2;
        pasTri[k] = pasTri[k - 1];
        for (; k >= 1; k--)
            pasTri[k] += pasTri[k - 1];
    }
}

bool isPrime(unsigned long n)
{
    if (n > pasTriMax)
    {
        cout << n << " is out of range" << endl;
        exit(1);
    }

    pascalTriangle(n);
    bool res = true;
    int i = n / 2;
    while (res && (i > 1))
    {
        res = res && (pasTri[i] % n == 0);
        i--;
    }
    return res;
}


int main(){
    ofstream salidaPrimos("salidaPrimos.txt");
    int n = 21;
    for (int num = 0; num < pow(2, n); ++num) {
        if (isPrime(num)) salidaPrimos << '1';
        else salidaPrimos << '0';
    }
}
