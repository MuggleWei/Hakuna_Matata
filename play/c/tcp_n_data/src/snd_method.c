#include "snd_method.h"
#include "data.h"

muggle_thread_ret_t snd_data_thread_routine(void *p_args)
{
	muggle_socket_context_t *ctx = (muggle_socket_context_t *)p_args;
	session_data_t *session_data = get_session_info(ctx);
	cfg_t *cfg = session_data->cfg;
	session_data->total_data_nbytes = sizeof(data_t) * cfg->n;

	// prepared datas
	// NOTE: the raw data uses non-contiguous memory.
	size_t raw_data_size = sizeof(data_t) + 128;
	raw_data_size = MUGGLE_ROUND_UP_POW_OF_2_MUL(raw_data_size, 1024);

	LOG_INFO("data size: %u", (unsigned int)sizeof(data_t));
	LOG_INFO("raw data size: %u", (unsigned int)raw_data_size);
	char *datas = malloc(raw_data_size * cfg->n);
	data_t **p_datas = (data_t **)malloc(sizeof(data_t *) * cfg->n);

	for (size_t i = 0; i < cfg->n; ++i) {
		p_datas[i] = (data_t *)((char *)datas + raw_data_size * i);
	}

	// snd message
	if (strcmp(cfg->method, SND_METHOD_WRITE) == 0) {
		method_write(ctx, p_datas, cfg->n);
	} else if (strcmp(cfg->method, SND_METHOD_WRITEV) == 0) {
		method_writev(ctx, p_datas, cfg->n);
	} else if (strcmp(cfg->method, SND_METHOD_BIG_WRITE) == 0) {
		method_big_write(ctx, p_datas, cfg->n);
	} else if (strcmp(cfg->method, SND_METHOD_ASSEMBLE_WRITE) == 0) {
		method_assemble_write(ctx, p_datas, cfg->n);
	} else if (strcmp(cfg->method, SND_METHOD_ASSEMBLE_WRITEV) == 0) {
		method_assemble_writev(ctx, p_datas, cfg->n);
	}

	// cleanup
	free(p_datas);
	free(datas);

	return 0;
}

void method_write(muggle_socket_context_t *ctx, data_t **p_datas, size_t n)
{
	muggle_event_fd fd = muggle_socket_ctx_get_fd(ctx);
	session_data_t *session_data = get_session_info(ctx);

	// send datas
	muggle_time_counter_start(&session_data->r_tc);
	muggle_time_counter_start(&session_data->w_tc);
	for (size_t i = 0; i < n; ++i) {
		int ret = muggle_socket_block_write(fd, p_datas[i], sizeof(data_t), 40);
		if (ret != (int)sizeof(data_t)) {
			LOG_ERROR("ret=%d, errno=%d", ret, errno);
		}
	}
	muggle_time_counter_end(&session_data->w_tc);
}

static int block_writev(muggle_socket_t fd, muggle_socket_iovec_t *iov,
						int iovcnt, size_t len, unsigned long interval_ns)
{
	int total_bytes = (int)len;
	int total_snd_bytes = 0;
	int offset = 0;
	do {
		int send_bytes =
			muggle_socket_writev(fd, iov + offset, iovcnt - offset);
		if (send_bytes > 0) {
			total_snd_bytes += send_bytes;
		} else if (send_bytes == MUGGLE_EVENT_ERROR) {
			int last_errnum = muggle_sys_lasterror();
			if (last_errnum == MUGGLE_SYS_ERRNO_WOULDBLOCK ||
				last_errnum == MUGGLE_SYS_ERROR_AGAIN ||
				last_errnum == MUGGLE_SYS_ERRNO_INTR) {
				if (interval_ns > 0) {
					muggle_nsleep(interval_ns);
				}
				continue;
			} else {
				return MUGGLE_EVENT_ERROR;
			}
		} else {
			// LOG_ERROR("failed block write message: return=%d", send_bytes);
			return 0;
		}

		if (total_snd_bytes == total_bytes) {
			break;
		}

		// calculate offset and set iov data
		int remain_send_bytes = send_bytes;
		for (int i = offset; i < iovcnt; ++i) {
			int len = MUGGLE_SOCKET_IOVEC_GET_LEN(iov[i]);
			if (len > remain_send_bytes) {
				char *data = (char *)(MUGGLE_SOCKET_IOVEC_GET_BUF(iov[i]));
				data += remain_send_bytes;
				MUGGLE_SOCKET_IOVEC_SET_BUF(iov[i], data);
				MUGGLE_SOCKET_IOVEC_SET_LEN(iov[i], len - remain_send_bytes);
				break;
			} else if (len <= remain_send_bytes) {
				offset += 1;
				remain_send_bytes -= len;
			}
		}
	} while (true);

	return total_bytes;
}

