#include <QDebug>
#include <QTextCodec>
#include "abcsmf.h"
#include <drumstick/qsmf.h>

AbcSmf::AbcSmf(struct abc* yy, int x, QObject *parent) : QSmf(parent),
        yy(yy),
        x(x),
        t(nullptr),
        kh(nullptr),
        tpu(0),
        upm(0),
        tempo(0),
        expr(EXPRESSION_DEFAULT),
        z_tick(0),
        in_tie(false),
        in_slur(SHORTEN_DEFAULT),
        in_grace(false),
        in_cresc(0),
        mark_dyn(DYN_DEFAULT),
        cur_dyn(DYN_DEFAULT),
        grace_tick(0),
        noteon(0x90),
        program(0xc0),
        control(0xb0)
{
        connect(this, &QSmf::signalSMFWriteTempoTrack, this, &AbcSmf::onSMFWriteTempoTrack);
        connect(this, &QSmf::signalSMFWriteTrack, this, &AbcSmf::onSMFWriteTrack);

        t = abc_find_tune(yy, x);
        upm = abc_unit_per_measure(t);
        tempo = abc_tempo(t);

        setDivision(DPQN);
        setTextCodec(QTextCodec::codecForName("UTF-8"));
        setFileFormat(1);
        setTracks(t->count);

        long num, den;
        struct abc_header* lh = abc_find_header(t, 'L');
        if (!lh)
                num = 1, den = 8;
        else
                abcGetNumDen(lh->text, &num, &den);

        tpu = (num * 4 * DPQN) / (den);
        qWarning() << "ticks per unit" << tpu;

        kh = abc_find_header(t, 'K');
        if (!kh) {
                mode = 0, ks = NULL;
        } else {
            ks = kh->text;
            mks = getSMFKeySignature(ks, &mode);
        }
}

void AbcSmf::onSMFWriteTempoTrack(void) {
    qWarning() << __func__;
}

