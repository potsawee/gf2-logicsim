#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <string>

#include "scanner.h"

using namespace std;

scanner::scanner(names* nmz, const char* defname) // nmz is a pointer to names class, so scanner can use it
{
	inf.open(defname); //open defname
	if(!inf){
		cout<< "Error: Failed to open file" << endl;
		exit(1);
	}
	inf(clear); //clear fail bits
	inf.seekg(0, ios::beg);//find the beginning of the file
	currentline.clear();
	linenum = 1;
	cout << "File opened successfully" << endl;
	// cursym = badsym;

	// pointer to names class
	_nmz = nmz;

}
scanner::~scanner()
{
	inf.close(); //close defname
}
void scanner::getsymbol(symbol& s, name& id, int& num)
{
	s = badsym;	//initialisation
	id = blankname;
	num = -1;

    skipspaces(&inf, curch, eofile); // curch is not a white-space

    if(eofile){
        s = eofsym;
		return;
	}

    if(isdigit(curch)) { //current symbol is a number
        s = numsym;
        getnumber(&inf, curch, eofile, num); 	// after this function is called,
												//curch becomes the character after the last digit
    }
    else if(isalpha(curch) || (curch == '_')) { //current symbol is a name
		bool is_keyword;

		getname(&inf, curch, eofile, id, is_keyword);

		if (is_keyword){ // DEVICES, CONNECTIONS, MONITORS
			switch (id) {
				case dev_id: s = devsym; break;
				case con_id: s = consym; break;
				case mon_id: s = monsym; break;
				default: break; //ERROR!!
			}
		}
		else { // not a keyword i.e. it is a user-defined name
			s = namesym;
			// id already set
		}

    }
    else { // neither number nor name
        switch (curch) {
            case '=': s = equals; break;
            case ';': s = semicol; break;
            case ',': s = comma; break;
            default:  s = badsym; break;
        }
    }
}

void scanner::getnumber(ifstream *infp, char& curch, bool& eofile, int& num)
{
	string s = "";
	while(!eofile){
		if(isdigit(curch)){
			s += curch;
			eofile = !((*infp).get(curch));
		}
		else {
			number = stoi(s);
			return;
		}
	}
	number = stoi(s);
	return;
}

void scanner::getname(ifstream *infp, char& curch, bool& eofile, name& id, bool& is_keyword)
{
	/*
	a string of letters could be
	1. name of devices, monitors (e.g. sw1, monitor3 etc) => is_keyword = false
	2. keyword (e.g. DEVICES, XOR, QBAR)				  => is_keyword = true
	*/
	namestring str = "";
	str += curch;

    while(!eofile){
		eofile = !((*infp).get(curch));
        if(isdigit(curch) || isalpha(curch) || curch == "_"){
            str += curch;
        }
        else {
            break;
        }

    }

	// check if the string is a keyword or not
	is_keyword = _nmz->is_keyword(str);

	if(is_keyword){ // if it is a keyword, id will be an id from keyword_table
		switch (str) {
			case "DEVICES": 		id = dev_id; break;
			case "CONNECTTIONS": 	id = con_id; break;
			case "MONITORS": 		id = mon_id; break;
			default:				id = -1; break; // ERROR!!
		}
	}
	else{
		id = _nmz->lookup(str);
	}

	// at this point curch is not alphabetic nor digit nor '_'
    return;
}


void scanner::getch()
{
	prevch = curch;
	eofile = (inf.get(curch) == 0);
	if (curch == '\n') linenum ++;
	if (eoline) {
		currentline.clear(); //clear the current line
		skipspaces();
		eoline = false;//reset eoline
	}
	if(prevch != '\n'){
		currentline.push_back(prevch);
	}
}

void scanner::skipspaces(ifstream *infp, char& curch, bool& eofile)
// return a non white-space character in curch
{
    eofile = !((*infp).get(curch));
    while(!eofile){
        if(isspace(curch))
            eofile = !((*infp).get(curch));
        else
            return;
    }
}

void scanner::skipcomments(ifstream *infp, char& curch, bool& eofile)
{
	if (curch =='/') {
		eofile = (inf.get(curch) == 0);
		if (curch == '\n') linenum ++;
		while (!eofile && prevch != '/') {
			prevch = curch;
			eofile = (inf.get(curch) == 0);
			if (curch == '\n') linenum ++;
		}
		if (eofile) {
			cout<<("Error: Comment not closed")<<endl;
		}
		skipspaces();
	}
}

string scanner::getline()
{
	if(cursym != semicol && cursym != colon && cursym != comma){
		while (curch !=':' && curch !=';' && curch !=',' && !eofile) {
			getch();
		}
		currentline.push_back(curch);
	}
	return currentline;
}
