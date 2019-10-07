#include "SecondPass.h"

SecondPass::SecondPass() {
}

SecondPass::SecondPass(string inputFile,
	string outputFile,
	string sectionStartAddress,
	unordered_map<int, Table*> *tableOfSymbols) : Parser(inputFile, outputFile, sectionStartAddress) {
	
	this->tableOfSymbols = tableOfSymbols;
	tableIndex = tableOfSymbols->size() + 1;

	codeTable = new unordered_map<string, string>();
	relocationInfo = new unordered_map<string, vector<RelocationTable*>* >();

	currentSection = "";
	currentCode = "";
	currentRelocations = nullptr;

	sections = new vector<string>();
}

SecondPass::~SecondPass() {
}

void SecondPass::parseFile() {
	handleLine();
}

void SecondPass::writeToFile() {
	ofstream output(outputFile, ios::trunc | ios::out);
	if (output.is_open()) {
		writeSymbolTable();

		for (auto i = sections->begin(); i != sections->end(); ++i) {
			if (*i == ".bss") continue;
			writeCode(codeTable->find((*i))->second);
			writeRelocations(relocationInfo->find((*i))->second);
		}
	}
}


void SecondPass::handleInstruction(stringstream & ss, string word) {
	string sufix = "";
	string instruction = word;

	if (word.size() > 4) {
		instruction = word.substr(0, word.size() - 2);
		sufix = word.substr(word.size() - 2, 2);
	}

	codeInstruction(ss, instruction, sufix);
}

void SecondPass::codeInstruction(stringstream & ss, string instruction, string sufix) {
	if (sufix == "al") {
		cout << "";
	}

	string dst = "00000";
	string src = "00000";
	string argument;
	bool additionalSpace = false;
	bool relativeRelocation = false;
	string code = "";

	if (hasZeroArguments(instruction)) {
		if (instruction == "iret") {
			src = "00111";
		}
		dst = "01111";
	} else if (hasOneArgument(instruction)) {
		ss >> argument;
		if (isRegDir(argument)) {
			code = "01" + registerCode(argument);
		} else {
			code = "10000";
			additionalSpace = true;
		}

		if (instruction == "jmp") {
			if (isRegDir(argument)) {
				instruction = "add";
				src = code;
				dst = "01111";
			} else {
				instruction = "mov";
				relativeRelocation = true;
				src = "01111";
				dst = code;
			}
		} else if (instruction == "pop") {
			dst = code;
		} else {
			if (instruction == "call") relativeRelocation = true;
			src = code;
		}
	} else {
		string relocationSymbol;
		for (int i = 0; i < 2; ++i) {
			ss >> argument;
			if (argument.at(argument.size() - 1) == ',') {
				argument = argument.substr(0, argument.size() - 1);
			}

			if (isRegDir(argument)) {
				code = "01" + registerCode(argument);
			} else {
				if (isRegDir(string(argument, 0, 2))) {
					code = "11" + registerCode(string(argument, 0, 2));
					relocationSymbol = argument.substr(3, argument.size() - 4);
				} else if (argument.at(0) == '&' || argument.at(0) == '$') {
					if (argument.at(0) == '$') {
						relativeRelocation = true;
					}
					argument = argument.substr(1, argument.size() - 1);
					for (auto i = tableOfSymbols->begin(); i != tableOfSymbols->end(); ++i) {
						if (i->second->name == argument) {
							argument = to_string(hexToInt(i->second->value));
							break;
						}
					}
					code = "10000";
					additionalSpace = true;
				} else if (argument.at(0) == '*') {
					argument = argument.substr(1, argument.size() - 1);
					code = "00000";
					additionalSpace = true;
				} else {
					bool immediate = true;
					for (auto i = tableOfSymbols->begin(); i != tableOfSymbols->end(); ++i) {
						if (i->second->name == argument) {
							immediate = false;
							break;
						}
					}
					if (immediate) {
						code = "00000";
					} else {
						code = "10000";
					}
				}
				additionalSpace = true;
				relocationSymbol = argument;
			}
			if (i == 0) {
				dst = code;
			} else {
				src = code;
			}
		}
		argument = relocationSymbol;
	}

	string binaryInstruction = sufixCode(sufix) + instructionCode(instruction) + dst + src;
	currentCode += binToHex(binaryInstruction);
	locationCounter += 2;
	if (additionalSpace) {
		handleArgument(argument, 2, relativeRelocation);
	}
}

