#include "names.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

using namespace std;


/* Name storage and retrieval routines */

names::names(void)  /* the constructor */
{
	// there are 3 keywords!!
	// other special names are added to name_table in devices.cc
	keyword_table.push_back("DEVICES");
	keyword_table.push_back("CONNECTIONS");
	keyword_table.push_back("MONITORS");
}

name names::lookup (namestring str)
{
	/* This function returns the name corresponding to the given string. If the name does not exist yet, it will be created*/
	if (str == "") return blankname;

	name id = cvtname(str); // id = blankname unless it already exists

	if (id == blankname){ 	//if the name does not exist yet, create it
		name_table.push_back(str); //store the name in the nametable
		return name_table.size() - 1;
	} else {				// the name already exists
		return id;
	}
}

name names::cvtname (namestring str)
{
	/* This function returns the name of a given string only if that name has previously
	been stored in the name table, otherwise it returns blankname */
    // check if the name already exists
    for(int i=0; i<name_table.size(); i++){
        if(name_table[i] == str)
            return i;
    }
    // not found
    return blankname;
}

void names::writename (name id)
{
    if(id >= 0 && id < name_table.size())
        cout << name_table[id] << endl;
    else
        cout << "(names::writename) the id is out of range." << endl;
}

namestring names::getname (name id)
{
    if(id >= 0 && id < name_table.size())
        return name_table[id];
    else
        return "";
}

int names::namelength (name id)
{
    if(id >= 0 && id < name_table.size())
        return name_table[id].length();
    else
        cout << "(names::namelength) the id is out of range." << endl;
        return -1;

}

bool names::is_keyword (namestring str)
{
	for(int i=0; i<keyword_table.size(); i++){
		if(str == keyword_table[i])
			return true;
	}
	return false;

}
