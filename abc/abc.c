#define _GNU_SOURCE
#include <ctype.h>	/* isdigit */
#include <stdlib.h>	/* atoi, qsort */
#include <stdio.h>	/* asprintf */
#include <string.h>
#include <math.h>
#include "abc.h"
#include "parser.h"

void abc_tune_append(struct abc* yy, const char* yytext)
{
    struct abc_tune* new = calloc(1, sizeof (struct abc_tune));
    new->x = atoi(yytext);

    yy->tunes = realloc(yy->tunes, sizeof (struct abc_tune*) * (yy->count + 1));
    yy->tunes[yy->count] = new;
    yy->count++;
}

void abc_header_append(struct abc* yy, const char* yytext, const char which)
{
    struct abc_tune* tune = yy->tunes[yy->count-1];
    struct abc_header* new = calloc(1, sizeof (struct abc_header));

    if (!tune->headers) {
        tune->headers = new;
    } else {
        struct abc_header* h;
        for (h = tune->headers; h->next; h = h->next) {;}

        h->next = new;
    }

    new->h = which;
    new->text = strdup(yytext);
}

void abc_voice_append(struct abc* yy, const char* yytext)
{
    struct abc_voice* new = calloc(1, sizeof (struct abc_voice));
    new->v = strdup(yytext);

    struct abc_tune* tune = yy->tunes[yy->count-1];
    tune->voices = realloc(tune->voices, sizeof (struct abc_voice*) * (tune->count + 1));
    tune->voices[tune->count] = new;
    tune->count++;
}

struct abc_symbol* abc_last_symbol(struct abc* yy)
{
    struct abc_tune* tune = yy->tunes[yy->count-1];
    if (tune->count == 0) /* voice 1 can be implicit */
        abc_voice_append(yy, "1");

    struct abc_voice* voice = tune->voices[tune->count-1];

    return voice->last;
}

struct abc_symbol* abc_new_symbol(struct abc* yy)
{
    struct abc_symbol* new = calloc(1, sizeof (struct abc_symbol));
    new->start_den = 1;
    new->dur_den = 1;

    struct abc_tune* tune = yy->tunes[yy->count-1];
    if (tune->count == 0) /* voice 1 can be implicit */
        abc_voice_append(yy, "1");

    struct abc_voice* voice = tune->voices[tune->count-1];

    if (voice->last == NULL) {
        voice->last = new;
        voice->first = voice->last;
    } else {
        struct abc_symbol* s = voice->last;
        voice->last = new;
        s->next = new;
        new->prev = s;
        new->index = s->index + 1;
    }

    return new;
}

void abc_instruction_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_INST;
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

    struct abc_symbol* s =  abc_last_symbol(yy);
    while (s->kind != ABC_NOTE && s->prev)
        s = s->prev;

    while (s->kind != ABC_EOL && s->prev)
        s = s->prev;

    for (i = 0; i < count; i++) {
        while (s->kind != ABC_NOTE && s->next)
            s = s->next;

        s->lyric = syllables[i];
        if (s->next)
            s = s->next;
    }

    free(syllables);
}

void abc_eol_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_EOL;
    new->text = strdup(yytext);
}
void abc_space_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_SPACE;
    new->text = strdup(yytext);
}

void abc_note_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);

    new->kind = ABC_NOTE;
    new->dur_num = 1;
    new->dur_den = 1;
    new->text = strdup(yytext);
    if (new->text[0] == 'Z') {
	    /* fix numerator */
	    struct abc_tune* tune = yy->tunes[yy->count-1];
	    new->dur_num = abc_unit_per_measure(tune);
    }
}

