#include "uuri.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

static bool
parse_and_select(char *_url_lists[], int _index, struct uuri *_uri);

static char const HELP[] =
"Usage: uuri OPTIONS URI1 URI2..."   "\n"
""                                   "\n"
"Parse URLs to extract information." "\n"
""                                   "\n"
"-f FIELD1,... : Print the specified fields, by default with , separator." "\n"
"-s SEP        : Separator to use, it will error if a field contains it."  "\n"
"-n INDEX      : Print the Nth URL in arguments (By default 0)."           "\n"
""                                                                         "\n"
"The following fields are supported: name, proto, user, pass, host, port," "\n"
"path, param, query, fragment, full, rec."                                 "\n";

static char buffer[1024*5];

int
main(int _argc, char *_argv[])
{
	char	*opt_fields = "full";
	char	*opt_separator = ",";
	int	 opt_index = 0;
	int	 opt;
	struct	 uuri uri;
	char	*field;
	char	 const *value;
	int	 field_n, len;
	
	if (_argc > 1 && (!strcmp(_argv[1], "--help") || !strcmp(_argv[1], "-h"))) {
		fputs(HELP, stdout);
		return 0;
	}
	
	/* Parse command line arguments. */
	while((opt = getopt(_argc, _argv, "f:s:n:")) != -1) {
		switch (opt) {
		case 'f': opt_fields    = optarg; break;
		case 's': opt_separator = optarg; break;
		case 'n': opt_index     = atoi(optarg); break;
		case '?':
		default:
			return 1;
		}
	}
	
	/* Parse and search URL. */
	if (!parse_and_select(_argv+optind, opt_index, &uri)) {
		return 0;
	}
	
	/* Print fields. */
	for (field_n = 0, field = strtok(opt_fields, ",");
	     field;
	     field_n++, field = strtok(NULL, ","))
	{
		if (field_n) {
			fputs(opt_separator, stdout);
		}
		if (!strcmp(field, "full")) {
			len = uuri_sprintf(&uri, buffer, sizeof(buffer));
			if (len >= sizeof(buffer)) {
				fprintf(stderr, "error: Too large URI.\n");
				return 1;
			}
			fputs(buffer, stdout);
			continue;
		}
		if (!strcmp(field, "rec")) {
			uuri_fput(&uri, stdout);
			continue;
		}
		value = uuri_get(&uri, field);
		if (value) {
			fputs(value, stdout);
		}
	}
	fputc('\n', stdout);
	return 0;
}

static bool
parse_and_select(char *_url_lists[], int _index, struct uuri *_uri)
{
	int	 i,index;
	char	*url_list;
	for (i=0,index=0; _url_lists[i]; i++) {
		for (url_list = _url_lists[i];
		     uuri_parse(index == _index? _uri : NULL, url_list, &url_list);
		     index++) {
			if (index == _index) {
				return true;
			}
		}
	}
	return false;
}
