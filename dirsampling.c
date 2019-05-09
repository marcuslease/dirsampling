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

// global class pointer
void *random_samples_class;

typedef struct random_samples
{
	t_object obj;
	t_atomarray *samples;
	void *outlet_1;
} random_samples;

// function prototypes
void *random_samples_new();
void random_samples_free();
void random_samples_bang();
void random_samples_in(random_samples *, t_symbol *);
bool random_samples_dir_accessible(const char *);
void random_samples_dir_walk(const char *, random_samples *);

void ext_main(void *r)
{
	t_class *c;
	c = class_new(
		"random_samples",
		(method)random_samples_new,
		(method)random_samples_free,
		(long)sizeof(random_samples),
		0L,
		A_GIMME,
		0
	);
	class_addmethod(c, (method)random_samples_bang, "bang", 0);
	class_addmethod(c, (method)random_samples_in, "in", A_SYM, 0);
	class_register(CLASS_BOX, c);
	random_samples_class = c;
}

void *random_samples_new()
{
	random_samples *x = (random_samples *)object_alloc(random_samples_class);
	x->samples = atomarray_new(0, NULL);
	x->outlet_1 = listout((t_object *)x);
	return x;
}

void random_samples_free()
{
	;
}

void random_samples_bang(random_samples *x)
{
	t_atom argv[2];
	atom_setlong(argv, 4);
	atom_setsym(
		argv + 1,
		gensym("C:/Users/mdl1550/root/media/samples/vocabulary/dk/for/for-001.wav")
	);
	outlet_list(x->outlet_1, NULL, 2, argv);
	atom_setlong(argv, 5);
	atom_setsym(
		argv + 1,
		gensym("C:/Users/mdl1550/root/media/samples/vocabulary/dk/tid/tid-001.wav")
	);
	outlet_list(x->outlet_1, NULL, 2, argv);
}

void random_samples_in(random_samples *x, t_symbol *s)
{
	char *root = s->s_name;
	if (!random_samples_dir_accessible(root)) {
		object_error(&x->obj,
		             "root for directory walk is not accessible\n");
		return;
	}
	// TODO: do i need to free elements of array?
	atomarray_clear(x->samples);
	random_samples_dir_walk(root, x);
}

bool random_samples_dir_accessible(const char *dir)
{
	DIR *d = opendir(dir);
	closedir(d);
	return (d) ? true : false;
}

void random_samples_dir_walk(const char *dir, random_samples *x)
{
	if (!random_samples_dir_accessible(dir)) {
		object_error(&x->obj,
		             "dir for directory walk is not accessible\n");
		return;
	}

	struct dirent *entry;
	struct _stat *d_stat;
	DIR *d = opendir(dir);
	while (1) {
		entry = readdir(d);
		if (!entry) break;

		char *foo = "\\Users";
		int err = _stat(foo, d_stat);
		if (!err) {
			if (d_stat->st_mode & _S_IFDIR) {
				/*
				if (strcmp(entry->d_name, "..") != 0 &&
			    	    strcmp(entry->d_name, ".") != 0) {
				//	random_samples_dir_walk(entry->d_name, x);
					;
				}
				*/
			} else if (d_stat->st_mode & _S_IFREG) {
				/*
				t_atom atom[1];
				atom_setsym(atom, gensym(entry->d_name));
				atomarray_appendatom(x->samples, atom);
				post("%s", entry->d_name);
		*/
			}
		} else {
			post("err = %d", err);
		}
	}
	if (closedir(d)) {
		object_error(&x->obj,
		             "could not close dir during directory walk");
	}
}
