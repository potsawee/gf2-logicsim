#include <iostream>
#include "parser.h"

using namespace std;

/* The parser for the circuit definition files */
bool parser::readin (void)
{
	// return true if definition file parsed OK

	smz->getsymbol(cursym, curid, curnum);
	// Some file may contain nothing!
	if(cursym == eofsym){ // prevent segmentation error!
		error(deffile_empty);
		return false;
	}

	/* Syntax error detection */
	/* Semantic error detection */
	try{
		if(cursym == devsym){ // "DEVICES:"
			smz->skipcolon();
			devicelist();
		}
		else{
			error(dev_key);
			smz->skipcolon();
			devicelist();
		}
		smz->getsymbol(cursym, curid, curnum);
		if(cursym == consym){ // "CONNECTIONS:"
			smz->skipcolon();
			connectionlist();
		}
		else{
			error(con_key);
			smz->skipcolon();
			connectionlist();
		}
		smz->getsymbol(cursym, curid, curnum);
		if(cursym == monsym){ // "MONITORS:"
			smz->skipcolon();
			monitorlist();
		}
		else{
			error(mon_key);
			smz->skipcolon();
			monitorlist();
		}
	}
	catch(...){
		/* if there are still other cases of errors not really detected
		detect the rest here and terminate the program
		this stops the program to detect further error down the file
		but it shoud be safer than uncaught error */
		cout << "Unexpected error occured.. terminate program safely." << endl;
	}
	smz->getsymbol(cursym, curid, curnum);
	if(cursym != eofsym){
		error(after_last_semicol);
	}
	if(errorcount == 0){
		// cout << "0 error dectected" << endl;
		return true;
	}
	else{
		cout << errorcount << " error(s) dectected" << endl;
		return false;
	}
}

parser::parser (network* network_mod, devices* devices_mod,
		monitor* monitor_mod, scanner* scanner_mod, names* names_mod)
{
	netz = network_mod;  /* make internal copies of these class pointers */
	dmz = devices_mod;   /* so we can call functions from these classes  */
	mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
	smz = scanner_mod;   /* class you say:                               */
	                   	 /* netz->makeconnection (i1, i2, o1, o2, ok);   */

  	/* more initialisation */
	nmz = names_mod;
	errorcount = 0;

}

