#include <string.h>
#include "abc.h"
#include "parser.h"
#include "abcparse.h"

struct abc* abc2smf_abc_parse(const char* buffer, int size) {
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

void abc2smf_abc_release(struct abc* yy) {
	abc_release_yy(yy);
}

struct tune* find_tune(struct abc* yy, int x) {
	struct tune* t = NULL;
	for (int i = 0; i < yy->count; i++) {
		if (yy->tunes[i]->x == x) {
			t = yy->tunes[i];
			break;
		}
	}

	return t;
}

struct header* find_header(struct tune* t, char h) {
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

struct voice* find_voice(struct tune* t, int v) {
	struct voice* voice = NULL;
	for (int i = 0; i < t->count; i++) {
		if (t->voices[i]->v == v) {
			voice = t->voices[i];
			break;
		}
	}

	return voice;
}

struct symbol* find_start_repeat(struct symbol* s) {
	while (s->prev) {
        s = s->prev;
		if (s->kind == BAR && strstr(s->text, "|:"))
            return s->next;
	}

	return s;
}

struct symbol* find_next_repeat(struct symbol* s) {
    while (s->next) {
        s = s->next;
        if (s->kind == BAR && is_repeat(s))
            return s;
    }

    return s;
}

struct symbol* find_next_alt(struct symbol* s, int alt) {
    while (s->next) {
        s = s->next;

        if (s->kind == BAR && is_repeat(s))
            return NULL;

        if (s->kind == BAR && is_endbar(s)) {
            return s;
        }

        if ((s->kind == ALT) && alt_is_of(s, alt)) {
                return s->prev;
        }

        if (s->kind == BAR) {
            if (!s->in_alt)
                return s;
        }
    }

    return s;
}

struct symbol* find_next_segno(struct symbol* s) {
	while (s->next) {
		s = s->next;
		if (s->kind == DECO && !strcmp(s->text, "segno"))
			return s;
	}

	return s;
}


double apply_divide(const char* div) {
	if (!strchr(div, '/'))
		return 0.0;

	int num, den;
	if(2 != sscanf(div, "%d/%d", &num, &den))
		return 0.0;
	return (double) num / (double) den;
}

int unit_per_measure(struct tune* t) {
	struct header* lh = find_header(t, 'L');
	struct header* mh = find_header(t, 'M');

	int ln, ld, mn, md;
	if (lh && (2 == sscanf(lh->text, "%d/%d", &ln, &ld))) {;}
	else { ln = 1; ld = 8; }

	if (mh && (2 == sscanf(mh->text, "%d/%d", &mn, &md))) {;}
	else { mn = 4; md = 4; }

	return ld * mn / md;
}

double second_per_unit(struct tune* t) {
	double s = 1.0;
	struct header* lh = find_header(t, 'L');
	struct header* qh = find_header(t, 'Q');

	double l;
	if (lh)
		l = apply_divide(lh->text);
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