void abc_notepunct_set(struct abc* yy, const char* yytext)
{
	int count = strlen(yytext);

	struct abc_tune* tune = yy->tunes[yy->count-1];
	struct abc_voice* voice = tune->voices[tune->count-1];

	if (yytext[0] == '>') {
		voice->last->dur_den *= pow(2, count);

		int num, den;
		num = voice->last->prev->dur_num;
		den = voice->last->prev->dur_den;
		while (count) {
			abc_frac_add(&voice->last->prev->dur_num, &voice->last->prev->dur_den, num, den * pow(2, count));
			count--;
		}
	} else if (yytext[0] == '<') {
		voice->last->prev->dur_den *= pow(2, count);

		int num, den;
		num = voice->last->dur_num;
		den = voice->last->dur_den;
		while (count) {
			abc_frac_add(&voice->last->dur_num, &voice->last->dur_den, num, den * pow(2, count));
			count--;
		}
	}
}

void abc_grace_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_GRACE;
    new->text = strdup(yytext);
}

void abc_chord_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_CHORD;
    new->text = strdup(yytext);
    if (new->text[0] == ']') {
        struct abc_symbol* s = abc_chord_rewind(new->prev);
        while (s->next && s->next->text[0] != ']') {
            s = s->next;
            if (s->kind == ABC_NOTE)
                s->in_chord = 1;
        }
    }
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

    struct abc_tune* tune = yy->tunes[yy->count-1];
    struct abc_voice* voice = tune->voices[tune->count-1];

    voice->last->dur_num = num? num: 1;
    voice->last->dur_den = 1;
}

void abc_duration_den_set(struct abc* yy, const char* yytext)
{
    struct abc_tune* tune = yy->tunes[yy->count-1];
    struct abc_voice* voice = tune->voices[tune->count-1];

    if (yytext[0] == '/')
	    voice->last->dur_den = pow(2, strlen(yytext));
    else
	    voice->last->dur_den = atoi(yytext);
}

int abc_is_endbar(const struct abc_symbol* s) {
    return (strstr(s->text, "||") || strstr(s->text, "|]"));
}

int abc_is_start(const struct abc_symbol* s) {
    return (strstr(s->text, "|:") != NULL);
}

int abc_is_repeat(const struct abc_symbol* s) {
    return (strstr(s->text, ":|") != NULL);
}

void abc_bar_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_BAR;
    new->text = strdup(yytext);
    struct abc_tune* t = yy->tunes[yy->count-1];
    struct abc_voice* v = t->voices[t->count-1];
    if (abc_is_endbar(new) || abc_is_repeat(new)) {
        v->in_alt = 0;
    }
    // for now, only bars and have 'in_alt' field set.
    new->in_alt = v->in_alt;
}

int abc_alt_is_of(const struct abc_symbol* s, int alt) {
    return (strchr(s->text, alt + '0') != NULL);
}

void abc_alt_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_ALT;
    new->text = strdup(yytext);
    struct abc_tune* t = yy->tunes[yy->count-1];
    struct abc_voice* v = t->voices[t->count-1];
    v->in_alt = 1;
}

void abc_nuplet_append(struct abc* yy, int p, int q, int r)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_NUP;
    if (-1 == asprintf(&new->text, "%d:%d:%d", p, q, r)) {;}
}

void abc_deco_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_DECO;
    new->text = strdup(yytext);
}

void abc_gchord_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_GCHORD;
    new->text = strdup(yytext);
}

void abc_tie_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_TIE;
    new->text = strdup(yytext);
    new->prev->in_tie = 1;
}

void abc_slur_append(struct abc* yy, const char* yytext)
{
    struct abc_symbol* new = abc_new_symbol(yy);
    new->kind = ABC_SLUR;
    new->text = strdup(yytext);
}

