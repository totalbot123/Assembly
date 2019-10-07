#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cmath>
#include "Table.h"

class Parser {

public:
	Parser();
	Parser(string inputFile, string outputFile, string sectionStartAddress);
	virtual ~Parser();

protected:
	virtual void handleLine();
	virtual void handleInstruction(stringstream &ss, string word) = 0;
	virtual void handleDirective(stringstream &ss, string word) = 0;
	virtual void handleLabel(string s) = 0;
	void updateLocationCounter(stringstream &ss, string word);
	int incrementLocationCounter(stringstream &ss, int size);
	int stringToInt(string s);
	string formalHexadecimal(int i);
	string intToHex(int i);
	int hexToInt(string hex);
	string intToBin(int i, int binarySize);
	string binToHex(string binary);
	bool isRegDir(string word);

	string inputFile;
	string outputFile;
	int sectionStartAddress;

	unordered_map<int, Table*> *tableOfSymbols;
	int tableIndex;
	int locationCounter;
	int currentSection;
};

