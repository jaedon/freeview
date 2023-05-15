/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: id3.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:03p $
 *
 * Module Description: ID3 v1.0 and v1.1 reader
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/id3.c $
 * 
 * 1   2/7/05 8:03p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   5/12/03 4:42p erickson
 * cheops devel
 *
 * Irvine_BSEAVSW_Devel/2   5/2/03 4:00p erickson
 * initial cheops work
 *
 * Irvine_BSEAVSW_Devel/1   4/30/03 5:02p erickson
 * new code common to narmer and cheops
 *
 ****************************************************************************/
#include "id3.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define READ(FIELD,SIZE) \
	n = read(fd, info->FIELD, SIZE); \
	if (n == -1) return errno; \
	info->title[n] = 0

#define READBUF(SIZE) \
	n = read(fd, buf, SIZE); \
	if (n == -1) return errno; \
	buf[n] = 0;

int id3_read(int fd, id3_info *info)
{
	int n;
	char buf[10];

	if (lseek(fd, -128, SEEK_END) == -1)
		return errno;
	READBUF(3);
	if (strcmp(buf, "TAG")) {
#if 0
		// I've seen 1 or 2 nulls get appended to the end. Why's this?
		// Instead of "supporting" it, I'm chosing to reject until I learn differently.
		
		if (lseek(fd, -129, SEEK_END) == -1)
			return errno;
		READBUF(3);
		if (strcmp(buf, "TAG")) {
			if (lseek(fd, -130, SEEK_END) == -1)
				return errno;
			READBUF(3);
			if (strcmp(buf, "TAG"))
				return -1;
		}
#endif
		fprintf(stderr, "id3_read found: %s\n", buf);
		return -1;
	}

	READ(title,30);
	READ(artist,30);
	READ(album,30);
	READBUF(4);
	info->year = atoi(buf);
	READ(comment,30);

	if (!info->comment[28])
		/* v1.1 */
		info->albumtrack = info->comment[29];
	else
		/* v1.0 */
		info->albumtrack = 0;

	READBUF(1);
	info->genre = buf[0];
	return 0;
}

int id3_write(int fd, id3_info *info)
{
	int n, rc;
	char buf[10];

	rc = lseek(fd, -128, SEEK_END);
	if (rc < 0) {
		READBUF(3);
		rc = strcmp(buf, "TAG");
	}
	else
		rc = 0;
	if (rc) {
		// append a new tag
		lseek(fd, 0, SEEK_END);
	}

	// we can't just write the whole struct because of extra space for trailing nulls
	write(fd, "TAG", 3);
	write(fd, info->title, 30);
	write(fd, info->artist, 30);
	write(fd, info->album, 30);
	sprintf(buf, "%d", info->year);
	write(fd, buf, 4);
	write(fd, info->comment, 28); // force v1.1
	buf[0] = 0;
	buf[1] = info->albumtrack;
	buf[2] = info->genre;
	write(fd, buf, 3);
	return 0;
}

/* found on the web, obviously could go out of date */
static const char *g_genre[] = {
	"Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop",
	"Jazz", "Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock",
	"Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
	"Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk", "Fusion", "Trance",
	"Classical", "Instrumental", "Acid", "House", "Game", "Sound Clip", "Gospel", "Noise",
	"Alt. Rock", "Bass", "Soul", "Punk", "Space", "Meditative", "Instrum. Pop",
	"Instrum. Rock", "Ethnic", "Gothic", "Darkwave", "Techno-Indust.", "Electronic",
	"Pop-Folk", "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",
	"Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret",
	"New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi", "Tribal",
	"Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock",
	"Folk", "Folk/Rock", "National Folk", "Swing", "Fusion", "Bebob", "Latin", "Revival",
	"Celtic", "Bluegrass", "Avantgarde", "Gothic Rock", "Progress. Rock", "Psychadel. Rock",
	"Symphonic Rock", "Slow Rock", "Big Band", "Chorus", "Easy Listening",
	"Acoustic", "Humour", "Speech", "Chanson", "Opera", "Chamber Music", "Sonata",
	"Symphony", "Booty Bass", "Primus", "Porn Groove", "Satire", "Slow Jam", "Club",
	"Tango", "Samba", "Folklore", "Ballad", "Power Ballad", "Rhythmic Soul",
	"Freestyle", "Duet", "Punk Rock", "Drum Solo", "A Capella", "Euro-House", "Dance Hall",
	"Goa", "Drum & Bass", "Club-House", "Hardcore", "Terror", "Indie", "BritPop",
	"Negerpunk", "Polsk Punk", "Beat", "Christian Gangsta Rap", "Heavy Metal",
	"Black Metal", "Crossover", "Contemporary Christian", "Christian Rock", "Merengue",
	"Salsa", "Thrash Metal", "Anime", "Jpop", "Synthpop", 0
};
static int g_totalgenres = -1;

