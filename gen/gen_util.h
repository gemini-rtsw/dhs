
#ifndef _GEN_UTIL_H
#define _GEN_UTIL_H

/* Replacement for tempnam(3), which gcc complains about. (XXX allan) */
char* gen_tempnam(const char *dir, const char *pfx);

#endif /* _GEN_UTIL_H */
