#define MAXAPI_USE_MSCRT
#include "ext.h"
#include "ext_obex.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

const int DEFAULT_NUM_SAMPLES = 99;

// class pointer
void *dirsampling_class;

// instance struct
typedef struct dirsampling
{
	t_object obj;
	t_atomarray *samples;
	void *outlet_1;
} dirsampling;

void *dirsampling_new();
void dirsampling_free();
void dirsampling_bang();
bool dirsampling_dir_accessible(const char *);

// define class
void ext_main(void *r)
{
	t_class *c;
	c = class_new(
		"dirsampling",
		(method)dirsampling_new,
		(method)dirsampling_free,
		(long)sizeof(dirsampling),
		0L,
		A_GIMME,
		0
	);
	class_addmethod(c, (method)dirsampling_bang, "bang", 0);
	class_register(CLASS_BOX, c);
	dirsampling_class = c;
}

// initialize instance
void *dirsampling_new()
{
	dirsampling *x = (dirsampling *)object_alloc(dirsampling_class);
	x->samples = atomarray_new(0, NULL);
	x->outlet_1 = listout((t_object *)x);
	return x;
}

void dirsampling_free()
{
	;
}

void dirsampling_bang(dirsampling *x)
{
	post("in bang");
}

bool dirsampling_dir_accessible(const char *dir)
{
	DIR *d = opendir(dir);
	closedir(d);
	return (d) ? true : false;
}
