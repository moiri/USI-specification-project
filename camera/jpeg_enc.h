/*  jpeg_enc.h */
#ifndef _JPEG_ENC
#define _JPEG_ENC

#include        "r2b.h"
#include        "dct.h"
#include        "quantize1.h"
#include        "zz_enc1.h"
#include        "rl_enc1.h"
#include        "bit_packing.h"
#include        "P2FF.h"
#include        "FF2P.h"

#define      block1   1
#define     block2    1
#define     block3    1

SC_MODULE(jpeg_enc) {

    /* in and output */
    my_fifo_in<int>   input;
    my_fifo_in<int>   parameters;
    my_fifo_out<int>  output;
    sc_in<bool>           clk;

    /* internal FIFOs */
    fifo_stat<int>        r2b_out;
    fifo_stat<float>      dct_out;
    fifo_stat<int>        zz_enc_out;

    /* internal modules */
    r2b r2b_1;
    dct dct_1;
    quantize1 quant_1;
    zz_enc1 zz_enc_1;
    rl_enc1 rl_enc_1;

    FF2P<float> ff2p_dct_quantize;
    fifo_stat<float>    dct_quantize_out;
    sc_signal<float>  float_dct_quantize;
    sc_signal<bool> bool_dct_quantize_ask;
    sc_signal<bool> bool_dct_quantize_ready;

//    P2FF<int> p2ff_quantize_zz;
    fifo_stat<int>    quantize_zz_out;
    sc_signal<int>  int_quantize_zz;
    sc_signal<bool> bool_quantize_zz_ask;
    sc_signal<bool> bool_quantize_zz_ready;

//    FF2P<int> ff2p_zz_rl_enc;
//    P2FF<int> p2ff_zz_rl_enc;
    sc_signal<int> int_zz_rl_enc;
    sc_signal<bool> bool_zz_rl_enc_ask;
    sc_signal<bool> bool_zz_rl_enc_ready;
    fifo_stat<int> zz_rl_enc_out;

    FF2P<int> ff2p_rl_enc_bit("ff2p_rl_enc_bit");

    jpeg_enc(sc_module_name _name, int* _quantization, int _maxwidth):
        sc_module(_name),
        r2b_1("r2b_1",_maxwidth),
        dct_1("dct_1"),
        ff2p_dct_quantize("ff2p_dct_quantize_"),
        quant_1("quant_1",_quantization),
//        p2ff_quantize_zz("p2ff_quantize_zz"),
        zz_enc_1("zz_enc_1"),
//        ff2p_zz_rl_enc("ff2p_zz_rl_enc"),
//        p2ff_zz_rl_enc("p2ff_zz_rl_enc"),
        rl_enc_1("rl_enc_1"),
        ff2p_rl_enc_bit("ff2p_rl_enc_bit"),
        r2b_out("r2b_out",1),
        dct_out("dct_out",1),
        dct_quantize_out("dct_quantize_out",1),
        quantize_zz_out("quantize_zz_out",1),
        zz_enc_out("zz_enc_out",1),
        zz_rl_enc_out("zz_rl_enc_out",1)
        {

            r2b_1.input(input);
            r2b_1.parameters(parameters);
            r2b_1.output(r2b_out);

            dct_1.input(r2b_out);
            dct_1.output(dct_out);

            ff2p_dct_quantize.clk(clk);
            ff2p_dct_quantize.input(dct_out);
            ff2p_dct_quantize.output(float_dct_quantize);
            ff2p_dct_quantize.ask(bool_dct_quantize_ask);
            ff2p_dct_quantize.ready(bool_dct_quantize_ready);

            quant_1.input(float_dct_quantize);
            quant_1.clk(clk);
            quant_1.ready_i(bool_dct_quantize_ready);
            quant_1.ask_i(bool_dct_quantize_ask);
            quant_1.output(int_quantize_zz);
            quant_1.ready_o(bool_quantize_zz_ready);
            quant_1.ask_o(bool_quantize_zz_ask);

            zz_enc_1.input(int_quantize_zz);
            zz_enc_1.ask_i(bool_quantize_zz_ask);
            zz_enc_1.ready_i(bool_quantize_zz_ready);
            zz_enc_1.ask_o(bool_zz_rl_enc_ask);
            zz_enc_1.ready_o(bool_zz_rl_enc_ready);
            zz_enc_1.clk(clk);
            zz_enc_1.output(int_zz_rl_enc);

            rl_enc_1.input(int_zz_rl_enc);
            rl_enc_1.clk(clk);
            rl_enc_1.ask_i(bool_zz_rl_enc_ask);
            rl_enc_1.ready_i(bool_zz_rl_enc_ready);
            rl_enc_1.output(rl_enc_out);

            ff2p_rl_enc_bit.input(rl_enc_out);
            ff2p_rl_enc_bit.clk(clk);
            ff2p_rl_enc_bit.ask(bool_rl_enc_bit_ask);
            ff2p_rl_enc_bit.ready(bool_rl_enc_bit_ready);
            ff2p_rl_enc_bit.input(rl_enc_bit_out);

            bit_packing bit_packing_1("bit_packing_1");
            bit_packing_1.input(rl_enc_bit_out);
            bit_packing_1.ask_i(bool_rl_enc_bit_ask);
            bit_packing_1.ask_o();
            bit_packing_1.ready_i(bool_rl_enc_bit_ready);
            bit_packing_1.ready_o();
            bit_packing_1.clk(clk1);
            bit_packing_1.output();

        }

    ~jpeg_enc() {
    }

};

//=======================================================================================

SC_MODULE(jpeg_enc_pr) {

    // port
    my_fifo_in<int>   input;
    my_fifo_in<int>   parameters;
    sc_out<sc_int<32> >           output;
    sc_in<bool>           ask;
    sc_out<bool>          ready;
    sc_in<bool>           clk;

    // internal FIFO
    sc_fifo<int>          temp;

    // submodules
    jpeg_enc jpeg_enc_1;
    FF2PC<int,sc_int<32> > fifo2prot;

    // constructor
    jpeg_enc_pr(sc_module_name _name, int* _quantization, int _maxwidth):
        sc_module(_name),
        input("jpeg_enc_pr_input"),
        jpeg_enc_1("jpeg_enc_1", _quantization, _maxwidth),
        fifo2prot("fifo2prot1") {

            jpeg_enc_1.input(input);
            jpeg_enc_1.parameters(parameters);
            jpeg_enc_1.output(temp);
            jpeg_enc_1.clk(clk);

            fifo2prot.input(temp);
            fifo2prot.ask(ask);
            fifo2prot.ready(ready);
            fifo2prot.output(output);
            fifo2prot.clk(clk);
        }

    // destructor
    ~jpeg_enc_pr() {
    }

};

#endif

