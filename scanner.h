/* scanner
1. to translate the sequence of characters in the definition file
into a sequence of 'symbols'
2. symbols => keyword, name, number, punctuation
3. skipping comments, spaces, line breaks
*/

#ifndef scanner_h
#define scanner_h

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "names.h"

using namespace std;

typedef enum {namesym, numsym, devsym, consym, monsym,
	closym, swisym, dtysym, andsym, nandsym, orsym, norsym, xorsym,
	comma, semicol, equals, leftbrksym, rightbrksym, badsym, eofsym
	} symbol;
typedef int name;


class scanner {
private:
    ifstream inf; //input definition file
    char curch; //current input character
    bool eofile; //true when end of file is reached

    void getch(char& curch); //reads next character, updates curch
    void skipspaces(ifstream *infp, char& curch, bool& eofile); //skips spaces
    void skipcomments(); //skips comments
    void getnumber(int& num); //reads number 
    void getname(name& id); //reads name
    void displayerror(string errormessage); //displays error messages

public:
    scanner(names* names_mod, // pointer to names class
            const char* defname); // name of the def file
            /*the constructor takes care of openning the definition file
            as well as initialisation any global scanner variables */

    ~scanner(); // destructor to close file

    static void getsymbol(symbol& s,   // symbol read from the file
                   name& id,    // name id if it is a name
                   int& num);   // value if it is a number
    // return successive symbols in the definition file
};



#endif /* scanner_h */
