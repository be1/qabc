#ifndef ABCSMF_H
#define ABCSMF_H

#include <QObject>
#include <drumstick/qsmf.h>
#include "../abc/abc.h"

using namespace drumstick::File;

class AbcSmf : public QSmf
{
    Q_OBJECT
public:
    explicit AbcSmf(struct abc* yy, int x = 1, QObject *parent = nullptr);

private slots:
    void onSMFWriteTempoTrack(void);
    void onSMFWriteTrack(int track);

private:
#define DPQN 960
#define DYN_DEFAULT 80
#define SHORTEN_DEFAULT 10
#define EXPRESSION_DEFAULT 0

    void manageDecoration(struct abc_symbol* s);
    void writeSingleNote(int track, struct abc_symbol* s);
    void getNumDen(const char* text, long* num, long* den);
    int getSMFKeySignature(const char* text, int* mode);
    unsigned char pitch_diff_0x3c(const char* ks, int note);
    unsigned char note2midi (const char* keysig, const char* note, int* measure_accid);
    void setDynamic(long dur);
    void writeExpression(int track);
    void writeLyric(const char* lyric);
    void feedPitchDiff(void);

    struct abc* yy; /* takes ownership of yy */
    int x;
    struct abc_tune* t;     /* tune selected by X */
    char* ks;               /* tune's kh->text */
    char* curks;            /* voice's kh->text change */
    int mks;                /* MIDI smf key signature */
    int mode;               /* MIDI smf mode (maj/min) */

    long tpu;      /* ticks per unit */
    long upm;      /* units per measure */
    long tempo;    /* quarter per minute */

    int expr;      /* expression */
    long last_tick;
    long dur;      /* note duration */
    int in_slur;
    int shorten;
    double grace_mod; /* duration modified for grace notes */
    int in_cresc;
    unsigned char mark_dyn;
    unsigned char cur_dyn;
    long grace_tick; /* ticks elapsed by a grace group */
    char noteon;
    char program;
    char control;
    int transpose;
    long num; /* tune L numerator */
    long den; /* tune L denominator */

    /* measure accidentals context */
    int measure_accid['h']; /* 'g' + 1 */

    signed char _pitch_diff_CMaj_0x3c['h'] = {0};
    signed char _pitch_diff_GMaj_0x3c['h'] = {0};
    signed char _pitch_diff_DMaj_0x3c['h'] = {0};
    signed char _pitch_diff_AMaj_0x3c['h'] = {0};
    signed char _pitch_diff_EMaj_0x3c['h'] = {0};
    signed char _pitch_diff_BMaj_0x3c['h'] = {0};
    signed char _pitch_diff_FSharpMaj_0x3c['h'] = {0};
    signed char _pitch_diff_GFlatMaj_0x3c['h'] = {0};
    signed char _pitch_diff_CSharpMaj_0x3c['h'] = {0};
    signed char _pitch_diff_DFlatMaj_0x3c['h'] = {0};
    signed char _pitch_diff_AFlatMaj_0x3c['h'] = {0};
    signed char _pitch_diff_EFlatMaj_0x3c['h'] = {0};
    signed char _pitch_diff_BFlatMaj_0x3c['h'] = {0};
    signed char _pitch_diff_FMaj_0x3c['h'] = {0};
};

#endif // ABCSMF_H
