# 
# Copyright(C) 2011-2017 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
#              2016-2017 Davidson Francis <davidsondfgl@gmail.com>
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

#Newlib defines
export CFLAGS += -DSIGNAL_PROVIDED
export CFLAGS += -DMISSING_SYSCALL_NAMES
export CFLAGS += -U__STRICT_ANSI__

# C source files.
C_SRC = $(wildcard common/*.c) \
	$(wildcard complex/*.c)    \
	$(wildcard math/*.c)       \

# Object files.
OBJ = $(C_SRC:.c=.o)

# Library name.
LIB = libm.a

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