/* 1. Devices */
void parser::devicelist()
{
	// cout << "DEVICELIST" << endl;
	device();
	while(1){
		while(cursym == comma){
			device();
		}
		if(cursym == semicol) // good
			break;
		else if(cursym == consym){
			error(semicol_missing);
		}
		else if(cursym == eofsym){
			error(deffile_incomplete);
			return;
		}
		else{
			smz->getsymbol(cursym, curid, curnum);
			error(comma_missing);
		}
	}

}
void parser::device() // scan up to ',' or ';'
{
	smz->getsymbol(cursym, curid, curnum); // expect SWITCH, CLOCK etc.
	if(cursym != namesym){
		error(device_def);
		return;
	}
	// here cursym == namesym, so curid tells the NAME

	devicekind dkind = dmz->devkind(curid);

	bool wrongkind = false;

	try{
		switch (dkind) {
			case aclock: 	clockdev(); 	break;
			case aswitch: 	switchdev(); 	break;
			case dtype: 	dtypedev(); 	break;
			case andgate: 	anddev(); 		break;
			case nandgate: 	nanddev(); 		break;
			case orgate: 	ordev(); 		break;
			case norgate: 	nordev(); 		break;
			case xorgate: 	xordev(); 		break;
			case notgate:	notdev();		break;
			case rccircuit:	rcdev();		break;
			default: wrongkind = true;		break;
		}
	}
	catch(errortype err){
		return; // error() already moved the symbol to ',' or ';'
	}
	if(!wrongkind)
		smz->getsymbol(cursym, curid, curnum);
	else
		error(device_def); //a device definition is expected
}
void parser::switchdev()
{
	devicekind dkind = aswitch;
	name did; // device id
	int variant;
	bool ok;
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		did = name1();
		smz->getsymbol(cursym, curid, curnum);
		if(cursym == leftbrk){
			smz->getsymbol(cursym, curid, curnum);
			if(cursym == numsym){
				if(curnum == 0 || curnum == 1){
					variant = curnum;
				}
				else{
					error(intial_state);
				}
				smz->getsymbol(cursym, curid, curnum);
				if(cursym != rightbrk){
					error(rightbracket);
				}
			}
			else {
				error(num_expected);
			}
		}
		else{
			error(leftbracket);
		}
	}
	else{
		error(equal_expected);
	}
	// Up to this stage the switch must have been defiened correctly
	// So make a call to makedevice
	dmz->makedevice(dkind, did, variant, ok);
	if(!ok){
		error(make_device);
	}
}
void parser::dtypedev()
{
	devicekind dkind = dtype;
	name did; // device id
	int variant=0;
	bool ok;
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		did = name1();
	}
	else{
		error(equal_expected);
	}
	// Up to this stage the dtype must have been defined correctly
	// So make a call to makedevice
	dmz->makedevice(dkind, did, variant, ok);
	if(!ok){
		error(make_device);
	}
}
void parser::xordev()
{
	devicekind dkind = xorgate;
	name did; // device id
	int variant = 0;
	bool ok;
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		did = name1();
	}
	else{
		error(equal_expected);
	}
	dmz->makedevice(dkind, did, variant, ok);
	if(!ok){
		error(make_device);
	}
}
void parser::clockdev()
{
	devicekind dkind = aclock;
	dev_name_num(dkind);
}
void parser::rcdev()
{
	devicekind dkind = rccircuit;
	dev_name_num(dkind);
}
void parser::anddev()
{
	devicekind dkind = andgate;
	dev_name_num(dkind);
}
void parser::nanddev()
{
	devicekind dkind = nandgate;
	dev_name_num(dkind);
}
void parser::ordev()
{
	devicekind dkind = orgate;
	dev_name_num(dkind);
}
void parser::nordev()
{
	devicekind dkind = norgate;
	dev_name_num(dkind);
}
void parser::notdev()
{
	devicekind dkind = notgate;
	name did; // device id
	int variant = 0;
	bool ok;
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		did = name1();
	}
	else{
		error(equal_expected);
	}
	dmz->makedevice(dkind, did, variant, ok);
	if(!ok){
		error(make_device);
	}
}
void parser::dev_name_num(devicekind dkind)
{
	name did; // device id
	int variant;
	bool ok;
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == equals){
		did = name1();
		smz->getsymbol(cursym, curid, curnum);
		if(cursym == leftbrk){
			smz->getsymbol(cursym, curid, curnum);
			if(cursym == numsym){

				if(dkind != aclock && dkind != rccircuit && (curnum > 16 || curnum <=0)){
					error(input_num);
				}

				variant = curnum;

				smz->getsymbol(cursym, curid, curnum);
				if(cursym != rightbrk){
					error(rightbracket);
				}
			}
			else {
				error(num_expected);
			}
		}
		else{
			error(leftbracket);
		}
	}
	else{
		error(equal_expected);
	}
	dmz->makedevice(dkind, did, variant, ok);
	if(!ok){
		error(make_device);
	}
}
name parser::name1()
{
	smz->getsymbol(cursym, curid, curnum);
	if (cursym == namesym){

		/* --------- Semantic error #1,2 ------- */
		// 1. two devices cannot have the same name
		devlink finddev = netz->finddevice(curid); // return NULL if not defined yet
		if(finddev)
		{
			semantic(dev_name_existed);
		}
		// 2. device names cannot be the same as reserved keyword
		if(is_name_reserved(curid))
		{
			semantic(dev_name_reserved);
		}
		/* ------------------------------------- */

		name deviceid = curid;
		return deviceid;
	}
	else if (cursym == devsym || cursym == consym || cursym == monsym){
		semantic(dev_name_reserved);
	}
	else {
		error(name_violate);
	}
	return -1; // it won't reach this just make warning disappear [error(8) throw error]
}
bool parser::is_name_reserved(name id)
{
	// To deal with semantic error #2
	namestring namestr = nmz->getname(id);
	if(nmz->is_keyword(namestr))
		return true;
	if(namestr == "SWITCH" || namestr == "CLOCK" || namestr == "AND" || namestr == "NAND" ||
	namestr == "OR" || namestr == "NOR" || namestr == "XOR" || namestr == "DTYPE" ||
	namestr == "RC" || namestr == "NOT" ||
	namestr == "DATA" || namestr == "CLK" || namestr == "SET" || namestr == "CLEAR" ||
	namestr == "Q" || namestr == "QBAR")
		return true;

	return false;
}

