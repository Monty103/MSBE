#ifndef __OS_TYPE_H
#define __OS_TYPE_H


#define OS_TICKS_PER_SEC          10000

#if defined CONFIG_UCOS_ENABLE


#define  OS_EVENT_TYPE_UNUSED         0u
#define  OS_EVENT_TYPE_Q              1u
#define  OS_EVENT_TYPE_SEM            2u
#define  OS_EVENT_TYPE_MUTEX          3u

typedef struct {
    unsigned char OSEventType;
    int aa;
    void *bb;
    unsigned char value;
    unsigned char prio;
    unsigned short cc;
} OS_SEM, OS_MUTEX, OS_QUEUE;


#elif defined CONFIG_FREE_RTOS_ENABLE

#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/semphr.h"
#include "FreeRTOS/task.h"

typedef StaticSemaphore_t OS_SEM, OS_MUTEX;
typedef StaticQueue_t OS_QUEUE;




#else
#error "no_os_defined"
#endif





















#endif
