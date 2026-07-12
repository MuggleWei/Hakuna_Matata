#include "cfg.h"
#include "data.h"
#include "serv.h"
#include "cli.h"

int main(int argc, char *argv[])
{
	// parse sys arguments
	cfg_t cfg;
	if (!parse_args(argc, argv, &cfg)) {
		fprintf(stderr, "failed parse arguments\n");
		exit(EXIT_FAILURE);
	}

	// initialize log
	if (muggle_log_complicated_init(LOG_LEVEL_INFO, -1, NULL)) {
		fprintf(stderr, "failed init log\n");
		exit(EXIT_FAILURE);
	}

	// initialize socket library
	if (muggle_socket_lib_init() != 0) {
		LOG_ERROR("failed initialize socket library");
		exit(EXIT_FAILURE);
	}

	// init event loop
	muggle_event_loop_init_args_t ev_init_args;
	memset(&ev_init_args, 0, sizeof(ev_init_args));
	ev_init_args.evloop_type = MUGGLE_EVLOOP_TYPE_NULL;
	ev_init_args.hints_max_fd = 32;
	ev_init_args.use_mem_pool = 0;

	muggle_event_loop_t *evloop = muggle_evloop_new(&ev_init_args);
	if (evloop == NULL) {
		LOG_ERROR("failed new event loop");
		exit(EXIT_FAILURE);
	}
	LOG_INFO("success new event loop");

	// set evloop data
	evloop_data_t evloop_data;
	memset(&evloop_data, 0, sizeof(evloop_data));
	evloop_data.evloop = evloop;
	evloop_data.cfg = &cfg;
	muggle_evloop_set_data(evloop, &evloop_data);

	// prepare evloop handle
	muggle_socket_evloop_handle_t handle;
	muggle_socket_evloop_handle_init(&handle);

	// run
	if (strcmp(cfg.role, "server") == 0) {
		run_server(evloop, &handle);
	} else if (strcmp(cfg.role, "client") == 0) {
		run_client(evloop, &handle);
	} else {
		LOG_ERROR("invalid role: %s", cfg.role);
	}

	// cleanup
	muggle_socket_evloop_handle_destroy(&handle);
	muggle_evloop_delete(evloop);

	return 0;
}
