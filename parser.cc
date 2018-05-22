#include <iostream>
#include "parser.h"

using namespace std;

/* The parser for the circuit definition files */


bool parser::readin (void)
{
	/* over to you */
	// return true if definition file parsed OK

	scanner::getsymbol(cursym, curid, curnum);

	/* Syntax error detection */
	while(cursym != eofsym){
		if(cursym == devsym){ // device

		} else if(cursym == consym){ // "CONNECTIONS:"
			// CONNECTION = signame , "=" , signame;
			connections();
		} else if(cursym == monsym){ // monitor

		}
	}
	/* Semantic error detection */
	return true;
}

parser::parser (network* network_mod, devices* devices_mod,
		monitor* monitor_mod, scanner* scanner_mod)
{
	netz = network_mod;  /* make internal copies of these class pointers */
	dmz = devices_mod;   /* so we can call functions from these classes  */
	mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
	smz = scanner_mod;   /* class you say:                               */
	                   /* netz->makeconnection (i1, i2, o1, o2, ok);   */

  	/* any other initialisation you want to do? */

}


/* Private routines */
/* 1. Devices */

/* 2. Connections */
void parser::connections()
{
	connection();
	while(cursym == comma){ // while loop breaks when cursym == semicol
		connection();
	}
	if (cursym == semicol)
		scanner::getsymbol(cursym, curid, curnum);
	else
		throw "error: connections";
}
void parser::connection()
{
	signame();
	if (cursym == equals){
		scanner::getsymbol(cursym, curid, curnum);
		signame();
	} else {
		throw "error: connection";
	}
}
void parser::signame()
{
	scanner::getsymbol(cursym, curid, curnum);
	if(cursym == namesym){ // expect name to come first
		scanner::getsymbol(cursym, curid, curnum);
		if(cursym == dotsym){
			portname();
		} else {
			// this is OK. some devices do not have any port
		}
	}
	else { // if the first symbol when signame() is called is NOT name => error
		throw "error: signame";
	}

}
void parser::portname()
{
	// Complete this!!
}

/* 3. Monitors */
