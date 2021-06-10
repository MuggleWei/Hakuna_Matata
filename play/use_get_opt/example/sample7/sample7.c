/******************************************************************************
 *  @from  azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
 *  @brief        
 *  	Long Options
 *
 *  |      command line options           |            output               |
 *  | --------------------------------------------------------------------- |
 *  | --delete=foo -c5 --add=yes --append | long option delete with arg foo | 
 *  |                                     | option c with value '5'         |
 *  |                                     | long option add with arg yes    |
 *  |                                     | long option append              |
 *  | --------------------------------------------------------------------- |
 *  | --d=foo --ad=yes --ap --a           | long option delete with arg foo |
 *  |                                     | long option add with arg yes    |
 *  |                                     | long option append              |
 *  |                                     | Unknown option                  |
 *  | --------------------------------------------------------------------- |
 *  | --create=5 --create 6 --c=7 --c 8   | long option create with arg 5   |
 *  |                                     | long option create with arg 6   |
 *  |                                     | long option create with arg 7   |
 *  |                                     | long option create with arg 8   |
 *  | --------------------------------------------------------------------- |
 *  | --file=5 --file 6 --file7           | long option file with arg 5     | 
 *  |                                     | long option file                |
 *  |                                     | regular argument '6'            |
 *  |                                     | Unknown option                  |
 *****************************************************************************/

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int c;
	while (1)
	{
		int option_index = 0;
		static struct option long_options[] =
		{
			{"add",     required_argument, NULL, 0},
			{"append",  no_argument,       NULL, 0},
			{"delete",  required_argument, NULL, 0},
			{"verbose", no_argument,       NULL, 0},
			{"create",  required_argument, NULL, 0},
			{"file",    optional_argument, NULL, 0},
			{NULL,      0,                 NULL, 0}
		};

		c = getopt_long( argc, argv, "-:abc:d::", long_options, &option_index);
		if (c == -1)
			break;

		switch(c)
		{
			case 0:
			{
				printf("long option %s", long_options[option_index].name);
				if (optarg)
					printf(" with arg %s", optarg);
				printf("\n");
			}break;
			case 1:
			{
				printf("regular argument '%s'\n", optarg); /* non-option arg */
			}break;
			case 'a':
			{
				printf("option a\n");
			}break;
			case 'b':
			{
				printf("option b\n");
			}break;
			case 'c':
			{
				printf("option c with value '%s'\n", optarg);
			}break;
			case 'd':
			{
				printf("option d with value '%s'\n", optarg ? optarg : "NULL");
			}break;
			case '?':
			{
				printf("Unknown option %c\n", optopt);
			}break;
			case ':':
			{
				printf("Missing option for %c\n", optopt);
			}break;
			default:
			{
				printf("?? getopt returned character code 0%o ??\n", c);
			}break;
		}
	}
	
	/* Deal with non-option arguments here */
	if ( optind < argc )
	{
		while ( optind < argc )
		{
			
		}
	}

	return 0;
}
