# 
# Copyright(C) 2011-2014 Pedro H. Penna <pedrohenriquepenna@gmail.com> 
#
# This file is part of Nanvix.
#
# Nanvix is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Nanvix is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Nanvix.  If not, see <http://www.gnu.org/licenses/>.
#


# Source files.
SRC = $(wildcard *.c)          \
      $(wildcard ctype/*.c)    \
      $(wildcard dirent/*.c)   \
      $(wildcard errno/*.c)    \
      $(wildcard fcntl/*.c)    \
      $(wildcard signal/*.c)   \
      $(wildcard stdio/*.c)    \
      $(wildcard stdlib/*.c)   \
      $(wildcard string/*.c)   \
      $(wildcard stropts/*.c)  \
      $(wildcard sys/stat/*.c) \
      $(wildcard sys/wait/*.c) \
      $(wildcard termios/*.c)  \
      $(wildcard unistd/*.c)   \

# Object files.
OBJ = $(SRC:.c=.o)

# Library name.
LIB = libc.a

# Builds the C library.
all: $(OBJ)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(LIB) $^

# Builds object file from C source file.
%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

# Cleans compilation files.
clean:
	@rm -f $(LIBDIR)/$(LIB)
	@rm -f $(OBJ)