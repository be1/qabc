#define _GNU_SOURCE
#include <ctype.h>	/* isdigit */
#include <stdlib.h>	/* atoi */
#include <stdio.h>	/* asprintf */
#include <string.h>
#include "abc.h"

void tune_append(struct abc* yy, const char* yytext)
{
	struct tune* new = calloc(1, sizeof (struct tune));
	new->x = atoi(yytext);

	yy->tunes = realloc(yy->tunes, sizeof (struct tune*) * (yy->count + 1));
	yy->tunes[yy->count] = new;
	yy->count++;
}

void header_append(struct abc* yy, const char* yytext, const char which)
{
	struct tune* tune = yy->tunes[yy->count-1];
	struct header* new = calloc(1, sizeof (struct header));

	if (!tune->headers) {
		tune->headers = new;
    } else {
        struct header* h;
        for (h = tune->headers; h->next; h = h->next) {;}

		h->next = new;
	}

	new->h = which;
	new->text = strdup(yytext);
}

void voice_append(struct abc* yy, const char* yytext)
{
	struct voice* new = calloc(1, sizeof (struct voice));
	new->v = atoi(yytext);

	struct tune* tune = yy->tunes[yy->count-1];
    tune->voices = realloc(tune->voices, sizeof (struct voice*) * (tune->count + 1));
	tune->voices[tune->count] = new;
	tune->count++;
}

struct symbol* last_symbol(struct abc* yy)
{
	struct tune* tune = yy->tunes[yy->count-1];
	if (tune->count == 0) /* voice 1 can be implicit */
		voice_append(yy, "1");

	struct voice* voice = tune->voices[tune->count-1];

	return voice->last;
}

struct symbol* new_symbol(struct abc* yy)
{
	struct symbol* new = calloc(1, sizeof (struct symbol));

	struct tune* tune = yy->tunes[yy->count-1];
	if (tune->count == 0) /* voice 1 can be implicit */
		voice_append(yy, "1");

	struct voice* voice = tune->voices[tune->count-1];

	if (voice->last == NULL) {
		voice->last = new;
		voice->first = voice->last;
	} else {
		struct symbol* s = voice->last;
		voice->last = new;
		s->next = new;
		new->prev = s;
		new->index = s->index + 1;
	}

	return new;
}

void instruction_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
	new->kind = INST;
	new->text = strdup(yytext);
}

void lyrics_append(struct abc* yy, const char* yytext)
{
	char** syllables = NULL;
	int count = 0;
	int i = 0;
	int in = 0;
	const char* b;
	const char* e;

	while (yytext[i]) {
		if (isspace(yytext[i])) {
			in = 0;
		} else {
			if (!in) {
				in = 1;
				count++;
				b = &yytext[i];
				for (e = b; *e && !isspace(*e); e++);

				char* text = malloc(e-b +1);
				strncpy(text, b, e-b);
				text[e-b] = 0;

				syllables = realloc(syllables, count * sizeof (char*));
				syllables[count-1] = text;

				i += (e - b - 1);
			}
		}

		i++;
	}

	struct symbol* s =  last_symbol(yy);
	while (s->kind != NOTE && s->prev)
		s = s->prev;

	while (s->kind != EOL && s->prev)
		s = s->prev;

	for (i = 0; i < count; i++) {
		while (s->kind != NOTE && s->next)
			s = s->next;

		s->lyric = syllables[i];
		if (s->next)
			s = s->next;
	}

	free(syllables);
}

void eol_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
	new->kind = EOL;
	new->text = strdup(yytext);
}
void space_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
	new->kind = SPACE;
	new->text = strdup(yytext);
}

void note_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);

	new->kind = NOTE;
	new->dur_num = 1;
	new->dur_den = 1;
	new->text = strdup(yytext);
}

void grace_append(struct abc* yy, const char* yytext, int stroke)
{
	struct symbol* new = new_symbol(yy);
	new->kind = GRACE;
	new->stroke = stroke;
	new->text = strdup(yytext);
}

void chord_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
	new->kind = CHORD;
	new->text = strdup(yytext);
}

