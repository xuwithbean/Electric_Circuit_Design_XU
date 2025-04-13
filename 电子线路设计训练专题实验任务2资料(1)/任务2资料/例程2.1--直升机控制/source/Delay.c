#include "Delay.h"

void Delay_us(unsigned int val) {
    unsigned int i;

    for (i = 0; i < val; i++) {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        // _nop_();
        // _nop_();
        // _nop_();
    }
}

void Delay_ms(unsigned int val) {
    unsigned int i;

    for (i = 0; i < val; i++)
        Delay_us(1000);
}

// void Delay_s(unsigned int val) {
//     unsigned int i;

//     for (i = 0; i < val; i++)
//         Delay_ms(1000);
// }