#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

int main()
{
	int total = 100;
	int cur = 0;
	while (cur <= total) {
		fprintf(stdout, "\r");
		fprintf(stdout, "[");
		for (int i = 1; i < total; i++) {
			if (i < cur) {
				fprintf(stdout, "=");
			} else if (i == cur) {
				fprintf(stdout, ">");
			} else {
				fprintf(stdout, " ");
			}
		}
		fprintf(stdout, "]");
		fprintf(stdout, "%d%%", cur);
		fflush(stdout);

		cur += 1;
		usleep(100000);
	}
}