int chord_parse_num(const char* chord)
{
	int ret = 0;
	int num = 0;

	while (*chord) {
		if ('/' == *chord)
			return ret? ret: 1;

		if (isdigit(*chord)) {
			num = 1;
			ret *= 10;
			ret += *chord - '0';
		} else {
			num = 0;
		}

		if (ret && !num)
			return ret;

		chord++;
	}

	if (ret)
		return ret;

	return 1;
}

int chord_parse_den(const char* chord)
{
	int den = 0;
	int ret = 0;

	while (*chord) {
		if (isalpha(*chord)) {
			if (ret && den)
				return ret;
		}

		if ('/' == *chord) {
			if (ret && den)
				return ret;

			den = !den;
		}

		if (den && isdigit(*chord)) {
			ret *= 10;
			ret += *chord - '0';
		}

		chord++;
	}

	if (ret)
		return ret;

	return 1;
}

void duration_num_set(struct abc* yy, const char* yytext)
{
	int num = atoi(yytext);

	struct tune* tune = yy->tunes[yy->count-1];
	struct voice* voice = tune->voices[tune->count-1];

	voice->last->dur_num = num? num: 1;
}

void duration_den_set(struct abc* yy, const char* yytext)
{
	struct tune* tune = yy->tunes[yy->count-1];
	struct voice* voice = tune->voices[tune->count-1];

	voice->last->dur_den = atoi(yytext);
}

int is_endbar(const struct symbol* s) {
    return (strstr(s->text, "||") || strstr(s->text, "|]"));
}

int is_start(const struct symbol* s) {
    return (strstr(s->text, "|:") != NULL);
}

int is_repeat(const struct symbol* s) {
    return (strstr(s->text, ":|") != NULL);
}

void bar_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
    new->kind = BAR;
    new->text = strdup(yytext);
    struct tune* t = yy->tunes[yy->count-1];
    struct voice* v = t->voices[t->count-1];
    if (is_endbar(new) || is_repeat(new)) {
        v->in_alt = 0;
    }
    // for now, only bars and have 'in_alt' field set.
    new->in_alt = v->in_alt;
}

int alt_is_of(const struct symbol* s, int alt) {
    return (strchr(s->text, alt + '0') != NULL);
}

void alt_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
    new->kind = ALT;
    new->text = strdup(yytext);
    struct tune* t = yy->tunes[yy->count-1];
    struct voice* v = t->voices[t->count-1];
    v->in_alt = 1;
}

void nuplet_append(struct abc* yy, int p, int q, int r)
{
	struct symbol* new = new_symbol(yy);
	new->kind = NUP;
    if (-1 == asprintf(&new->text, "%d:%d:%d", p, q, r)) {;}
}

void deco_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
	new->kind = DECO;
	new->text = strdup(yytext);
}

void gchord_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
	new->kind = GCHORD;
	new->text = strdup(yytext);
}

void tie_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
	new->kind = TIE;
	new->text = strdup(yytext);
}

void slur_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = new_symbol(yy);
	new->kind = SLUR;
	new->text = strdup(yytext);
}

void delete_symbols(struct symbol* s) {
	struct symbol* next = s;

	while (s) {
		free(s->text);
		free(s->lyric);
		next = s->next;
		free(s);
		s = next;
	}
}

struct abc* abc_alloc_yy(void)
{

	struct abc* yy = calloc(1, sizeof (*yy));
	yy->buffer = calloc(1, sizeof (*yy->buffer));
	return yy;
}

void abc_release_yy(struct abc* yy)
{
	free(yy->buffer->buf);
	free(yy->buffer);

	for (int i = 0; i < yy->count; i++) {
		struct tune* t = yy->tunes[i];
		struct header* h = t->headers;
		while (h) {
			struct header* hn = h->next;
			free(h->text);
			free(h);
			h = hn;
		}

		for (int j = 0; j < t->count; j++) {
			struct voice* v = t->voices[j];
			struct symbol* s = v->first;
			while (s != v->last) {
				struct symbol* sn = s->next;
				free(s->lyric);
				free(s->text);
				free(s);
				s = sn;
			}

			free(v->last->lyric);
			free(v->last->text);
			free(v->last);

			free(v);
		}

		free(t->voices);
		free(t);
	}

	free(yy->tunes);
	free(yy);
}
