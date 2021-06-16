/******************************************************************************
 *  @from  azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
 *  @brief        
 *  	read option without arguments
 *
 *  | command line options |         output          |
 *  | ---------------------------------------------- |
 *  | -b                   | Option 'b' was provided |
 *  | ---------------------------------------------- |
 *  | -b -X -a             | Option 'b' was provided |
 *  |                      | Option 'X' was provided |
 *  |                      | Option 'a' was provided |
 *  | ---------------------------------------------- |
 *  | -bXa                 | Option 'b' was provided |
 *  |                      | Option 'X' was provided |
 *  |                      | Option 'a' was provided |
 *  | ---------------------------------------------- |
 *  | -t                   | Invalid option  -- 't'  |
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
	while ((opt = getopt(argc, argv, "abX")) != -1)
	{
		switch (opt)
		{
			case 'a':
			{
				printf("Option 'a' was provided\n");
			}break;
			case 'b':
			{
				printf("Option 'b' was provided\n");
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

