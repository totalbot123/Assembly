#include "FirstPass.h"

FirstPass::FirstPass() {
}

FirstPass::FirstPass(string inputFile, string outputFile, string sectionStartAddress) : 
	Parser(inputFile, outputFile, sectionStartAddress) {

}

FirstPass::~FirstPass() {
}

void FirstPass::parseFile() {
	handleLine();
}

unordered_map<int, Table*> *FirstPass::getTableOfSymbols() {
	return tableOfSymbols;
}

void FirstPass::handleInstruction(stringstream &ss, string word) {
	updateLocationCounter(ss, word);
}

void FirstPass::handleDirective(stringstream &ss, string word) {
	string argument;

	if (word == ".end") {
		Table *previousSection = tableOfSymbols->find(currentSection)->second;
		previousSection->size = formalHexadecimal(locationCounter - stringToInt(previousSection->startAddress));
		return;
	} else if (word == ".global") {
		return;
	} else if (word == ".text" || word == ".data" || word == ".bss" || word == ".rodata") {
		Table *newRow = new Table(
			word,
			tableIndex,
			"SEG",
			tableIndex,
			string(),
			formalHexadecimal(locationCounter),
			string(),
			string()
		);

		tableOfSymbols->insert({ tableIndex, newRow });

		if (currentSection) {
			Table *previousSection = tableOfSymbols->find(currentSection)->second;
			previousSection->size = formalHexadecimal(locationCounter - stringToInt(previousSection->startAddress));
		}
		currentSection = tableIndex;
		++tableIndex;
	} else {
		updateLocationCounter(ss, word);
	}
}

void FirstPass::handleLabel(string word) {
	Table *newRow = new Table(
		string(word, 0, word.size() - 1),
		tableIndex,
		string("SYM"),
		currentSection,
		formalHexadecimal(locationCounter),
		string(),
		string(),
		string("L")
	);
	tableOfSymbols->insert({ tableIndex, newRow });
	++tableIndex;
}