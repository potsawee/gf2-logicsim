#ifndef parser_h
#define parser_h

#include "names.h"
#include "scanner.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"


using namespace std;

// error to be throw by error() and semantic()
// if time allows, could distingiush them further
typedef enum {nameerror, devicedeferror, signalerror, semanticerror
} errortype;


class parser {
private:
    network* netz; // instantiations of various classes for parser to use.
    devices* dmz;
    monitor* mmz;
    scanner* smz; // internal pointer to the scanner
    names*   nmz;

    symbol cursym;
    name curid;
    int curnum;
    int errorcount;

    /* Private routines */
    /* 1. Devices */
    void devicelist();
    void device();
    void clockdev();
    void switchdev();
    void dtypedev();
    void anddev();
    void nanddev();
    void ordev();
    void nordev();
    void xordev();
    void dev_name_num(devicekind dkind);   // to reduce repetition of the above devices
    name name1();         // use name1 to avoid same name as name (int)


    /* 2. Connections */
    void connectionlist();
    void connection();
    void signalout(name& dev, name& port);
    void signalin(name& dev, name& port);
    void dtypeout(name& port);
    void dtypein(name& port);
    void gatein(name& port);

    /* 3. Monitors */
    void monitorlist();
    void monitor1();      // use monitor1 to avoid same name as monitor


    /* 4. Error Handling */
    void error(int errn); // a function to throw error
    void semantic(int errn); // for semantic error dectection
    bool is_name_reserved(name id); //check if name is reserved

 public:
    bool readin ();
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */

    parser (network* network_mod, devices* devices_mod,
      monitor* monitor_mod, scanner* scanner_mod, names* names_mod);
    /* the constructor takes pointers to various other classes as parameters */

};

#endif /* parser_h */
