/* scanner
1. to translate the sequence of characters in the definition file
into a sequence of 'symbols'
2. symbols => keyword, name, number, punctuation
3. skipping comments, spaces, line breaks
*/

#ifndef scanner_h
#define scanner_h

#include "names.h"
using namespace std;

typedef enum {namesym, numsym, devsym, consym,
    monsym, endsym, comma, semicol, equals, badsym, eofsym} symbol;
typedef int name;


class scanner {
private:
    ifstream inf;
    char curch; //current input character
    bool eofile;

    void getch(char& curch);
    void skipspaces(ifstream *infp, char& curch, bool& eofile);
    void getnumber(int& num);
    void getname(name& id);

public:
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
