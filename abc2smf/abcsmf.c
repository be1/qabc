#include "abc.h"
#include "abcparse.h"
#include <smf.h>

static
const unsigned char _pitch_diff_CMaj_0x3c[] = {
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

static
const unsigned char _pitch_diff_GMaj_0x3c[] = {
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 7 + 12
};

static
const unsigned char _pitch_diff_DMaj_0x3c[] = {
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 7 + 12
};

static
const unsigned char _pitch_diff_AMaj_0x3c[] = {
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

static
const unsigned char _pitch_diff_EMaj_0x3c[] = {
    ['A'] = 9,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 3,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 9 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 3 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

static
const unsigned char _pitch_diff_BMaj_0x3c[] = {
    ['A'] = 10,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 3,
    ['E'] = 4,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 10 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 3 + 12,
    ['e'] = 4 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

static
const unsigned char _pitch_diff_FSharpMaj_0x3c[] = {
    ['A'] = 10,
    ['B'] = 11,
    ['C'] = 1,
    ['D'] = 3,
    ['E'] = 5,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 10 + 12,
    ['b'] = 11 + 12,
    ['c'] = 1 + 12,
    ['d'] = 3 + 12,
    ['e'] = 5 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

static
const unsigned char _pitch_diff_GFlatMaj_0x3c[] = {
    ['A'] = 8,
    ['B'] = 10,
    ['C'] = -1,
    ['D'] = 1,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 6,

    ['a'] = 8 + 12,
    ['b'] = 10 + 12,
    ['c'] = -1 + 12,
    ['d'] = 1 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 6 + 12
};

static
const unsigned char _pitch_diff_CSharpMaj_0x3c[] = {
    ['A'] = 10,
    ['B'] = 12,
    ['C'] = 1,
    ['D'] = 3,
    ['E'] = 5,
    ['F'] = 6,
    ['G'] = 8,

    ['a'] = 10 + 12,
    ['b'] = 12 + 12,
    ['c'] = 1 + 12,
    ['d'] = 3 + 12,
    ['e'] = 5 + 12,
    ['f'] = 6 + 12,
    ['g'] = 8 + 12
};

static
const unsigned char _pitch_diff_DFlatMaj_0x3c[] = {
    ['A'] = 8,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 1,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 6,

    ['a'] = 8 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 1 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 6 + 12
};


static
const unsigned char _pitch_diff_AFlatMaj_0x3c[] = {
    ['A'] = 8,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 1,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 8 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 1 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

static
const unsigned char _pitch_diff_EFlatMaj_0x3c[] = {
    ['A'] = 8,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 8 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

static
const unsigned char _pitch_diff_BFlatMaj_0x3c[] = {
    ['A'] = 9,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 3,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 3 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};

static
const unsigned char _pitch_diff_FMaj_0x3c[] = {
    ['A'] = 9,
    ['B'] = 10,
    ['C'] = 0,
    ['D'] = 2,
    ['E'] = 4,
    ['F'] = 5,
    ['G'] = 7,

    ['a'] = 9 + 12,
    ['b'] = 10 + 12,
    ['c'] = 0 + 12,
    ['d'] = 2 + 12,
    ['e'] = 4 + 12,
    ['f'] = 5 + 12,
    ['g'] = 7 + 12
};






/* semitones between note over C or c. */
static
unsigned char pitch_diff_0x3c(const char* ks, int note) {
    if (!ks || !strcasecmp(ks, "") || !strcasecmp(ks, "C") || !strcasecmp(ks, "Cmaj") || !strcasecmp(ks, "Amin") /* || !strcasecmp(ks, "Ddor") ... */ )
        return _pitch_diff_CMaj_0x3c[note];

    if (!strcasecmp(ks, "G") || !strcasecmp(ks, "Gmaj") || !strcasecmp(ks, "Emin"))
        return _pitch_diff_GMaj_0x3c[note];

    if (!strcasecmp(ks, "D") || !strcasecmp(ks, "Dmaj") || !strcasecmp(ks, "Bmin"))
        return _pitch_diff_DMaj_0x3c[note];

    if (!strcasecmp(ks, "A") || !strcasecmp(ks, "Amaj") || !strcasecmp(ks, "F#min"))
        return _pitch_diff_AMaj_0x3c[note];

    if (!strcasecmp(ks, "E") || !strcasecmp(ks, "Emaj") || !strcasecmp(ks, "C#min"))
        return _pitch_diff_EMaj_0x3c[note];

    if (!strcasecmp(ks, "B") || !strcasecmp(ks, "Bmaj") || !strcasecmp(ks, "G#min"))
        return _pitch_diff_BMaj_0x3c[note];

    if (!strcasecmp(ks, "F#") || !strcasecmp(ks, "F#maj") || !strcasecmp(ks, "D#min"))
        return _pitch_diff_FSharpMaj_0x3c[note];

    if (!strcasecmp(ks, "Gb") || !strcasecmp(ks, "Gbmaj") || !strcasecmp(ks, "Ebmin"))
        return _pitch_diff_GFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "C#") || !strcasecmp(ks, "C#maj") || !strcasecmp(ks, "A#min"))
        return _pitch_diff_CSharpMaj_0x3c[note];

    if (!strcasecmp(ks, "C#") || !strcasecmp(ks, "Dbmaj") || !strcasecmp(ks, "Bbmin"))
        return _pitch_diff_DFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "Ab") || !strcasecmp(ks, "Abmaj") || !strcasecmp(ks, "Fmin"))
        return _pitch_diff_AFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "Eb") || !strcasecmp(ks, "Ebmaj") || !strcasecmp(ks, "Cmin"))
        return _pitch_diff_EFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "Bb") || !strcasecmp(ks, "Bbmaj") || !strcasecmp(ks, "Gmin"))
        return _pitch_diff_BFlatMaj_0x3c[note];

    if (!strcasecmp(ks, "F") || !strcasecmp(ks, "Fmaj") || !strcasecmp(ks, "Dmin"))
        return _pitch_diff_FMaj_0x3c[note];

    /* default */
    return _pitch_diff_CMaj_0x3c[note];
}

