#include "muggle/cpp/muggle_cpp.h"
#include "demo/codec_bytes.h"
#include "demo/dispatcher.h"
#include "demo_msg.h"
#include "tcp_client_handle.h"
#include "client_peer.h"

typedef struct sys_args
{
	char host[64];
	char port[16];
} sys_args_t;

void parse_sys_args(int argc, char **argv, sys_args_t *args)
{
	char str_usage[1024];
	snprintf(str_usage, sizeof(str_usage),
		"Usage: %s -H <host> -P <port>\n"
		"\t-h print help information\n"
		"\t-H listen/connect host\n"
		"\t-P listen/connect port",
		argv[0]);

	memset(args, 0, sizeof(*args));

	int opt;
	while ((opt = getopt(argc, argv, "hvt:H:P:")) != -1)
	{
		switch (opt)
		{
		case 'h':
		{
			LOG_INFO("\n%s", str_usage);
			exit(EXIT_SUCCESS);
		}break;
		case 'H':
		{
			strncpy(args->host, optarg, sizeof(args->host)-1);
		}break;
		case 'P':
		{
			strncpy(args->port, optarg, sizeof(args->port)-1);
		}break;
		}
	}

	if (args->host[0] == '\0' ||
		args->port[0] == '\0')
	{
		LOG_ERROR("\n%s", str_usage);
		exit(EXIT_FAILURE);
	}

	LOG_INFO(
		"\n"
		"----- input args -----\n"
		"host=%s\n"
		"port=%s\n"
		"----------------------",
		args->host, args->port);
}

#define DISPATCHER_REGISTER(msg_id, func) \
	dispatcher.registerCallback(msg_id, ClientPeer::s_##func);

void initDispatcher(Dispatcher &dispatcher)
{
	CodecBytes *codec = new CodecBytes();
	codec->setRecvUnitSize(4 * 1024);
	codec->setMsgLenLimit(32 * 1024);

	dispatcher.setTotalMessage(MAX_DEMO_MSG_ID);
	dispatcher.appendCodec(codec);

	DISPATCHER_REGISTER(DEMO_MSG_ID_PONG, onPong);
	DISPATCHER_REGISTER(DEMO_MSG_ID_RSP_LOGIN, onRspLogin);
	DISPATCHER_REGISTER(DEMO_MSG_ID_RSP_SUM, onRspSum);
}

void run_tcp_client(const char *host, const char *port)
{
	// init dispatcher
	Dispatcher dispatcher;
	initDispatcher(dispatcher);

	// run client
	NetEventLoop evloop(8, 0);
	TcpClientHandle handle;

	handle.setDispatcher(&dispatcher);
	handle.setEventLoop(&evloop);
	handle.setServerAddr(host, port);

	evloop.SetHandle(&handle);
	evloop.SetTimerInterval(3000);

	handle.connect();

	evloop.Run();
}

int main(int argc, char *argv[])
{
	// init log
	if (!muggle::Log::ComplicatedInit(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL))
	{
		fprintf(stderr, "failed init log");
		exit(EXIT_FAILURE);
	}

	// init socket
	if (muggle_socket_lib_init() != 0)
	{
		LOG_ERROR("failed initalize socket library");
		exit(EXIT_FAILURE);
	}

	// init rand
	srand((unsigned int)time(NULL));

	// parse system arguments
	sys_args_t args;
	parse_sys_args(argc, argv, &args);

	// run tcp server
	run_tcp_client(args.host, args.port);
	
	return 0;
}
