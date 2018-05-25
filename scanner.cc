#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <string>
#include "scanner.h"

using namespace std;

scanner::scanner(names* names_mod, const char* defname)
{
	inf.open(defname); //open defname
	if(!inf){
		cout<< "Error: Failed to open file" << endl;
		exit;
	}
	inf(clear); //clear fail bits
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
static void scanner::getsymbol(symbol& s, name& id, int& num)
{
	s = badsym;//initialisation
	id = blankname;
	num = 0;
	
    skipspaces(&inf, curch, eofile);
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
                if (id == devicename)
                    s = devsym;
                else {
                	if (id == connectionname){
                		s = consym;
                		else
                		s = namesym;
					}
                	
				}
                    
            }
            else { // neither number nor name
                switch (curch) {
                    case '=': s = equals; break;
                    case ';': s = semicol; break;
                    case ',': s = comma; break;
                    default:  s = badsym; break;
                }
                getch(curch);
            }
        }
    }
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