/* 2. Connections */
void parser::connectionlist()
{
	// cout << "CONNECTIONLIST" << endl;
	connection();
	while(1){
		while(cursym == comma){ // while loop breaks when cursym == semicol
			connection();
		}
		if (cursym == semicol)
			break;
		else if(cursym == monsym)
			error(semicol_missing);
		else if(cursym == eofsym){
			error(deffile_incomplete);
			return;
		}
		else{
			smz->getsymbol(cursym, curid, curnum);
			error(comma_missing);
		}
	}

	/* --------- Semantic error #3 --------- */
	// 3. every input should be connected to a (valid) output
	bool ok;
	netz->checknetwork(ok);
	if(!ok)
		semantic(make_conn);
	/* ------------------------------------- */
}
void parser::connection()
{
	/* change monday 28 may 2018
	connection: signalout => signalin
	e.g. d1.Q => xor1.I1
	------------------------- */
	name outputdev, outputport, inputdev, inputport;
	bool ok;
	try{
		signalout(outputdev, outputport);

		smz->getsymbol(cursym, curid, curnum); // expect '='
		if(cursym != equals)
			error(arrow_sym);
		smz->getsymbol(cursym, curid, curnum); // expect '>'
		if(cursym != greaterthan)
			error(arrow_sym);

		signalin(inputdev, inputport);

	}
	catch (errortype err){
		return; // error() already moved the symbol to ',' or ';'
	}

	smz->getsymbol(cursym, curid, curnum); // to move to cursym to ',' or ';'

	netz->makeconnection(inputdev, inputport, outputdev, outputport, ok);
	if(!ok){
		/* --------- Semantic error #4 --------- */
		// 4. Portnumber does not exist e.g. gate1.I20
		semantic(port_num_invalid);
		/* ------------------------------------- */
	}
}
void parser::signalout(name& dev, name& port)
{
	// note that only DTYPE has two output ports
	smz->getsymbol(cursym, curid, curnum);
	if(cursym == namesym){ // expect name to come first e.g. 'G1'
		dev = curid;
		/* find device kind */
		devlink dlink = netz->finddevice(dev); // return NULL if not defined yet
		/* --------- Semantic error #5 --------- */
		// Invalid device (not defined)
		if(!dlink)
		{
			semantic(dev_not_defined);
		}
		/* ------------------------------------- */
		devicekind dkind = dlink->kind;
		/* ---------------- */
		if(dkind == dtype){
			smz->getsymbol(cursym, curid, curnum);
			if(cursym == fullstop){
				dtypeout(port);
			}
			else
				error(dtype_out);
		}
		else {
			port = blankname;
		}
	}
	else { // if the first symbol when signame() is called is NOT name => error
		error(name_missing);
	}
}
void parser::signalin(name& dev, name& port)
{
	// note that only DTYPE, gates, xor have more than 1 input ports
	smz->getsymbol(cursym, curid, curnum);
	if(cursym == namesym){ // expect name to come first e.g. 'G1'
		dev = curid;
		/* find device kind */
		devlink dlink = netz->finddevice(dev);
		/* --------- Semantic error #5 --------- */
		// Invalid device (not defined)
		if(!dlink)
		{
			semantic(dev_not_defined);
		}
		/* ------------------------------------- */
		devicekind dkind = dlink->kind;
		/* ---------------- */
		if(dkind == dtype){
			smz->getsymbol(cursym, curid, curnum);
			if(cursym == fullstop){
				dtypein(port);
			}
			else
				error(dtype_in);
		}
		else if(dkind == xorgate){
			smz->getsymbol(cursym, curid, curnum);
			if(cursym == fullstop){
				gatein(port, dkind);
			}
			else
				error(xor_in);
		}
		else if(dkind == andgate || dkind == nandgate || dkind == orgate || dkind == norgate || dkind == notgate){
			smz->getsymbol(cursym, curid, curnum);
			if(cursym == fullstop){
				gatein(port, dkind);
			}
			else
				error(gate_in);
		}
		else { // CLOCK or SWITCH or RC
			error(noinput_expected);
		}

		/* --------- Semantic error #6 --------- */
		// This input port is already connected
		inplink ilink = netz->findinput(dlink, port);
		outplink conn = ilink->connect;
		if(conn != NULL)
		{
			semantic(port_connected); // throw an error
		}
		/* ------------------------------------- */

	}
	else { // if the first symbol when signame() is called is NOT name => error
		error(name_missing);
	}
}
void parser::dtypeout(name& port)
{
	name q    = nmz->lookup("Q");
	name qbar = nmz->lookup("QBAR");
	smz->getsymbol(cursym, curid, curnum); // expect Q or QBAR
	if (curid == q || curid == qbar){
		port = curid;
	}
	else {
		error(dtype_out);
	}
}
void parser::dtypein(name& port)
{
	name data = nmz->lookup("DATA");
    name clk  = nmz->lookup("CLK");
    name set  = nmz->lookup("SET");
    name clr  = nmz->lookup("CLEAR");
	smz->getsymbol(cursym, curid, curnum); // expect DATA, CLK, SET, CLEAR
	if (curid == data || curid == clk || curid == set || curid == clr){
		port = curid;
	}
	else {
		error(dtype_in);
	}
}
void parser::gatein(name& port, devicekind dkind)
{
	/* Improve->check is the port is valid */
	// gatein = ( "I" , number )
	smz->getsymbol(cursym, curid, curnum);
	if(dkind == notgate){ // Not gate input e.g. notgate.I
		if(nmz->lookup("I") == curid){
			// this is for not gate
			port = curid;
			return;
		}
		error(notgate_in);
	}
	else if(dkind == xorgate){ // XOR gate always have input I1, I2
		for(int i=1; i<=2; i++){
			string s = "I" + to_string(i);
			name checkid = nmz->lookup(s);
			if(checkid == curid){
				port = curid;
				return;
			}
		}
		error(xor_in);
	}
	else{ // the rest of the gates can have up to 16 inputs
		for(int i=1; i<=16; i++){
			string s = "I" + to_string(i);
			name checkid = nmz->lookup(s);
			if(checkid == curid){
				port = curid;
				return;
			}
		}
		error(gate_in);
	}
}

