#include "muggle/c/muggle_c.h"
#include "pcap.h"

void output_inet_dev_info(struct pcap_addr *dev_addr)
{
	char straddr[INET_ADDRSTRLEN+1];
	char strnetmask[INET_ADDRSTRLEN+1];
	char err_msg[128];

	struct sockaddr_in *addr = (struct sockaddr_in*)dev_addr->addr;
	struct sockaddr_in *netmask = (struct sockaddr_in*)dev_addr->netmask;

	if (!inet_ntop(addr->sin_family, &addr->sin_addr, straddr, sizeof(straddr)))
	{
		muggle_socket_strerror(MUGGLE_SOCKET_LAST_ERRNO, err_msg, sizeof(err_msg));
		LOG_ERROR("failed get device address: %s", err_msg);
		return;
	}

	if (!inet_ntop(netmask->sin_family, &netmask->sin_addr, strnetmask, sizeof(strnetmask)))
	{
		muggle_socket_strerror(MUGGLE_SOCKET_LAST_ERRNO, err_msg, sizeof(err_msg));
		LOG_ERROR("failed get device netmask: %s", err_msg);
	}

	LOG_INFO("inet %s netmask %s", straddr, strnetmask);
}

void output_inet6_dev_info(struct pcap_addr *dev_addr)
{
	char straddr[INET6_ADDRSTRLEN+1];
	char strnetmask[INET6_ADDRSTRLEN+1];
	char err_msg[128];

	struct sockaddr_in6 *addr = (struct sockaddr_in6*)dev_addr->addr;
	struct sockaddr_in6 *netmask = (struct sockaddr_in6*)dev_addr->netmask;

	if (!inet_ntop(addr->sin6_family, &addr->sin6_addr, straddr, sizeof(straddr)))
	{
		muggle_socket_strerror(MUGGLE_SOCKET_LAST_ERRNO, err_msg, sizeof(err_msg));
		LOG_ERROR("failed get device address: %s", err_msg);
		return;
	}

	if (!inet_ntop(netmask->sin6_family, &netmask->sin6_addr, strnetmask, sizeof(strnetmask)))
	{
		muggle_socket_strerror(MUGGLE_SOCKET_LAST_ERRNO, err_msg, sizeof(err_msg));
		LOG_ERROR("failed get device netmask: %s", err_msg);
	}

	LOG_INFO("inet6 %s  netmask %s", straddr, strnetmask);
}

void output_dev_info(pcap_if_t *device)
{
	LOG_INFO("-------------------------");
	LOG_INFO("found device: %s", device->name);

	pcap_addr_t *dev_addr = NULL;
	for (dev_addr = device->addresses; dev_addr; dev_addr = dev_addr->next)
	{
		if (dev_addr->addr == NULL)
		{
			continue;
		}

		if (dev_addr->netmask == NULL)
		{
			continue;
		}

		switch (dev_addr->addr->sa_family)
		{
		case AF_INET:
			{
				output_inet_dev_info(dev_addr);
			}break;
		case AF_INET6:
			{
				output_inet6_dev_info(dev_addr);
			}break;
		default:
			{
				// ignore
			}break;
		}
	}
}

int main()
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_INFO, -1, NULL);

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
		output_dev_info(device);
	}

	// free devices list
	if (device_list)
	{
		pcap_freealldevs(device_list);
	}
	
	return 0;
}
