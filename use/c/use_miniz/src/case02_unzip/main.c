#define MUGGLE_HOLD_LOG_MACRO 1
#include "muggle/c/muggle_c.h"
#include "miniz.h"
#include "miniz_zip.h"

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

		size_t uncomp_size = 0;
		void *p = mz_zip_reader_extract_to_heap(&zipfile, i, &uncomp_size, 0);
		if (p == NULL) {
			LOG_ERROR("failed extract %d(%s)", i, filename);
			continue;
		}

		LOG_INFO("success extract %d(%s), %lu bytes in it", i, filename,
				 uncomp_size);

		FILE *fp = muggle_os_fopen(filename, "wb");
		if (fp) {
			LOG_INFO("unzip %s", filename);
			fwrite(p, 1, uncomp_size, fp);
			fclose(fp);
		}

		mz_free(p);
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