void method_writev(muggle_socket_context_t *ctx, data_t **p_datas, size_t n)
{
	muggle_event_fd fd = muggle_socket_ctx_get_fd(ctx);
	session_data_t *session_data = get_session_info(ctx);

	// prepare iovec data
	muggle_socket_iovec_t *iov =
		(muggle_socket_iovec_t *)malloc(sizeof(muggle_socket_iovec_t) * n);
	for (size_t i = 0; i < n; ++i) {
		MUGGLE_SOCKET_IOVEC_SET_BUF(iov[i], p_datas[i]);
		MUGGLE_SOCKET_IOVEC_SET_LEN(iov[i], sizeof(data_t));
	}

	const int max_iovcnt = 1024;
	int n_batch = n / max_iovcnt;
	int batch_remain = n % max_iovcnt;

	// send datas
	muggle_time_counter_start(&session_data->r_tc);
	muggle_time_counter_start(&session_data->w_tc);
	for (int i = 0; i < n_batch; ++i) {
		int ret = block_writev(fd, iov + i * max_iovcnt, max_iovcnt,
							   max_iovcnt * sizeof(data_t), 40);
		if (ret != (int)(max_iovcnt * sizeof(data_t))) {
			LOG_ERROR("ret=%d, errno=%d", ret, errno);
		}
	}
	if (batch_remain > 0) {
		int ret = block_writev(fd, iov + n_batch * max_iovcnt, batch_remain,
							   batch_remain * sizeof(data_t), 40);
		if (ret != (int)(batch_remain * sizeof(data_t))) {
			LOG_ERROR("ret=%d, errno=%d", ret, errno);
		}
	}
	muggle_time_counter_end(&session_data->w_tc);

	// cleanup
	free(iov);
}

void method_big_write(muggle_socket_context_t *ctx, data_t **p_datas, size_t n)
{
	muggle_event_fd fd = muggle_socket_ctx_get_fd(ctx);
	session_data_t *session_data = get_session_info(ctx);

	// prepare datas
	int nbytes = session_data->total_data_nbytes;
	data_t *datas = (data_t *)malloc(nbytes);
	for (size_t i = 0; i < n; ++i) {
		memcpy(&datas[i], p_datas[i], sizeof(data_t));
	}

	// send datas
	muggle_time_counter_start(&session_data->r_tc);
	muggle_time_counter_start(&session_data->w_tc);
	int ret = muggle_socket_block_write(fd, datas, nbytes, 40);
	if (ret != nbytes) {
		LOG_ERROR("ret=%d, errno=%d", ret, errno);
	}
	muggle_time_counter_end(&session_data->w_tc);

	// cleanup
	free(datas);
}

