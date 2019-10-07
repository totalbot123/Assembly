#include "Parser.h"

using namespace std;

Parser::Parser() {
}

Parser::Parser(string inputFile, string outputFile, string sectionStartAddress) {
	this->inputFile = inputFile; 
	this->outputFile = outputFile;
	this->sectionStartAddress = stringToInt(sectionStartAddress);

	this->tableOfSymbols = new unordered_map<int, Table*>();

	tableIndex = 1;
	currentSection = 0;
	locationCounter = this->sectionStartAddress;
}

Parser::~Parser() {
}

void Parser::handleLine() {
	ifstream inputFile(this->inputFile);

	while (inputFile.is_open()) {
		string line;
		while (getline(inputFile, line)) {
			stringstream ss(line);
			string word;

			ss >> word;

			if (word == "#" || (word.size() > 1 && word.at(0) == '#')) continue;

			if (word.size() > 1 && word.at(word.size() - 1) == ':') {
				handleLabel(word);
				if (ss >> word && !ss.eof()) {
					
				} else {
					continue;
				}
			}

			if (word == "") {
				continue;
			} else {
				if (word.at(0) == '.') {
					handleDirective(ss, word);
					if (word == ".end") return;
				} else {
					handleInstruction(ss, word);
				}
			}

		}
		inputFile.close();
	}
}

void Parser::updateLocationCounter(stringstream &ss, string word) {
	if (word == "") return;

	if (word.at(0) == '.') {
		string argument;

		if (word == ".char") {
			locationCounter += incrementLocationCounter(ss, 1);
		} else if (word == ".word") {
			locationCounter += incrementLocationCounter(ss, 2);
		} else if (word == ".long") {
			locationCounter += incrementLocationCounter(ss, 4);
		} else {
			ss >> argument;

			if (word == ".skip") {
				locationCounter += stringToInt(argument);
			} else {
				locationCounter += locationCounter%stringToInt(argument);
			}
		}
	} else {
		string operand;
		string instruction = word;

		if (word.size() > 4) {
			instruction = word.substr(0, word.size() - 2);
		}

		if (instruction == "call") {
			locationCounter += 4;
		} else if (instruction == "ret" || instruction == "iret") {
			locationCounter += 2;
		} else {
			ss >> operand;

			if (instruction == "mov") {
				string modifiedOperand(operand, 0, operand.size() - 1);
				if (!isRegDir(operand)) {
					locationCounter += 4;
					return;
				}
			}

			if (instruction != "pop" && instruction != "push" && instruction != "jmp") {
				ss >> operand;
			}

			if (!isRegDir(operand)) {
				locationCounter += 4;
			} else {
				locationCounter += 2;
			}
		}
	}
}

int Parser::incrementLocationCounter(stringstream & ss, int size) {
	int result = 0;
	string word;

	while (ss >> word && word != ";") {
		result += size;
	}

	return result;
}

int Parser::stringToInt(string s) {
	if (s == "*niz") {
		cout << "";
	}

	string number(s);
	stringstream ss;
	if (s.size() > 1 && s.at(1) == 'x') {
		number = s.substr(2, s.size() - 2);
		ss << std::hex << number;
	} else {
		ss << number;
	}
	int result;
	ss >> result;
	return result;
}

string Parser::formalHexadecimal(int i) {
	string result("0x");
	result.append(intToHex(i));
	return result;
}

string Parser::intToHex(int i) {
	string result;
	stringstream ss;
	ss << hex << i;
	ss >> result;
	return result;
}

int Parser::hexToInt(string hex) {
	int result;

	string hexNumber = hex;
	if (hex.size() > 1 && hex.at(1) == 'x') {
		hexNumber = hex.substr(2, hex.size() - 2);
	}

	stringstream ss;
	ss << std::hex << hexNumber;
	ss >> result;
	return result;
}

string Parser::intToBin(int i, int sizeOfBinary) {
	string result;

	while (i) {
		if (i & 1) result.insert(0, "1");
		else result.insert(0, "0");

		i >>= 1;
	}

	if (sizeOfBinary < result.size()){
		result = result.substr(result.size() - sizeOfBinary - 1, sizeOfBinary);
	} else {
		int difference = sizeOfBinary - result.size();
		while (difference--) result.insert(0, "0");
	}

	return result;
}

string Parser::binToHex(string binary) {
	if (binary == "0000010110000000") {
		cout << "";
	}

	if (binary.size() != 16) cout << "skk" << endl;

	int binNum = 0;

	for (int i = binary.size() - 1; i >= 0; --i) {
		if (binary[i] == '1') {
			binNum += pow(2, binary.size() - i - 1);
		}
	}
	string result = intToHex(binNum).size() == 4 ? intToHex(binNum) : "0" + intToHex(binNum);
	return result;
}

bool Parser::isRegDir(string operand) {
	if (operand.size() == 2 && operand.at(0) == 'r' && isdigit(operand.at(1))) {
		return true;
	}
	return false;
}