void AbcSmf::onSMFWriteTrack(int track) {
        qWarning() << __func__ << track;
        long mspqn = 60000 / tempo;
        writeTempo(0, mspqn);
        writeBpmTempo(getCurrentTime(), tempo);
        writeKeySignature(getCurrentTime(), mks, mode);

        struct abc_voice* v = t->voices[track];
        struct abc_symbol* s = v->first;

        noteon = 0x90;
        program = 0xc0;
        control = 0xb0;

        in_tie = 0; /* inside a tie */
        in_grace = 0; /* inside a grace */
        in_cresc = 0;
        mark_dyn = DYN_DEFAULT;
        cur_dyn = mark_dyn; /* current dynamic in the tune */
        grace_tick = 0; /* grace group duration */

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

        struct abc_symbol* repeat = NULL;
        int pass = 1;
        int p, q, r = 0; /* n-uplet definition */
        dur_mod = 1.0; /* duration modified for n-uplets */
        nuplets = 0; /* number of notes in n-uplets */
        grace_mod = 1; /* duration modified for graces */
        expr = EXPRESSION_DEFAULT;
        in_slur = SHORTEN_DEFAULT;
        shorten = in_slur; /* dur will be shortened of 10% of a unit */
        z_tick = 0;

        writeMetaEvent(getCurrentTime(), 0x03, QString(v->v)); /* textual voice name */

        while (s) {
                long dur = 0;

                switch (s->kind) {
                case ABC_GCHORD:
                case ABC_EOL:
                case ABC_SPACE: {
                        break;
                }
                case ABC_NOTE: {
                        dur = duration(s);
                        setDynamic(dur);

                        if (s->text[0] == 'Z') {
                                dur = upm * tpu;
                                z_tick += dur;
                        } else if (s->text[0] == 'z') {
                                z_tick += dur;
                        } else if (abc_has_tie(s, 0) && in_tie) {
                                /* do not replay note at all */
                                z_tick += dur; /* add durations as they follow */
                        } else if (abc_has_tie(s, 0) || !in_tie) {
                                /* prepend with expression pedal if any */
                                writeExpression(track);

                                /* set note lyrics if any */
                                writeLyric(s->lyric);

                                /* find MIDI pitch */
                                const char* ks = kh ? kh->text : NULL;
                                unsigned char n = note2midi(ks, s->text, measure_accid);

                                writeMidiEvent(getCurrentTime() + z_tick, noteon, track, n, cur_dyn); /* note on */
                                if (abc_has_tie(s, 0)) {
                                        z_tick = dur;
                                } else {
                                        writeMidiEvent(getCurrentTime() + dur - (dur / shorten), noteon, track, n, 0x00); /* note off */
                                        z_tick = dur / shorten;
                                        shorten = in_slur;
                                }
                        } else if (in_tie) {
                                /* do not play note on */

                                /* prepend with expression pedal if any */
                                writeExpression(track);

                                /* set note lyrics if any */
                                writeLyric(s->lyric);

                                /* find MIDI pitch */
                                const char* ks = kh ? kh->text : NULL;
                                unsigned char n = note2midi(ks, s->text, measure_accid);

                                writeMidiEvent(getCurrentTime() + z_tick + dur - (dur /shorten), noteon, track, n, 0x00); /* note off */
                                z_tick = dur / shorten;
                                shorten = in_slur;
                                in_tie = 0;
                        }
                        break;
                }
                case ABC_NUP: {
                        if(3 != sscanf(s->text, "%d:%d:%d", &p, &q, &r)) { /* should not happen */;}
                        abc_compute_pqr(&p, &q, &r, t);
                        nuplets = r;
                        dur_mod = (double) q / (double) p;
                        break;
                }
                case ABC_GRACE: {
                        in_grace = strchr(s->text, '{') ? 1 : 0;
                        if (in_grace) {
                                double gd = abc_grace_duration(s); /* in units L */
                                grace_mod = tpu / (4 * gd); /* play 'fourth' faster */
                                grace_tick = (gd * 4.0) / tpu;
                        } else {
                                grace_tick = 0;
                        }
                        break;
                }
                case ABC_CHORD: {
                        if (abc_has_tie(s, 1) && in_tie) {
                            /* just take duration of first note and wait */
                            z_tick += duration(s->next);
                        } else if (abc_has_tie(s, 1) || !in_tie) {
                                /* align starts */
                                if (s->text[0] == '[') {
                                        s = s->next;
                                        while (s->kind != ABC_CHORD) { /* until ']' */
                                                if (s->kind == ABC_NOTE) {
                                                        setDynamic(dur);

                                                        /* prepend with expression pedal if any */
                                                        writeExpression(track);

                                                        /* set note lyrics if any */
                                                        writeLyric(s->lyric);

                                                        /* find MIDI pitch */
                                                        const char* ks = kh ? kh->text : NULL;
                                                        unsigned char n = note2midi(ks, s->text, measure_accid);

                                                        writeMidiEvent(getCurrentTime() + z_tick, noteon, track, n, cur_dyn); /* note on */
                                                        z_tick = 0;
                                                }
                                                s = s->next;
                                        }
                                }
                                /* now s is a  ']' chord symbol */
                                /* back to '[' and feed notes off, optionnaly */
                                s = s->prev;
                                while (s->kind != ABC_CHORD) { /* back until '[' */
                                        s = s->prev;
                                }
                                s = s->next;
                                dur = duration(s); /* take first note of chord for duration */
                                while (s->kind != ABC_CHORD) { /* until ']' */
                                        if (s->kind == ABC_NOTE) {
                                                /* find MIDI pitch */
                                                const char* ks = kh ? kh->text : NULL;
                                                unsigned char n = note2midi(ks, s->text, measure_accid);
                                                if (abc_has_tie(s, 1)) {
                                                    z_tick = dur;
                                                } else {
                                                        writeMidiEvent(getCurrentTime() + dur - (dur / shorten), noteon, track, n, 0x00); /* note off */
                                                        z_tick = dur / shorten;
                                                        shorten = in_slur;
                                                        dur = 0;
                                                }
                                        }
                                        s = s->next;
                                }
                        } else if (in_tie) {
                                /* do not play note on */
                                if (s->text[0] == '[') {
                                        s = s->next;
                                        dur = duration(s); /* take first note of chord for duration */
                                        while (s->kind != ABC_CHORD) { /* until ']' */
                                                if (s->kind == ABC_NOTE) {
                                                    /* prepend with expression pedal if any */
                                                    writeExpression(track);

                                                    /* set note lyrics if any */
                                                    writeLyric(s->lyric);

                                                    /* find MIDI pitch */
                                                    const char* ks = kh ? kh->text : NULL;
                                                    unsigned char n = note2midi(ks, s->text, measure_accid);

                                                    writeMidiEvent(getCurrentTime() + z_tick + dur - (dur / shorten), noteon, track, n, 0x00); /* note off */
                                                    z_tick = dur / shorten;
                                                    shorten = in_slur;
                                                    dur = 0;

                                                }
                                                s = s->next;
                                        }

                                        in_tie = 0;
                                } else { /* chord is ']' */
                                    /* play notes off */
                                    /* back to '[' and feed notes off, optionnaly */
                                    s = s->prev;
                                    while (s->kind != ABC_CHORD) { /* back until '[' */
                                            s = s->prev;
                                    }
                                    s = s->next;
                                    dur = duration(s); /* take first note of chord for duration */
                                    while (s->kind != ABC_CHORD) { /* until ']' */
                                            if (s->kind == ABC_NOTE) {
                                                /* prepend with expression pedal if any */
                                                writeExpression(track);

                                                /* set note lyrics if any */
                                                writeLyric(s->lyric);

                                                /* find MIDI pitch */
                                                const char* ks = kh ? kh->text : NULL;
                                                unsigned char n = note2midi(ks, s->text, measure_accid);

                                                writeMidiEvent(getCurrentTime() + z_tick + dur - (dur / shorten), noteon, track, n, 0x00); /* note off */
                                                z_tick = dur / shorten;
                                                shorten = in_slur;
                                                in_tie = 0;
                                            }
                                            s = s->next;
                                    }
                                }
                        }
                        shorten = in_slur;
                        expr = EXPRESSION_DEFAULT;
                        break;
                }
                case ABC_DECO: {
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
                                else if (!strcmp(deco, ".")) shorten = 2;
                                else if (!strcmp(deco, "H")) shorten = 100;
                                else if (!strcmp(deco, "tenuto")) shorten = 100;
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
                                        s = abc_find_next_segno(s);
                                        continue;
                                } else if (!strcmp(deco,"D.C.")) { /* FIXME */; }
                        } else { /* ERROR */ ;}
                        break;
                }
                case ABC_TIE: {
                        in_tie = 1;
                        break;
                }
                case ABC_SLUR: {
                        if (strchr(s->text, '(')) {
                                in_slur = 100;
                        } else {
                                in_slur = SHORTEN_DEFAULT;
                        }
                        break;
                }
                case ABC_BAR: {
                        /* reset measure accidentals */
                        memset(measure_accid, 0, sizeof measure_accid);

                        /* check loop for repeat bars */
                        if (abc_is_repeat(s)) {
                                if (!repeat || repeat->index < s->index) {
                                        repeat = s;
                                        s = abc_find_start_repeat(s);
                                        pass++;
                                        continue;
                                } else if (repeat == s) {
                                        repeat = NULL;
                                        if (abc_is_start(s)) {
                                                pass = 1;
                                        }
                                } else if (abc_is_start(s)) {
                                        pass = 1;
                                }
                        } else if (abc_is_start(s)) {
                                pass = 1;
                        }
                        break;
                }
                case ABC_ALT: {
                        if (!abc_alt_is_of(s, pass)) {
                                struct abc_symbol*  n;
                                if ((n = abc_find_next_alt(s, pass))) {
                                        s = n;
                                        continue;
                                } else {
                                        if (!repeat) {
                                                repeat = abc_find_next_repeat(s);
                                                s = abc_find_start_repeat(repeat);
                                                pass++;
                                                continue;
                                        } else {
                                                struct abc_symbol* r = abc_find_next_repeat(s);
                                                if (r) {
                                                        if (abc_is_start(r)) {
                                                                pass = 1;
                                                        }
                                                        s = r->next;
                                                        continue;
                                                } else if (abc_is_start(s)) {
                                                        pass = 1;
                                                }
                                        }
                                }
                        }
                        break;
                }
                case ABC_INST: {
                        int prog;
                        if (sscanf(s->text, "MIDI program %d", &prog)) {
                                writeMidiEvent(getCurrentTime(), program, track, prog);
                        }
                        break;
                }
                } /* OE case */
                s = s->next;
        }
        writeMetaEvent(getCurrentTime(), 0x2F);
}

