/*$file${HSMs::../src::button_SM.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: ClockAlarm.qm
* File:  ${HSMs::../src::button_SM.cpp}
*
* This code has been generated by QM 5.2.2 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${HSMs::../src::button_SM.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include <Arduino.h>
#include "qpn.h"
#include "button_SM.h"
#include "ClockAlarm_SM.h"
#include "main.h"


/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpn version 6.9.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${HSMs::AO_Button} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${HSMs::AO_Button} .......................................................*/
QActive *const AO_Button = &Button_obj.super; //adress of the button class object
/*$enddef${HSMs::AO_Button} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${HSMs::Button} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${HSMs::Button} ..........................................................*/
Button Button_obj;

/*${HSMs::Button::SM} ......................................................*/
QState Button_initial(Button * const me) {
    /*${HSMs::Button::SM::initial} */
    return Q_TRAN(&Button_Button);
}

/*${HSMs::Button::SM::Button} ..............................................*/
QState Button_Button(Button * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${HSMs::Button::SM::Button::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            uint8_t b1,b2,btn_pad_value;

            //1. read the button pad status
            b1 = digitalRead(PIN_BUTTON1);
            b2 = digitalRead(PIN_BUTTON2);

            btn_pad_value = (b1<<1u)|b2;

            if(btn_pad_value){
                if(btn_pad_value == BTN_PAD_VALUE_SET){
                  QACTIVE_POST(AO_ClockAlarm, SET_SIG, 0U);
                }else if(btn_pad_value == BTN_PAD_VALUE_OK){
                  QACTIVE_POST(AO_ClockAlarm, OK_SIG, 0U);
                }else if(btn_pad_value == BTN_PAD_VALUE_ABRT){
                  QACTIVE_POST(AO_ClockAlarm, ABRT_SIG, 0U);
                }
            }

            QF_INT_DISABLE();
            flag_report_button_press = true;
            QF_INT_ENABLE();
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*$enddef${HSMs::Button} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${HSMs::Button_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${HSMs::Button_ctor} .....................................................*/
void Button_ctor(void) {
    QActive_ctor(AO_Button, Q_STATE_CAST(&Button_initial));
}
/*$enddef${HSMs::Button_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
