# 
# Copyright(C) 2011-2018 Pedro H. Penna   <pedrohenriquepenna@gmail.com> 
#              2017-2017 Romane Gallier   <romanegallier@gmail.com>
#              2018-2018 Davidson Francis <davidsondfgl@gmail.com>
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

# Toolchain configuration.
export CFLAGS  += -Os
export LDFLAGS += -Wl,-e,_start
export LIBS = $(LIBDIR)/libc.a -lgcc $(LIBDIR)/libm.a

# Resolves conflicts
.PHONY: foobar
.PHONY: init
.PHONY: shutdown
.PHONY: test

# Builds everything.
all: init shutdown test

# Builds foobar.
foobar:
	$(CC) $(CFLAGS) $(LDFLAGS) foobar/*.c -o $(SBINDIR)/foobar $(LIBS)

# Builds init.
init:
	$(CC) $(CFLAGS) $(LDFLAGS) init/*.c -o $(SBINDIR)/init $(LIBS)

# Builds shutdown.
shutdown:
	$(CC) $(CFLAGS) $(LDFLAGS) shutdown/*.c -o $(SBINDIR)/shutdown $(LIBS)

# Builds test.
test:
	$(CC) $(CFLAGS) $(LDFLAGS) test/*.c -o $(SBINDIR)/test $(LIBS)
	
# Cleans compilations files.
clean:
	@rm -f $(SBINDIR)/foobar
	@rm -f $(SBINDIR)/init
	@rm -f $(SBINDIR)/shutdown
	@rm -f $(SBINDIR)/test