/* 3. Monitors */
void parser::monitorlist()
{
	// cout << "MONITORLIST" << endl;
	monitor1();
	while(cursym == comma){ // while loop breaks when cursym == semicol
		monitor1();
	}
	if (cursym != semicol)
		error(semicol_missing);
}
void parser::monitor1()
{
	name devid; 		// device id (to be monitored)
	name outputport; 	// output port of the deivce
	bool ok;

	/* --------- signal to be monitored --------- */
	try{
		signalout(devid, outputport);
	}
	catch (errortype err){
		return; // error() already moved the symbol to ',' or ';'
	}
	/* --------- signal to be monitored --------- */
	smz->getsymbol(cursym, curid, curnum); // expect ',' or ';'

	mmz->makemonitor(devid, outputport, ok);
	if(!ok){
		error(make_monitor);
	}
}

/* 4. Error Handling */
void parser::error(errornumber errn)
{
	errorcount++;
	if(errn <= 50){
		smz->skip_dueto_error(cursym, curid, curnum);
		cout << "***ERROR " << errn << ": " ;
	}
	switch(errn)
	{
		case undefined:
			cout << "undefined error" << endl; break;
		case semicol_missing:
			cout << "a semicolon is expected before this expression" << endl; break;
		case device_def:
			cout << "a device definition is wrong" << endl; break;
		case intial_state:
			cout << "Initial state is either 0 or 1" << endl;
			throw devicedeferror; break;
		case rightbracket:
			cout << "expect a ) symbol " << endl;
			throw devicedeferror; break;
		case num_expected:
			cout << "expect a number symbol" << endl;
			throw devicedeferror; break;
		case leftbracket:
			cout << "expect a ( symbol " << endl;
			throw devicedeferror; break;
		case equal_expected:
			cout << "expect an '=' symbol" << endl;
			throw devicedeferror; break;
		case name_violate:
			cout << "name error. hint: name must start with a letter or '_'" << endl;
			throw nameerror; break;
		case input_num:
			cout << "the maximum number of inputs is 16 and minimum is 1" << endl;
			throw devicedeferror; break;
		case notgate_in:
			cout << "not gate can only have one input so call it not.I (no number after I)" << endl;
			throw signalerror; break;
		case dtype_out:
			cout << "Q or QBAR is expected for dtype output" << endl;
			throw signalerror; break;
		case dtype_in:
			cout << "DATA, CLK, SET, CLEAR is expected for dtype input" << endl;
			throw signalerror; break;
		case xor_in:
			cout << "XOR input I1 or I2" << endl;
			throw signalerror; break;
		case gate_in:
			cout << "gate input I1 to I16" << endl;
			throw signalerror; break;
		case arrow_sym:
			cout << "expect => after signalout (or apart from dtype no port should be defined.)" << endl;
			throw signalerror; break;
		case noinput_expected:
			cout << "CLOCK/SWITCH/RC do not have an input" << endl;
			throw signalerror; break;
		case name_missing:
			cout << "name of a device is expected" << endl;
			throw signalerror; break;

		case make_device:
			cout << "Makedevice error" << endl; break;
		case make_monitor:
			cout << "monitor error. hint: check the signal" << endl; break;

		case after_last_semicol:
			cout << "Nothing is expected after the last ;" << endl; break;
		case comma_missing:
			cout << "a comma is expected before this expression" << endl; break;
		case deffile_incomplete:
			cout << "the definition file is not complete" << endl; break;

		// Special cases use number above 50 (enumeration)
		case dev_key: smz->print_line_error(8);
			cout << "***ERROR 51: 'DEVICES:'' keyword expected" << endl; break;
		case con_key: smz->print_line_error(8);
			cout << "***ERROR 52: 'CONNECTIONS:'' keyword expected" << endl; break;
		case mon_key: smz->print_line_error(8);
			cout << "***ERROR 53: 'MONITORS:'' keyword expected" << endl; break;
		case deffile_empty:
			cout << "the file is empty. nothing to parse" << endl; break;
	}
}
void parser::semantic(semanticnumber errn)
{
	errorcount++;
	if(errn == make_conn) // make connection case -> no need to print any line
		smz->skip_dueto_error(cursym, curid, curnum, false);
	else
		smz->skip_dueto_error(cursym, curid, curnum);

	// Semantic error will be shown above 100
	cout << "***ERROR " << errn << ": " ;
	switch (errn)
	{
		case dev_name_existed:
			cout << "device name invalid - two devices cannot have the same name" << endl;
			throw semanticerror; break;
		case dev_name_reserved:
			cout << "device name invalid - cannot use reserved keyword" << endl;
			throw semanticerror; break;
		case make_conn:
			cout << "CONNECTIONS: every input should be connected to a (valid) output" << endl;
			break;
		case port_num_invalid:
			cout << "Portnumber does not exist" << endl;
			break;
		case dev_not_defined:
			cout << "Invalid device (not defined)" << endl;
			throw semanticerror; break;
		case port_connected:
			cout << "This input port is already connected" << endl;
			throw semanticerror; break;
	}
}
