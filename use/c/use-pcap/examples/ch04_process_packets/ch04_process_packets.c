#include "muggle/c/muggle_c.h"
#include "pcap.h"

typedef struct sys_args
{
	char device[256];
} sys_args_t;

void parse_sys_args(int argc, char *argv[], sys_args_t *args)
{
	memset(args, 0, sizeof(*args));

	char usage[1024];
	snprintf(usage, sizeof(usage), 
		"Usage of %s: -d <ifname>\n"
	 	"  -d string\n\tnetwork interface name",
	 	argv[0]);

	int opt;
	while ((opt = getopt(argc, argv, "d:h")) != -1)
	{
		switch (opt)
		{
		case 'd':
		{
			LOG_INFO("capture in network interface: %s", optarg);
			strncpy(args->device, optarg, sizeof(args->device));
		}break;
		case 'h':
		{
			LOG_INFO("\n%s", usage);
			exit(EXIT_FAILURE);
		}break;
		}
	}

	if (args->device[0] == '\0')
	{
		LOG_ERROR("\n%s", usage);
		exit(EXIT_FAILURE);
	}
}

void print_packet_info(const u_char *packet, const struct pcap_pkthdr *header)
{
	LOG_INFO("packet capture length: %d", header->caplen);
	LOG_INFO("packet total length: %d", header->len);
}

void my_packet_handler(
	u_char *args,
	const struct pcap_pkthdr *packet_header,
	const u_char *packet_body)
{
	print_packet_info(packet_body, packet_header);
}

#ifndef BUFSIZ
#define BUFSIZ 65536
#endif

void live_capture(pcap_if_t *device)
{
	char err_buf[PCAP_ERRBUF_SIZE];
	int packet_count_limit = 1;
	int timeout_limit = 1000; // in milliseconds

	// open device for live capture
	pcap_t *handle = pcap_open_live(
		device->name,
		BUFSIZ,
		packet_count_limit,
		timeout_limit,
		err_buf);
	if (handle == NULL)
	{
		LOG_ERROR("failed pcap_open_live, error message: %s", err_buf);
		return;
	}

	pcap_loop(handle, 0, my_packet_handler, NULL);

	pcap_close(handle);
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_INFO, -1, NULL);

	// parse system arguments
	sys_args_t args;
	parse_sys_args(argc, argv, &args);

	// get the devices list
	pcap_if_t *device_list = NULL;
	char err_buf[PCAP_ERRBUF_SIZE];
	if (pcap_findalldevs(&device_list, err_buf) != 0)
	{
		LOG_ERROR("Call pcap_findalldevs return error, %s", err_buf);
		exit(EXIT_FAILURE);
	}

	// scan device list
	pcap_if_t *device = NULL;
	for (device = device_list; device; device = device->next)
	{
		if (strncmp(device->name, args.device, sizeof(args.device)) == 0)
		{
			break;
		}
	}

	// try to capture
	if (device != NULL)
	{
		live_capture(device);
	}

	// free devices list
	if (device_list)
	{
		pcap_freealldevs(device_list);
	}
	
	return 0;
}
