#define _GNU_SOURCE
#include <ctype.h>	/* isdigit */
#include <stdlib.h>	/* atoi */
#include <stdio.h>	/* asprintf */
#include <string.h>
#include "abc.h"
#include "parser.h"

void abc_tune_append(struct abc* yy, const char* yytext)
{
	struct tune* new = calloc(1, sizeof (struct tune));
	new->x = atoi(yytext);

	yy->tunes = realloc(yy->tunes, sizeof (struct tune*) * (yy->count + 1));
	yy->tunes[yy->count] = new;
	yy->count++;
}

void abc_header_append(struct abc* yy, const char* yytext, const char which)
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

void abc_voice_append(struct abc* yy, const char* yytext)
{
	struct voice* new = calloc(1, sizeof (struct voice));
	new->v = strdup(yytext);

	struct tune* tune = yy->tunes[yy->count-1];
    tune->voices = realloc(tune->voices, sizeof (struct voice*) * (tune->count + 1));
	tune->voices[tune->count] = new;
	tune->count++;
}

struct symbol* abc_last_symbol(struct abc* yy)
{
	struct tune* tune = yy->tunes[yy->count-1];
	if (tune->count == 0) /* voice 1 can be implicit */
		abc_voice_append(yy, "1");

	struct voice* voice = tune->voices[tune->count-1];

	return voice->last;
}

struct symbol* abc_new_symbol(struct abc* yy)
{
	struct symbol* new = calloc(1, sizeof (struct symbol));

	struct tune* tune = yy->tunes[yy->count-1];
	if (tune->count == 0) /* voice 1 can be implicit */
		abc_voice_append(yy, "1");

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

void abc_instruction_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = INST;
	new->text = strdup(yytext);
}

void abc_lyrics_append(struct abc* yy, const char* yytext)
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

	struct symbol* s =  abc_last_symbol(yy);
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

void abc_eol_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = EOL;
	new->text = strdup(yytext);
}
void abc_space_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = SPACE;
	new->text = strdup(yytext);
}

void abc_note_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);

	new->kind = NOTE;
	new->dur_num = 1;
	new->dur_den = 1;
	new->text = strdup(yytext);
}

void abc_grace_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = GRACE;
	new->text = strdup(yytext);
}

void abc_chord_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = CHORD;
	new->text = strdup(yytext);
}

int abc_chord_parse_num(const char* chord)
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

int abc_chord_parse_den(const char* chord)
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

void abc_duration_num_set(struct abc* yy, const char* yytext)
{
	int num = atoi(yytext);

	struct tune* tune = yy->tunes[yy->count-1];
	struct voice* voice = tune->voices[tune->count-1];

	voice->last->dur_num = num? num: 1;
}

void abc_duration_den_set(struct abc* yy, const char* yytext)
{
	struct tune* tune = yy->tunes[yy->count-1];
	struct voice* voice = tune->voices[tune->count-1];

	voice->last->dur_den = atoi(yytext);
}

int abc_is_endbar(const struct symbol* s) {
    return (strstr(s->text, "||") || strstr(s->text, "|]"));
}

int abc_is_start(const struct symbol* s) {
    return (strstr(s->text, "|:") != NULL);
}

int abc_is_repeat(const struct symbol* s) {
    return (strstr(s->text, ":|") != NULL);
}

void abc_bar_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
    new->kind = BAR;
    new->text = strdup(yytext);
    struct tune* t = yy->tunes[yy->count-1];
    struct voice* v = t->voices[t->count-1];
    if (abc_is_endbar(new) || abc_is_repeat(new)) {
        v->in_alt = 0;
    }
    // for now, only bars and have 'in_alt' field set.
    new->in_alt = v->in_alt;
}

int abc_alt_is_of(const struct symbol* s, int alt) {
    return (strchr(s->text, alt + '0') != NULL);
}

void abc_alt_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
    new->kind = ALT;
    new->text = strdup(yytext);
    struct tune* t = yy->tunes[yy->count-1];
    struct voice* v = t->voices[t->count-1];
    v->in_alt = 1;
}

void abc_nuplet_append(struct abc* yy, int p, int q, int r)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = NUP;
    if (-1 == asprintf(&new->text, "%d:%d:%d", p, q, r)) {;}
}

void abc_deco_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = DECO;
	new->text = strdup(yytext);
}

void abc_gchord_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = GCHORD;
	new->text = strdup(yytext);
}

void abc_tie_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = TIE;
	new->text = strdup(yytext);
}

void abc_slur_append(struct abc* yy, const char* yytext)
{
	struct symbol* new = abc_new_symbol(yy);
	new->kind = SLUR;
	new->text = strdup(yytext);
}

void abc_delete_symbols(struct symbol* s) {
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

            free(v->v);
			free(v);
		}

		free(t->voices);
		free(t);
	}

	free(yy->tunes);
	free(yy);
}

struct abc* abc_parse(const char* buffer, int size) {
	struct abc* yy = abc_alloc_yy();

	yy->buffer->buf = malloc(size * sizeof (char));
	yy->buffer->buf = memcpy(yy->buffer->buf, buffer, size);
	yy->buffer->count = size;

    pcc_context_t *ctx = pcc_create(yy);

