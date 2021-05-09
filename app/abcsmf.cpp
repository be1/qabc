#include <QDebug>
#include <QTextCodec>
#include "abcsmf.h"
#include <drumstick/qsmf.h>

AbcSmf::AbcSmf(struct abc* yy, int x, QObject *parent) : QSmf(parent),
        yy(yy),
        x(x),
        t(nullptr),
        ks(nullptr),
        curks(nullptr),
        tpu(0),
        upm(0),
        tempo(0),
        expr(EXPRESSION_DEFAULT),
        last_tick(0),
        dur(0),
        in_slur(SHORTEN_DEFAULT),
        in_grace(false),
        in_cresc(0),
        mark_dyn(DYN_DEFAULT),
        cur_dyn(DYN_DEFAULT),
        grace_tick(0),
        noteon(0x90),
        program(0xc0),
        control(0xb0),
        num(1),
        den(8)
{
        connect(this, &QSmf::signalSMFWriteTempoTrack, this, &AbcSmf::onSMFWriteTempoTrack);
        connect(this, &QSmf::signalSMFWriteTrack, this, &AbcSmf::onSMFWriteTrack);

        t = abc_find_tune(yy, x);
        if (!t)
            return;

	const char* l;
	const char* m;
	struct abc_header* lh = abc_find_header(t, 'L');
	if (lh)
		l = lh->text;
	else
		l = "1/8";
	struct abc_header* mh = abc_find_header(t, 'M');
	if (mh)
		m = mh->text;
	else
		m = "4/4";

        upm = abc_unit_per_measure(l, m);
        qWarning() << "unit per measure" << upm;
        tempo = abc_tempo(t);

        setDivision(DPQN);
        setTextCodec(QTextCodec::codecForName("UTF-8"));
        setFileFormat(1);
        setTracks(t->count);

        getNumDen(l, &num, &den);

        tpu = (num * 4 * DPQN) / (den);
        qWarning() << "ticks per unit" << tpu;

        struct abc_header* kh = abc_find_header(t, 'K');
        if (!kh) {
                ks = nullptr, mode = 0;
        } else {
            ks = kh->text;
            mks = getSMFKeySignature(ks, &mode);
        }

        memset(measure_accid, 0, sizeof (measure_accid));
}

void AbcSmf::manageDecoration(struct abc_symbol* s) {
    if (!strcmp(s->text, "pppp")) mark_dyn = cur_dyn = 30;
    else if (!strcmp(s->text, "ppp")) mark_dyn = cur_dyn = 30;
    else if (!strcmp(s->text, "pp")) mark_dyn = cur_dyn = 45;
    else if (!strcmp(s->text, "p")) mark_dyn = cur_dyn = 60;
    else if (!strcmp(s->text, "mp")) mark_dyn = cur_dyn = 75;
    else if (!strcmp(s->text, "mf")) mark_dyn = cur_dyn = 90;
    else if (!strcmp(s->text, "f")) mark_dyn = cur_dyn = 105;
    else if (!strcmp(s->text, "ff")) mark_dyn = cur_dyn = 120;
    else if (!strcmp(s->text, "fff")) mark_dyn = cur_dyn = 127;
    else if (!strcmp(s->text, "ffff")) mark_dyn = cur_dyn = 127;
    else if (!strcmp(s->text, "sfz")) mark_dyn = cur_dyn = 100;
    else if (!strcmp(s->text, ".")) shorten = 2;
    else if (!strcmp(s->text, "H")) shorten = 100;
    else if (!strcmp(s->text, "tenuto")) shorten = 100;
    else if (!strcmp(s->text, "L")) expr = 127;
    else if (!strcmp(s->text, "accent")) expr = 127;
    else if (!strcmp(s->text, "emphasis")) expr = 127;
    else if (!strcmp(s->text, "crescendo(")) in_cresc = 1;
    else if (!strcmp(s->text, "<(")) in_cresc = 1;
    else if (!strcmp(s->text, "crescendo)")) in_cresc = 0;
    else if (!strcmp(s->text, "<)")) in_cresc = 0;
    else if (!strcmp(s->text, "diminuendo(")) in_cresc = -1;
    else if (!strcmp(s->text, ">(")) in_cresc = -1;
    else if (!strcmp(s->text, "diminuendo)")) in_cresc = 0;
    else if (!strcmp(s->text, ">)")) in_cresc = 0;
}

