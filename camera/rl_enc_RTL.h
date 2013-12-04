//*  rl_enc.h */
#include <systemc.h>
#include "add2systemc.h"

SC_MODULE(rl_enc_RTL) {


	// ports

	sc_out< sc_int<32> >	output;
	sc_in<bool>				ask_o;
	sc_out<bool>			ready_o;
	sc_in< sc_int<32> >		input;
	sc_in<bool>				ready_i;
	sc_out<bool>			ask_i;
	sc_in<bool>				clk;
	sc_in<bool>				reset;

// variables as signals
	enum		ctrl_state {RESET, READZIGZAG, WRITEDC, READZEROS, COUNTING , WRITECOUNT, WRITEVALUE, TERMINATION};
	sc_signal<ctrl_state>	state;//	ctrl_state	state;
	sc_signal< sc_int<32> >	count;
	sc_signal< sc_int<32> >	increment;
	sc_signal< sc_int<32> >	value;

	void process_RTL();

  SC_CTOR(rl_enc_RTL){
			SC_METHOD(process_RTL);
			sensitive << clk.pos();
  }

};

