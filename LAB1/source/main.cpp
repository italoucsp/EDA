#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>

using namespace std;

void setData(vector<vector<int>> &data_set, int N, int dimensions) {
	data_set.resize(N);
	for (auto &sdst : data_set) {
		for (int i = 0; i < dimensions; i++) {
			sdst.push_back(rand() % 1000 + 500);
		}
	}
}

double euclideanDistance(vector<int> &sds1, vector<int> &sds2) {
	double distance = 0.0;
	for (int i = 0; i < int(sds1.size()); i++) {
		distance += pow(sds1[i] - sds2[i],2);
	}
	return sqrt(distance);
}

double curse_dimensionalityTimer(vector<vector<int>> &data_set) {
	unsigned t0, t1;

	t0 = clock();

	for (auto &sdst : data_set) {
		for (auto &sdst_o : data_set) {
			euclideanDistance(sdst, sdst_o);
		}
	}

	t1 = clock();

	return (double(t1 - t0) / CLOCKS_PER_SEC) * 1000.0;
}

int main() {
	srand(time(NULL));
	vector<vector<vector<int>>> VEC;//vec10k, vec15k, vec20k, vec25k
	int tamaños[4] = { 10000,15000,20000,25000 };
	int dimensiones[7] = { 4,6,8,10,12,18,20 };
	for (int tam = 0; tam < 4; tam++) {
		for (int dim = 0; dim < 7; dim++) {
			vector<vector<int>> DS;
			setData(DS, tamaños[tam], dimensiones[dim]);
			VEC.push_back(DS);
		}
	}
	cout << "Vectores creados!\n";
	for (auto &svec : VEC) {
		cout << "Tiempo en ms (" << int(svec.size()) << "," << int(svec[0].size()) << "): " << curse_dimensionalityTimer(svec) << endl;
	}
	system("pause");
	return 0;
}
