#include <iostream>
#include "FirstPass.h"
#include "SecondPass.h"

using namespace std;

int main(int argc, char *argv[]) {
	FirstPass *firstPass = new FirstPass(argv[1], argv[2], argv[3]);
	firstPass->parseFile();

	SecondPass *secondPass = new SecondPass(argv[1], argv[2], argv[3], firstPass->getTableOfSymbols());
	secondPass->parseFile();
	secondPass->writeToFile();

	return 0;
}
