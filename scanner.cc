#include <iostream>
#include <fstream>
#include "scanner.h"

using namespace std;

scanner::scanner(names* names_mod, const char* defname)
{

}
scanner::~scanner()
{

}
void scanner::getsymbol(symbol& s, name& id, int& num)
{
    skipspaces(&inf, curch, eofile);
    if(eofile)
        s = eofsym;
    else {
        if(isdigit(curch)) {
            s = numsym;
            getnumber(num);
        }
        else {
            if (isalpha(curch)) { // name
                // getname(id);
                // if (id == devicename)
                //     s = devsym;
                // else
                //     (id == connectionname)
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
