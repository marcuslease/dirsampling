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
#include <ftw.h>
#include <errno.h>

const int DEFAULT_NUM_SAMPLES = 99;

// class pointer
void *dirsampling_class;

// instance struct
typedef struct dirsampling
{
	t_object obj;
	void *outlet1;
} dirsampling;

// hash table of the results of dir walking
t_hashtab *dirsampling_dir_hash;

void *dirsampling_new();
void dirsampling_free();
void dirsampling_bang(dirsampling *);
void dirsampling_dir(dirsampling *, t_symbol *);
int dirsampling_nftw_callback(const char *,
                              const struct stat *,
                              int,
                              struct FTW *);
void dirsampling_send_hardcoded_samples(dirsampling *);
bool dirsampling_dir_accessible(const char *);

void ext_main(void *r)
{
	// define class
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
	class_addmethod(c, (method)dirsampling_dir, "dir", A_SYM, 0);
	class_register(CLASS_BOX, c);
	dirsampling_class = c;

	// initialize hash table
	dirsampling_dir_hash = (t_hashtab *)hashtab_new(0);
}

// initialize instance
void *dirsampling_new()
{
	dirsampling *x = (dirsampling *)object_alloc(dirsampling_class);
	//x->outlet1 = outlet_new((t_object *)x, NULL);
	x->outlet1 = listout((t_object *)x);
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

int dirsampling_counter;
t_symbol *dirsampling_current_dir;

void dirsampling_dir(dirsampling *x, t_symbol *dir)
{
	dirsampling_current_dir = dir;

	post("dir: %s", dir->s_name);
	// TODO check if entry already exists
	t_atomarray *arr = atomarray_new(0, NULL);
	hashtab_store(
		dirsampling_dir_hash,
		dirsampling_current_dir,
		(t_object *)arr
	);
	// TODO do we need a counter?
	dirsampling_counter = 0;

	int err = nftw(
		dir->s_name,
		dirsampling_nftw_callback,
		4,
		0
	);

	post("err: %d", err);
	post("counter: %d", dirsampling_counter);
	//dirsampling_send_hardcoded_samples(x);
}

int dirsampling_nftw_callback(const char *file,
                              const struct stat *st,
                              int i,
                              struct FTW *ftw)
{
	if (i != FTW_F) return 0;
	post("counter: %d", dirsampling_counter);
	post("file: %s", file);

	t_atomarray *arr;
	hashtab_lookup(
		dirsampling_dir_hash,
		dirsampling_current_dir,
		(t_object **)arr
	);
	// ??? if this is a pointer, max dies and i idk why
	t_atom val;
	atom_setsym(&val, gensym(file));
	// ??? bombs with this line
//	atomarray_appendatom(arr, &val);
	dirsampling_counter++;

	return 0;
}

void dirsampling_send_hardcoded_samples(dirsampling *x)
{
	char *sample1 =
		"C:/Users/mdl1550/root/media/samples/vocabulary/dk/for/for-001.wav";
	t_atom argv1[2];
	atom_setlong(argv1, 1);
	atom_setsym(argv1 + 1, gensym(sample1));
	outlet_list(x->outlet1, NULL, 2, argv1);

	char *sample2 =
		"C:/Users/mdl1550/root/media/samples/vocabulary/dk/tid/tid-001.wav";
	t_atom argv2[2];
	atom_setlong(argv2, 2);
	atom_setsym(argv2 + 1, gensym(sample2));
	outlet_list(x->outlet1, NULL, 2, argv2);
}

bool dirsampling_dir_accessible(const char *dir)
{
	DIR *d = opendir(dir);
	closedir(d);
	return (d) ? true : false;
}
