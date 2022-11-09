#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "muggle/c/muggle_c.h"

/////////////////////////////////////////////////////////////////////
//           Structure of a classical generic MBR
// ------------------------------------------------------------------
// |  offset  |  Size  |                  Desc
// |----------|--------|---------------------------------------------
// |    0     |   440  | Code Area
// |----------|--------|---------------------------------------------
// |   446    |   64   | Partition Table scheme, Four 16-byte enries
// |----------|--------|---------------------------------------------
// |   510    |   2    | Boot signature (0x55, 0xAA)
// ------------------------------------------------------------------
//
//                     Partition Entry
// ------------------------------------------------------------------
// |  offset  |  Size  |                  Desc
// |----------|--------|---------------------------------------------
// |    0     | 1 byte | Status:
// |          |        | 0x80=bootable
// |          |        | 0x00=non-bootable
// |          |        | other=malformed
// |----------|--------|---------------------------------------------
// |    1     | 1 byte | Starting head
// |    2     | 6 bits | Starting sector
// |          | 2 bits | Starting cylinder
// |    3     | 1 byte | Cylinder
// |----------|--------|---------------------------------------------
// |    4     | 1 byte | Partition type
// |----------|--------|---------------------------------------------
// |    5     | 3 bytes| CHS add
// ------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////

#define MBR_SIZE 512

;
#pragma pack(push, 1)

struct mbr_partition
{
	union
	{
		char entry[16];
		struct
		{
			uint8_t status;
			uint8_t start_head;
			uint16_t start_sector : 6;
			uint16_t start_cylinder : 10;
			uint8_t part_type;
			uint8_t end_head;
			uint16_t end_sector : 6;
			uint16_t end_cylinder : 10;
			uint32_t lba_offset;
			uint32_t lba_sectors;
		};
	};
};

struct MBR
{
	char                 code_area[446];
	struct mbr_partition tables[4];
	char                 boot_signature[2];
};

#pragma pack(pop)

int read_mbr(const char *dev, char *mbr_buf, size_t mbr_bufsize)
{
	int fd = -1;

	fd = open(dev, O_RDONLY | O_CLOEXEC | O_NONBLOCK);
	if (fd < 0)
	{
		char errmsg[512];
		snprintf(errmsg, sizeof(errmsg), "failed open %s", dev);
		LOG_SYS_ERR(LOG_LEVEL_ERROR, errmsg);
		goto err_read_mbr;
	}

	struct stat sb;
	if (fstat(fd, &sb) != 0)
	{
		char errmsg[512];
		snprintf(errmsg, sizeof(errmsg), "failed fstat(%s)", dev);
		LOG_SYS_ERR(LOG_LEVEL_ERROR, errmsg);
		goto err_read_mbr;
	}

	if ((sb.st_mode & __S_IFMT) != __S_IFBLK)
	{
		LOG_ERROR("%s is not block device");
		goto err_read_mbr;
	}

	uint64_t numbytes = 0;
	if (ioctl(fd, BLKGETSIZE64, &numbytes) < 0)
	{
		char errmsg[512];
		snprintf(errmsg, sizeof(errmsg), "failed get device size %s", dev);
		LOG_SYS_ERR(LOG_LEVEL_ERROR, errmsg);
		goto err_read_mbr;
	}

	int sector_size = 0;
	if (ioctl(fd, BLKSSZGET, &sector_size) < 0)
	{
		char errmsg[512];
		snprintf(errmsg, sizeof(errmsg), "failed get sector size %s", dev);
		LOG_SYS_ERR(LOG_LEVEL_ERROR, errmsg);
		goto err_read_mbr;
	}

	LOG_INFO("device(%s), sector size: %d, device size: %llu",
		dev, sector_size, (unsigned long long)numbytes);

	ssize_t n = read(fd, mbr_buf, mbr_bufsize);
	if (n != (ssize_t)mbr_bufsize)
	{
		LOG_ERROR("failed read MBR, expect size: %u, real read: %u",
			(unsigned int)mbr_bufsize, (unsigned int)n);
		goto err_read_mbr;
	}

	close(fd);

	return (int)n;

err_read_mbr:
	if (fd != -1)
	{
		close(fd);
	}

	return -1;
}

void hexdump(const char *mbr, int len)
{
	for (int i = 0; i < len; i++)
	{
		printf("%02x ", mbr[i] & 0xff);

		if ((i + 1) % 16 == 0)
		{
			printf("\n");
		}
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL);

	if (argc < 2)
	{
		LOG_ERROR("Usage: %s <device>\ne.g.\n\t%s /dev/sda", argv[1], argv[1]);
		exit(EXIT_FAILURE);
	}

	// read MBR
	const char *dev = argv[1];
	struct MBR mbr;
	if (read_mbr(dev, (char*)&mbr, MBR_SIZE) != MBR_SIZE)
	{
		LOG_ERROR("failed read MBR");
		exit(EXIT_FAILURE);
	}

	// code area
	printf("---------------- Code Area ----------------\n");
	hexdump(mbr.code_area, sizeof(mbr.code_area));
	printf("\n");

	printf("---------------- Partition Table ----------------\n");
	for (int i = 0; i < 4; i++)
	{
		printf("##### Partition entry #%d #####\n", i+1);
		hexdump((const char*)&mbr.tables[i], sizeof(struct mbr_partition));
		printf("status: %u\n", (unsigned int)mbr.tables[i].status);
		printf("partition type: %02x\n", mbr.tables[i].part_type & 0xff);
		printf("start head: %u\n", (unsigned int)mbr.tables[i].start_head);
		printf("start sector: %u\n", (unsigned int)mbr.tables[i].start_sector);
		printf("start cylinder: %u\n",
			(unsigned int)mbr.tables[i].start_cylinder);
		printf("end head: %u\n", (unsigned int)mbr.tables[i].end_head);
		printf("end sector: %u\n", (unsigned int)mbr.tables[i].end_sector);
		printf("end cylinder: %u\n",
			(unsigned int)mbr.tables[i].end_cylinder);
		printf("LBA offset: %u\n", (unsigned int)mbr.tables[i].lba_offset);
		printf("LBA sector count: %u\n", (unsigned int)mbr.tables[i].lba_sectors);
		printf("\n");
	}

	printf("---------------- Boot Signature ----------------\n");
	hexdump(mbr.boot_signature, sizeof(mbr.boot_signature));

	return 0;
}