    while (pcc_parse(ctx, NULL)) {
        if (yy->error){
#if 0
            FILE* out = fopen("out.abc", "w");
            fprintf(out, "%.*s", yy->buffer->count, yy->buffer->buf);
            fclose(out);
#endif
            break;
        }
	}

	pcc_destroy(ctx);
	return yy;
}

struct tune* abc_find_tune(struct abc* yy, int x) {
	struct tune* t = NULL;
	for (int i = 0; i < yy->count; i++) {
		if (yy->tunes[i]->x == x) {
			t = yy->tunes[i];
			break;
		}
	}

	return t;
}

struct header* abc_find_header(struct tune* t, char h) {
    if (!t) return NULL;

    struct header* header = t->headers;

    while(header) {
        if (header->h == h) {
			return header;
		}

        header = header->next;
    }

    return header;
}

struct symbol* abc_find_start_repeat(struct symbol* s) {
	while (s->prev) {
        s = s->prev;
		if (s->kind == BAR && strstr(s->text, "|:"))
            return s->next;
	}

	return s;
}

struct symbol* abc_find_next_repeat(struct symbol* s) {
    while (s->next) {
        s = s->next;
        if (s->kind == BAR && abc_is_repeat(s))
            return s;
    }

    return s;
}

struct symbol* abc_find_next_alt(struct symbol* s, int alt) {
    while (s->next) {
        s = s->next;

        if (s->kind == BAR && abc_is_repeat(s))
            return NULL;

        if (s->kind == BAR && abc_is_endbar(s)) {
            return s;
        }

        if ((s->kind == ALT) && abc_alt_is_of(s, alt)) {
                return s->prev;
        }

        if (s->kind == BAR) {
            if (!s->in_alt)
                return s;
        }
    }

    return s;
}

struct symbol* abc_find_next_segno(struct symbol* s) {
	while (s->next) {
		s = s->next;
		if (s->kind == DECO && !strcmp(s->text, "segno"))
			return s;
	}

	return s;
}

int abc_has_tie(struct symbol* s, int chord) {
    while (s->next) {
        s = s->next;
        if (chord) {
            if ((s->kind == CHORD) && s->text[0] == ']') {
                if (s->next && (s->next->kind == TIE))
                    return 1;
                return 0;
            }
        } else {
            if (s->kind == TIE)
                return 1;
            return 0;
        }
    }

    return 0;
}

double abc_grace_duration(struct symbol* s) {
    if (s->kind != GRACE)
        return 0.0;

    double dur = 0.0;
    while (s->next) {
        s = s->next;
        if (s->kind == NOTE) {
            dur += (double) s->dur_num / (double) s->dur_den;
        }

        if (s->kind == GRACE) /* end of grace */
            return dur;
    }

    return dur;
}

double abc_apply_divide(const char* div) {
	if (!strchr(div, '/'))
		return 0.0;

	int num, den;
	if(2 != sscanf(div, "%d/%d", &num, &den))
		return 0.0;
	return (double) num / (double) den;
}

int abc_unit_per_measure(struct tune* t) {
	struct header* lh = abc_find_header(t, 'L');
	struct header* mh = abc_find_header(t, 'M');

	int ln, ld, mn, md;
	if (lh && (2 == sscanf(lh->text, "%d/%d", &ln, &ld))) {;}
	else { ln = 1; ld = 8; }

	if (mh && (2 == sscanf(mh->text, "%d/%d", &mn, &md))) {;}
	else { mn = 4; md = 4; }

	return ld * mn / md;
}

double abc_second_per_unit(struct tune* t) {
	double s = 1.0;
	struct header* lh = abc_find_header(t, 'L');
	struct header* qh = abc_find_header(t, 'Q');

	double l;
	if (lh)
		l = abc_apply_divide(lh->text);
	else
		l = 0.125; /* 1/8 of a whole */
	
	if (qh) {
		int q = 0;
		int num, den;
		if (3 == sscanf(qh->text, "%d/%d=%d", &num, &den, &q)) {
			double b = (double) num / (double) den;
			s = 60.0 / ((double) q * (b / l));
			return s;
		} else if (1 == sscanf(qh->text, "%d", &q)) {
			s = 60.0 / (double) q;
			return s;
        } else if (!strncasecmp(qh->text, "\"Largo\"", 5))
			q = 40;
        else if (!strncasecmp(qh->text, "\"Larghetto\"", 9))
			q = 60;
        else if (!strncasecmp(qh->text, "\"Lento\"", 5))
			q = 50;
        else if (!strncasecmp(qh->text, "\"Adagio\"", 6))
			q = 60;
        else if (!strncasecmp(qh->text, "\"Andante\"", 7))
			q = 80;
        else if (!strncasecmp(qh->text, "\"Moderato\"", 8))
			q = 90;
        else if (!strncasecmp(qh->text, "\"Allegro\"", 7))
			q = 120;
        else if (!strncasecmp(qh->text, "\"Presto\"", 6))
			q = 140;
        else if (!strncasecmp(qh->text, "\"Vivace\"", 6))
			q = 160;
		else
			q = 120;
		s = 60.0 / ((double) q * (0.25 / l));
	} else {
		s = 60.0 / (120.0 * (0.25 / l)); /* 120 QPM */
	}

	return s;
}
