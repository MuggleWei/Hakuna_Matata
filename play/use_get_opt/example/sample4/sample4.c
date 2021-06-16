/******************************************************************************
 *  @from  azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
 *  @brief        
 *  	Non-Option arguments
 *
 *  | command line options |         output                     |
 *  | --------------------------------------------------------- |
 *  | x -a one y -X z      | Option 'a' has arg: one            |
 *  |                      | Option 'X' was provided            |
 *  |                      | Non-option args: x y z             |
 *  | --------------------------------------------------------- |
 *  | x y z -a one -b two  | Option 'a' has arg: one            |
 *  |                      | Option 'b' has arg: two            |
 *  |                      | Non-option args: x y z             |
 *****************************************************************************/
 
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++)
	{
		printf("arg#%d = %s\n", i, argv[i]);
	}

	int opt;
	while ((opt = getopt(argc, argv, ":a:b:X")) != -1)
	{
		switch (opt)
		{
			case 'a':
			{
				printf("Option 'a' has arg: %s\n", optarg);
			}break;
			case 'b':
			{
				printf("Option 'b' has arg: %s\n", optarg);
			}break;
			case 'X':
			{
				printf("Option 'X' was provided\n");
			}break;
			case '?':
			{
				printf("Unknown option: %c\n", optopt);
			}break;
			case ':':
			{
				printf("Missing arg for '%c'\n", (char)opt);
			}break;
		}
	}

	// Get all of the non-option arguments
	if (optind < argc)
	{
		printf("Non-option args: ");
		while (optind < argc)
		{
			printf("%s ", argv[optind++]);
		}
		printf("\n");
	}

	printf("bye bye\n");

	return 0;
}

