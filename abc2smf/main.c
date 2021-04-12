#include "abcparse.h"
#include "abcsmf.h"
#include <smf.h>

int main() {
	int c;
	char* buf = NULL;
	int count = 0;

	/* read stdin */
	while (EOF != (c = getc(stdin))) {
		count++;
		buf = realloc(buf, count);
		buf[count -1] = c;
	}
	
	struct abc* yy = abc2smf_abc_parse(buf, count);
	if (yy->error)
		puts(yy->error_string);

	printf("tunes: %d\n", yy->count);
	for (int i = 0; i < yy->count; i++) {
		struct tune* t = yy->tunes[i];
		printf("tune %d\n", t->x);

		struct header* h = t->headers;
		if (h)
			while (h && h->h != 'T')
				h = h->next;

		printf("title: %s\n", h? h->text: "");
		for (int j = 0; j < t->count; j++) {
			struct voice* v = t->voices[j];
			printf("tune %d, voice %d\n", t->x, v->v);
			struct symbol* s;
			for (s = v->first; s; s = s->next) {
				printf("%d %d/%d ", s->kind, s->dur_num, s->dur_den);
				if (s->text) puts(s->text);
				if (s->lyric) puts(s->lyric);
			}
		}
	}

	if (yy->error)
		goto end;

	smf_t* smf = abc2smf(yy, 1);
	
	if(smf_save(smf, "out1.mid"))
		puts("error");

	smf_delete(smf);

end:
	abc2smf_abc_release(yy);
	free(buf);
    return 0;
}
