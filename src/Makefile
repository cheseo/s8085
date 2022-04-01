# Copyright (C) 2022 by boink
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

CC = gcc -ggdb3 -Wall -Wextra -Wpedantic -Wshadow -O0 -std=gnu99 -D_GNU_SOURCE
LDLIBS = -lm 			# apparantly this is how it should be done
OBJ1 = main.o instructions.o glue.o
s8085: $(OBJ1)
	$(CC) -o $@  $(LDFLAGS) $(OBJ1) $(LDLIBS)
	etags *.c *.h
OBJ2 = main.c
main.o: $(OBJ2)
	$(CC) -c $(OBJ2)

instructions.o: instructions.c instructions.h
	$(CC) -c instructions.c

glue.o: glue.c glue.h
	$(CC) -c glue.c
