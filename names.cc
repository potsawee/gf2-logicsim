#include "names.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;


/* Name storage and retrieval routines */

names::names(void)  /* the constructor */
{
    /* over to you */
    // initialise
    name_num = 0;

}

name names::lookup (namestring str)
{
    /* over to you */
    // check if the name already exists
    for(int i=0; i<name_num; i++){
        if(name_table[i] == str)
            return i;
    }
    // the name does not exist yet
    name_table[name_num] = str;
    name_num += 1;
    return name_num-1;
}

name names::cvtname (namestring str)
{
    /* over to you */
    // check if the name already exists
    for(int i=0; i<name_num; i++){
        if(name_table[i] == str)
            return i;
    }
    // not found
    return blankname;
}

void names::writename (name id)
{
    /* over to you */
    if(id >= 0 && id < name_num)
        cout << name_table[id] << endl;
    else
        cout << "(names::writename) the id is out of range.";
}

int names::namelength (name id)
{
    /* over to you */
    if(id >= 0 && id < name_num)
        return name_table[id].length();
    else
        cout << "(names::namelength) the id is out of range.";
        return -1;

}
