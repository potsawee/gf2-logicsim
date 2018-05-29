/*  Unit test for the scanner class
    date: 26 May 2018
    updated: 29 May 2018 => showing string of the symbol name
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
    network my_network(&my_names);
    devices my_devices(&my_names, &my_network);
    monitor my_monitor(&my_names, &my_network);

    const char file_path[] = "rcounter";

    scanner my_scanner(&my_names, file_path);

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
