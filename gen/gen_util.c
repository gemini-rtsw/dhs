#include <stdio.h>
#include <string.h>

/*
 * Replacement for tempnam(3), which gcc complains about. (XXX allan)
 */
char* gen_tempnam(const char *dir, const char *pfx) {
    char buf[1024];
    int fd;

    strcpy(buf, dir);
    strcat(buf, "/");
    strcat(buf, pfx);
    strcat(buf, "XXXXXX");
    fd = mkstemp(buf);
    if (fd != -1) {
	close(fd);
	unlink(buf);
	return strdup(buf);
    }
    return NULL;
}