void AbcSmf::writeSingleNote(int track, struct abc_symbol* s) {
        long delta_tick;

        if (s->text[0] == 'Z' || s->text[0] == 'z') {
            /* no event */
        } else {
                delta_tick = (tpu * s->start_num / s->start_den) - last_tick;
                last_tick += delta_tick;

                /* modify cur_dyn from context */
                setDynamic(delta_tick);

                /* prepend with expression pedal if any */
                writeExpression(track);

                /* set note lyrics if any */
                writeLyric(s->lyric);

                /* find MIDI pitch */
                unsigned char n = note2midi(curks ? curks : ks, s->text, measure_accid);

                if (s->value) {
                        writeMidiEvent(delta_tick, noteon, track, n, cur_dyn * s->value);
                } else {
                    long small = tpu * upm / 8;
                    small = (delta_tick > small) ? small : delta_tick;
                    writeMidiEvent(delta_tick - (small / shorten), noteon, track, n, 0x00); /* note off */
                    last_tick -= (small / shorten);
                    shorten = in_slur;
                }
#if 0
                long dur = tpu * s->dur_num / s->dur_den;
                if (in_grace) {
                        writeMidiEvent(dur, noteon, track, n, 0x00); /* note off */
                        last_tick += dur;
                } else {
                        long small = tpu * upm / 8;
                        small = (dur > small) ? small : dur;
                        writeMidiEvent(dur - (small / shorten) /* - grace_tick */, noteon, track, n, 0x00); /* note off */
                        last_tick += dur;
                        grace_tick = 0;
                }
                shorten = in_slur;
#endif
        }
}

void AbcSmf::onSMFWriteTempoTrack(void) {
    qWarning() << __func__;
}

void AbcSmf::onSMFWriteTrack(int track) {
        qWarning() << __func__ << track;
        long mspqn = 60000 / tempo;
        writeTempo(0, mspqn);
        writeBpmTempo(0, tempo);
        writeKeySignature(0, mks, mode);

        struct abc_voice* f = abc_unfold_voice(t->voices[track]);
        struct abc_voice* u = abc_untie_voice(f, t);
        abc_release_voice(f);
        struct abc_voice* v = abc_eventy_voice(u);
        abc_release_voice(u);

        struct abc_symbol* s = v->first;

        last_tick = 0;

        in_grace = 0; /* inside a grace */
        in_cresc = 0;
        mark_dyn = DYN_DEFAULT;
        cur_dyn = mark_dyn; /* current dynamic in the tune */
        grace_tick = 0; /* grace group duration */

        grace_mod = 1.0; /* duration modified for graces */
        expr = EXPRESSION_DEFAULT;
        in_slur = SHORTEN_DEFAULT;
        shorten = in_slur; /* dur will be shortened of 10% of a unit */

        writeMetaEvent(0, 0x03, QString(v->v)); /* textual voice name */

        while (s) {
                switch (s->kind) {
                case ABC_NUP:
                case ABC_CHORD: /* eventing is already done */
                case ABC_TIE: /* untying has been already done */
                case ABC_ALT: /* unfolding is already done */
                case ABC_GCHORD:
                case ABC_EOL:
                case ABC_SPACE: {
                        break;
                }
                case ABC_CHANGE: {
                    if (s->text[0] == 'K')
                            curks = &s->text[2]; /* remove "K:" */
                    break;
                }
                case ABC_NOTE: {
                        writeSingleNote(track, s);
                        break;
                }
                case ABC_GRACE: {
#if 0
                        in_grace = strchr(s->text, '{') ? 1 : 0;
                        if (in_grace) {
                                double gd = abc_grace_duration(s); /* in units */
                                grace_mod = den / (8.0 * gd * num); /* play in time of 1/8 */
                                grace_tick = tpu * upm / 8; /* a 1/8 in ticks */
                        }
#endif
                        break;
                }
                case ABC_DECO: {
                        manageDecoration(s);
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
                        break;
                }
                case ABC_INST: {
                        int prog;
                        if (sscanf(s->text, "MIDI program %d", &prog)) {
                                writeMidiEvent(0, program, track, prog);
                        }
                        break;
                }
                } /* EO case */
                s = s->next;
        }
        writeMetaEvent(0, 0x2F);
        abc_release_voice(v);
	curks = nullptr;
}

/* text must be %d/%d */
void AbcSmf::getNumDen(const char* text, long* num, long* den) {
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
                writeMidiEvent(0, control, track, 0x0b, expr);
                expr = EXPRESSION_DEFAULT;
        }
}

void AbcSmf::writeLyric(const char* lyric) {
        if (lyric) {
                writeMetaEvent(0, 0x05, QString(lyric));
        }
}
