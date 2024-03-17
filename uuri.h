/**
 * Very small URL parser and encoder.
 *
 * Copyright (c) 2019, Harkaitz Agirre Ezama, All rights reserved.
 */

#ifndef UURI_H
#define UURI_H

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdbool.h>

struct uuri {
	char const *name;
	char const *proto;
	char const *user,*pass;
	char const *host,*port;
	char const *path;
	char const *param;
	char const *query;
	char const *fragment;
};

static inline bool
uuri_parse(struct uuri *_uri, char _b[], char **_next)
{
	/*  NAME <PROTO:[//]USER:PASS@HOST:PORT/PATH;PARAM?QUERY#FRAGMENT>   ,
	 *  ^     ^         ^    ^    ^    ^    ^----^-----^-----^        ^  ^  
	 *  p1    p2        p3   p4   p5   p6   p                         pS pN
	 */
	
	char *p1,*p2,*p3,*p4,*p5,*p6,*pN,*pS;
	
	if (_uri) {
		memset(_uri, 0, sizeof(struct uuri));
	}
	if (!_b) {
		return false;
	}
	
	/* Shift initial spaces. */
	for (p1 = _b; *p1; p1++) {
		if (!strchr(" \t\n\r", *p1)) {
			break;
		}
	}
	
	/* p2, pN */
	if ((pN = strchr(p1, ','))) {
		*(pN++) = '\0';
	}
	if (_next) {
		*_next = pN;
	}
	if (!_uri) {
		return (pN != NULL);
	}
	p2 = strchr(p1, '<');
	if (p2) {
		for (char *c = p2-1; c > p1; c--) {
			if (!strchr(" \t\n\r", *c)) {
				break;
			}
			*c = '\0';
		}
		*(p2++) = '\0';
		pS = strchr(p2, '>');
		if (!pS) {
			return false;
		}
		*(pS++) = '\0';
	} else {
		p2 = p1;
		p1 = NULL;
	}
	if ((p3 = strstr(p2, "://"))) {
		*p3 = '\0';
		p3 += 3;
	} else if (!strncmp(p2, "sip:", 4)) {
		p3 = strchr(p2, ':');
		*(p3++) = '\0';
	} else {
		p3 = p2;
		p2 = NULL;
	}
	/* pS */
	for (char *p = p3; *p; p++) {
		switch (*p) {
		case '/': *p = '\0'; _uri->path  = p+1; break;
		case ';': *p = '\0'; _uri->param = p+1; break;
		case '?': *p = '\0'; _uri->query = p+1; break;
		case '#': *p = '\0'; _uri->fragment = p+1; break;
		}
	}
	
	/* p3, p5 */
	p5 = strchr(p3, '@');
	if (p5) {
		*(p5++) = '\0';
	} else {
		p5 = p3;
		p3 = NULL;
	}
	
	/* p4, p6 */
	p4 = NULL;
	if (p3 && (p4 = strchr(p3, ':'))) {
		*(p4++) = '\0';
	}
	if ((p6 = strchr(p5, ':'))) {
		*(p6++) = '\0';
	}
	
	_uri->name = p1;
	_uri->proto = p2;
	_uri->user = p3; _uri->pass = p4;
	_uri->host = p5; _uri->port = p6;
	
	return true;
}

static inline int
uuri_sprintf(struct uuri const *_uri, char _b[], int _bsz)
{
	int len = 0;
	
	/* name */
	if (_uri->name) {
		len += snprintf(_b+len, _bsz-len, "%s <", _uri->name);
	}
	
	/* proto */
	if (_uri->proto && !strcasecmp(_uri->proto, "sip")) {
		len += snprintf(_b+len, _bsz-len, "sip:");
	} else if (_uri->proto) {
		len += snprintf(_b+len, _bsz-len, "%s://", _uri->proto);
	}
	
	/* auth */
	if (_uri->user && _uri->pass) {
		len += snprintf(_b+len, _bsz-len, "%s:%s@", _uri->user, _uri->pass);
	} else if (_uri->user) {
		len += snprintf(_b+len, _bsz-len, "%s@", _uri->user);
	}
	
	/* host,port */
	if (_uri->host) {
		len += snprintf(_b+len, _bsz-len, "%s", _uri->host);
	}
	if (_uri->port) {
		len += snprintf(_b+len, _bsz-len, ":%s", _uri->port);
	}
	
	/* path,param,query,fragment */
	if (_uri->path) {
		len += snprintf(_b+len, _bsz-len, "/%s", _uri->path);
	}
	if (_uri->param) {
		len += snprintf(_b+len, _bsz-len, ";%s", _uri->param);
	}
	if (_uri->query) {
		len += snprintf(_b+len, _bsz-len, ";%s", _uri->query);
	}
	if (_uri->fragment) {
		len += snprintf(_b+len, _bsz-len, ";%s", _uri->fragment);
	}
	
	/* name */
	if (_uri->name) {
		len += snprintf(_b+len, _bsz-len, ">");
	}
	
	return len;
}

