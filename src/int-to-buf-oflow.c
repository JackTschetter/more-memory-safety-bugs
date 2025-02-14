#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

struct obj {
    char type;
    short ident;
    unsigned int canary;
    int x, y, z;
};

struct obj *read_objs(unsigned num_objs) {
    long i;
    unsigned int size = num_objs * (unsigned int)sizeof(struct obj);
    printf("Size (after mult.) is %u (0x%08x)\n", size, size);
    struct obj *objs = malloc(size + sizeof(struct obj));
    if (!objs) {
	fprintf(stderr, "Allocation failed\n");
	return 0;
    }
    struct obj *p = objs;
    struct obj *last_obj = (struct obj *)((char *)p + size);
    last_obj->canary = 0xcafebebe;
    for (i = 0; i < num_objs; i++) {
	int res;
	p->canary = 0;
	res = fread(p, sizeof(struct obj), 1, stdin);
	if (res != 1) {
	    fprintf(stderr, "Stopping after read failure\n");
	    exit(1);
	}
	if (p->ident == 0x3234) {
	    break;
	}
	/* ... */
	p++;
    }
    printf("Read %ld objects\n", i);
    if (last_obj->canary != 0xcafebebe) {
	printf("Last object canary corrupted to 0x%08x\n",
	       last_obj->canary);
    }
    return objs;
}

int main(int argc, char **argv) {
    unsigned num_objs;
    long result;
    if (argc != 2) {
	fprintf(stderr, "Usage: overflow-eg <num objs>\n");
	exit(1);
    }
    errno = 0;
    result = strtoul(argv[1], 0, 0);
    num_objs = result;
    printf("\"%s\" read as %lu (0x%016lx) converted to %u (0x%08x) %s\n",
	   argv[1], result, result, num_objs, num_objs,
	   (errno == ERANGE ? "out of range" : errno == 0 ? "success"
	    : "other error"));
    read_objs(num_objs);
    return 0;
}
