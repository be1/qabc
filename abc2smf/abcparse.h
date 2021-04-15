#ifndef _LIBABCPARSE_H
#define _LIBABCPARSE_H

#include "abc.h"

#ifdef __cplusplus
extern "C" {
#endif

struct abc* abc2smf_abc_parse(const char* buffer, int size);

void abc2smf_abc_release(struct abc* yy);

struct tune* find_tune(struct abc* yy, int x);
struct header* find_header(struct tune* t, char h);
struct voice* find_voice(struct tune* t, int v);
struct symbol* find_start_repeat(struct symbol* s);
struct symbol* find_next_segno(struct symbol* s);
struct symbol* find_next_alt(struct symbol* s, int alt);
struct symbol* find_next_repeat(struct symbol* s);

int unit_per_measure(struct tune* t);
double second_per_unit(struct tune* t);

#ifdef __cplusplus
}
#endif

#endif
