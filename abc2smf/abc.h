#ifndef ABC_H
#define ABC_H

#include <ctype.h>	/* isdigit */
#include <stdlib.h>	/* atoi */
#include <stdio.h>	/* asprintf */

#ifdef __cplusplus
extern "C" {
#endif


enum type { EOL, SPACE, NOTE, NUP, GRACE, CHORD, DECO, GCHORD, TIE, SLUR, BAR, ALT, INST };

struct buffer {
	char* buf;
	int count;
	int index;
};

struct abc {
	struct tune** tunes;
	int count;
	struct buffer* buffer;
	int error;
    int error_line;
};

struct header {
	char h;
	char* text;
	struct header* next;
};

struct tune {
	int x;
	struct header* headers;
	struct voice** voices;
	int count;
};

struct voice {
	int v;
	struct symbol* first;
    struct symbol* last;
    int in_alt;
};

struct symbol {
	enum type kind;
	int stroke;
	char* lyric;
	char* text;
	int dur_num;
    int dur_den;
    int index;
    int in_alt;
	struct symbol* next;
	struct symbol* prev;
};

void tune_append(struct abc* yy, const char* yytext);

void header_append(struct abc* yy, const char* yytext, const char which);

void voice_append(struct abc* yy, const char* yytext);

struct symbol* last_symbol(struct abc* yy);

struct symbol* new_symbol(struct abc* yy);

void instruction_append(struct abc* yy, const char* yytext);

void lyrics_append(struct abc* yy, const char* yytext);

void eol_append(struct abc* yy, const char* yytext);

void space_append(struct abc* yy, const char* yytext);

void note_append(struct abc* yy, const char* yytext);

void grace_append(struct abc* yy, const char* yytext, int stroke);

void chord_append(struct abc* yy, const char* yytext);

int chord_parse_num(const char* chord);

int chord_parse_den(const char* chord);

void duration_num_set(struct abc* yy, const char* yytext);

void duration_den_set(struct abc* yy, const char* yytext);

void bar_append(struct abc* yy, const char* yytext);

void alt_append(struct abc* yy, const char* yytext);

void nuplet_append(struct abc* yy, int p, int q, int r);

void deco_append(struct abc* yy, const char* yytext);

void gchord_append(struct abc* yy, const char* yytext);

void tie_append(struct abc* yy, const char* yytext);

void slur_append(struct abc* yy, const char* yytext);

void delete_symbols(struct symbol* s);

struct abc* abc_alloc_yy(void);

void abc_release_yy(struct abc* yy);

int alt_is_of(const struct symbol* s, int alt);

int is_start(const struct symbol* s);

int is_repeat(const struct symbol* s);

int is_endbar(const struct symbol* s);

#ifdef __cplusplus
}
#endif

#endif
