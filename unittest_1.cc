/*  Unit test for the scanner class
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
int main(int argc, char **argv)
{
    names my_names;
    network my_network(&my_names);
    devices my_devices(&my_names, &my_network);
    monitor my_monitor(&my_names, &my_network);

    const char file_path[] = "unittest_1_def";

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

        cout << "cursym: " << cursym << endl;
        cout << "curid: " << curid << endl;
        cout << "curnum: " << curnum << endl;

        if(cursym == namesym)
        {
            cout << "Namesym received: ";
            my_names.writename(curid);
        }

        my_scanner.print_curch();

        cout << "continue?? (y/n): ";
        cin >> cont;

    }

    cout << "end of testing" << endl;

    return 0;
}