bool SecondPass::hasZeroArguments(string instruction) {
	if (instruction == "ret" || instruction == "iret") return true;
	return false;
}

bool SecondPass::hasOneArgument(string instruction) {
	if (instruction == "jmp" ||
		instruction == "call" ||
		instruction == "push" ||
		instruction == "pop") return true;
	return false;
}

bool SecondPass::hasTwoArguments(string instruction) {
	return false;
}

string SecondPass::registerCode(string reg) {
	int number = reg.at(1) - '0';

	return intToBin(number, 3);

	return string();
}

string SecondPass::instructionCode(string instruction) {
	if (instruction == "add") {
		return "0000";
	} else if (instruction == "sub") {
		return "0001";
	} else if (instruction == "mul") {
		return "0010";
	} else if (instruction == "div") {
		return "0011";
	} else if (instruction == "cmp") {
		return "0100";
	} else if (instruction == "and") {
		return "0101";
	} else if (instruction == "or") {
		return "0110";
	} else if (instruction == "not") {
		return "0111";
	} else if (instruction == "test") {
		return "1000";
	} else if (instruction == "push") {
		return "1001";
	} else if (instruction == "pop") {
		return "1010";
	} else if (instruction == "call") {
		return "1011";
	} else if (instruction == "iret") {
		return "1100";
	} else if (instruction == "mov") {
		return "1101";
	} else if (instruction == "shl") {
		return "1110";
	} else {
		return "1111";
	} 
}

string SecondPass::sufixCode(string sufix) {
	if (sufix == "" || sufix == "al") {
		return "11";
	} else if (sufix == "gt") {
		return "10";
	} else if (sufix == "ne") {
		return "01";
	} else {
		return "00";
	}
}

void SecondPass::handleDirective(stringstream & ss, string word) {
	string argument;

	if (word == ".end") {
		codeTable->insert({ currentSection, currentCode });
		relocationInfo->insert({ currentSection, currentRelocations });
		return;
	} else if (word == ".global"){
		updateGlobalSymbols(ss);
	} else if (word == ".text" || word == ".data" || word == ".rodata" || word == ".bss") {
		if (currentSection != "") {
			codeTable->insert({ currentSection, currentCode });
			relocationInfo->insert({ currentSection, currentRelocations });
		}

		currentSection = word;
		currentCode = "";
		currentRelocations = new vector<RelocationTable*>();

		sections->emplace_back(word);
	} else {
		editDirective(ss, word);
	}
}

void SecondPass::updateGlobalSymbols(stringstream & ss) {
	string argument;

	while (ss >> argument) {
		if (argument.at(argument.size() - 1) == ',') {
			argument = argument.substr(0, argument.size() - 1);
		}

		bool inTableOfSymbols = false;

		for (auto i = tableOfSymbols->begin(); i != tableOfSymbols->end(); ++i) {
			if (i->second->name == argument) {
				inTableOfSymbols = true;
				i->second->flag = "G";
				break;
			}
		}

		if (!inTableOfSymbols) {
			Table *table = new Table(
				argument,
				tableIndex,
				string("SYM"),
				0,
				string("0x0"),
				string(),
				string(),
				string("G")
			);
			tableOfSymbols->insert({ tableIndex, table });
			++tableIndex;
		}
	}
}

