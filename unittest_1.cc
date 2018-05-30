/*  Unit test for the 'SCANNER' class

    Testing:
    1. This program is intended to print out symbols sent from
    the scanner to the parser one by one
    2. If a symbol is 'namesym' the corresponding name will also be printed
    3. If a symbol is 'numsym' the corresponding number will also be printed

    Usage:
    1. link this unittest to the names and scanner classes
       e.g. g++ -g -o unittest_1 unittest_1.o scanner.o names.o
    2. ./unittest_1 [filename]

    date: 26 May 2018
    @Potsawee
*/

#include "scanner.h"
#include "names.h"
#include "devices.h"
#include "network.h"
#include "monitor.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

string symnum_to_symstring(int num);

int main(int argc, char **argv)
{
    names my_names;

    if (argc != 2) {
        cout << "Usage:         " << argv[0] << " [filename]" << endl;
        exit(1);
    }

    // argv[1] contains the file path
    cout << "Definition file = " << argv[1] << endl;
    scanner my_scanner(&my_names, argv[1]);

    char cont = 'y';
    cout << "continue?? (y/n): ";
    cin >> cont;
    while(cont == 'y')
    {
        symbol cursym;
        name curid;
        int curnum;

        my_scanner.getsymbol(cursym, curid, curnum);

        if(my_scanner.is_eofile()){
            cout << "end of file is reached!" << endl;
            break;
        }

        cout << "cursym: " << symnum_to_symstring(cursym) << endl;

        if(cursym == namesym)
        {
            cout << "Namesym received: ";
            cout << "curid: " << curid << endl;
            my_names.writename(curid);
        }
        else if(cursym == numsym)
        {
            cout << "curnum: " << curnum << endl;
        }

        // my_scanner.print_curch();
        cout << "--------------------------------" << endl;
        cout << "continue?? (y/n): ";
        cin >> cont;

    }

    cout << "end of testing" << endl;

    return 0;
}

string symnum_to_symstring(int num)
{
    switch (num) {
        case 0: return "namesym";
    	case 1: return "numsym";
        case 2: return "devsym";
        case 3: return "consym";
        case 4: return "monsym";
    	case 5: return "comma";
    	case 6: return "semicol";
    	case 7: return "colon";
    	case 8: return "equal";
    	case 9: return "leftbracket";
    	case 10: return "rightbracket";
    	case 11: return "fullstop";
    	case 12: return "greaterthan";
    	case 13: return "badsym";
    	case 14: return "eofsym";
        default: return "undefined sym!!";
    }
}
