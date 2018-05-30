/*  Unit test for the 'PARSER' class

    Testing:
    1. This program is intended to check if the parser works correctly
       e.g. if it reports all the errors, etc.
    2. If a definition file contains errors, their types will be printed out

    Usage:
    1. link this unittest to the names, scanner, devices, monitor, and networks classes
       e.g. g++ -g -o unittest_2 unittest_2.o scanner.o names.o parser.o network.o devices.o monitor.o
    2. ./unittest_2 [filename]

    date: 26 May 2018
    @Potsawee
*/

#include "scanner.h"
#include "names.h"
#include "parser.h"
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

    if (argc != 2) {
        cout << "Usage:         " << argv[0] << " [filename]" << endl;
        exit(1);
    }

    cout << "Definition file = " << argv[1] << endl;

    scanner my_scanner(&my_names, argv[1]);
    parser my_parser(&my_network, &my_devices, &my_monitor, &my_scanner, &my_names);

    cout << "--------------------------------------------" << endl;
    cout << "Unit testing for the parser BEGIN..." << endl;

    if(my_parser.readin())
        cout << "=> the definition file is defined corectly!" << endl;

    cout << "End of testing..." << endl;
    cout << "--------------------------------------------" << endl;

    return 0;
}
