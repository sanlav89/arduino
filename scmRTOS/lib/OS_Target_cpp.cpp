//******************************************************************************
//*
//*     FULLNAME:  Single-Chip Microcontroller Real-Time Operating System
//*
//*     NICKNAME:  scmRTOS
//*
//*     PROCESSOR: AVR (Atmel)
//*
//*     TOOLKIT:   avr-gcc (GNU)
//*
//*     PURPOSE:   Target Dependent Stuff Source
//*
//*     Version: 3.10
//*
//*     $Revision: 395 $
//*     $Date:: 2011-04-27 #$
//*
//*     Copyright (c) 2003-2010, Harry E. Zhurov
//*
//*     Permission is hereby granted, free of charge, to any person
//*     obtaining  a copy of this software and associated documentation
//*     files (the "Software"), to deal in the Software without restriction,
//*     including without limitation the rights to use, copy, modify, merge,
//*     publish, distribute, sublicense, and/or sell copies of the Software,
//*     and to permit persons to whom the Software is furnished to do so,
//*     subject to the following conditions:
//*
//*     The above copyright notice and this permission notice shall be included
//*     in all copies or substantial portions of the Software.
//*
//*     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//*     EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//*     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//*     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//*     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//*     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
//*     THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//*     =================================================================
//*     See http://scmrtos.sourceforge.net for documentation, latest
//*     information, license and contact details.
//*     =================================================================
//*
//******************************************************************************
//*     avr-gcc port by Oleksandr O. Redchuk, Copyright (c) 2007-2010

#include <scmRTOS.h>

using namespace OS;

OS::TPrioMaskTable OS::PrioMaskTable;


//    AVR hardware stack pointer (SP) points to empty location, post-decrement used,
//  "push reg" is "*SP-- = reg"
//    StackPointer points to first element _after_ stack memory and *--StackPointer places
//  values in right positions, but StackPointer must be decremented by 1 more than items
//  "pushed"

//  avr-gcc even 4.2.x has no all set of microcontroller property definitions
//  like __AVR_HAVE_EIJMP_EICALL__, __AVR_HAVE_RAMPZ__ and so on
//  #ifdefs for SFR names are used instead
//
//  PENDING: Limit supported compiler list to 4.3.x and higher?


//------------------------------------------------------------------------------
void TBaseProcess::init_stack_frame( stack_item_t * Stack
                                   , void (*exec)() 
                                #if scmRTOS_DEBUG_ENABLE == 1
                                   , stack_item_t * StackPool
                                #endif
                                   )
{
    // stack pointer correction constant
    enum {
        REGS_COUNT = 31, // all but r1 (aka __zero_reg__)
#ifdef RAMPZ
        RAMPZ_PLACE = 1,
#else
        RAMPZ_PLACE = 0,
#endif
#ifdef EIND
        EIND_PLACE = 1,
#else
        EIND_PLACE = 0,
#endif
        // all but r1 + saved SFRs + 1 because of post-decrement behavior of AVR's SP
        STACK_POINTER_CORRECTION = REGS_COUNT + RAMPZ_PLACE + EIND_PLACE + 1
    };


    //---------------------------------------------------------------
    //
    //  Prepare Process Stack Frame
    //
    *(--Stack) = reinterpret_cast<uint16_t>(exec);       // return from interrupt address (low  byte)
    *(--Stack) = reinterpret_cast<uint16_t>(exec) >> 8;  // return from interrupt address (high byte)
#ifdef __AVR_3_BYTE_PC__
    *(--Stack) = 0;                              // all trampolines are placed in lower 128K
#endif
    *(--Stack) = 0;                              // initalize __zero_reg__ for this process
    *(--Stack) = (1 << SREG_I);                  // SREG value: I-bit set, enable interrupts
    Stack     -= STACK_POINTER_CORRECTION;

    StackPointer = Stack;

#if scmRTOS_DEBUG_ENABLE == 1
    //-----------------------------------------------------------------------
    //   Fill stack pool with predefined value for stack consumption checking
    //   Do not fill area for GPR and SFR.
    //   *Stack position is filled - the position is free because of AVR's SP behavior

    stack_item_t *fill_ptr = StackPool;
    while( fill_ptr <= Stack )
    {
	*fill_ptr++ = STACK_DEFAULT_PATTERN;
    }
#endif // scmRTOS_DEBUG_ENABLE

}

//------------------------------------------------------------------------------
//
//  System timer ISR
//
OS_INTERRUPT void SYSTEM_TIMER_VECTOR(void)
{
    scmRTOS_ISRW_TYPE ISR;
    Kernel.system_timer();

#if scmRTOS_SYSTIMER_HOOK_ENABLE == 1
  #if scmRTOS_SYSTIMER_NEST_INTS_ENABLE == 1
    ENABLE_NESTED_INTERRUPTS();
  #endif
    system_timer_user_hook();
#endif
}
//------------------------------------------------------------------------------

