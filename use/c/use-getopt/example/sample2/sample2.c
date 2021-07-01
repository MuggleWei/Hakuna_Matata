/******************************************************************************
 *  @from  azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
 *  @brief        
 *  	read option with option arguments
 *
 *  | command line options |         output                     |
 *  | --------------------------------------------------------- |
 *  | -a one -b two -X     | option 'a' has arg: one            |
 *  |                      | Option 'b' has arg: two            |
 *  |                      | option 'X' was provided            |
 *  | --------------------------------------------------------- |
 *  | -aone -btwo          | option 'a' has arg: one            |
 *  |                      | Option 'b' has arg: two            |
 *  | --------------------------------------------------------- |
 *  | -X -a                | option 'X' was provided            |
 *  |                      | Option requires an argument -- 'a' |
 *  | --------------------------------------------------------- |
 *  | -a -X                | option 'a' has args: -X            |
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
	while ((opt = getopt(argc, argv, "a:b:X")) != -1)
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
		}
	}

	printf("bye bye\n");

	return 0;
}


