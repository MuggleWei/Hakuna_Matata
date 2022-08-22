#include "muggle/c/muggle_c.h"
#include "zlib.h"

int32_t file_size(const char *filepath)
{
	FILE *fp = fopen(filepath, "rb");
	if (fp == NULL)
	{
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long len = ftell(fp);
	fclose(fp);

	return (int32_t)len;
}

int compress_one_file(const char *input_filepath, const char *output_filepath)
{
	FILE *input_fp = NULL;
	gzFile output_fp = NULL;

	input_fp = fopen(input_filepath, "rb");
	if (input_fp == NULL)
	{
		LOG_ERROR("failed open input file: %s", input_filepath);
		goto compress_one_file_except;
	}

	output_fp = gzopen(output_filepath, "wb");
	if (output_fp == NULL)
	{
		LOG_ERROR("failed open output file: %s", output_filepath);
		goto compress_one_file_except;
	}

	char buf[128];
	int num_read = 0, num_write = 0;
	uint32_t total_read = 0;
	while ((num_read = (int)fread(buf, 1, sizeof(buf), input_fp)) > 0)
	{
		total_read += (uint32_t)num_read;
		num_write = gzwrite(output_fp, buf, num_read);
		if (num_write != num_read)
		{
			LOG_ERROR("failed write!");
			goto compress_one_file_except;
		}
	}

	fclose(input_fp);
	gzclose(output_fp);

	int32_t total_write = file_size(output_filepath);
	LOG_INFO(
		"read %lu bytes, write %lu bytes, compression factor %4.2f%%",
		total_read, (uint32_t)total_write,
		(1.0 - (double)total_write / (double)total_read) * 100.0);

	return 0;

compress_one_file_except:
	if (input_fp)
	{
		fclose(input_fp);
		input_fp = NULL;
	}

	if (output_fp)
	{
		gzclose(output_fp);
		output_fp = NULL;
	}

	return -1;
}

int decompress_one_file(const char *input_filepath, const char *output_filepath)
{
	gzFile input_fp = NULL;
	FILE *output_fp = NULL;


	input_fp = gzopen(input_filepath, "rb");
	if (input_fp == NULL)
	{
		LOG_ERROR("failed open input file: %s", input_filepath);
		goto decompress_one_file_except;
	}

	output_fp = fopen(output_filepath, "wb");
	if (output_fp == NULL)
	{
		LOG_ERROR("failed open output file: %s", output_filepath);
		goto decompress_one_file_except;
	}

	char buf[128];
	int num_read = 0;
	while ((num_read = (int)gzread(input_fp, buf, sizeof(buf))) > 0)
	{
		fwrite(buf, 1, num_read, output_fp);
	}

	gzclose(input_fp);
	fclose(output_fp);

	return 0;

decompress_one_file_except:
	if (input_fp)
	{
		gzclose(input_fp);
		input_fp = NULL;
	}

	if (output_fp)
	{
		fclose(output_fp);
		output_fp = NULL;
	}

	return -1;
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc != 4)
	{
		LOG_ERROR(
			"Usage: %s <c|d> <src> <dst>\n"
			"gz [c]ompress or [d]epress file\n"
			"e.g.\n"
			"\t%s c zlib_wiki.md zlib_wiki.md.gz\n"
			"\t%s d zlib_wiki.md.gz zlib_wiki.md\n",
			argv[0], argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}

	int ret = 0;
	if (argv[1][0] == 'c' || argv[1][0] == 'C')
	{
		ret = compress_one_file(argv[2], argv[3]);
		if (ret != 0)
		{
			LOG_ERROR("failed gz compress file");
			exit(EXIT_FAILURE);
		}
	}
	else if (argv[1][0] == 'd' || argv[1][0] == 'D')
	{
		ret = decompress_one_file(argv[2], argv[3]);
		if (ret != 0)
		{
			LOG_ERROR("failed gz decompress file");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		LOG_ERROR("unrecognize command: %s", argv[1]);
	}
	
	return 0;
}
