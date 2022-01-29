#include "packet_handle.h"

typedef struct sys_args
{
	char pcapfile[512];
} sys_args_t;

void parse_sys_args(int argc, char *argv[], sys_args_t *args)
{
	memset(args, 0, sizeof(*args));

	char usage[1024];
	snprintf(usage, sizeof(usage), 
		"Usage of %s: -f <pcap_file>\n"
	 	"  -f string\n\tpcap offline file path",
	 	argv[0]);

	int opt;
	while ((opt = getopt(argc, argv, "f:h")) != -1)
	{
		switch (opt)
		{
		case 'f':
		{
			LOG_INFO("pcap offline file: %s", optarg);
			memset(args->pcapfile, 0, sizeof(args->pcapfile));
			strncpy(args->pcapfile, optarg, sizeof(args->pcapfile)-1);
		}break;
		case 'h':
		{
			LOG_INFO("\n%s", usage);
			exit(EXIT_FAILURE);
		}break;
		}
	}

	if (args->pcapfile[0] == '\0')
	{
		LOG_ERROR("\n%s", usage);
		exit(EXIT_FAILURE);
	}
}


#ifndef BUFSIZ
#define BUFSIZ 65536
#endif

void offline_capture(const char *pcapfile)
{
	char err_buf[PCAP_ERRBUF_SIZE];

	pcap_t *handle = pcap_open_offline(pcapfile, err_buf);
	if (handle == NULL)
	{
		LOG_ERROR("failed pcap_open_offline, error message: %s", err_buf);
		exit(EXIT_FAILURE);
	}

	pcap_loop(handle, 0, packet_handle, NULL);

	pcap_close(handle);
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_INFO, -1, NULL);

	// parse system arguments
	sys_args_t args;
	parse_sys_args(argc, argv, &args);

	offline_capture(args.pcapfile);

	return 0;
}
