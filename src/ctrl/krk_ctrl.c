/**
 * krk_ctrl.c - Krake configuration client
 * 
 * Copyright (c) 2010 Yang Yang <paulyang.inf@gmail.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <config.h>
#include <krk_core.h>
#include <krk_socket.h>
#include <krk_config.h>

#define KRK_OPTION_ENABLE 1
#define KRK_OPTION_DISABLE 2
#define KRK_OPTION_CHECKER 3
#define KRK_OPTION_CHECKER_CONF 4
#define KRK_OPTION_MONITOR 5
#define KRK_OPTION_INTERVAL 6
#define KRK_OPTION_TIMEOUT 7
#define KRK_OPTION_THRESHOLD 8
#define KRK_OPTION_NODE 9
#define KRK_OPTION_PORT 10


static const struct option optlong[] = {
	{"help", 0, NULL, 'h'},
	{"version", 0, NULL, 'v'},
	{"create", 0, NULL, 'C'},
	{"destroy", 0, NULL, 'D'},
	{"add", 0, NULL, 'A'},
	{"remove", 0, NULL, 'R'},
	{"enable", 0, NULL, KRK_OPTION_ENABLE},
	{"disable", 0, NULL, KRK_OPTION_DISABLE},
	{"checker", 1, NULL, KRK_OPTION_CHECKER},
	{"checker-conf", 1, NULL, KRK_OPTION_CHECKER_CONF},
	{"monitor", 1, NULL, KRK_OPTION_MONITOR},
	{"interval", 1, NULL, KRK_OPTION_INTERVAL},
	{"timeout", 1, NULL, KRK_OPTION_TIMEOUT},
	{"threshold", 1, NULL, KRK_OPTION_THRESHOLD},
	{"node", 1, NULL, KRK_OPTION_NODE},
	{"port", 1, NULL, KRK_OPTION_PORT},
	{NULL, 0, NULL, 0}
};

static const char* optstring = "hvCDAR";

static void krk_ctrl_usage(void)
{
	printf("Usage: krakectrl [option]\n"
			"\t--version/-v		Show Krake version\n"
			"\t--help/-h		Show this help\n");
}

static void krk_ctrl_version(void)
{
	printf("Krake ver: %s\n", PACKAGE_VERSION);
}

int main(int argc, char* argv[])
{
	int sock, ret, len;
	int opt, quit = 0, mutex = 0, n = 0, conf_len = 0;
	void *ptr;
	struct sockaddr_un addr;
	struct krk_config *config;
	char *result;

	/* 
	 * 1) handle argv
	 * 2) handle socket to krake daemon
	 * 3) send configuration to krake daemon
	 * 4) get result from krake daemon
	 */

	if (argc <= 1) {
		krk_ctrl_usage();
		return 1;
	}

	config = malloc(sizeof(struct krk_config));
	memset(config, 0, sizeof(struct krk_config));
	
	while (1) {
		opt = getopt_long(argc, argv, optstring, optlong, NULL);

		if (opt == -1)
			break;

		switch (opt) {
			case 'h':
				krk_ctrl_usage();
				quit = 1;
				break;
			case 'v':
				krk_ctrl_version();
				quit = 1;
				break;
			case 'C':
				if (mutex == 0) {
					config->command = KRK_CONF_CMD_CREATE;
					config->type = KRK_CONF_TYPE_MONITOR;
					mutex = 1;
				} else {
					goto failed;
				}
				break;
			case 'D':
				if (mutex == 0) {
					config->command = KRK_CONF_CMD_DESTROY;
					config->type = KRK_CONF_TYPE_MONITOR;
					mutex = 1;
				} else {
					goto failed;
				}
				break;
			case 'A':
				if (mutex == 0) {
					config->command = KRK_CONF_CMD_ADD;
					config->type = KRK_CONF_TYPE_NODE;
					mutex = 1;
				} else {
					goto failed;
				}
				break;
			case 'R':
				if (mutex == 0) {
					config->command = KRK_CONF_CMD_REMOVE;
					config->type = KRK_CONF_TYPE_NODE;
					mutex = 1;
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_ENABLE:
				if (mutex == 0) {
					config->command = KRK_CONF_CMD_ENABLE;
					config->type = KRK_CONF_TYPE_MONITOR;
					mutex = 1;
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_DISABLE:
				if (mutex == 0) {
					config->command = KRK_CONF_CMD_DISABLE;
					config->type = KRK_CONF_TYPE_MONITOR;
					mutex = 1;
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_MONITOR:
				if (config->type == KRK_CONF_TYPE_MONITOR
						|| config->type == KRK_CONF_TYPE_NODE) {
					strcpy(config->monitor, optarg);
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_CHECKER:
				if (config->type == KRK_CONF_TYPE_MONITOR
						&& config->monitor[0]) {
					strcpy(config->checker, optarg);
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_CHECKER_CONF:
				if (config->type == KRK_CONF_TYPE_MONITOR
						&& config->checker[0]) {
					conf_len = strlen(optarg) + 1;
					config = realloc(config, 
							sizeof(struct krk_config) + conf_len);
					config->checker_conf = config->data;
					strcpy(config->checker_conf, optarg);
					config->checker_conf_len = conf_len;
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_INTERVAL:
				if (config->type == KRK_CONF_TYPE_MONITOR
						&& config->monitor[0]) {
					config->interval = atoi(optarg);
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_TIMEOUT:
				if (config->type == KRK_CONF_TYPE_MONITOR
						&& config->monitor[0]) {
					config->timeout = atoi(optarg);
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_THRESHOLD:
				if (config->type == KRK_CONF_TYPE_MONITOR
						&& config->monitor[0]) {
					config->threshold = atoi(optarg);
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_NODE:
				if (config->type == KRK_CONF_TYPE_NODE
						&& config->monitor[0]) {
					strcpy(config->node, optarg);
				} else {
					goto failed;
				}
				break;
			case KRK_OPTION_PORT:
				if (config->type == KRK_CONF_TYPE_NODE
						&& config->node[0]) {
					config->port = atoi(optarg);
				} else {
					goto failed;
				}
				break;
			default:
				/* never could be here */
				break;
		}
	}

	if (quit) {
		return 0;
	}

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		return 1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, LOCAL_SOCK_PATH, 
			sizeof(addr.sun_path) - 1);

	ret = connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_un));
	if (ret < 0) {
		perror("connect");
		return 1;
	}

	ptr = config;
	len = sizeof(struct krk_config) + conf_len;
	
	while(1) {
		n = send(sock, ptr, len, 0);
		if (n < 0) {
			perror("send");
			return 1;
		}

		if (n == len) {
			break;
		} else {
			ptr += n;
			len -= n;
		}
	}

	/* wait for reply from krake daemon */
	result = malloc(KRK_CONF_RETVAL_LEN);
	len = KRK_CONF_RETVAL_LEN;
	ptr = result;

	while (1) {
		n = recv(sock, ptr, len, 0);
		if (n < 0) {
			perror("recv");
			return 1;
		}

		if (n == len) {
			break;
		} else {
			ptr += n;
			len -= n;
		}
	}

	/**
	 * the last 4 bytes of result is a number 
	 * to check if result is completly received.
	 * '0xcdef5adc' means the notes in C major, 
	 * 5 means G.
	 */
	if (*((int *)(result + 1)) != 0xcdef5abc) {
		fprintf(stderr, "result is not trustable\n");
	} else {
		fprintf(stderr, "Command OK\n");
	}

	if (result[0] != KRK_CONF_PARSE_OK) {
		goto failed;
	}

	close(sock);
	free(result);
	free(config);

	return 0;

failed:
	fprintf(stderr, "%s: parse command line failed\n", argv[0]);
	return 1;
}
