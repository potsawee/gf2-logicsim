#include <iostream>
#include <fstream>
#include "scanner.h"

using namespace std;

scanner::scanner(names* names_mod, const char* defname)
{
	inf.open(defname); //open defname
	if(!inf){
		displayerror("Error: Failed to open file");
		exit;
	}
	inf(clear); //clear fail bits
	inf.seekg(0, ios::beg); //
	

}
scanner::~scanner()
{
	inf.close(); //close defname
}
static void scanner::getsymbol(symbol& s, name& id, int& num)
{
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
void scanner::getch(char& curch)
{

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
