#include <stdio.h>
#include <time.h>

#define ANSI_ESC_CTRL_KEY "^["
#define ANSI_ESC_OCTAL "\033"
// #define ANSI_ESC_UNICODE \u001b
#define ANSI_ESC_HEX "\x1B"
#define ANSI_ESC_DEC "27"

#define ANSI_ESC ANSI_ESC_HEX
#define ANSI_CSI ANSI_ESC "["

#define ANSI_COLOR_RESET "0"

// 8 colors
#define ANSI_FG_COLOR_BLACK "30"
#define ANSI_FG_COLOR_RED "31"
#define ANSI_FG_COLOR_GREEN "32"
#define ANSI_FG_COLOR_YELLOW "33"
#define ANSI_FG_COLOR_BLUE "34"
#define ANSI_FG_COLOR_MAGENTA "35"
#define ANSI_FG_COLOR_CYAN "36"
#define ANSI_FG_COLOR_WHITE "37"

#define ANSI_BG_COLOR_BLACK "40"
#define ANSI_BG_COLOR_RED "41"
#define ANSI_BG_COLOR_GREEN "42"
#define ANSI_BG_COLOR_YELLOW "43"
#define ANSI_BG_COLOR_BLUE "44"
#define ANSI_BG_COLOR_MAGENTA "45"
#define ANSI_BG_COLOR_CYAN "46"
#define ANSI_BG_COLOR_WHITE "47"

// 256 colors
#define ANSI_FG_256_COLOR "38;5;"
#define ANSI_BG_256_COLOR "48;5;"

// true color
#define ANSI_COLOR_RGB "38;2;"

// attributes
#define ANSI_ATTR_NORMAL "0"
#define ANSI_ATTR_BOLD "1"
#define ANSI_ATTR_DIM "2"

int main()
{
	// basic 8 colors
	printf(ANSI_CSI ANSI_COLOR_RESET "m");
	printf("----------------\n");
	printf("foreground colors\n");
	printf(ANSI_CSI ANSI_FG_COLOR_BLACK "m"
										"BLACK\n");
	printf(ANSI_CSI ANSI_FG_COLOR_RED "m"
									  "RED\n");
	printf(ANSI_CSI ANSI_FG_COLOR_GREEN "m"
										"GREEN\n");
	printf(ANSI_CSI ANSI_FG_COLOR_YELLOW "m"
										 "YELLOW\n");
	printf(ANSI_CSI ANSI_FG_COLOR_BLUE "m"
									   "BLUE\n");
	printf(ANSI_CSI ANSI_FG_COLOR_MAGENTA "m"
										  "MAGENTA\n");
	printf(ANSI_CSI ANSI_FG_COLOR_CYAN "m"
									   "CYAN\n");
	printf(ANSI_CSI ANSI_FG_COLOR_WHITE "m"
										"WHITE\n");

	// SGR: CSI n m
	//      CSI m <=> CSI 0 m
	printf(ANSI_CSI ANSI_COLOR_RESET "m");
	printf("----------------\n");
	printf("SGR colors\n");
	printf(ANSI_CSI ANSI_ATTR_BOLD ";" ANSI_FG_COLOR_BLACK "m"
								   "BLACK\n");
	printf(ANSI_CSI ANSI_ATTR_BOLD ";" ANSI_FG_COLOR_RED "m"
								   "RED\n");
	printf(ANSI_CSI ANSI_ATTR_BOLD ";" ANSI_FG_COLOR_GREEN "m"
								   "GREEN\n");
	printf(ANSI_CSI ANSI_ATTR_BOLD ";" ANSI_FG_COLOR_YELLOW "m"
								   "YELLOW\n");
	printf(ANSI_CSI ANSI_ATTR_BOLD ";" ANSI_FG_COLOR_BLUE "m"
								   "BLUE\n");
	printf(ANSI_CSI ANSI_ATTR_BOLD ";" ANSI_FG_COLOR_MAGENTA "m"
								   "MAGENTA\n");
	printf(ANSI_CSI ANSI_ATTR_BOLD ";" ANSI_FG_COLOR_CYAN "m"
								   "CYAN\n");
	printf(ANSI_CSI ANSI_ATTR_BOLD ";" ANSI_FG_COLOR_WHITE "m"
								   "WHITE\n");

	// 256 colors
	// CSI38;5;{ID}m  Set foreground color
	// CSI48;5;{ID}m  Set background color
	// NOTE:
	//   color formula: 16 + 36r + 6g + b
	//   r, g and b are integers range: [0, 5]
	//   indices 232 through 255 are a grayscale ramp from dark (232) to light (255)
	printf(ANSI_CSI ANSI_COLOR_RESET "m");
	printf("----------------\n");
	printf("256 colors\n");
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int code = i * 16 + j;
			printf(ANSI_CSI ANSI_FG_256_COLOR "%dm", code);
			printf("%4d", code);
		}
		printf(ANSI_CSI ANSI_COLOR_RESET "m");
		printf("\n");
	}

	printf(ANSI_CSI ANSI_COLOR_RESET "m");
	printf("\n");
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int code = i * 16 + j;
			printf(ANSI_CSI ANSI_BG_256_COLOR "%dm", code);
			printf("%4d", code);
		}
		printf(ANSI_CSI ANSI_COLOR_RESET "m");
		printf("\n");
	}

	// true colors
	printf("\n");
	printf(ANSI_CSI ANSI_COLOR_RGB "%d;%d;%dm", 255, 0, 0);
	printf("RED");
	printf(ANSI_CSI ANSI_COLOR_RGB "%d;%d;%dm", 0, 255, 0);
	printf("GREEN");
	printf(ANSI_CSI ANSI_COLOR_RGB "%d;%d;%dm", 0, 0, 255);
	printf("BLUE");

	return 0;
}