/* converts a note text to a MIDI pitch */
static
unsigned char note2midi (const char* keysig, const char* note, int* measure_accid) {
    int octava = 0;
    for (const char* c = note; *c; c++) {
        if (*c == ',')
            octava--;
        else if (*c == '\'')
            octava++;
    }

#define ACCID_NATURAL 0xff

    int accid = 0;
    for (const char* c = note; *c && !isalpha(*c); c++) {
        if (*c == '_')
            accid--;
        else if (*c == '^')
            accid++;
        else if (*c == '=')
            accid = ACCID_NATURAL;
    }

    unsigned char pitch = 0;
    const char *c;

    for (c = note; *c && !isalpha(*c); c++) {;}

    if ((*c >= 'A' && *c <= 'G') || (*c >= 'a' && *c <= 'g')) {
        if (accid)
            measure_accid[(int)*c] = accid;

        if (accid == ACCID_NATURAL)
            pitch = pitch_diff_0x3c("Cmaj", *c) + 0 + (octava * 12);
        else if (!accid) {
            if (measure_accid[(int)*c] == ACCID_NATURAL)
                pitch = pitch_diff_0x3c("Cmaj", *c) + 0 + (octava * 12);
            else
                pitch = pitch_diff_0x3c(keysig, *c) + measure_accid[(int)*c] + (octava * 12);
        } else
            pitch = pitch_diff_0x3c("Cmaj", *c) + accid + (octava * 12);

        pitch += 0x3c; /* MIDI key for C */
    }

    return pitch;
}

