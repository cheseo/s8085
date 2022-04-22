/* Copyright (C) 2022 by boink */
/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <errno.h>		/* for program_invocation_short_name, glibc extension. */
#include <stdlib.h> 		/* for EXIT_SUCCESS and rand() */
#include <time.h>
#include <ctype.h> 		/* isalnum */
#include "glue.h"
#define PROGNAME "s8085"
void usage(int exit_code){
	FILE *fp = exit_code==EXIT_SUCCESS ? stdout : stderr; 
	fprintf( fp, "Synopsis:\n"
		 	"	%s 8085 instruction\n", PROGNAME);
	exit(exit_code);
}
int create_mem_file(FILE *fp){
	if(!fp)
		return 0;
	int len = 0xffff ; /* length of file. */
	char r[2];
	while(len){
		r[0] = rand();
		r[1] = rand();
		if(!(isxdigit(r[0]) && isxdigit(r[1])))
			continue;
		if(3 != fprintf(fp, "%c%c\n", r[0], r[1]))
			return 0;
		len--;
	}
	fflush(fp);
	return 1;
}
int main(int argc, char *argv[]){
	(void)argv;
	(void)argc;
	srand(time(NULL));
	const char *mem_file = "memory";
	FILE *mem = fopen(mem_file, "r+");
	if(!mem){
		fprintf(stderr, "creating `memory' file...");
		mem = fopen(mem_file, "w+");
		if(!( mem  && create_mem_file(mem)))
			perror("\nmemory file:"), exit(EXIT_FAILURE);
		fprintf(stderr, "done\n");
	}
	mem_fp = mem;
	struct instruction i;
	struct registers r;
	while(set_ins(&i)){
		if(!halt_exec){
			call_ins(&i, &r);
		}
		print_reg(&r);
	}
	fclose(mem);
	return 0;
}
