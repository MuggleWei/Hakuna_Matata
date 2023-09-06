#include "muggle/c/muggle_c.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	const char *sound_filepath = "sound.wav";
	if (argc > 1) {
		sound_filepath = argv[1];
	}

	ma_result result;
	ma_engine engine;

	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		LOG_ERROR("failed ma_engine_init");
		exit(EXIT_FAILURE);
	}

	LOG_INFO("prepare play sound: %s", sound_filepath);

	result = ma_engine_play_sound(&engine, sound_filepath, NULL);
	if (result != MA_SUCCESS) {
		LOG_ERROR("failed play sound");
		exit(EXIT_FAILURE);
	}

	printf("Press Enter to quit...");
	getchar();

	ma_engine_uninit(&engine);

	return 0;
}
