// fmt.h - formatare numerice FARA stdio (stdio consuma mult flash pe AVR)
#ifndef __FMT_H__
#define __FMT_H__

// Scrie v in dst, zero-pad la width caractere (ca sprintf "%03d"),
// cu terminator NUL. Returneaza numarul de caractere scrise (fara NUL).
// Preia valori -999..999 pentru width=3 (semnul urca pe prima pozitie,
// ca la sprintf).
unsigned fmt_num(char *dst, int v, unsigned width);

// Echivalent scurt pentru cele 3 cazuri folosite in cod
#define fmt3(d,v) fmt_num((d),(v),3)
#define fmt4(d,v) fmt_num((d),(v),4)

#endif
