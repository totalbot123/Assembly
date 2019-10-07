#pragma once
#include "Parser.h"

class FirstPass : public Parser {
public:
	FirstPass();
	FirstPass(string inputFile, string outputFile, string sectionStartAddress);
	~FirstPass();

	void parseFile();
	unordered_map<int, Table*> *getTableOfSymbols();

private:
	void handleInstruction(stringstream & ss, string word);
	void handleDirective(stringstream & ss, string word);
	void handleLabel(string word);
};

