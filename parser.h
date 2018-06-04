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
// and handle each of these differently
typedef enum {nameerror, devicedeferror, signalerror, semanticerror
} errortype;

// these enums are to be called by error(errornumber errn)
typedef enum
{
    undefined           = 0, // undefined error (for testing!)
    semicol_missing     = 1, // semicol missing
    device_def          = 2, // when define unexpected device type
    intial_state        = 3, // initial state either 0 or 1
    rightbracket        = 4, // ) mising
    num_expected        = 5, // number is expected
    leftbracket         = 6, // ( missing
    equal_expected      = 7, // = missing
    name_violate        = 8, // name violates the grammar
    input_num           = 12, // gate expected 1 to 16 to be number of input
    dtype_out           = 14, // dtype output wrong
    dtype_in            = 15, // dtype input wrong
    xor_in              = 16, // xor input either I1 or I2
    gate_in             = 17, // gate input I1 to I16 (or just I for not gate)
    arrow_sym           = 18, // '=>' expected
    noinput_expected    = 19, // CLOCK/SWITCH/RC do not have an input
    name_missing        = 20, // name symbol is expected
    make_device         = 31, // error from call to makedevice
    make_monitor        = 33, // error from call to makemonitor
    after_last_semicol  = 41, // if there is something after the last ;
    comma_missing       = 42, // comma missing
    deffile_incomplete  = 43, // def file is incomplete
    // above 50 are special errors to be handled differently
    dev_key             = 51, // DEVICES expected
    con_key             = 52, // CONNECTIONS expected
    mon_key             = 53, // MONITORS expected
    deffile_empty       = 61, // def file is empty

} errornumber;

typedef enum // define anothet typedef to get rid of the warning when call these
             // two different functions
{
    // Semantic error!!
    dev_name_existed    = 101, // two devices cannot have the same name
    dev_name_reserved   = 102, // reserved name is used
    make_conn           = 103, //error from call to makeconnection
    port_num_invalid    = 104, //portnumber does not exist
    dev_not_defined     = 105, // device not defined yet
    port_connected      = 106 // this input already connected
} semanticnumber;


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
    void notdev();
    void rcdev();
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
    void error(errornumber errn); // a function to throw error
    void semantic(semanticnumber errn); // for semantic error dectection
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
