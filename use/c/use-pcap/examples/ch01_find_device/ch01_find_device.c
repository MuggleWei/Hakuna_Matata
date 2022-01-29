#include "muggle/c/muggle_c.h"
#include "pcap.h"

int main()
{
	// initialize log
	muggle_log_complicated_init(MUGGLE_LOG_LEVEL_INFO, -1, NULL);

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
		LOG_INFO("found device: %s", device->name);
	}

	// free devices list
	if (device_list)
	{
		pcap_freealldevs(device_list);
	}
	
	return 0;
}
