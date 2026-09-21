#define MUGGLE_HOLD_LOG_MACRO 1
#include "miniz.h"
#include "miniz_zip.h"
#include "muggle/c/muggle_c.h"

typedef struct {
	size_t total_bytes;
	size_t cur;
	size_t percent;
} unzip_handle_t;

void print_progress(int percent)
{
	int bar_width = 50;
	int filled = percent * bar_width / 100;

	printf("\r[");
	for (int i = 0; i < bar_width; i++) {
		putchar(i < filled ? '=' : ' ');
	}
	printf("] %3d%%  ", percent);
	fflush(stdout);
}

static size_t handle_callback(void *pOpaque, mz_uint64 file_ofs,
							  const void *pBuf, size_t n)
{
	MUGGLE_UNUSED(pBuf);

	unzip_handle_t *handle = (unzip_handle_t *)pOpaque;
	MUGGLE_ASSERT(handle->cur == file_ofs);

	handle->cur += n;

	if (handle->total_bytes > 0) {
		handle->percent = (100 * handle->cur) / handle->total_bytes;
	}
	print_progress(handle->percent);

	return n;
}

void run_unzip(const char *filepath)
{
	mz_zip_archive zipfile;
	memset(&zipfile, 0, sizeof(zipfile));

	// init zip file
	if (!mz_zip_reader_init_file(&zipfile, filepath, 0)) {
		LOG_ERROR("failed init zipfile for write");
		exit(EXIT_FAILURE);
	}

	mz_uint nfiles = mz_zip_reader_get_num_files(&zipfile);
	LOG_INFO("total %u files in %s", nfiles, filepath);

	for (mz_uint i = 0; i < nfiles; ++i) {
		char filename[256];
		mz_zip_reader_get_filename(&zipfile, i, filename, sizeof(filename));

		mz_zip_archive_file_stat stat;
		mz_zip_reader_file_stat(&zipfile, i, &stat);

		unzip_handle_t handle;
		memset(&handle, 0, sizeof(handle));
		handle.total_bytes = stat.m_uncomp_size;
		if (!mz_zip_reader_extract_to_callback(&zipfile, i, handle_callback,
											   &handle, 0)) {
			LOG_ERROR("failed extract %d(%s)", i, filename);
			continue;
		}

		LOG_INFO("success extract %d(%s), %lu bytes in it", i, filename,
				 handle.cur);
	}

	mz_zip_reader_end(&zipfile);
}

int main(int argc, char *argv[])
{
	if (muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL) != 0) {
		fprintf(stderr, "failed init log");
		exit(EXIT_FAILURE);
	}

	if (argc < 2) {
		LOG_ERROR("usage: %s <zip_file>", argv[0]);
		exit(EXIT_FAILURE);
	}

	run_unzip(argv[1]);

	return 0;
}