static inline char const *
uuri_encode(struct uuri const *_uri, char _b[], int _bsz)
{
	int len = uuri_sprintf(_uri, _b, _bsz);
	if (len>=_bsz) {
		return "";
	} else {
		return _b;
	}
}

static inline void
uuri_fput(struct uuri const *_uri, FILE *_fp)
{
	fprintf(
	    _fp,
	    "name: %s"     "\n"
	    "proto: %s"    "\n"
	    "user: %s"     "\n"
	    "pass: %s"     "\n"
	    "host: %s"     "\n"
	    "port: %s"     "\n"
	    "path: %s"     "\n"
	    "param: %s"    "\n"
	    "query: %s"    "\n"
	    "fragment: %s" "\n",
	    (_uri->name)    ?_uri->name    :"",
	    (_uri->proto)   ?_uri->proto   :"",
	    (_uri->user)    ?_uri->user    :"",
	    (_uri->pass)    ?_uri->pass    :"",
	    (_uri->host)    ?_uri->host    :"",
	    (_uri->port)    ?_uri->port    :"",
	    (_uri->path)    ?_uri->path    :"",
	    (_uri->param)   ?_uri->param   :"",
	    (_uri->query)   ?_uri->query   :"",
	    (_uri->fragment)?_uri->fragment:""
	);  
}

static inline bool
str_match(char const _t[], char const _s[])
{
	if (_t && !strcmp(_t, "*")) {
		return false;
	}
	if (!_t || !_t[0]) {
		return !_s || !_s[0];
	}
	if (!_s) {
		return false;
	}
	return !strcasecmp(_t, _s);
}

static inline bool
uuri_match(struct uuri const *_tmpl, struct uuri const *_uri, const char **_field)
{
	if (!str_match(_tmpl->name,  _uri->name))  { if (_field) { *_field = "name"; } return false; } 
	if (!str_match(_tmpl->proto, _uri->proto)) { if (_field) { *_field = "proto"; } return false; }
	if (!str_match(_tmpl->user,  _uri->user))  { if (_field) { *_field = "user"; } return false; }
	if (!str_match(_tmpl->pass,  _uri->pass))  { if (_field) { *_field = "pass"; } return false; }
	if (!str_match(_tmpl->host,  _uri->host))  { if (_field) { *_field = "host"; } return false; }
	if (!str_match(_tmpl->port,  _uri->port))  { if (_field) { *_field = "port"; } return false; }
	if (!str_match(_tmpl->path,  _uri->path))  { if (_field) { *_field = "path"; } return false; }
	if (!str_match(_tmpl->param, _uri->param)) { if (_field) { *_field = "param"; } return false; }
	if (!str_match(_tmpl->query, _uri->query)) { if (_field) { *_field = "query"; } return false; }
	if (!str_match(_tmpl->fragment, _uri->fragment)) { if (_field) { *_field = "fragment"; } return false; }
	return true;
}

static inline char const *
uuri_get(struct uuri const *_uri, char const *_field)
{
	if (!strcmp(_field, "name"))     return _uri->name;
	if (!strcmp(_field, "proto"))    return _uri->proto;
	if (!strcmp(_field, "user"))     return _uri->user;
	if (!strcmp(_field, "pass"))     return _uri->pass;
	if (!strcmp(_field, "host"))     return _uri->host;
	if (!strcmp(_field, "port"))     return _uri->port;
	if (!strcmp(_field, "path"))     return _uri->path;
	if (!strcmp(_field, "param"))    return _uri->param;
	if (!strcmp(_field, "query"))    return _uri->query;
	if (!strcmp(_field, "fragment")) return _uri->fragment;
	return "";
}


#endif
/*
 * https://realtimecommunication.wordpress.com/2018/05/09/sip-uri-overview/
 */
