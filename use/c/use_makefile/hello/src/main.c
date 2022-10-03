#include <stdio.h>
#include <pthread.h>
#include "utils/foo.h"
#include "utils/bar.h"

void* thread_proc(void *args)
{
	for (int i = 0; i < 2; i++)
	{
		foo_print();
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	// foo_print();
	pthread_t thread_id;
	int ret = pthread_create(&thread_id, NULL, thread_proc, NULL);
	if (ret != 0)
	{
		fprintf(stderr, "failed create thread");
		return 0;
	}

	pthread_join(thread_id, NULL);
	fprintf(stdout, "bye\n");
	
	return 0;
}
