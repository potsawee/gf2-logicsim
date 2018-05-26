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

typedef enum {namesym, numsym, devsym, consym, monsym, typesym, sigsym,
	closym, swisym, dtysym, andsym, nandsym, orsym, norsym, xorsym,
	comma, semicol, colon, equals, leftbrksym, rightbrksym, badsym, eofsym
	} symbol;
typedef int name;


class scanner {
private:
    ifstream inf; //input definition file
    char curch; //current input character
    string currentline;	//current line
	char prevch; //previous input character
    bool eofile; //true when end of file is reached
    bool eoline; //true when end of line is reached
    symbol cursym; //current symbol
    int symlength; //current symbol length
    int linenum; //line number

    void getch(); //reads next character, updates curch
    void skipspaces(); //skips spaces
    void skipcomments(); //skips comments
    string getline(); //gets current line
    void getnumber(int& num); //reads number 
    void getname(name& id); //reads name

public:
	symbol s;
	name id;
	int num;
    
	scanner(names* names_mod, // pointer to names class
            const char* defname); // name of the def file
            /*the constructor takes care of openning the definition file
            as well as initialisation any global scanner variables */

    ~scanner(); // destructor to close file

    void getsymbol(symbol& s,   // symbol read from the file
                   name& id,    // name id if it is a name
                   int& num);   // value if it is a number
    // return successive symbols in the definition file
};



#endif /* scanner_h */
