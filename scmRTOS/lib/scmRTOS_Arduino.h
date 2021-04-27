
#ifndef _scmRTOS_Arduino_h
#define _scmRTOS_Arduino_h

// Wrapper macro to simplify the job of starting the RTOS.
//
#define scmRTOS_START()				\
do {						\
  UNLOCK_SYSTEM_TIMER();			\
  OS::run();					\
} while(0)

// Wrapper macro to simplify the job of defining a process.
//
// XXX do we still have to have a process defined for every possible
// priority level, or can we leave them out now?
//
#define scmRTOS_PROCESS(_number, _stack, _loop)			\
typedef OS::process<OS::pr##_number, _stack> __TProc##_number;	\
namespace OS {							\
  template<> void __TProc##_number::exec();			\
  template<> void __TProc##_number::exec() {			\
    for (;;)							\
      _loop();							\
  }								\
}								\
__TProc##_number __Proc##_number

#endif
