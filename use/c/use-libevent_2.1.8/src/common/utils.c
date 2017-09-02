#include "utils.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

int g_detect_endian = 1;

// copy from evutil_format_sockaddr_port_
const char* get_sockaddr_port(const struct sockaddr *sa, char *out, size_t outlen)
{
	char b[128];
	const char *res = NULL;
	int port;
	if (sa->sa_family == AF_INET) {
		const struct sockaddr_in *sin = (const struct sockaddr_in*)sa;
		res = evutil_inet_ntop(AF_INET, &sin->sin_addr, b, sizeof(b));
		port = ntohs(sin->sin_port);
		if (res) {
			evutil_snprintf(out, outlen, "%s:%d", b, port);
			return out;
		}
	}
	else if (sa->sa_family == AF_INET6) {
		const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6*)sa;
		res = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr, b, sizeof(b));
		port = ntohs(sin6->sin6_port);
		if (res) {
			evutil_snprintf(out, outlen, "[%s]:%d", b, port);
			return out;
		}
	}

	evutil_snprintf(out, outlen, "<addr with socktype %d>",
		(int)sa->sa_family);
	return out;
}