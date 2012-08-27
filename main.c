#define NDEBUG

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "debug.h"

#define DEFAULT_DICT "wordlist"

#define CR 13
#define LF 10

int compare_chars(const void *e1, const void *e2)
{
	char c1 = *(char *)e1;
	char c2 = *(char *)e2;
	return c1 - c2;
}

int is_anagram(char word_a[], char word_b[])
{
	int len_a = strlen(word_a);
	int len_b = strlen(word_b);

	if (len_a != len_b)
		return 0;

	qsort(word_a, len_a, sizeof(char), compare_chars);
	qsort(word_b, len_b, sizeof(char), compare_chars);

	return (!strcmp(word_a, word_b));
}

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	char *f_arr = NULL;
	char **line_array = NULL;
	char **line = NULL;
	char abbrev[argc];
	char *dictfile = NULL;

	check((argc >= 2), "No arguments given.");

	int opts;
	while ((opts = getopt(argc, argv, "f:")) != -1)
		switch (opts) {
		case 'f':
			dictfile = optarg;
			break;
		default:
			sentinel("Usage: %s [-f dictionary-file] word ...", argv[0]);
		}

	debug("using dictionary file:	%s", dictfile);
	debug("optind:	%i", optind);

	for (int i = 0; i < argc; ++i)
		debug("arg index %i:	%s", i, argv[i]);

	fp = fopen(dictfile, "rt");
	check(fp, "Could not open file '%s' for reading.", dictfile);

	fseek(fp, 0L, SEEK_END);
	long f_len = ftell(fp);
	rewind(fp);

	debug("file length:	%ld", f_len);

	f_arr = calloc(f_len + 1, sizeof(char));
	check(f_arr, "Insufficient memory to read file.");
	fread(f_arr, f_len, 1, fp);

	line_array = malloc((f_len + 1) * sizeof(char *));
	check(line_array, "Failed to allocate heap memory for line list.");
/*
 *	iterate over file character array, converting newlines to NULLs.
 *	this results in a block of strings, each representing a single line
 *	at the same time, keep a tally of how many lines were found
 *	and save a pointer to each line in the line array
 */
	long line_count = 0;		//TODO: should be unsigned
	for (int i = 0; i < f_len; ++i) {
		if (f_arr[i] == CR || f_arr[i] == LF)
			f_arr[i] = '\0';
		else if (i == 0 || f_arr[i - 1] == '\0') {
			line_array[line_count] = f_arr + i;
			++line_count;
		}
	}

	line = realloc(line_array, line_count * sizeof(char *));
	check(line, "Failed to reallocate heap memory for line list.");
	debug("line count:	%li", line_count);

	for (int i = optind; i < argc; ++i)
		abbrev[i - optind] = tolower(argv[i][0]);
	abbrev[argc - optind] = '\0';

	debug("abbrev:	%s", abbrev);

	for (long i = 0; i < line_count; ++i) {
		int len = strlen(line[i]);
		char line_dup[len];
		strcpy(line_dup, line[i]);	//buffer overflow with long dict words

		if (is_anagram(line_dup, abbrev))
			printf("%s\n", line[i]);
	}

	free(f_arr);
	free(line);
	fclose(fp);					//TODO: move to earlier

	return 0;

 error:
	if (fp)
		fclose(fp);
	if (f_arr)
		free(f_arr);
	if (line)
		free(line);
	if (line_array)
		free(line_array);

	return 1;
}
