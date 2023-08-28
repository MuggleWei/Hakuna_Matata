#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static const int n = 5;

int main()
{
	int f[n];
	bool ret = false;
	memset(f, 0, sizeof(f));

	for (int i = 0; i < n; i++) {
		if (f[i + 1] != 0) {
			printf("%d no!\n", i);
			ret = false;
		} else {
			printf("%d yes!\n", i);
		}
	}

	return ret;
}
