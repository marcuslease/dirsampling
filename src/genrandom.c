#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>

union bytes2long {
	byte array[4];
	unsigned long integer;
};

int main(int argc, char *argv[])
{
	/* Declare variables */
	HCRYPTPROV hCryptProv;
	byte result[4];

	/* Display a help message */
	if (argc == 2 && (!strcmp(argv[1], "/?") || !strcmp(argv[1], "-?") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
	{
		printf("Generates and displays a random hex string 4 characters long\n");
		printf("\n");
		printf("GENRANDOM\n");
		exit(1);
	}

	if (CryptAcquireContext(
		&hCryptProv,
		NULL,
		"Microsoft Base Cryptographic Provider v1.0",
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		if (CryptGenRandom(
			hCryptProv,
			4,
			result))
		{
			printf("Random sequence generated: %02X%02X%02X%02X\n",
				result[0], result[1], result[2], result[3]
			);
			union bytes2long converter = {result[0], result[1], result[2], result[3]};
			printf("%ld", converter.integer % (99 + 1 - 0) + 0);
			printf("%ld", sizeof(long));
			if (CryptReleaseContext(hCryptProv, 0))
			{
				return 0;
			}
			else
			{
				printf("Error during CryptReleaseContext.\n");
				return 4;
			}
		}
		else
		{
			if (CryptReleaseContext(hCryptProv, 0))
			{
				printf("Error during CryptGenRandom.\n");
				return 2;
			}
			else
			{
				printf("Error during CryptReleaseContext.\n");
				return 3;
			}
		}
	}
	else
	{
		printf("Error during CryptAcquireContext!\n");
		return 1;
	}
}
