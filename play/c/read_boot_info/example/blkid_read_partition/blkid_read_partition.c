#include "muggle/c/muggle_c.h"
#include "blkid/blkid.h"

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL);

	if (argc < 2)
	{
		LOG_ERROR("Usage: %s <device|file>", argv[0]);
		exit(EXIT_FAILURE);
	}

	const char *dev = argv[1];
	blkid_probe probe = blkid_new_probe_from_filename(dev);
	if (!probe)
	{
		LOG_ERROR("%s: failed to create a new libblkid probe", dev);
		exit(EXIT_FAILURE);
	}

	// binary interface
	blkid_partlist partlist = blkid_probe_get_partitions(probe);
	if (!partlist)
	{
		LOG_ERROR("%s: failed to read partitions");
		exit(EXIT_FAILURE);
	}

	blkid_parttable root_tab = blkid_partlist_get_table(partlist);
	if (!root_tab)
	{
		LOG_ERROR("%s: does not contains any known partition table");
		exit(EXIT_FAILURE);
	}

	LOG_INFO("size: %lld, sector size: %u, PT: %s, offset: %lld, id=%s",
		(long long)blkid_probe_get_size(probe),
		(unsigned)blkid_probe_get_sectorsize(probe),
		blkid_parttable_get_type(root_tab),
		(long long)blkid_parttable_get_offset(root_tab),
		blkid_parttable_get_id(root_tab));

	blkid_free_probe(probe);

	return 0;
}
