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
	try{
		while(cursym != eofsym){
			if(cursym == devsym){ // device
				devicelist();
			} else if(cursym == consym){ // "CONNECTIONS:"
				// CONNECTION = signame , "=" , signame;
				connectionlist();
			} else if(cursym == monsym){ // monitor
				monitorlist();
			}
			scanner::getsymbol(cursym, curid, curnum);
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
	scanner::getsymbol(cursym, curid, curnum);
	while(cursym == comma){
		device();
		scanner::getsymbol(cursym, curid, curnum);
	}
	if (cursym == semicol)
		scanner::getsymbol(cursym, curid, curnum);
	else
		error(1);

}
void parser::device()
{
	scanner::getsymbol(cursym, curid, curnum);
	switch (cursym) {
		case closym: 	clockdev(); 	break;
		case swisym: switchdev(); 	break;
		case dtysym: 	dtypedev(); 	break;
		case andsym: 	anddev(); 		break;
		case nandsym: 	nanddev(); 		break;
		case orsym: 	ordev(); 		break;
		case norsym: 	nordev(); 		break;
		case xorsym: 	xordev(); 		break;
		default: error(2);	break;
	}
}
void parser::clockdev()
{
	read_equal_name_num();
}
void parser::switchdev()
{
	scanner::getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		name();
		scanner::getsymbol(cursym, curid, curnum);
		if(cursym == leftbracketsym){
			scanner::getsymbol(cursym, curid, curnum);
			if(cursym == numsym){
				if(curnum == 0 || curnum == 1){
					// int state0 = curnum;
				}
				else
					error(3);
				scanner::getsymbol(cursym, curid, curnum);
				if(cursym != rightbracketsym)
					throw "error expect a ) symbol ";
			}
			else {
				throw "error expect a number symbol";
			}
		}
		else{
			throw "error expect a ( symbol";
		}
	}
	else{
		throw "error expect an equal symbol";
	}
}
void parser::dtypedev()
{
	scanner::getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		name();
	}
	else{
		throw "error expect an equal symbol";
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
	scanner::getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		name();
	}
	else{
		throw "error expect an equal symbol";
	}
}
void parser::read_equal_name_num()
{
	scanner::getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		name();
		scanner::getsymbol(cursym, curid, curnum);
		if(cursym == leftbracketsym){
			scanner::getsymbol(cursym, curid, curnum);
			if(cursym == numsym){
				// int freq = curnum;
				scanner::getsymbol(cursym, curid, curnum);
				if(cursym != rightbracketsym)
					throw "error expect a ) symbol ";
			}
			else {
				throw "error expect a number symbol";
			}
		}
		else{
			throw "error expect a ( symbol";
		}
	}
	else{
		throw "error expect an equal symbol";
	}
}
void parser::name()
{
	scanner::getsymbol(cursym, curid, curnum);
	if (cursym == namesym){
		// name id & names::writename(id)
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

		/* read the actual name from the name_table */
		// name id = curid
		// to print out the name names::writename(id)

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
void parser::monitorlist()
{
	monitor1();
	while(cursym == comma){ // while loop breaks when cursym == semicol
		monitor1();
	}
	if (cursym == semicol)
		scanner::getsymbol(cursym, curid, curnum);
	else
		throw "error: monitors";
}
void parser::monitor1()
{
	name();
	scanner::getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		signame();
	} else {
		throw "error: monitor";
	}
}
//
void parser::error(int errn)
{
	switch(errn)
	{
		case 1: cout << "a semicolon is expected" << endl; break;
		case 2: cout << "a device definition is expected" << endl; break;
		case 3: cout << "Initial state is either 0 or 1" << endl; break;





	}





}
