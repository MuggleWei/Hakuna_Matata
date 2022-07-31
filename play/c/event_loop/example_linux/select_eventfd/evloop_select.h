#ifndef EVLOOP_SELECT_H_
#define EVLOOP_SELECT_H_

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

typedef struct ev_loop
{
	fd_set allset; //!< all fd set
	int    nfds;   //!< highest-numbered file descriptor, use *nix

	muggle_linked_list_t *linked_list; //!< linked list save context

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
 * @param evloop        event loop
 * @param hints_max_fd  hints max event fd count
 * @param use_mem_pool  use memory pool; 0 - don't use, 1 - use
 *
 * @return 
 *     0 - success
 *     otherwise - failed init eventloop
 *
 * @note
 * if hints_max_fd < 1, then hints_max_fd be set 8
 * if use_mem_pool is true, then the hints_max_fd use as memory pool size
 */
int evloop_init(ev_loop_t *evloop, int hints_max_fd, int use_mem_pool);

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

#endif /* ifndef EVLOOP_SELECT */
