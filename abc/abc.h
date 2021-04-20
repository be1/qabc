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
    int error_char;
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
	char* v;
	struct symbol* first;
    struct symbol* last;
    int in_alt;
};

struct symbol {
	enum type kind;
	char* lyric;
	char* text;
	int dur_num;
    int dur_den;
    int index;
    int in_alt;
	struct symbol* next;
	struct symbol* prev;
};

void abc_tune_append(struct abc* yy, const char* yytext);

void abc_header_append(struct abc* yy, const char* yytext, const char which);

void abc_voice_append(struct abc* yy, const char* yytext);

struct symbol* abc_last_symbol(struct abc* yy);

struct symbol* abc_new_symbol(struct abc* yy);

void abc_instruction_append(struct abc* yy, const char* yytext);

void abc_lyrics_append(struct abc* yy, const char* yytext);

void abc_eol_append(struct abc* yy, const char* yytext);

void abc_space_append(struct abc* yy, const char* yytext);

void abc_note_append(struct abc* yy, const char* yytext);

void abc_grace_append(struct abc* yy, const char* yytext);

void abc_chord_append(struct abc* yy, const char* yytext);

int abc_chord_parse_num(const char* chord);

int abc_chord_parse_den(const char* chord);

void abc_duration_num_set(struct abc* yy, const char* yytext);

void abc_duration_den_set(struct abc* yy, const char* yytext);

void abc_bar_append(struct abc* yy, const char* yytext);

void abc_alt_append(struct abc* yy, const char* yytext);

void abc_nuplet_append(struct abc* yy, int p, int q, int r);

void abc_deco_append(struct abc* yy, const char* yytext);

void abc_gchord_append(struct abc* yy, const char* yytext);

void abc_tie_append(struct abc* yy, const char* yytext);

void abc_slur_append(struct abc* yy, const char* yytext);

void abc_delete_symbols(struct symbol* s);

struct abc* abc_alloc_yy(void);

void abc_release_yy(struct abc* yy);

int abc_alt_is_of(const struct symbol* s, int alt);

int abc_is_start(const struct symbol* s);

int abc_is_repeat(const struct symbol* s);

int abc_is_endbar(const struct symbol* s);

struct tune* abc_find_tune(struct abc* yy, int x);

struct header* abc_find_header(struct tune* t, char h);

struct symbol* abc_find_start_repeat(struct symbol* s);

struct symbol* abc_find_next_segno(struct symbol* s);

struct symbol* abc_find_next_alt(struct symbol* s, int alt);

struct symbol* abc_find_next_repeat(struct symbol* s);

int abc_has_tie(struct symbol* s, int chord);

double abc_grace_duration(struct symbol* s);

int abc_unit_per_measure(struct tune* t);

double abc_second_per_unit(struct tune* t);

struct abc* abc_parse(const char* buffer, int size);

#ifdef __cplusplus
}
#endif

#endif
