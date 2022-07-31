#ifndef EVENT_PIPE_H_
#define EVENT_PIPE_H_

#include "muggle/c/muggle_c.h"

typedef struct event_pipe
{
	int            fds[2]; //!< pipe fd
	muggle_mutex_t *mtx;   //!< pipe mutex
} event_pipe_t;

/**
 * @brief init event pipe
 *
 * @param ev_pipe  event pipe
 *
 * @return 
 *     0 - success
 *     otherwise - failed
 */
int event_pipe_init(event_pipe_t *ev_pipe);

/**
 * @brief destroy event pipe
 *
 * @param ev_pipe  event pipe
 */
void event_pipe_destroy(event_pipe_t *ev_pipe);

/**
 * @brief event pipe wake up
 *
 * @param ev_pipe  event pipe
 *
 * @return 
 *     0 - success
 *     otherwise - failed and errno is set
 */
int event_pipe_wakeup(event_pipe_t *ev_pipe);

/**
 * @brief event pipe recv wakeup and clearup all bytes in buffer
 *
 * @param ev_pipe  event pipe
 *
 * @return  
 * number of times be wakeup, on failed return -1 and errno is set
 */
int event_pipe_clearup(event_pipe_t *ev_pipe);

#endif /* ifndef EVENT_PIPE_H_ */
