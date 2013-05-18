#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>


char *charclass = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/'[]\\;,.";
char *progname;
int charlen;
int length = 0;
int verbose = 0;


void append_single(int c)
{
    if (isprint(c) && !isspace(c))
	charclass[charlen++] = c;
}

void append_range(int first, int last)
{
    if (first > last) {
	fprintf(stderr, "bad range %c-%c\n", first, last);
	exit(EXIT_FAILURE);
    }
    while (first<=last)
	append_single(first++);
}

int intcompar(const void *x, const void *y)
{
    return *(char *)x-*(char *)y;
}

void uniq(char *str)
{
    char *p = str;
    char c;
    
    c = *p++;
    while (*str) {
	if (*str != c) {
	    c = *str;
	    *p++ = c;
	}
	str++;
    }
    *p=0;
}

void construct_charclass(char *cc)
{
    int i,j,k;
    int len = strlen(cc);

    charclass = malloc(128);
    charlen = 0;
    
    while (i<len) {
	switch(cc[i]) {
	case '-':
	    append_range(cc[i-1], cc[i+1]);
	    i+=2;
	    break;
	case 0:
	    append_single(cc[i-1]);
	    return;
	default:
	    append_single(cc[i++ - 1]);
	}
    }

    charclass[charlen] = 0;
    qsort(charclass, charlen, sizeof(char), intcompar);
    uniq(charclass);
    charlen = strlen(charclass);

    if (verbose)
	printf ("Character class: '%s'\n", charclass);
}

void usage()
{
    fprintf (stderr, "usage: %s [-l length] [-c charclass]\n"
	     "where:\n"
	     "  length is the length of desired password (0<length<256)\n"
	     "  charclass is character class, e.g. acg-kz => acghijkz\n", progname);
    exit(EXIT_FAILURE);
}

main(int argc, char **argv)
{
	FILE *f;
	char *pwd;
	int i;
	int opt;

	charlen = strlen(charclass);
	progname = strdup(argv[0]);

	while ((opt = getopt(argc, argv, "l:c:v")) != -1) {
	    switch(opt) {
	    case 'l':
		length = atoi(optarg);
		break;
	    case 'c':
		construct_charclass(strdup(optarg));
		break;
	    case 'v':
		verbose = 1;
		break;
	    default:
		usage();
	    }
	}

	if ((length==0) || (length>256)) {
	    usage();
	    exit(EXIT_FAILURE);
	}


	f = fopen("/dev/urandom", "r");
	if (!f) {
		perror(progname);
		exit(EXIT_FAILURE);
	}

	pwd = malloc(length+1);
	if (fread(pwd, 1, length, f) != length) {
		perror(progname);
		exit(EXIT_FAILURE);
	}

	for (i=0;i<length;i++) {
		unsigned char idx = pwd[i];

		idx %= charlen;
		pwd[i] = charclass[idx];
	}
	pwd[length] = 0;

	printf ("%s\n", pwd);

	free(pwd);
}
