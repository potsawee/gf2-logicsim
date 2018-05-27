/*  Unit test for the parser
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

    const char file_path[] = "unittest_2_def";

    scanner my_scanner(&my_names, file_path);
    parser my_parser(&my_network, &my_devices, &my_monitor, &my_scanner, &my_names);

    cout << "Unit testing for the parser BEGIN..." << endl;

    if(my_parser.readin())
        cout << "the definition file is defined corectly!" << endl;
    else
        cout << "syntax or/and semantic error(s)" << endl;

    cout << "end of testing" << endl;

    return 0;
}
