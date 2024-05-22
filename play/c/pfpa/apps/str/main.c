#include "apps/str/str_handle.h"
#include "muggle/c/muggle_c.h"
#include "muggle/c/net/socket.h"
#include "pfpa/pfpa.h"
#include <arpa/inet.h>

typedef struct {
	char input[512];
	char output[512];
} sys_args_t;

static void output_usage(const char *argv0, FILE *fp)
{
	fprintf(fp,
			"--------------------------------\n"
			"Usage: %s <options>\n"
			"    -i, --input       input file\n"
			"    -o, --output      output file\n"
			"    -f, --filter      filter host and port\n"
			"\n"
			"e.g.\n"
			"%s -i x.cap -o x.txt\n"
			"%s -i x.cap -o x.txt -f 127.0.0.1:8080\n"
			"--------------------------------\n"
			"",
			argv0, argv0, argv0);
}

static bool parse_args(int argc, char **argv, sys_args_t *cfg,
					   pfpa_context_t *ctx)
{
	int c;

	memset(cfg, 0, sizeof(sys_args_t));

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "input", required_argument, NULL, 'i' },
			{ "output", required_argument, NULL, 'o' },
			{ "filter", required_argument, NULL, 'f' },
			{ NULL, 0, NULL, 0 }
		};

		c = getopt_long(argc, argv, "hi:o:f:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h': {
			output_usage(argv[0], stdout);
			exit(EXIT_SUCCESS);
		} break;
		case 'i': {
			strncpy(cfg->input, optarg, sizeof(cfg->input) - 1);
		} break;
		case 'o': {
			strncpy(cfg->output, optarg, sizeof(cfg->output) - 1);
		} break;
		case 'f': {
			ctx->enable_fileter = true;

			pfpa_addr_t *tuple = (pfpa_addr_t *)malloc(sizeof(pfpa_addr_t));

			char addr[MUGGLE_SOCKET_ADDR_STRLEN];
			memset(addr, 0, sizeof(addr));
			strncpy(addr, optarg, sizeof(addr) - 1);

			int pos = muggle_str_find(addr, ":", 0, 0);
			if (pos >= 0) {
				addr[pos] = '\0';
				tuple->port = atoi(&addr[pos + 1]);
			}
			inet_pton(AF_INET, addr, &tuple->addr);

			pfpa_ctx_add_filter(ctx, tuple);
		} break;
		default: {
			LOG_ERROR("invalid input args: '%c'", (char)c);
			return false;
		} break;
		}
	}

	if (cfg->input[0] == '\0') {
		LOG_ERROR("sys args without 'input'");
		return false;
	}
	if (cfg->output[0] == '\0') {
		LOG_ERROR("sys args without 'output'");
		return false;
	}

	return true;
}

void offline_capture(sys_args_t *cfg, pfpa_context_t *ctx)
{
	FILE *fp = fopen(cfg->output, "w");
	if (fp == NULL) {
		LOG_ERROR("failed open output file: %s", cfg->output);
		return;
	}
	ctx->fp = fp;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle = pcap_open_offline_with_tstamp_precision(
		cfg->input, PCAP_TSTAMP_PRECISION_NANO, errbuf);
	if (handle == NULL) {
		LOG_ERROR("failed open input file: %s", cfg->input);
		return;
	}

	pcap_loop(handle, 0, pfpa_packet_handle, (void *)ctx);
	LOG_INFO("total handle packet num: %u", ctx->total_pack);

	pcap_close(handle);
	fclose(fp);
}

int main(int argc, char *argv[])
{
	if (!pfpa_log_init(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, "logs/hello.log")) {
		fprintf(stderr, "failed init log");
		exit(EXIT_FAILURE);
	}

	pfpa_context_t ctx;
	pfpa_ctx_init(&ctx);

	sys_args_t args;
	if (!parse_args(argc, argv, &args, &ctx)) {
		output_usage(argv[0], stdout);
		exit(EXIT_FAILURE);
	}

	ctx.tcp_callback = on_str_tcp_packet;
	ctx.udp_callback = on_str_udp_packet;

	offline_capture(&args, &ctx);

	pfpa_ctx_destroy(&ctx);

	return 0;
}