const char *id3_genreStr(int genre)
{
	if (g_totalgenres == -1) {
		while (g_genre[++g_totalgenres]);
	}

	if (genre < 0 || genre >= g_totalgenres)
		return NULL;
	else
		return g_genre[genre];
}

#ifdef TEST
int main(int argc, char **argv)
{
	id3_info info;
	int fd, rc, i;
	int update = 0;
	int genre_id = -1, year = -1, albumtrack = -1;
	const char *album = NULL, *artist = NULL, *title = NULL, *comment = NULL;

	id3_genreStr(0); // make sure g_totalgenres is initialized
	for (i=1;i<argc;i++) {
		if (!strcmp(argv[i], "--help")) {
			printf(
			"./id3 [-OPTION OPTIONVALUE ...] file1 [file2 ...]\n"
			"\n"
			"OPTIONs allow you to write id3 information.\n"
			"OPTIONs include -title, -artist, -album, -year, -comment, -albumtrack, -genre\n"
			);
		}
		else if (!strcmp(argv[i], "-genre") && i+1<argc) {
			const char *genre = argv[++i];
			for (genre_id=0;genre_id<g_totalgenres;genre_id++) {
				if (!strcasecmp(g_genre[genre_id], genre))
					break;
			}
			if (genre_id == g_totalgenres) {
				fprintf(stderr, "ERROR: invalid genre %s\n", genre);
				return 1;
			}
			update = 1;
		}
		else if (!strcmp(argv[i], "-year") && i+1<argc) {
			year = atoi(argv[++i]);
			update = 1;
		}
		else if (!strcmp(argv[i], "-albumtrack") && i+1<argc) {
			albumtrack = atoi(argv[++i]);
			update = 1;
		}
		else if (!strcmp(argv[i], "-album") && i+1<argc) {
			album = argv[++i];
			update = 1;
		}
		else if (!strcmp(argv[i], "-artist") && i+1<argc) {
			artist = argv[++i];
			update = 1;
		}
		else if (!strcmp(argv[i], "-title") && i+1<argc) {
			title = argv[++i];
			update = 1;
		}
		else if (!strcmp(argv[i], "-comment") && i+1<argc) {
			comment = argv[++i];
			update = 1;
		}
		else if (!strcmp(argv[i], "--print-genres")) {
			int i;
			for (i=0;i<g_totalgenres;i++)
				printf("%s\n", g_genre[i]);
			return 0;
		}
		else {
			const char *filename = argv[i];
			fd = open(filename, update?O_RDWR:O_RDONLY);
			if (fd < 0) {
				printf("ERROR: open '%s' failed %d\n", filename, errno);
				continue;
			}
			rc = id3_read(fd, &info);
			if (rc) {
				if (update) {
					// it's a new one
					memset(&info, 0, sizeof(info));
				}
				else {
					printf("ERROR: id3_read of '%s' failed %d\n", filename, rc);
					continue;
				}
			}

			if (update) {
				if (genre_id != -1)
					info.genre = genre_id;
				if (year != -1)
					info.year = year;
				if (albumtrack != -1)
					info.albumtrack = albumtrack;
				if (album)
					strncpy(info.album, album, 30);
				if (artist)
					strncpy(info.artist, artist, 30);
				if (title)
					strncpy(info.title, title, 30);
				if (comment)
					strncpy(info.comment, comment, 30);
				rc = id3_write(fd, &info);
				if (rc) {
					printf("ERROR: id3_write of '%s' failed %d\n", filename, rc);
					continue;
				}
			}

			printf(
				"File: %s\n"
				"Title: %s\n"
				"Artist: %s\n"
				"Album: %s\n"
				"Year: %d\n"
				"Comment: %s\n"
				"Album track: %d\n"
				"Genre: %s\n",
				filename,
				info.title, info.artist, info.album, info.year, info.comment,
				info.albumtrack, id3_genreStr(info.genre));
			close(fd);
		}
	}
	return 0;
}
#endif
