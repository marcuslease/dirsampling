#define MAXAPI_USE_MSCRT
#include "ext.h"
#include "ext_obex.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <errno.h>

const int DEFAULT_NUM_SAMPLES = 99;

/*
 * class pointer
 */
void *dirsampling_class;

/*
 * instance struct
 */
typedef struct dirsampling
{
	t_object obj;
	void *outlet1;
} dirsampling;

void *dirsampling_new();
void dirsampling_free();
void dirsampling_bang(dirsampling *);
void dirsampling_dir(dirsampling *, t_symbol *);
int dirsampling_nftw_callback(const char *,
                              const struct stat *,
                              int,
                              struct FTW *);
bool dirsampling_is_dir_accessible(const char *);
bool dirsampling_is_file_audio(const char *);
void dirsampling_send_hardcoded_samples(dirsampling *);

/*
 * buffer to hold results of a dir walk
 */
static t_atomarray *dirsampling_buffer;

/*
 * define class
 */
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
	class_addmethod(c, (method)dirsampling_dir, "dir", A_SYM, 0);
	class_register(CLASS_BOX, c);
	dirsampling_class = c;
}

/*
 * initialize instance
 */
void *dirsampling_new()
{
	dirsampling *x = (dirsampling *)object_alloc(dirsampling_class);
	x->outlet1 = listout((t_object *)x);
	return x;
}

void dirsampling_free()
{
	;
}

/*
 * fills buffer with all audio files in dir, then returns a random sampling
 */
void dirsampling_dir(dirsampling *x, t_symbol *dir)
{
	dirsampling_buffer = atomarray_new(0, NULL);

	int err = nftw(dir->s_name, dirsampling_nftw_callback, 4, 0);
	if (err != 0) perror("nftw() error");

	for (int i = 0; i < DEFAULT_NUM_SAMPLES; i++) {
		int min = 0, max = DEFAULT_NUM_SAMPLES - 1;
		int r = min + rand() / (RAND_MAX / (max - min + 1) + 1);

		t_atom *a;
		atomarray_getindex(dirsampling_buffer, r, a);

		t_atom argv[2];
		atom_setlong(argv, i + 1);
		atom_setsym(argv + 1, atom_getsym(a));
		outlet_list(x->outlet1, NULL, 2, argv);
	}

	atomarray_clear(dirsampling_buffer);
}

/*
 * adds current file to buffer
 */ 
int dirsampling_nftw_callback(const char *file,
                              const struct stat *st,
                              int i,
                              struct FTW *ftw)
{
	if (i != FTW_F || !dirsampling_is_file_audio(file)) return 0;

	t_atom *a;
	atom_setsym(a, gensym(file));
	atomarray_appendatom(dirsampling_buffer, a);

	return 0;
}

bool dirsampling_is_dir_accessible(const char *dir)
{
	DIR *d = opendir(dir);
	closedir(d);
	return (d) ? true : false;
}


/*
 * determined by file extension
 */ 
bool dirsampling_is_file_audio(const char *file)
{
	const int max = 4;
	int len = strlen(file);

	char ext[max];
	for (int i = 0; *(file + len - max + i) != '\0' && i < max; i++)
		ext[i] = *(file + len - max + i);
	
	if (strcmp(ext, ".wav") == 0 || strcmp(ext, ".mp3") == 0)
		return true;
	else
		return false;
}
