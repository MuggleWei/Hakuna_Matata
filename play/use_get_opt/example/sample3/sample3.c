/******************************************************************************
 *  @from  azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
 *  @brief        
 *  	Unknown options and missing option arguments, deal with the
 *  	errors ourselves
 *
 *  | command line options |         output                     |
 *  | --------------------------------------------------------- |
 *  | -a                   | Missing arg for a                  |
 *  | --------------------------------------------------------- |
 *  | -t                   | Unknown option: 't'                |
 *  | --------------------------------------------------------- |
 *  | -a one -t -X -b      | Option 'a' has arg: one            |
 *  |                      | Unknown option: t                  |
 *  |                      | Option 'X' was provided            |
 *  |                      | Missing arg for 'b'                |
 *  | --------------------------------------------------------- |
 *  | -a one,two,three     | Option 'a' has arg: one,two,three  |
 *  | --------------------------------------------------------- |
 *  | -a "one two three"   | Option 'a' has arg: one two three  |
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

	printf("bye bye\n");

	return 0;
}
