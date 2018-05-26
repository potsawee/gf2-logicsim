#include <iostream>
#include "parser.h"

using namespace std;

/* The parser for the circuit definition files */


bool parser::readin (void)
{
	/* over to you */
	// return true if definition file parsed OK

	smz->getsymbol(cursym, curid, curnum);

	/* Syntax error detection */
	try{
		while(cursym != eofsym){
			if(cursym == devsym){ // "DEVICES:"
				devicelist();
			} else if(cursym == consym){ // "CONNECTIONS:"
				// CONNECTION = signame , "=" , signame;
				connectionlist();
			} else if(cursym == monsym){ // "MONITORS:"
				monitorlist();
			}
			smz->getsymbol(cursym, curid, curnum);
		}
	} catch (const char* msg) {
		cerr << msg << endl;
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
void parser::devicelist()
{
	device();
	smz->getsymbol(cursym, curid, curnum);
	while(cursym == comma){
		device();
		smz->getsymbol(cursym, curid, curnum);
	}
	if (cursym == semicol)
		smz->getsymbol(cursym, curid, curnum);
	else
		error(1);

}
void parser::device()
{
	smz->getsymbol(cursym, curid, curnum);
	if(cursym != namesym){
		error(0);
		return;
	}
	// here cursym == namesym, so curid tells the NAME
	devicekind _devkind = dmz->devkind(curid);

	switch (_devkind) {
		case aclock: 	clockdev(); 	break;
		case aswitch: 	switchdev(); 	break;
		case dtype: 	dtypedev(); 	break;
		case andgate: 	anddev(); 		break;
		case nandgate: 	nanddev(); 		break;
		case orgate: 	ordev(); 		break;
		case norgate: 	nordev(); 		break;
		case xorgate: 	xordev(); 		break;
		default: error(2);	break; //a device definition is expected
	}
}
void parser::clockdev()
{
	read_equal_name_num();
}
void parser::switchdev()
{
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		name();
		smz->getsymbol(cursym, curid, curnum);
		if(cursym == leftbrk){
			smz->getsymbol(cursym, curid, curnum);
			if(cursym == numsym){
				if(curnum == 0 || curnum == 1){
					// int state0 = curnum;
				}
				else
					error(3);
				smz->getsymbol(cursym, curid, curnum);
				if(cursym != rightbrk)
					error(4);
			}
			else {
				error(5);
			}
		}
		else{
			error(6);
		}
	}
	else{
		error(7);
	}
}
void parser::dtypedev()
{
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		name();
	}
	else{
		error(7);
	}
}
void parser::anddev()
{
	read_equal_name_num();
}
void parser::nanddev()
{
	read_equal_name_num();
}
void parser::ordev()
{
	read_equal_name_num();
}
void parser::nordev()
{
	read_equal_name_num();
}
void parser::xordev()
{
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		name();
	}
	else{
		error(7);
	}
}
void parser::read_equal_name_num()
{
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		name();
		smz->getsymbol(cursym, curid, curnum);
		if(cursym == leftbrk){
			smz->getsymbol(cursym, curid, curnum);
			if(cursym == numsym){
				// int freq = curnum;
				smz->getsymbol(cursym, curid, curnum);
				if(cursym != rightbrk)
					error(4);
			}
			else {
				error(5);
			}
		}
		else{
			error(6);
		}
	}
	else{
		error(7);
	}
}
void parser::name()
{
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == namesym){
		// name id & names::writename(id)
		// TODO: Comple this
		// For testing

		cout << "parser::name() => " << curid << endl;
		/////////////
	}
	else {
		throw "name error";
	}
}
/* 2. Connections */
void parser::connectionlist()
{
	connection();
	while(cursym == comma){ // while loop breaks when cursym == semicol
		connection();
	}
	if (cursym == semicol)
		smz->getsymbol(cursym, curid, curnum);
	else
		throw "error: connections";
}
void parser::connection()
{
	signame();
	if (cursym == equals){
		smz->getsymbol(cursym, curid, curnum);
		signame();
	} else {
		throw "error: connection";
	}
}
void parser::signame()
{
	smz->getsymbol(cursym, curid, curnum);
	if(cursym == namesym){ // expect name to come first

		devicekind _devkind = dmz->devkind(curid);
		//TODO: finish this up!!
		if(_devkind == dtype){
			smz->getsymbol(cursym, curid, curnum); //expect '.'
			if(cursym == fullstop){
				smz->getsymbol(cursym, curid, curnum); //expect "DATA" etc..
				portname();
			}
			else{

			}

		}

		smz->getsymbol(cursym, curid, curnum);
		if(cursym == fullstop){
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
	// TODO: Complete this!!
	// datapin = nmz->cvtname("DATA");
	// clkpin  = nmz->cvtname("CLK");
	// setpin  = nmz->cvtname("SET");
	// clrpin  = nmz->cvtname("CLEAR");
	// qpin    = nmz->cvtname("Q");
	// qbarpin = nmz->cvtname("QBAR");





}
/* 3. Monitors */
void parser::monitorlist()
{
	monitor1();
	while(cursym == comma){ // while loop breaks when cursym == semicol
		monitor1();
	}
	if (cursym == semicol)
		smz->getsymbol(cursym, curid, curnum);
	else
		throw "error: monitors";
}
void parser::monitor1()
{
	name();
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		signame();
	} else {
		throw "error: monitor";
	}
}
//
void parser::error(int errn=0)
{
	switch(errn)
	{
		case 0: cout << "undefined error" << endl; break;
		case 1: cout << "a semicolon is expected" << endl; break;
		case 2: cout << "a device definition is expected" << endl; break;
		case 3: cout << "Initial state is either 0 or 1" << endl; break;
		case 4: cout << "error expect a ) symbol " << endl; break;
		case 5: cout << "expect a number symbol" << endl; break;
		case 6: cout << "error expect a ( symbol " << endl; break;
		case 7: cout << "error expect an equal symbol" << endl; break;

	}

}
