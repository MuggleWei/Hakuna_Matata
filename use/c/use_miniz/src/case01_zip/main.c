#define MUGGLE_HOLD_LOG_MACRO 1
#include "muggle/c/muggle_c.h"
#include "miniz.h"
#include "miniz_zip.h"

void run_zip(const char *output_file, const char **input_files, int n_input)
{
	mz_zip_archive zipfile;
	memset(&zipfile, 0, sizeof(zipfile));

	bool has_err = false;
	FILE *fp = NULL;
	unsigned long nbytes = 0;
	char *buf = NULL;

	// init zip file
	if (!mz_zip_writer_init_file(&zipfile, output_file, 0)) {
		LOG_ERROR("failed init zipfile for write");
		exit(EXIT_FAILURE);
	}

	// add file into zipfile
	for (int i = 0; i < n_input; ++i) {
		// read file
		fp = muggle_os_fopen(input_files[i], "rb");
		if (fp == NULL) {
			LOG_ERROR("failed open file: %s", input_files[i]);
			has_err = true;
			goto run_zip_exit;
		}

		fseek(fp, 0, SEEK_END);
		nbytes = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		buf = (char *)malloc((size_t)nbytes);
		if (buf == NULL) {
			LOG_ERROR("failed allocate %ld bytes", nbytes);
			has_err = true;
			goto run_zip_exit;
		}

		unsigned long nread = fread(buf, 1, nbytes, fp);
		if (nread != nbytes) {
			LOG_ERROR(
				"read file nbytes not equal expect, expect: %lu, actual: %lu",
				nbytes, nread);
			has_err = true;
			goto run_zip_exit;
		}

		// write into zipfile
		const char *entry_name = input_files[i];
		if (!mz_zip_writer_add_mem(&zipfile, entry_name, buf, nbytes,
								   MZ_DEFAULT_COMPRESSION)) {
			LOG_ERROR("failed write %s, err: %s", entry_name,
					  mz_zip_get_error_string(mz_zip_get_last_error(&zipfile)));
			has_err = true;
			goto run_zip_exit;
		}

		// cleanup
		free(buf);
		buf = NULL;

		fclose(fp);
		fp = NULL;
	}

	if (!mz_zip_writer_finalize_archive(&zipfile)) {
		LOG_ERROR("failed finalize archive, err: %s",
				  mz_zip_get_error_string(mz_zip_get_last_error(&zipfile)));
		goto run_zip_exit;
	}

run_zip_exit:
	mz_zip_writer_end(&zipfile);

	if (buf != NULL) {
		free(buf);
		buf = NULL;
	}

	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}

	if (has_err) {
		muggle_os_remove(output_file);
	}

	if (!has_err) {
		LOG_INFO("zipfile %s create completed", output_file);
	}
}

int main(int argc, char *argv[])
{
	if (muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL) != 0) {
		fprintf(stderr, "failed init log");
		exit(EXIT_FAILURE);
	}

	if (argc < 3) {
		LOG_ERROR("usage: %s <output> <input_file1> [input_files...]", argv[0]);
		exit(EXIT_FAILURE);
	}

	run_zip(argv[1], (const char **)&argv[2], argc - 2);

	return 0;
}
