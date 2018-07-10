#include <iostream>
#include <ctime>
#include "persistent_set.h"

using namespace std;

int main() {
	srand(time(0));
	persistent_set x;
	int k = 1;
	int arr[100];
	for (int i = 0; i < 100; i++) arr[i] = rand() % 512;
	for (int i = 0; i < 100; i++) {
		x.insert(arr[i]);
	}
	for (int i = 0; i < 100; i++) swap(arr[rand() % 100], arr[rand() % 100]);
	persistent_set y(x);
	auto it = x.begin();
	while (it != x.end()) {
		cout << *(it++) << " ";
	}
	cout << "\n";
	it = x.end();
	while (it != x.begin()) {
		cout << *(--it) << " ";
	}
	cout << "\n";
	while (x.begin() != x.end()) {
		cout << *(x.begin()) << " ";
		x.erase(x.begin());
	}
	cout << "\n";
	for (int i = 0; i < 100; i++) {
		if (y.find(arr[i]) != y.end()) y.erase(y.find(arr[i]));
	}
	cout << (y.begin() == y.end());
	return 0;
}