void compute_pqr(int* p, int* q, int* r, char* ts) {
    int num, den;

    if (ts[0] == 'C')
        num = den = 4;
    else if ( 2 != sscanf(ts, "%d/%d", &num, &den))
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

#define DYN_DEFAULT 80
#define SHORTEN_DEFAULT 0.1
#define EXPRESSION_DEFAULT 0

/* converts an ABC syntax tree into an SMF struct */
smf_t* abc2smf(struct abc* yy, int x) {
    smf_t* smf;

    smf = smf_new();
    if (!smf)
        return NULL;

    struct tune* t = find_tune(yy, x);
    if(!t) return NULL;

    struct header* kh = find_header(t, 'K');
    double spu = second_per_unit(t);
    int upm = unit_per_measure(t);
	double ulen = 1.0 / upm;

    for (int j = 0; j < t->count && j < 16; j++) {
        struct voice* v = t->voices[j];
        struct symbol* s = v->first;

        int noteon = 0x90 + j;
        int program = 0xc0 + j;
        int control = 0xb0 + j;

        smf_track_t* track = smf_track_new();
        if (!track) {
            smf_delete(smf);
            return NULL;
        }

        if(smf_set_ppqn(smf, 96)) {;}

        smf_add_track(smf, track);

        int chord = 0; /* inside a chord */
        int grace = 0; /* inside a grace */
        double cur_sec = 0.0; /* current second in the tune */
        int in_cresc = 0;
        int mark_dyn = DYN_DEFAULT;
        unsigned char cur_dyn = mark_dyn; /* current dynamic in the tune */
        double chord_dur = 0.0; /* chord duration */
		double grace_shift = 0.0; /* grace group duration */

        /* measure accidentals context */
        int measure_accid[] = {
            ['A'] = 0,
            ['B'] = 0,
            ['C'] = 0,
            ['D'] = 0,
            ['E'] = 0,
            ['F'] = 0,
            ['G'] = 0,
            ['a'] = 0,
            ['b'] = 0,
            ['c'] = 0,
            ['d'] = 0,
            ['e'] = 0,
            ['f'] = 0,
            ['g'] = 0
        };

        struct symbol* repeat = NULL;
        int pass = 1;
        int p, q, r = 0; /* n-uplet definition */
        double dur_mod = 1.0; /* duration modified for n-uplets */
        double grace_mod = 1.0; /* duration modified for graces */
        int expr = EXPRESSION_DEFAULT;
        double in_slur = SHORTEN_DEFAULT;
        double shorten = in_slur; /* dur will be shortened of 10% of a unit */
        struct symbol* tie = NULL;

        smf_event_t* ev = smf_event_new_textual(3, v->v);
        smf_track_add_event_seconds(track, ev, cur_sec);

        while (s) {
            double dur  = 0.0;

            if (s->kind == NOTE) {
                dur = (double) s->dur_num * spu / (double) s->dur_den;
                /* n-uplet */
                if (r) {
                    dur *= dur_mod;
                    r--;
                }
	
				if (grace)
					dur *= grace_mod;

                /* chord duration is the longest note duration */
                chord_dur = chord_dur < dur ? dur : chord_dur;

                int d = (dur > 1.0 ? 4 : dur > 0.5 ? 2 : 1);
                if (in_cresc > 0)
                    cur_dyn = (cur_dyn + d) < 128 ? cur_dyn + d : 127;
                else if (in_cresc < 0)
                    cur_dyn = (cur_dyn - d) > 30 ? cur_dyn - d : 30;
                else
                    cur_dyn = mark_dyn;

                if (s->text[0] == 'Z') {
                    dur = (double) upm * spu;
                } else if (s->text[0] == 'z') {
                    /* do nothing else for 'z' */
                } else {
                    smf_event_t* ev;

                    double start = cur_sec - grace_shift;
                    if (expr) {
                        ev = smf_event_new_from_bytes(control, 0x0b, expr); /* L => CC expression level */
                        smf_track_add_event_seconds(track, ev, start);
                    }

                    if (s->lyric) {
                        ev = smf_event_new_textual(5, s->lyric);
                        smf_track_add_event_seconds(track, ev, start);
                    }

                    const char* ks = kh ? kh->text : NULL;
                    unsigned char n = note2midi(ks, s->text, measure_accid);

                    if (!tie) {
                        ev = smf_event_new_from_bytes(noteon, n, cur_dyn); /* note on */
                        smf_track_add_event_seconds(track, ev, start);
                    } else {
                        if (tie->text[0] != ']')
                            tie = NULL;
                    }

                    if (!has_tie(s, chord)) {
                        ev = smf_event_new_from_bytes(noteon, n, 0x00); /* note off */
                        double stop = start + dur - (spu * shorten);
                        smf_track_add_event_seconds(track, ev, stop);
                    }
                }

                if (!chord) {
                    if (expr) {
                        smf_event_t* ev = smf_event_new_from_bytes(0xb0, 0x0b, expr);
                        smf_track_add_event_seconds(track, ev, cur_sec);
                        expr = EXPRESSION_DEFAULT;
                    }

                    chord_dur = 0.0;
                    cur_sec += dur; /* works for silences too */
                    shorten = in_slur;
                }
			} else if (s->kind == GRACE) {
                grace = strchr(s->text, '{') ? 1 : 0;
				if (grace) {
					double gd = grace_duration(s); /* in units L */
					grace_mod = 0.5 / gd; /* play 'twice' faster */
					grace_shift = gd * spu / 2.0;
				} else {
                    cur_sec -= grace_shift;
					grace_shift = 0.0;
				}
            } else if (s->kind == TIE) {
                tie = s->prev;
            } else if (s->kind == CHORD) {
                if (s->text[0] == '[')
                    chord = 1;
                else {
                    tie = NULL;
                    shorten = in_slur;
                    expr = EXPRESSION_DEFAULT;
                    cur_sec += chord_dur;
                    chord_dur = 0.0;
                    chord = 0;
                }
            } else if (s->kind == BAR) {
                /* reset measure accidentals */
                memset(measure_accid, 0, sizeof measure_accid);

                /* check loop for repeat bars */
                if (is_repeat(s)) {
                    if (!repeat || repeat->index < s->index) {
                        repeat = s;
                        s = find_start_repeat(s);
                        pass++;
                        continue;
                    } else if (repeat == s) {
                        repeat = NULL;
                        if (is_start(s)) {
                            pass = 1;
                        }
                    } else if (is_start(s)) {
                        pass = 1;
                    }
                } else if (is_start(s)) {
                    pass = 1;
                }
            } else if (s->kind == ALT) {
                if (!alt_is_of(s, pass)) {
                    struct symbol*  n;
                    if ((n = find_next_alt(s, pass))) {
                        s = n;
                        continue;
                    } else {
                        if (!repeat) {
                            repeat = find_next_repeat(s);
                            s = find_start_repeat(repeat);
                            pass++;
                            continue;
                        } else {
                            struct symbol* r = find_next_repeat(s);
                            if (r) {
                                if (is_start(r)) {
                                    pass = 1;
                                }
                                s = r->next;
                                continue;
                            } else if (is_start(s)) {
                                pass = 1;
                            }
                        }
                    }
                }
            } else if (s->kind == NUP) {
                if(3 != sscanf(s->text, "%d:%d:%d", &p, &q, &r)) { /* should not happen */;}
                struct header* th = find_header(t, 'T');
                compute_pqr(&p, &q, &r, th->text);
                dur_mod = (double) q / (double) p;
            } else if (s->kind == SLUR) {
                if (strchr(s->text, '(')) {
                    in_slur = 0.01;
                } else {
                    in_slur = SHORTEN_DEFAULT;
                }
            } else if (s->kind == DECO) {
                char deco[32];
                if (sscanf(s->text, "%s", deco)) {
                    if (!strcmp(deco, "pppp")) mark_dyn = cur_dyn = 30;
                    else if (!strcmp(deco, "ppp")) mark_dyn = cur_dyn = 30;
                    else if (!strcmp(deco, "pp")) mark_dyn = cur_dyn = 45;
                    else if (!strcmp(deco, "p")) mark_dyn = cur_dyn = 60;
                    else if (!strcmp(deco, "mp")) mark_dyn = cur_dyn = 75;
                    else if (!strcmp(deco, "mf")) mark_dyn = cur_dyn = 90;
                    else if (!strcmp(deco, "f")) mark_dyn = cur_dyn = 105;
                    else if (!strcmp(deco, "ff")) mark_dyn = cur_dyn = 120;
                    else if (!strcmp(deco, "fff")) mark_dyn = cur_dyn = 127;
                    else if (!strcmp(deco, "ffff")) mark_dyn = cur_dyn = 127;
                    else if (!strcmp(deco, "sfz")) mark_dyn = cur_dyn = 100;
                    else if (!strcmp(deco, ".")) shorten = 0.5;
                    else if (!strcmp(deco, "H")) shorten = 0.01;
                    else if (!strcmp(deco, "tenuto")) shorten = 0.01;
                    else if (!strcmp(deco, "L")) expr = 127;
                    else if (!strcmp(deco, "accent")) expr = 127;
                    else if (!strcmp(deco, "emphasis")) expr = 127;
                    else if (!strcmp(deco, "crescendo(")) in_cresc = 1;
                    else if (!strcmp(deco, "<(")) in_cresc = 1;
                    else if (!strcmp(deco, "crescendo)")) in_cresc = 0;
                    else if (!strcmp(deco, "<)")) in_cresc = 0;
                    else if (!strcmp(deco, "diminuendo(")) in_cresc = -1;
                    else if (!strcmp(deco, ">(")) in_cresc = -1;
                    else if (!strcmp(deco, "diminuendo)")) in_cresc = 0;
                    else if (!strcmp(deco, ">)")) in_cresc = 0;
                    else if (!strcmp(deco, "trill")) /* FIXME */;
                    else if (!strcmp(deco, "trill(")) /* FIXME */;
                    else if (!strcmp(deco, "trill)")) /* FIXME */;
                    else if (!strcmp(deco, "D.S.")) {
                        memset(measure_accid, 0, sizeof measure_accid);
                        s = find_next_segno(s);
                        continue;
                    } else if (!strcmp(deco,"D.C.")) { /* FIXME */; }
                } else { /* ERROR */ ;}
            } else if (s->kind == INST) {
                int prog;
                if (sscanf(s->text, "MIDI program %d", &prog)) {
                    smf_event_t* ev = smf_event_new_from_bytes(program, prog, -1);
                    smf_track_add_event_seconds(track, ev, cur_sec);
                }
            }

            s = s->next;
        }

        if (smf_track_add_eot_seconds(track, cur_sec)) {;}
    }

    return smf;
}
