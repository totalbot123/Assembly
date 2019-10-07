#ifndef _TABLE_H_
#define _TABLE_H_

#include <string>

using namespace std;

typedef struct table{
	string name;
	int index;
	string type;
	int indexOfSection;
	string value;
	string startAddress;
	string size;
	string flag;

	table() {};

	table(string n, int i, string t, int iOS, string v, string sA, string s, string f)
		: name(n), index(i), type(t), indexOfSection(iOS), value(v), startAddress(sA), size(s), flag(f) {
	};

} Table;

typedef struct relocation {
	string relocationAddress;
	string type;
	int index;

	relocation(string rA, string t, int i) : relocationAddress(rA), type(t), index(i) {}

} RelocationTable;

#endif