/* text must be %d/%d */
void AbcSmf::abcGetNumDen(char* text, long* num, long* den) {
        bool ok;
        QString str(text);
        QStringList sl = str.split('/');
        if (sl.count() < 2) *num = 1, *den = 8;
        *num = sl.at(0).toLong(&ok);
        if (!ok) *num = 1;
        *den = sl.at(1).toLong(&ok);
        if (!ok) *den = 8;
}

int AbcSmf::getSMFKeySignature(char* text, int* mode) {
        QString str(text);

        *mode = 0;
        if (str.contains("min"))
                *mode = 1;

        if (str.isEmpty() || str == "C" || str == "Cmaj" || str == "Amin")
                return 0;

        if (str == "G" || str == "Gmaj" || str == "Emin")
                return 1;

        if (str == "D" || str == "Dmaj" || str == "Bmin")
                return 2;

        if (str == "A" || str == "Amaj" || str == "F#min")
                return 3;

        if (str == "E" || str == "Emaj" || str == "C#min")
                return 4;

        if (str == "B" || str == "Bmaj" || str == "G#min")
                return 5;

        if (str == "F#" || str == "F#maj" || str == "D#min")
                return 6;

        if (str == "C#" || str == "C#maj" || str == "A#min")
                return 7;


        if (str == "F" || str == "Fmaj" || str == "Dmin")
                return -1;

        if (str == "Bb" || str == "Bbmaj" || str == "Gmin")
                return -2;

        if (str == "Eb" || str == "Ebmaj" || str == "Cmin")
                return -3;

        if (str == "Ab" || str == "Abmaj" || str == "Fmin")
                return -4;

        if (str == "Db" || str == "Dbmaj" || str == "Bbmin")
                return -5;

        if (str == "Gb" || str == "Gbmaj" || str == "Ebmin")
                return -6;

        if (str == "Cb" || str == "Cbmaj" || str == "Abmin")
                return -7;

        return 0;
}

