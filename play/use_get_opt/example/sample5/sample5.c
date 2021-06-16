/******************************************************************************
 *  @from  azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
 *  @brief        
 *  	Non-Option arguments, parse and return the non-option arguments in
 *  	the while loop(in the order specified
 *
 *  | command line options     |         output                 |
 *  | --------------------------------------------------------- |
 *  | x y z -a foo             | Non-option arg: x              |
 *  |                          | Non-option arg: y              |
 *  |                          | Non-option arg: z              |
 *  |                          | Option 'a' has arg: foo        |
 *  | --------------------------------------------------------- |
 *  | x -a foo y -b bar z -X w | Non-option arg: x              |
 *  |                          | Option 'a' has arg: foo        |
 *  |                          | Non-option arg: y              |
 *  |                          | Option 'b' has arg: bar        |
 *  |                          | Non-option arg: z              |
 *  |                          | Option 'X' was provided        |
 *  |                          | Non-option arg: w              |
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
	while ((opt = getopt(argc, argv, "-:a:b:X")) != -1)
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
			case 1:
			{
				printf("Non-option arg: %s\n", optarg);
			}break;
		}
	}

	printf("bye bye\n");

	return 0;
}


