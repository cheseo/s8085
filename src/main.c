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
#include <stdlib.h> 		/* for EXIT_SUCCESS */
#include "glue.h"

void usage(int exit_code){
	FILE *fp = exit_code==EXIT_SUCCESS ? stdout : stderr; 
	fprintf( fp, "Synopsis:\n"
		 	"	%s 8085 instruction\n", program_invocation_short_name);
	exit(exit_code);
}

int main(int argc, char *argv[]){
	(void)argv;
	(void)argc;
	struct instruction i;
	struct registers r;
	initialize_instruction(&i);
	
	char opcode[5], op1[5], op2[5];
	char line[20];
	while( fgets(line, sizeof(line), stdin) ){
		sscanf(line,"%s %s %s", opcode, op1, op2);
		printf("opcode: %s, op1: %s, op2: %s\n",opcode, op1, op2);
		i.opcode = opcode;
		i.operand1 = op1;
		i.operand2 = op2;
		sanitize_ins(&i);
		call_ins(&i, &r);
		print_registers(&r);
	}
	
	return 0;
}