AbcSmf* AbcSmf::fromABC(const char* abc, int x, QObject *parent) {
        struct abc* yy = abc_parse_buffer(abc, strlen(abc));
        AbcSmf* smf = new AbcSmf(yy, x, parent);
        return smf;
}

/* semitones between note over C or c. */
unsigned char AbcSmf::pitch_diff_0x3c(const char* ks, int note) {
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
unsigned char AbcSmf::note2midi (const char* keysig, const char* note, int* measure_accid) {
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

long AbcSmf::duration(struct abc_symbol* s) {
    long dur;

    /* note duration */
    dur = s->dur_num * tpu / s->dur_den;

    /* n-uplet duration */
    if (nuplets > 0) {
            dur *= dur_mod;
            nuplets--;
    }

    /* grace note duration */
    if (in_grace)
            dur /= grace_mod;

    return dur;
}

void AbcSmf::setDynamic(long dur) {
    /* dynamics */
    long d = (dur > 4 * tpu ? 4 : dur > 2 * tpu ? 2 : 1);
    if (in_cresc > 0)
            cur_dyn = (cur_dyn + d) < 128 ? cur_dyn + d : 127;
    else if (in_cresc < 0)
            cur_dyn = (cur_dyn - d) > 30 ? cur_dyn - d : 30;
    else
            cur_dyn = mark_dyn;
}

void AbcSmf::writeExpression(int track) {
        if (expr) {
                writeMidiEvent(getCurrentTime() + z_tick, control, track, 0x0b, expr);
                expr = EXPRESSION_DEFAULT;
                z_tick = 0;
        }
}

void AbcSmf::writeLyric(const char* lyric) {
        if (lyric) {
                writeMetaEvent(getCurrentTime() + z_tick, 0x05, QString(lyric));
                z_tick = 0;
        }
}
