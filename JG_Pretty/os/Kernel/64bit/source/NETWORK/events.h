#ifndef EVENTS_H
#define EVENTS_H

#include "../task.h"
#include "../Synchronization.h"
#include "../usb/list2.h"

#define MAX_EVENTS 2000 // One queue stores at maximum xx events. If a queue is full, an EVENT_OVERFLOW event is appended and no further events are accepted


typedef enum
{
    EVENT_NONE, EVENT_BUFFER_TO_SMALL, EVENT_OVERFLOW,
    EVENT_TCP_CONNECTED, EVENT_TCP_RECEIVED, EVENT_TCP_CLOSED, // -> c.f. tcp.h/c
    EVENT_UDP_RECEIVED,                                        // -> c.f. udp.h/c
} EVENT_t;

typedef enum
{
    TCP_T,
	UDP_T
} protocoltype;

typedef struct
{
    EVENT_t type;
    size_t  length;
    void*   data;
} event_t;

typedef struct
{
    list_t  list;
    size_t  num;
    MUTEX   mutex;
} event_queue_t;

//event_queue_t* event_createQueue(void);
void 		   event_createQueue(event_queue_t* queue);
void           event_deleteQueue(event_queue_t* queue);
uint8_t        event_issue(event_queue_t* destination, EVENT_t type, const void* data, size_t length); // Sends an event to an event queue
void           event_issueToDisplayedTasks(EVENT_t type, const void* data, size_t length); // Sends an event to event queue of each displayed task
event_t*       event_peek(event_queue_t* eventQueue, uint32_t i);
bool           event_unlockTask(void* data);
bool           waitForEvent(uint32_t timeout);
EVENT_t 	   event_poll(void* destination, size_t maxLength, EVENT_t filter,protocoltype ptype,uint16_t port,uint32_t tcpid);

#endif