void SecondPass::editDirective(stringstream &ss, string word) {
	string argument;

	if (word == ".char") {
		while (ss >> argument && argument != ";") {
			handleArgument(argument, 1);
		}
	} else if (word == ".word") {
		while (ss >> argument && argument != ";") {
			handleArgument(argument, 2);
		}
	} else if (word == ".long") {
		while (ss >> argument && argument != ";") {
			handleArgument(argument, 4);
		}
	} else {
		ss >> argument;

		int size;
		if (word == ".skip") {
			size = stringToInt(argument);
			//locationCounter += size;
		} else {
			size = locationCounter%stringToInt(argument);
			//locationCounter += size;
		}

		if (ss >> argument) {
			codeData(1, size, stringToInt(argument));
		} else {
			codeData(1, size, 0);
		}
	}
}

void SecondPass::handleArgument(string argument, int argumentSize, bool relative) {
	for (auto i = tableOfSymbols->begin(); i != tableOfSymbols->end(); ++i) {
		if (i->second->name == argument) {
			int code;
			if (relative) {
				code = -2;
			} else {
				if (i->second->type == "SEG") {
					code = hexToInt(i->second->startAddress);
				} else {
					code = hexToInt(i->second->value);
				}
			}
			int value;

			if (i->second->indexOfSection == 0) {
				value = hexToInt(i->second->value);
			} else {
				value = i->second->indexOfSection;
			}

			string flag = relative ? "R" : "A";
			RelocationTable *relocationRow = new RelocationTable(
				string(formalHexadecimal(locationCounter)),
				flag,
				value
			);
			currentRelocations->emplace_back(relocationRow);
			codeData(2, 1, code);
			return;
		}
	}

	codeData(argumentSize, 1, stringToInt(argument));
}

void SecondPass::codeData(int dataSize, int size, int fill) {
	string hexFill = intToHex(fill);
	inverseStringByTwo(hexFill);

	hexFill.resize(dataSize*2);

	for (int i = 0; i < hexFill.size(); ++i) {
		if (hexFill.at(i) == '\0') {
			hexFill.at(i) = '0';
		}
	}

	while (size--) {
		currentCode += hexFill;
		locationCounter += dataSize;
	}
}

void SecondPass::inverseStringByTwo(string &s) {
	string result = "";
	int n = s.size();
	int i;
	for (i = n - 2; i >= 0; i-= 2) {
		string sub(s, i, 2);
		result += sub;
	}
	if (s.size()%2 == 1) {
		result += s[0];
	}
	s = result;
}

void SecondPass::handleLabel(string word) {

}

void SecondPass::writeSymbolTable() {
	ofstream output(outputFile, ios::app);
	vector<Table*> *symbols = new vector<Table*>(tableOfSymbols->size());
	for (auto i = tableOfSymbols->begin(); i != tableOfSymbols->end(); ++i) {
		symbols->at(i->first - 1) = i->second;
	}

	for (auto i = 0; i < symbols->size(); ++i) {
		output << i + 1
			<< "\t" << symbols->at(i)->name
			<< "\t\t" << symbols->at(i)->type
			<< "\t" << symbols->at(i)->indexOfSection;
		if (symbols->at(i)->type == "SEG") {
			output << "\t" << symbols->at(i)->startAddress;
			output << "\t" << symbols->at(i)->size;
		} else {
			output << "\t" << symbols->at(i)->value;
		}
		output << "\t" << symbols->at(i)->flag << "\n" << endl;
	}
}

void SecondPass::writeCode(string code) {
	ofstream output(outputFile, ios::app);

	int i = 0;
	while (i < code.size()) {
		if (i != 0 && i % 32 == 0) {
			output << "\n";
		} else if (i != 0 && i % 2 == 0) {
			output << " ";
		}

		output << code.at(i++);
	}
	output << "\n\n";
}

void SecondPass::writeRelocations(vector<RelocationTable*>* relocations) {
	ofstream output(outputFile, ios::app);

	for (auto i = relocations->begin(); i != relocations->end(); ++i) {
		output << (*i)->relocationAddress << "\t"
			<< (*i)->type << "\t"
			<< (*i)->index << endl;
	}
	output << endl;
}
