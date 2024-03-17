#include "uuri.h"

struct uuri_test {
	char const *in;
	struct uuri uri;
};

static struct uuri_test tests1[];

int
main(int _argc, char *_argv[])
{
	char	buffer[1024];
	struct	uuri uri;
	int	i = 0;
	bool	r;
	char	const *field;
	
	for (i=0; tests1[i].in; i++) {
		printf("%s : ", tests1[i].in); fflush(stdout);
		strcpy(buffer, tests1[i].in);
		
		r = uuri_parse(&uri, buffer, NULL);
		if (!r) {
			printf("Failed to parse.\n");
			return 1;
		}
		if (!uuri_match(&tests1[i].uri, &uri, &field)) {
			printf("Not matching: %s\n", field);
			return 1;
		}
		printf("OK\n");
		
	}
}




static struct uuri_test tests1[] = {
	{
		"http://www.example.com",
		{
			.proto = "http",
			.host = "www.example.com",
		},  
	},
	{
		"http://www.example.com:8080",
		{
			.proto = "http",
			.host = "www.example.com",
			.port = "8080",
		},  
	},
	{
		"http://www.example.com:8080/",
		{
			.proto = "http",
			.host = "www.example.com",
			.port = "8080",
			.path = "",
		},  
	},
	{
		"http://www.example.com:8080/path",
		{
			.proto = "http",
			.host = "www.example.com",
			.port = "8080",
			.path = "path",
		},  
	},
	{
		"http://www.example.com:8080/path;param",
		{
			.proto = "http",
			.host = "www.example.com",
			.port = "8080",
			.path = "path",
			.param = "param",
		},
	},
	{
		"http://www.example.com:8080/path;param?query",
		{
			.proto = "http",
			.host = "www.example.com",
			.port = "8080",
			.path = "path",
			.param = "param",
			.query = "query",
		},  
	},
	{
		"http://www.example.com:8080/path;param?query#fragment",
		{
			.proto = "http",
			.host = "www.example.com",
			.port = "8080",
			.path = "path",
			.param = "param",
			.query = "query",
			.fragment = "fragment",
		},
	},
	{
		NULL
	}
};

