/*
 * copyright 2009 william light <visinin@gmail.com>
 *
 * this file is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *
 */

#define _GNU_SOURCE

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <regex.h>

#include <tag_c.h>

typedef struct {
	char *extension;
	enum {
		HAS_METADATA,
		NO_METADATA
	} metadata;
	regex_t *regex;
} filetype_t;

filetype_t filetypes[] = {
	{"mp3$",  HAS_METADATA, NULL},
	{"ogg$",  HAS_METADATA, NULL},
	{"flac$", HAS_METADATA, NULL},
	{"mp4$",  HAS_METADATA, NULL},
	{"wav$",  NO_METADATA,  NULL},
	{NULL}
};

int print_metadata(const char *fbuf, char *basename) {
	filetype_t *cursor = filetypes;
	char *title, *artist;

	TagLib_File *tfile;
	TagLib_Tag *tag;

	do {
		if( !regexec(cursor->regex, fbuf, 0, NULL, 0) ) {
			if( !(tfile = taglib_file_new(fbuf)) )
				return 1;

			tag = taglib_file_tag(tfile);
			artist = taglib_tag_artist(tag);
			title = taglib_tag_title(tag);

			if( !*title )
				title = basename;

			printf("%s\t%s\t%s\n", fbuf, artist, title);

			taglib_tag_free_strings();
			taglib_file_free(tfile);

			return 0;
		}
	} while( (++cursor)->extension );

	return 1;
}

int pdir(const char *dir, int skipdotfiles) {
	DIR *ectory;
	struct dirent *ent;
	struct stat file;
	char *fbuf, *dbuf;


	ectory = opendir(dir);
	dbuf = realpath(dir, NULL);

	while( (ent = readdir(ectory)) ) {
		if( (ent->d_name[0] == '.' && (!ent->d_name[1] || skipdotfiles)) ||
			(ent->d_name[1] == '.' && !ent->d_name[2]) )
			continue;

		asprintf(&fbuf, "%s/%s", dbuf, ent->d_name);

		stat(fbuf, &file);

		switch( file.st_mode & S_IFMT ) {
		case S_IFREG:
			print_metadata(fbuf, ent->d_name);
			break;

		case S_IFDIR:
			pdir(fbuf, skipdotfiles);
			break;
		}

		free(fbuf);
	}

	free(dbuf);
	closedir(ectory);

	return 0;
}

void init_regexes(filetype_t *types) {
	filetype_t *cursor = types;

	do {
		cursor->regex = calloc(1, sizeof(regex_t));

		if( regcomp(cursor->regex, cursor->extension, 0) ) {
			free(cursor->regex);
			printf("error compiling regex \"%s\"\n", cursor->extension);
		}
	} while( (++cursor)->extension );
}

int main(int argc, char **argv) {
	if( argc != 2 )
		return 1;

	init_regexes(filetypes);
	
	pdir(argv[1], 0);

	return 0;
}
