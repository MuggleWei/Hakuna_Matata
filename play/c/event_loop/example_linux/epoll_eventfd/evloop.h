#ifndef MY_EVLOOP_H_
#define MY_EVLOOP_H_

#include "muggle/c/muggle_c.h"

struct ev_context;
struct ev_loop;

/**
 * @brief event loop callback
 *
 * @param evloop  event loop
 * @param ctx     event context
 *
 * @return 
 */
typedef void (*fn_evloop_callback)(struct ev_loop *evloop, struct ev_context *ctx);

/**
 * @brief event context
 */
typedef struct ev_context
{
	int  fd;    //!< file descriptor
	void *data; //!< event context data
} ev_context_t;

/**
 * @brief event loop
 */
typedef struct ev_loop
{
	int          epfd;     //!< epoll fd
	ev_context_t evfd_ctx; //!< eventfd context
	int          exit;     //!< wait to exit

	struct timespec last_ts; //!< last timer trigger
	int             timeout; //!< timeout expires in milliseconds

	fn_evloop_callback read_cb;  //!< read callback
	fn_evloop_callback err_cb;   //!< error callback
	fn_evloop_callback wake_cb;  //!< wakeup callback
	fn_evloop_callback timer_cb; //!< timer callback

	muggle_thread_id loop_thread;  //!< event loop run thread id
} ev_loop_t;

/**
 * @brief initialize event loop
 *
 * @param evloop  event loop
 *
 * @return 
 *     0 - success
 *     otherwise - failed init eventloop
 */
int evloop_init(ev_loop_t *evloop);

/**
 * @brief destroy event loop
 *
 * @param evloop  event loop
 */
void evloop_destroy(ev_loop_t *evloop);

/**
 * @brief set event loop timeout
 *
 * @param evloop  event loop
 * @param timeout  timer interval in milliseconds
 */
void evloop_set_timer(ev_loop_t *evloop, int timeout);

/**
 * @brief set event loop read callback
 *
 * @param evloop  event loop
 * @param cb      read callback
 */
void evloop_set_cb_read(ev_loop_t *evloop, fn_evloop_callback cb);

/**
 * @brief set event loop error callback
 *
 * @param evloop  event loop
 * @param cb      error callback
 */
void evloop_set_cb_error(ev_loop_t *evloop, fn_evloop_callback cb);

/**
 * @brief set event loop wakeup callback
 *
 * @param evloop  event loop
 * @param cb      wakeup callback
 * @param data    data in context on wakeup callback
 */
void evloop_set_cb_wakeup(ev_loop_t *evloop, fn_evloop_callback cb, void *data);

/**
 * @brief set event loop timer callback
 *
 * @param evloop  event loop
 * @param cb      timer callback
 */
void evloop_set_cb_timer(ev_loop_t *evloop, fn_evloop_callback cb);

/**
 * @brief wakeup event loop
 *
 * @param evloop  event loop
 */
void evloop_wakeup(ev_loop_t *evloop);

/**
 * @brief add new event loop context
 *
 * @param evloop  event loop
 * @param ctx     event context
 *
 * @return
 *     0 - success add new event context
 *     otherwise - failed add
 *
 * @note
 * only support add context in the same thread of event loop run
 * if in other thread, need put new context in somewhere, and 
 * add context in wakeup callback
 */
int evloop_add_ctx(ev_loop_t *evloop, ev_context_t *ctx);

/**
 * @brief run event loop
 *
 * @param evloop   event loop
 */
void evloop_run(ev_loop_t *evloop);

#endif /* ifndef EVLOOP_H_ */