void method_assemble_write(muggle_socket_context_t *ctx, data_t **p_datas,
						   size_t n)
{
	muggle_event_fd fd = muggle_socket_ctx_get_fd(ctx);
	session_data_t *session_data = get_session_info(ctx);

	// prepare datas
	int mss;
	muggle_socklen_t len = sizeof(mss);
	muggle_getsockopt(fd, IPPROTO_TCP, TCP_MAXSEG, &mss, &len);
	LOG_INFO("MSS: %d", mss);

	int nbytes = session_data->total_data_nbytes;
	data_t *datas = (data_t *)malloc(nbytes);
	for (size_t i = 0; i < n; ++i) {
		memcpy(&datas[i], p_datas[i], sizeof(data_t));
	}

	// send datas
	muggle_time_counter_start(&session_data->r_tc);
	muggle_time_counter_start(&session_data->w_tc);
	int remain_bytes = nbytes;
	char *p = (char *)datas;
	do {
		int cur_nbytes = remain_bytes > mss ? mss : remain_bytes;
		int ret = muggle_socket_block_write(fd, p, cur_nbytes, 40);
		if (ret != cur_nbytes) {
			LOG_ERROR("ret=%d, errno=%d", ret, errno);
		}
		p += cur_nbytes;
		remain_bytes -= cur_nbytes;
	} while (remain_bytes > 0);
	muggle_time_counter_end(&session_data->w_tc);

	// cleanup
	free(datas);
}

void method_assemble_writev(muggle_socket_context_t *ctx, data_t **p_datas,
							size_t n)
{
	muggle_event_fd fd = muggle_socket_ctx_get_fd(ctx);
	session_data_t *session_data = get_session_info(ctx);

	// prepare datas
	int mss;
	muggle_socklen_t len = sizeof(mss);
	muggle_getsockopt(fd, IPPROTO_TCP, TCP_MAXSEG, &mss, &len);
	LOG_INFO("MSS: %d", mss);

	int nbytes = session_data->total_data_nbytes;
	data_t *datas = (data_t *)malloc(nbytes);
	for (size_t i = 0; i < n; ++i) {
		memcpy(&datas[i], p_datas[i], sizeof(data_t));
	}

	int n_iov = nbytes / mss;
	if (nbytes % mss > 0) {
		++n_iov;
	}
	muggle_socket_iovec_t *iov =
		(muggle_socket_iovec_t *)malloc(sizeof(muggle_socket_iovec_t) * n_iov);
	char *p = (char *)datas;
	int remain = nbytes;
	for (int i = 0; i < n_iov; ++i) {
		MUGGLE_SOCKET_IOVEC_SET_BUF(iov[i], p);
		int cur_nbytes = remain > mss ? mss : remain;
		MUGGLE_SOCKET_IOVEC_SET_LEN(iov[i], cur_nbytes);

		remain -= cur_nbytes;
		p += cur_nbytes;
	}
	if (remain != 0) {
		LOG_FATAL("failed calculate remain");
	}

	const int max_iovcnt = 1024;
	int n_batch = n_iov / max_iovcnt;
	if (n_iov % max_iovcnt > 0) {
		++n_batch;
	}
	batch_iov_t *batch_iov =
		(batch_iov_t *)malloc(sizeof(batch_iov_t) * n_batch);
	for (int i = 0; i < n_batch; ++i) {
		batch_iov[i].iov = iov + max_iovcnt * i;
		batch_iov[i].iovcnt = 0;
		batch_iov[i].nbytes = 0;
		for (int j = 0; j < max_iovcnt; ++j) {
			int idx = max_iovcnt * i + j;
			if (idx >= n_iov) {
				break;
			}
			batch_iov[i].iovcnt += 1;
			batch_iov[i].nbytes += MUGGLE_SOCKET_IOVEC_GET_LEN(iov[idx]);
		}
	}

	// send datas
	muggle_time_counter_start(&session_data->r_tc);
	muggle_time_counter_start(&session_data->w_tc);
	for (int i = 0; i < n_batch; ++i) {
		int ret = block_writev(fd, batch_iov[i].iov, batch_iov[i].iovcnt,
							   batch_iov[i].nbytes, 40);
		if (ret != (int)batch_iov[i].nbytes) {
			LOG_ERROR("ret=%d, errno=%d", ret, errno);
		}
	}
	muggle_time_counter_end(&session_data->w_tc);

	// cleanup
	free(batch_iov);
	free(iov);
	free(datas);
}
