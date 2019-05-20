#define MAXAPI_USE_MSCRT
#define DIRSAMPLING_RAND_BYTE_LEN 2
#include "ext.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <ftw.h>
#include <windows.h>
#include <Wincrypt.h>

const long DIRSAMPLING_DEFAULT_NUM_SAMPLES = 100;

union dirsampling_bytes2long {
	byte array[DIRSAMPLING_RAND_BYTE_LEN];
	unsigned long integer;
} dirsampling_bytes2long;

/*
 * class pointer
 */
static t_class *dirsampling_class;

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
int dirsampling_rand(long, long, long *);

/*
 * buffer for audio files within a dir
 */
static t_atomarray *dirsampling_buffer;

/*
 * define class
 */
void ext_main(void *r) {
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

	// additional initialization
	dirsampling_buffer = atomarray_new(0, NULL);
}

/*
 * initialize instance
 */
void *dirsampling_new() {
	dirsampling *x = (dirsampling *)object_alloc(dirsampling_class);
	x->outlet1 = listout((t_object *)x);
	return x;
}

void dirsampling_free() {
	;
}

/*
 * fills buffer with all audio files in dir, then returns a random sampling
 */
void dirsampling_dir(dirsampling *x, t_symbol *dir) {
	if (!dirsampling_is_dir_accessible(dir->s_name)) {
		object_error((t_object *)x, "dir not accessible (%s)", dir->s_name);
		return;
	}

	char *func;
	long status;

	/*
 	 * fill buffer with all audio files from dir
 	 */
	status = nftw(dir->s_name, dirsampling_nftw_callback, 4, 0);
	if (status != 0) { strcpy(func, "nftw"); goto error; }

	/*
 	 * send the requested number of random samples
 	 * TODO need better random num generation
 	 */
	long len = atomarray_getsize(dirsampling_buffer);
	int min = 0;
	int max = len - 1;
	for (int i = 0; i < DIRSAMPLING_DEFAULT_NUM_SAMPLES; i++) {
		long random_i;
		status = dirsampling_rand(min, max, &random_i);
		if (status != 0) { strcpy(func, "dirsampling_rand"); goto error; }

		t_atom cur[2];
		status = atom_setlong(cur, i);
		if (status != 0) { strcpy(func, "atom_setlong"); goto error; }
		status = atomarray_getindex(dirsampling_buffer, random_i, cur + 1);
		if (status != 0) { strcpy(func, "atomarray_getindex"); goto error; }

		outlet_list(x->outlet1, NULL, 2, cur);
	}

	atomarray_clear(dirsampling_buffer);
	return;
error:
	object_error(
		(t_object *)x,
		"%s() returned (%ld) in dirsampling_dir()",
		 func, status
	);
}

/*
 * adds current file to buffer
 */ 
int dirsampling_nftw_callback(const char *file,
                              const struct stat *st,
                              int i,
                              struct FTW *ftw) {
	if (i != FTW_F || !dirsampling_is_file_audio(file)) return 0;

	char *func;
	long status;
	t_atom *a_file;

	status = atom_setsym(a_file, gensym(file));
	if (status != 0) { strcpy(func, "atomarray_setsym"); goto error; }
	atomarray_appendatom(dirsampling_buffer, a_file);

	return 0;
error:
	error("%s() returned (%ld) in dirsampling_nftw_callback()", func, status);
}

bool dirsampling_is_dir_accessible(const char *dir) {
	DIR *d = opendir(dir);
	closedir(d);
	return (d) ? true : false;
}

/*
 * determined by file extension
 */ 
bool dirsampling_is_file_audio(const char *file) {
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

int dirsampling_rand(long min, long max, long *integer) {
	int status;
	HCRYPTPROV hCryptProv;
	byte result[DIRSAMPLING_RAND_BYTE_LEN];

	int success = CryptAcquireContext(
		&hCryptProv,
		NULL,
		"Microsoft Base Cryptographic Provider v1.0",
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT
	);
	if (!success) { status = 1; goto error; };
	success = CryptGenRandom(hCryptProv, DIRSAMPLING_RAND_BYTE_LEN, result);
	if (!success) {
		success = CryptReleaseContext(hCryptProv, 0);
		if (success) { status = 2; goto error; }
		else { status = 3; goto error; }
	}
	success = CryptReleaseContext(hCryptProv, 0);
	if (!success) { status = 4; goto error; }

	for (int i = 0; i < DIRSAMPLING_RAND_BYTE_LEN; i++) {
		dirsampling_bytes2long.array[i] = result[i];
	}
	//post("integer %ld", dirsampling_bytes2long.integer);
	//post("rand %ld", (dirsampling_bytes2long.integer % (max - min + 1)) + min);
	*integer = (dirsampling_bytes2long.integer % (max - min + 1)) + min;

	return 0;
error:
	return status;
}
