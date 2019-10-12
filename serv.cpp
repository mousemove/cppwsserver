#include <iostream>
#include "WSServer.h"
using namespace std;
int main() {
	WSServer serv(8086,"adr");
	serv.setStrategy(new registrStrategy);

	cout << "" << endl; // prints 
	std::thread tdisp(std::ref(serv));
	tdisp.join();

	return 0;
}
