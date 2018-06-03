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
	// cout << "File opened successfully" << endl;

	/* append lines for report lines containing errors */
	string str;
	while(getline(inf, str)){
		lines.push_back(str);
	}
	inf.clear(); 					// clear eof state
	inf.seekg(0, inf.ios::beg);		// go to the beginning of the file
	/* -------------------------------------------------*/

	linenum = 0;
	eofile = !(inf.get(curch));

	// pointer to names class
	_nmz = nmz;
}
scanner::~scanner()
{
	inf.close(); //close defname
}
void scanner::getsymbol(symbol& s, name& id, int& num)
{
	// s = badsym;	//initialisation
	// id = blankname;
	// num = -1;

	skipspaces(); // curch is not a white-space

    if(eofile){
        s = eofsym;
		return;
	}

    if(isdigit(curch)) { //current symbol is a number
        s = numsym;
        getnumber(num); 	// after this function is called,
							//curch becomes the character after the last digit
    }
    else if(isalpha(curch) || (curch == '_')) { //current symbol is a name

		bool is_keyword;

		getname(id, is_keyword);

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
    else { // neither alphabetic nor '_' nor digit
        switch (curch) {
            case '=': s = equals; break;
            case ';': s = semicol; break;
            case ',': s = comma; break;
			case ':': s = colon; break;
			case '(': s = leftbrk; break;
			case ')': s = rightbrk; break;
			case '.': s = fullstop; break;
			case '>': s = greaterthan; break;
			case '/': s = slash;
						skipcomments();
						getsymbol(s, id, num);
						return;

			case '\n': s = eoline;
						linenum ++;
						getsymbol(s, id, num);
						return;

			default:  s = badsym; break;
        }
		eofile = !(inf.get(curch));
    }
}

void scanner::getnumber(int& number)
// This function also updates inf, curch, eofile.
{
	string s = "";
	while(!eofile){
		if(isdigit(curch)){
			s += curch;
			eofile = !(inf.get(curch));
		}
		else {
			number = stoi(s);
			return;
		}
	}
	number = stoi(s);
	return;
}

void scanner::getname(name& id, bool& is_keyword)
// This function also updates inf, curch, eofile.
{
	/*
	a string of letters could be
	1. name of devices, monitors (e.g. sw1, monitor3 etc) => is_keyword = false
	2. keyword (e.g. DEVICES, XOR, QBAR)				  => is_keyword = true
	*/
	namestring str = "";
	str += curch;

    while(!eofile){
		eofile = !(inf.get(curch));
        if(isdigit(curch) || isalpha(curch) || curch == '_'){
            str += curch;
        }
        else {
            break;
        }
    }

	// check if the string is a keyword or not
	is_keyword = _nmz->is_keyword(str);

	if(is_keyword){ // if it is a keyword, id will be an id from keyword_table
		if(str == "DEVICES")
			id = dev_id;
		else if(str == "CONNECTIONS")
			id = con_id;
		else if(str == "MONITORS")
			id = mon_id;
		else
			id = -1; //ERROR!!
	}
	else{
		id = _nmz->lookup(str);
	}

	// at this point curch is not alphabetic nor digit nor '_'
    return;
}

void scanner::skipspaces()
// return a non white-space character in curch
// This function updates inf, curch, eofile
{
    while(!eofile){
        if(isspace(curch)){
			if(curch == '\n'){
				linenum++;
			}
			eofile = !(inf.get(curch));
		}
        else
            return;
    }
}

void scanner::skipcomments()
{
	while(!eofile){
		eofile = !(inf.get(curch));
		if (curch == '/'){
			while (!eofile && curch != '\n'){
				eofile = !(inf.get(curch));
				if (curch == '\n') {

					//cout <<"It's the end of a line"<<endl;
					// cout << "linenum2++" << endl;
					// linenum ++; @potsawee DOUBLE counting!
					break;
				}
			}
		}
		else if (curch == '*'){
			//skip to '*/'
			prevch = curch;
			while (!eofile && !(prevch == '*' && curch == '/')){
				prevch = curch;
				eofile = !(inf.get(curch));
				if (curch == '\n'){
					linenum ++;
				}
				if (eofile)
 					cout<<("Error: Comment not closed")<<endl;
			}
			prevch = curch;
			eofile = !(inf.get(curch));
		}
		return;
	}
}

void scanner::skipcolon()
{
	if(curch == ':')
		eofile = !(inf.get(curch));
}
void scanner::skip_dueto_error(symbol& s, name& id, int& num, bool print)
{

	if(print){
		/* Find how many char from line beginning to current location */
		int counter = 0;
		char mychar;
		streampos curposition = inf.tellg();
		inf.get(mychar);
		while(mychar!='\n'){
			if(!inf.get(mychar)) //if eofile
				break;
			counter++;
		}
		inf.seekg(curposition);
		//cout << "counter = " << counter << endl;
		int position = lines[linenum].size() - counter;
		/* --------------------------------------------------------- */
		print_line_error(position);

	}
	if(curch == ','){
		s = comma;
		eofile = !(inf.get(curch));
		return;
	}
	else if(curch == ';'){
		s = semicol;
		eofile = !(inf.get(curch));
		return;
	}
	eofile = !(inf.get(curch));
	if(curch != '\n'){
		while(!(eofile || s == comma || s == semicol || curch == '\n')){
			getsymbol(s, id, num);
		}
	}

}

void scanner::print_line_error(int n)
{
	cout << "In line " << (linenum+1) << ": " << lines[linenum] << endl;
	int x = int((linenum+1) / 10); // to account for line1 and line10, line100 etc.
	cout << string(9 + x , ' ') << string(n, ' ') << "^" << endl;
}

// Functions for unit testing
void scanner::print_curch()
{
	cout << "curch: '" << curch << "'" << endl;
}
bool scanner::is_eofile()
{
	return eofile;
}
