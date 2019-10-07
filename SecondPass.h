#pragma once
#include "Parser.h"
#include <vector>

class SecondPass :
	public Parser {
public:
	SecondPass();
	SecondPass(string inputFile,
		string outputFile,
		string sectionStartAddress,
		unordered_map<int, Table*> *tableOfSymbols);
	~SecondPass();

	void parseFile();
	void writeToFile();

private:
	void handleInstruction(stringstream & ss, string word);
	void codeInstruction(stringstream &ss, string instruction, string sufix);
	bool hasZeroArguments(string instruction);
	bool hasOneArgument(string instruction);
	bool hasTwoArguments(string instruction);
	string registerCode(string reg);
	string instructionCode(string instruction);
	string sufixCode(string sufix);

	void handleDirective(stringstream & ss, string word);
	void updateGlobalSymbols(stringstream &ss);
	void editDirective(stringstream &ss, string word);
	void handleArgument(string argument, int argumentSize, bool relative = false);
	void codeData(int dataSize, int size, int fill);
	void inverseStringByTwo(string &s);
	
	void handleLabel(string word);

	void writeSymbolTable();
	void writeCode(string code);
	void writeRelocations(vector<RelocationTable*>* relocations);

	unordered_map<string, vector<RelocationTable*>* > *relocationInfo;
	unordered_map<string, string> *codeTable;

	string currentSection;
	string currentCode;
	vector<RelocationTable*> *currentRelocations;

	vector<string> *sections;
};

