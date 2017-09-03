#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <signal.h>
#endif
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "common/utils.h"
#include "common/peer.h"
#include "common/object.h"
#include "common_cpp/base_server.h"

#if WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

void myReadcb(struct Peer *peer)
{
	unsigned char *mem;

	struct evbuffer *input_ev = bufferevent_get_input(peer->bev);
	while (1)
	{
		size_t total_bytes = evbuffer_get_length(input_ev);
		if (total_bytes < 4)
		{
			return;
		}

		mem = evbuffer_pullup(input_ev, (ev_ssize_t)4);
		if (mem == NULL)
		{
			return;
		}

		int32_t len = util_ntoh_32(*(int32_t*)mem);
		if ((int32_t)total_bytes >= len)
		{
			void *stream_bytes = malloc(len);
			if (evbuffer_remove(input_ev, stream_bytes, (size_t)len) < 0)
			{
				return;
			}

			struct baseObj* obj = decodeMsg(stream_bytes, len);
			if (obj != NULL)
			{
				// printObj(obj);
				free(obj);
			}
			bufferevent_write(peer->bev, stream_bytes, len);
			free(stream_bytes);
		}
		else
		{
			return;
		}
	}
}

int main(int argc, char *argv[])
{
	std::function<void(struct Peer*)> read_cb = std::bind(myReadcb, std::placeholders::_1);

	BaseServer server;
	server.IOThread(8);
	server.IdleTimeout(30);
	server.RegistReadCb(read_cb);

	server.Run(40713);

	return 0;
}
