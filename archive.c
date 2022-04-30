#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/**
 * Like mkdir, but creates parent paths as well
 *
 * @return 0, or -1 on error, with errno set
 * @see mkdir(2)
 */
int mkpath(const char *pathname, mode_t mode)
{
	char *tmp = malloc(strlen(pathname) + 1);
	strcpy(tmp, pathname);
	for (char *p = tmp; *p != '\0'; ++p)
	{
		if (*p == '/')
		{
			*p = '\0';
			struct stat st;
			if (stat(tmp, &st))
			{
				if (mkdir(tmp, mode))
				{
					free(tmp);
					return -1;
				}
			}
			else if (!S_ISDIR(st.st_mode))
			{
				free(tmp);
				return -1;
			}
			*p = '/';
		}
	}
	free(tmp);
	return 0;
}

/**
 * Allocates a string containing the CWD
 *
 * @return allocated string
 */
char *
getcwd_a(void)
{
	char *pwd = NULL;
	for (size_t sz = 128;; sz *= 2)
	{
		pwd = realloc(pwd, sz);
		if (getcwd(pwd, sz)) break;
		if (errno != ERANGE) err(errno, "getcwd()");
	}
	return pwd;
}


/** 
 * Packs a single file or directory recursively
 *
 * @param fn The filename to pack
 * @param outfp The file to write encoded output to
 */
void pack(char * const fn, FILE *outfp)
{
	//data

	char buffer[200];
	int f_index;
	struct stat st;
	DIR *dir;
	struct dirent *dent;
	int len;
	FILE *reg_file_fp;
	int num_bytes;

	// code

	stat(fn, &st);
	if (S_ISREG(st.st_mode))
	{
		fprintf(stderr, "File: [%s]\n", fn);
		f_index = 0;
		len = strlen(fn);
		f_index = sprintf(buffer, "%d", len);
		buffer[f_index++] = ':';
		strcpy(&buffer[f_index], fn);
		f_index+= strlen(fn);
		fwrite(buffer, 1, f_index, outfp);
		reg_file_fp = fopen(fn, "r");
		if (reg_file_fp == NULL)
		{
			printf("Unable to open data file [%s]\n", fn);
			return;
		}
		f_index = sprintf(buffer, "%d", (int)st.st_size);
		buffer[f_index++] = ':';
		fwrite(buffer, 1, f_index, outfp);

		while (1)
		{
			num_bytes = fread(buffer, 1, sizeof(buffer), reg_file_fp);
			if (num_bytes == 0)
				break;
			
			fwrite(buffer, 1, num_bytes, outfp);

		}
	}
	else if (S_ISDIR(st.st_mode))
	{
		// fprintf(stderr, "Recursing `%s'\n", fn);
		printf("directory: [%s]\n", fn);
		f_index = 0;
		len = strlen(fn);
		f_index = sprintf(buffer, "%d", (len+1));
		buffer[f_index++] = ':';
		strcpy(&buffer[f_index], fn);
		f_index+= strlen(fn);
		buffer[f_index++] = '/';
		fwrite(buffer, 1, f_index, outfp);
		dir = opendir(fn);   //this part
		if(dir!=NULL)
		{
			chdir(fn);
			while((dent=readdir(dir))!=NULL)
			{
				if ((strcmp(dent->d_name, ".") == 0 )|| (strcmp(dent->d_name, "..") == 0))
					continue;
				pack(dent->d_name, outfp);
			}
			closedir(dir);
			fwrite("0:", 1, 2, outfp);
			chdir("..");
		}
	}
	else
	{
		fprintf(stderr, "Skipping non-regular file `%s'.\n", fn);
	}
}

/**
 * Unpacks an entire archive
 *
 * @param fp The archive to unpack
 */
int unpack(FILE *fp)
{
	char chr;
	int name_length;
	char *fn;
	int my_index;
	FILE *reg_file_fp;
	int i;
	int ret_code;
	int data_length;

	/* Get file name */
	name_length = 0;
	while (1)
	{
		chr = fgetc(fp);
		if (isdigit(chr) == 0)
			break;
		name_length = ((10*name_length) + (chr-'0'));
	}
	if (name_length == 0)
	{
		chr = fgetc(fp);
		return(1);
	}
	fn = malloc(name_length + 1);
	for (my_index = 0; (my_index < (name_length)); my_index++)
		fn[my_index] = fgetc(fp);
	fn[name_length] = 0;

	if (fn[strlen(fn)-1] == '/') // is a folder:
	{
		if (mkpath(fn, 0700)) err(errno, "mkpath()");
		fprintf(stderr, "Recursing into `%s'\n", fn);
		chdir(fn);
		while (1)
		{
			ret_code = unpack(fp);
			if (ret_code == 1)
				break;
		}
		chdir("..");

	}
	else // is a file:
	{
		fprintf(stderr, "Unpacking file %s\n", fn);
		/* TODO */
		reg_file_fp = fopen(fn, "w");
		if (reg_file_fp == NULL)
		{
			printf("Unable to open data file [%s]\n", fn);
			free(fn);
			return(0);
		}

		data_length = 0;
		while (1)
		{
			chr = fgetc(fp);
			if (isdigit(chr) == 0)
				break;
			data_length = ((10*data_length) + (chr-'0'));
		}

		for (i = 0; i < data_length; i++)
		{
			chr = fgetc(fp);
			fputc(chr, reg_file_fp);
		}
		fclose(reg_file_fp);

	}
	free(fn);
	return (0);
}


int
main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s FILE... OUTFILE\n"
										"       %s INFILE\n", argv[0], argv[0]);
		exit(1);
	}
	char *fn = argv[argc-1];
	if (argc > 2)
	{ /* Packing files */
		FILE *fp = fopen(fn, "w");
		if (fp == NULL){
			printf("Could not open output file");
			exit(1);
		}
		for (int argind = 1; argind < argc - 1; ++argind)
		{
			pack(argv[argind], fp);
		}
		fclose(fp);
	}
	else
	{ /* Unpacking an archive file */
		FILE *fp = fopen(fn, "r");
		unpack(fp);
	}
}
