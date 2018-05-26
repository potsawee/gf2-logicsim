#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <string>
#include "scanner.h"
#include "names.h"

using namespace std;

scanner::scanner(names* names_mod, const char* defname)
{
	inf.open(defname); //open defname
	if(!inf){
		cout<< "Error: Failed to open file" << endl;
		exit;
	}
	inf.clear(); //clear fail bits
	inf.seekg(0, ios::beg);//find the beginning of the file
	currentline.clear();
	linenum = 1;
	cout << "File opened successfully"<< endl;
	s = badsym;
	
}
scanner::~scanner()
{
	inf.close(); //close defname
}
void scanner::getsymbol(symbol& s, name& id, int& num)
{
	s = badsym;//initialisation
	id = blankname;
	num = 0;
	
    skipspaces();
    if(eofile)
        s = eofsym;
    else {
        if(isdigit(curch)) { //current symbol is a number 
            s = numsym;
            getnumber(num);
        }
        else {
            if (isalpha(curch) || (curch == '_')) { //current symbol is a name
                getname(id);
                if (id == 0) s = devsym; else
				if (id == 1) s = consym; else
				if (id == 2) s = monsym; else
				if (id >= 3 && id <= 10) s = typesym; else
				if(id >=11 && id <= 32) s = sigsym; else
				s = namesym;   
            }
            else { // neither number nor name
                switch (curch) {
                    case '=': s = equals; break;
                    case ';': s = semicol; break;
                    case ',': s = comma; break;
                    default:  s = badsym; break;
                }
                symlength = 1;
                getch();
                if (prevch==':' || prevch==';' || prevch==','){
					eoline = true;
				}
            }
        }
    }
    cursym = s;
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

void scanner::getnumber(int &number)
{
	symlength = 0;
	number = 0; 
	string num = "";
	while (isdigit(curch)) { 
	  num+=curch;
          getch();
	  symlength++;
	}
	number = atoi(num.c_str());
}

void scanner::getname(name &id)
{
	symlength = 0;
	int i = 1;
	namestring str;
	while (isalnum(curch)) { 
		str.push_back(curch) ;
		symlength++;
		if (i == maxlength) { 			// Continue reading string
			id = names_mod->lookup(str);
		}
		i++;
		getch();	
	}
	if (i < maxlength) { 		// If str < maxlength, put/find in table
		id = names_mod->lookup(str);
	} else { 					//if str > maxlength, inform the user
	 //error message here
	}
}

void scanner::skipspaces()
{
    eofile = (inf.get(curch) == 0);
    while(!eofile){
        if(isspace(curch))
            eofile = (inf.get(curch) == 0);
        else
            return;
    }
}

void scanner::skipcomments()
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