void abc_delete_symbols(struct abc_symbol* s) {
    struct abc_symbol* next = s;

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
        struct abc_tune* t = yy->tunes[i];
        struct abc_header* h = t->headers;
        while (h) {
            struct abc_header* hn = h->next;
            free(h->text);
            free(h);
            h = hn;
        }

        for (int j = 0; j < t->count; j++) {
            struct abc_voice* v = t->voices[j];
            struct abc_symbol* s = v->first;
            while (s != v->last) {
                struct abc_symbol* sn = s->next;
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

struct abc* abc_parse_buffer(const char* buffer, int size) {
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

struct abc_tune* abc_find_tune(struct abc* yy, int x) {
    struct abc_tune* t = NULL;
    for (int i = 0; i < yy->count; i++) {
        if (yy->tunes[i]->x == x) {
            t = yy->tunes[i];
            break;
        }
    }

    return t;
}

struct abc_header* abc_find_header(const struct abc_tune* t, char h) {
    if (!t) return NULL;

    struct abc_header* header = t->headers;

    while(header) {
        if (header->h == h) {
            return header;
        }

        header = header->next;
    }

    return header;
}

/* rewind and return the first symbol just after start repeat */
struct abc_symbol* abc_find_start_repeat(struct abc_symbol* s) {
    while (s->prev) {
        s = s->prev;
        if (s->kind == ABC_BAR && strstr(s->text, "|:"))
            return s->next;
    }

    return s;
}

/* return the next repeat bar */
struct abc_symbol* abc_find_next_repeat(struct abc_symbol* s) {
    while (s->next) {
        s = s->next;
        if (s->kind == ABC_BAR && abc_is_repeat(s))
            return s;
    }

    return s;
}

/* return a bar symbol matching next alternation in pass of alt */
struct abc_symbol* abc_find_next_alt(struct abc_symbol* s, int alt) {
    while (s->next) {
        s = s->next;
        /*
           if (s->kind == ABC_BAR && abc_is_repeat(s))
           return NULL;
           */
        if (s->kind == ABC_BAR && abc_is_endbar(s)) {
            return s;
        }

        if ((s->kind == ABC_ALT) && abc_alt_is_of(s, alt)) {
            return s->prev;
        }

        if (s->kind == ABC_BAR) {
            if (!s->in_alt)
                return s;
        }
    }

    return s;
}

struct abc_symbol* abc_find_next_segno(struct abc_symbol* s) {
    while (s->next) {
        s = s->next;
        if (s->kind == ABC_DECO && !strcmp(s->text, "segno"))
            return s;
    }

    return s;
}

int abc_has_pair(struct abc_symbol* s, int chord) {
    const char* note = s->text;
    if (chord) {
        int in = 0;
        s = abc_chord_forward(s);
        s = s->next; /* '-' */
        while (s->next && s->next->text[0] != ']') {
            s = s->next;
            if (!in && (s->kind == ABC_NOTE))
                return 0;

            if (in && (s->kind == ABC_NOTE)) {
                if (!strcmp(note, s->text))
                    return 1;
            }
            if (s->text[0] == '[')
                in = 1;
        }
    } else {
        while (s->next) {
            s = s->next; /* '-' */
            if (s->kind == ABC_NOTE) {
                if (!strcmp(note, s->text))
                    return 1;
                return 0;
            }
        }
    }

    return 0;
}

int abc_has_tie(struct abc_symbol* s, int chord) {
    if (s->in_tie)
        return 1;

    while (s->next) {
        s = s->next;
        if (chord) {
            if ((s->kind == ABC_CHORD) && s->text[0] == ']') {
                if (s->next && (s->next->kind == ABC_TIE))
                    return 1;
                return 0;
            }
        } else {
            if (s->kind == ABC_TIE)
                return 1;
            return 0;
        }
    }

    return 0;
}

double abc_grace_duration(struct abc_symbol* s) {
    if (s->kind != ABC_GRACE)
        return 0.0;

    double dur = 0.0;
    while (s->next) {
        s = s->next;
        if (s->kind == ABC_NOTE) {
            dur += (double) s->dur_num / (double) s->dur_den;
        }

        if (s->kind == ABC_GRACE) /* end of grace */
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

int abc_unit_per_measure(struct abc_tune* t) {
    struct abc_header* lh = abc_find_header(t, 'L');
    struct abc_header* mh = abc_find_header(t, 'M');

    int ln, ld, mn, md;
    if (lh && (2 == sscanf(lh->text, "%d/%d", &ln, &ld))) {;}
    else { ln = 1; ld = 8; }

    if (mh && (2 == sscanf(mh->text, "%d/%d", &mn, &md))) {;}
    else { mn = 4; md = 4; }

    return ld * mn / md;
}

/* tempo in quarter note per minute */
long abc_tempo(struct abc_tune* t) {
    long tempo = 120;
    struct abc_header* qh = abc_find_header(t, 'Q');
    if (qh) {
        int q = 0;
        int num, den;
        if (3 == sscanf(qh->text, "%d/%d=%d", &num, &den, &q))
            tempo = (q * 4 * num) / den;
        else if (1 == sscanf(qh->text, "%d", &q))
            tempo = q;
        else if (!strncasecmp(qh->text, "\"Largo\"", 5))
            tempo = 40;
        else if (!strncasecmp(qh->text, "\"Larghetto\"", 9))
            tempo = 60;
        else if (!strncasecmp(qh->text, "\"Lento\"", 5))
            tempo = 50;
        else if (!strncasecmp(qh->text, "\"Adagio\"", 6))
            tempo = 60;
        else if (!strncasecmp(qh->text, "\"Andante\"", 7))
            tempo = 80;
        else if (!strncasecmp(qh->text, "\"Moderato\"", 8))
            tempo = 90;
        else if (!strncasecmp(qh->text, "\"Allegro\"", 7))
            tempo = 120;
        else if (!strncasecmp(qh->text, "\"Presto\"", 6))
            tempo = 140;
        else if (!strncasecmp(qh->text, "\"Vivace\"", 6))
            tempo = 160;
        else
            tempo = 120;
    }

    return tempo;
}

void abc_compute_pqr(int* p, int* q, int* r, const struct abc_tune* t) {
    int num, den;
    struct abc_header* mh = abc_find_header(t, 'M');

    if (!mh)
        num = den = 4;
    if (mh->text[0] == 'C')
        num = den = 4;
    else if (2 != sscanf(mh->text, "%d/%d", &num, &den))
        num = den = 4;

    if (!*r)
        *r = *p;

    if (!*q) {
        switch (*p) {
            case 2: *q = 3; break;
            case 3: *q = 2; break;
            case 4: *q = 3; break;
            case 5: if (!(num % 3) && (den == 8)) *q = 3; else *q = 2; break;
            case 6: *q = 2; break;
            case 7: if (!(num % 3) && (den == 8)) *q = 3; else *q = 2; break;
            case 8: *q = 3; break;
            case 9: if (!(num % 3) && (den == 8)) *q = 3; else *q = 2; break;
        }

    }
}

struct abc_symbol* abc_dup_symbol(struct abc_symbol* from) {
    struct abc_symbol* to = calloc(1, sizeof (struct abc_symbol));
    to = memcpy(to, from, sizeof (*from));
    to->prev = NULL;
    to->next = NULL;
    if (from->text)
        to->text = strdup(from->text);
    if (from->lyric)
        to->lyric = strdup(from->lyric);
    return to;
}

void abc_voice_append_symbol(struct abc_voice* voice, struct abc_symbol* s) {
    if (s) {
        if (voice->last == NULL) {
            voice->last = s;
            voice->first = voice->last;
        } else {
            struct abc_symbol* l = voice->last;
            voice->last = s;
            l->next = s;
            s->prev = l;
            s->index = l->index + 1;
        }
    }
}

void abc_frac_add(long* num, long* den, long from_num, long from_den) {
	long ppcm = *den > from_den ? *den : from_den;

	while (1) {
		if ((ppcm % *den) || (ppcm % from_den)) {
			ppcm++;
			continue;
		}
		break;
	}


	long cofactor1 = ppcm / *den;
	long cofactor2 = ppcm / from_den;

	long num1 = *num * cofactor1;
	long num2 = from_num * cofactor2;

	*num = num1 + num2;
	*den = ppcm;
}

void abc_duration_add(struct abc_symbol* to, struct abc_symbol* with) {
    long num1 = to->dur_num * with->dur_den;
    long num2 = with->dur_num * to->dur_den;
    long den = to->dur_den * with->dur_den;
    to->dur_num = num1 + num2;
    to->dur_den = den;
}

static int event_cmp(const void* a, const void* b) {
	const struct abc_symbol* s1 = a;
	const struct abc_symbol* s2 = b;
	double s1_v, s2_v;

	s1_v = (double) s1->start_num / (double) s1->start_den;
	s2_v = (double) s2->start_num / (double) s2->start_den;

	if (s1_v > s2_v)
		return 1;
	if (s1_v < s2_v)
		return -1;

	if (!strcmp(s1->text, s2->text))
		return 0;

	return  -1;
}

struct abc_voice* abc_eventy_voice(const struct abc_voice* v) {
    int in_chord = 0;

    /* use absolute ticks */
    long tick_num = 0, tick_den = 1;

    struct abc_voice* voice = calloc(1, sizeof (struct abc_voice));
    voice->v = strdup(v->v);

    struct abc_symbol* s = v->first;
    while (s) {
	    switch (s->kind) {
		    case ABC_CHORD: {
					    if (s->text[0] == '[')
						    in_chord = 1;
					    else
						    in_chord = 0;
				    }
				    break;
		    case ABC_NOTE: {
					   if (!in_chord) {
						   struct abc_symbol* on = NULL;
						   struct abc_symbol* off = NULL;

						   on = abc_dup_symbol(s);
						   on->value = 1;

						   abc_frac_add(&tick_num, &tick_den, on->dur_num, on->dur_den);

						   off = abc_dup_symbol(on);
						   off->start_num = tick_num;
						   off->start_den = tick_den;
						   off->dur_num = 0;
						   off->dur_den = 1;
						   off->value = 0;

						   abc_voice_append_symbol(voice, on);
						   abc_voice_append_symbol(voice, off);
					   } else {
						   /* prepare note on */
						   struct abc_symbol* starts = NULL;
						   int start_count = 0;
						   while (s->kind != ABC_CHORD) { /* got to ']' */
							   if (s->kind == ABC_NOTE) {
								   starts = realloc(starts, ++start_count * sizeof (*starts));
								   starts[start_count - 1] = *abc_dup_symbol(s);

								   starts[start_count - 1].value = 1;
							   }
							   s = s->next;
						   }

						   /* prepare note off based on previous array */
						   struct abc_symbol* stops = NULL;
						   int stop_count = start_count;
						   stops = realloc(stops, stop_count * sizeof (*stops));
						   for (int i = 0; i < stop_count; i++) {
							   stops[i] = *abc_dup_symbol(&starts[i]);
							   stops[i].value = 0;
						   }

						   /* reorganize note off members */
						   for (int i = 0; i < stop_count; i++) {
							   struct abc_symbol* e = &stops[i];
							   abc_frac_add(&e->start_num, &e->start_den, e->dur_num, e->dur_den);

							   e->dur_num = 0;
							   e->dur_den = 1;
						   }

						   struct abc_symbol* events = NULL;
						   int event_count = start_count + stop_count;
						   events = realloc(events, event_count * sizeof (*events));
						   for (int i = 0; i < start_count; i++) {
							   events[i] = *abc_dup_symbol(&starts[i]);
						   }

						   /* free garbage */
						   for (int i = 0; i < start_count; i++) {
							   free(starts[i].text);
							   free(starts[i].lyric);
						   }

						   free(starts);

						   for (int i = 0; i < stop_count; i++) {
							   events[i + start_count] = *abc_dup_symbol(&stops[i]);
						   }

						   /* free garbage */
						   for (int i = 0; i < stop_count; i++) {
							   free(stops[i].text);
							   free(stops[i].lyric);
						   }

						   free(stops);

						   /* sort in time on/off events */
						   qsort(events, event_count, sizeof (*events), event_cmp);

						   for (int i = 0; i < event_count; i++) {
							   abc_voice_append_symbol(voice, abc_dup_symbol(&events[i]));
						   }

						   /* we must use the complete chord duration as duration to add to main tick! */
						   int first_start_num = events[0].start_num;
						   int first_start_den = events[0].start_den;
						   int last_stop_num = events[event_count -1].start_num;
						   int last_stop_den = events[event_count - 1].start_den;
						   abc_frac_add(&tick_num, &tick_den, last_stop_num, last_stop_den);
						   abc_frac_add(&tick_num, &tick_den, -first_start_num, first_start_den);

						   /* free garbage */
						   for (int i = 0; i < event_count; i++) {
							   free(events[i].text);
							   free(events[i].lyric);
						   }

						   free(events);

						   /* go just before ']' */
						   s = s->prev;
					   }
				   }
				   break;
		    default: {
				     /* and ABC_BAR */
				     struct abc_symbol* new = NULL;
				     new = abc_dup_symbol(s);
				     abc_voice_append_symbol(voice, new);
			     }

	    }

	    s = s->next;
    }

    return voice;
}

struct abc_voice* abc_untie_voice(struct abc_voice* v, struct abc_tune* t) {
    /* use absolute ticks */
    long tick_num = 0, tick_den = 1;   /* current tick */

    int nup_p, nup_q, nup_r = 0;
    int in_tie = 0;
    int in_chord = 0;
    long chord_num = 0, chord_den = 1; /* chord duration */
    struct abc_voice* voice = calloc(1, sizeof (struct abc_voice));
    voice->v = strdup(v->v);

    struct abc_symbol* s = v->first;
    while (s) {
        struct abc_symbol* new = NULL;

        switch (s->kind) {
            case ABC_NUP: {
                              if (3 == sscanf(s->text, "%d:%d:%d", &nup_p, &nup_q, &nup_r)) {
                                  abc_compute_pqr(&nup_p, &nup_q, &nup_r, t);
                              }
                          }
                          break;
            case ABC_CHORD: {
                                if (!in_tie)
                                    new = abc_dup_symbol(s);

                                if (s->text[0] == '[') {
                                    in_chord = 1;
                                } else {
                                    in_chord = 0;
                                    in_tie = 0;
                                    abc_frac_add(&tick_num, &tick_den, chord_num, chord_den);
                                    chord_num = 0, chord_den = 1;
                                }
                            }
                            break;
            case ABC_NOTE: {
                               if (!in_tie) {
                                   if (!in_chord) {
                                       /* produce simple note */
                                       new = abc_dup_symbol(s);

                                       if (nup_r) {
                                           new->dur_num *= nup_q;
                                           new->dur_den *= nup_p;
                                           nup_r--;
                                       }

                                       abc_frac_add(&new->start_num, &new->start_den, tick_num, tick_den);
                                       abc_frac_add(&tick_num, &tick_den, new->dur_num, new->dur_den);
                                   } else {
                                       new = abc_dup_symbol(s);
                                       abc_frac_add(&new->start_num, &new->start_den, tick_num, tick_den);
                                       /* we use the first note of chord as chord duration! */
                                       if (chord_num == 0) {
                                           chord_num = s->dur_num;
                                           chord_den = s->dur_den;
                                       }
                                   }
                               } else {
                                   if (!in_chord) {
                                       int prev_chord = 0;
                                       /* prev note just lasts longer */
                                       struct abc_symbol* p = voice->last;
                                       while (p->kind != ABC_NOTE) {
                                           if (p->kind == ABC_CHORD)
                                               prev_chord = 1;
                                           p = p->prev;
                                       }
                                       /* prev note could be in a chord! */
                                       if (prev_chord) {
                                           while (strcmp(p->text, s->text)) {
                                               p = p->prev;
                                               if (p->kind == ABC_CHORD)
                                                   break;
                                           }
                                       }

                                       int nup_num = s->dur_num;
                                       int nup_den = s->dur_den;
                                       if (nup_r) {
                                           nup_num *= nup_q;
                                           nup_den *= nup_p;
                                           nup_r--;
                                       }

                                       if (p->kind == ABC_NOTE) {
                                           abc_frac_add(&p->dur_num, &p->dur_den, nup_num, nup_den);
                                           abc_frac_add(&tick_num, &tick_den, nup_num, nup_den);
                                       } 
                                       in_tie = 0;
                                   } else {
                                       /* look if previous note is in a chord or a single note */
                                       struct abc_symbol* p = voice->last;
                                       while (p->kind != ABC_CHORD && p->kind != ABC_NOTE)
                                           p = p->prev;

                                       if (p->kind == ABC_CHORD) {
                                           /* for each s note, find the p note if any */
                                           while (s ->kind != ABC_CHORD) {
                                               if (s->kind == ABC_NOTE) {
                                                   p = abc_chord_rewind(p->prev);
                                                   p = abc_chord_first_note(p);

                                                   while (p->kind != ABC_CHORD) {
                                                       if (p->kind == ABC_NOTE && !strcmp(p->text, s->text)) {
                                                           abc_duration_add(p, s);
                                                           /* we use the first note of this chord to add to chord duration! */
                                                           if (chord_num == 0) {
                                                               abc_frac_add(&chord_num, &chord_den, s->dur_num, s->dur_den);
                                                           }
                                                           break;
                                                       }

                                                       p = p->next;
                                                   }

                                                   /* corresponding p note to s was not found? */
                                                   if (p->kind == ABC_CHORD) {
                                                           struct abc_symbol* n = abc_dup_symbol(s);
                                                           abc_frac_add(&n->start_num, &n->start_den, tick_num, tick_den);
                                                           /* do not append this note after the ']' chord, instert it! */
                                                           p = p->prev;

                                                           n->next = p->next;
                                                           p->next = n;
                                                           n->prev = p;
                                                           n->next->prev = n;

                                                           /* we use the first note of this chord to add to chord duration! */
                                                           if (chord_num == 0) {
                                                                   abc_frac_add(&chord_num, &chord_den, s->dur_num, s->dur_den);
                                                           }
                                                   }
                                               } else {
                                                   /* any other symbol goes here */
                                                   struct abc_symbol* u = abc_dup_symbol(s);
                                                   abc_voice_append_symbol(voice, u);
                                               }

                                               s = s->next;
                                           }

                                           new = NULL;
                                           in_tie = 0;
                                           /* do not pass by CHORD token, but manage it here: */
                                           abc_frac_add(&tick_num, &tick_den, chord_num, chord_den);
                                           chord_num = 0, chord_den = 1;
                                           in_chord = 0;
                                       } else {
                                           /* instert chord while we are in tie
                                            * and play some of its notes later
                                            */
                                           struct abc_symbol* new = calloc(1, sizeof (struct abc_symbol));
                                           new->kind = ABC_CHORD;
                                           new->text = strdup("[");
					   new->start_den = 1;
					   new->dur_den = 1;

                                           /* insert new virtual chord just before the previously accepted note */
                                           p->prev->next = new;
                                           new->prev = p->prev;
                                           p->prev = new;
                                           new->next = p;
                                           int num = p->dur_num;
                                           int den = p->dur_den;

                                           while (s->kind != ABC_CHORD) {
                                               if (s->kind  == ABC_NOTE) {
                                                   if (!strcmp(p->text, s->text)) { /* same note */
                                                       abc_duration_add(p, s);
                                                       num = s->dur_num; 
                                                       den = s->dur_den;
                                                   } else {
                                                       struct abc_symbol* n = abc_dup_symbol(s);
                                                       abc_frac_add(&n->start_num, &n->start_den, tick_num, tick_den);
                                                       abc_voice_append_symbol(voice, n);
                                                   }
                                               } else {
                                                   /* any other symbol goes here */
                                                   struct abc_symbol* u = abc_dup_symbol(s);
                                                   abc_voice_append_symbol(voice, u);
                                               }

                                               s = s->next;
                                           }
                                           /* use the first note modified duration to add to main tick */
                                           abc_frac_add(&tick_num, &tick_den, num, den);

                                           s = s->prev; /* this will allow chord closing */
                                           new = NULL;
                                           in_tie = 0;
                                       }
                                   }
                               }
                           }
                           break;
            case ABC_TIE: {
                              in_tie = 1;
                          }
                          break;
            case ABC_BAR:
                          /* don't break, use bars for accidentals context */
            default: {
                         new = abc_dup_symbol(s);
                     }
        }

        if (new) {
            abc_voice_append_symbol(voice, new);
        }

        s = s->next;
    }

    return voice;
}

struct abc_voice* abc_unfold_voice(struct abc_voice* v) {
    int pass = 1;
    struct abc_symbol* cur_repeat = NULL;

    struct abc_voice* voice = calloc(1, sizeof (struct abc_voice));
    voice->v = strdup(v->v);

    struct abc_symbol* s = v->first;
    while (s) {
        struct abc_symbol* new = NULL;

        switch (s->kind) {
            case ABC_BAR: {
                              new = calloc(1, sizeof (struct abc_symbol));
                              new->kind = ABC_BAR;
                              new->text = strdup("|");
			      new->start_den = 1;
			      new->dur_den = 1;

                              if (abc_is_repeat(s)) {
                                  if (cur_repeat == s) {
                                      /* never reset cur_repeat to NULL (loops forever) */

                                      if (abc_is_start(s)) {
                                          pass = 1;
                                      }
                                  } else if (!cur_repeat || (cur_repeat->index < s->index)) {
                                      cur_repeat = s;
                                      s = abc_find_start_repeat(s);
                                      abc_voice_append_symbol(voice, new);
                                      pass++;
                                      continue;
                                  }
                              } else if (abc_is_start(s)) {
                                  pass = 1;
                              }
                              break;
                          }
            case ABC_ALT: {
                              new = NULL;

                              if (!abc_alt_is_of(s, pass)) {
                                  struct abc_symbol* n;
                                  if ((n = abc_find_next_alt(s, pass))) {
                                      s = n; /* s is a BAR */
                                      continue;
                                  }
                              }

                              break;
                          }
            default: {
                         new = abc_dup_symbol(s);
                     }
        }

        if (new) {
            abc_voice_append_symbol(voice, new);
        }
        s = s->next;
    }

    return voice;
}

struct abc_symbol* abc_chord_first_note(struct abc_symbol* s) {
    if (s->kind == ABC_CHORD) {
        while (s->next) {
            s = s->next;
            if (s->kind == ABC_NOTE)
                return s;
        }
    }

    return NULL;
}

struct abc_symbol* abc_chord_rewind(struct abc_symbol* s) {
    while (s) {
        if (s->kind == ABC_CHORD && s->text[0] == ']')
            return NULL;

        if ((s->kind == ABC_CHORD) && (s->text[0] == '['))
            return s;
        s = s->prev;
    }

    return NULL;
}

struct abc_symbol* abc_chord_forward(struct abc_symbol* s) {
    while (s) {
        if ((s->kind == ABC_CHORD) && (s->text[0] == ']'))
            return s;
        s = s->next;
    }

    return NULL;
}

void abc_release_voice(struct abc_voice* v) {
    struct abc_symbol* s = v->first;
    while (s != v->last) {
        struct abc_symbol* sn = s->next;
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
