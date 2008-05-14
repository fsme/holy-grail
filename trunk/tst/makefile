# @(#) Make unit test
# Author: Andrew Wingorodov <http://andr.mobi/>
# Licence: GPLv3
# $Id$

LIBCXX= ../../../libcxx/trunk/src
SRCDIR= ../src

CXXFLAGS+=\
	-I.\
	-I/usr/local/include\
	-I/usr/local/include/libxml2\
	-I$(SRCDIR)\
	-I$(LIBCXX)/logostream\
	-I$(LIBCXX)/except\
	-I$(LIBCXX)/env\
	-I$(LIBCXX)/timeter\
	-L/usr/local/lib\

LDLIBS+= -lcurl -lxml2

RATESTREAM_SRC= ratestream_test.cc
RATESTREAM_HDR=\
	$(SRCDIR)/ratestream.hpp

all: ratestream_test summ_unit_test

#
ratestream_test: $(RATESTREAM_SRC) $(RATESTREAM_HDR)
	$(CXX) $(CXXFLAGS) $(RATESTREAM_SRC) -o $@

SUMMUNIT_SRC= summ_unit_test.cc
SUMMUNIT_HDRS=\
	$(LIBCXX)/logostream/logostream.hpp\
	$(SRCDIR)/average.hpp
SUMMUNIT_OBJS=\
	$(LIBCXX)/logostream/logostream.o\
#
summ_unit_test: $(SUMMUNIT_SRC) $(SUMMUNIT_OBJS) $(SUMMUNIT_HDRS)
	$(CXX) $(CXXFLAGS) $(SUMMUNIT_SRC) $(SUMMUNIT_OBJS) -o $@

#
$(LIBCXX)/logostream/logostream.o:
	cd $(LIBCXX)/logostream && make logostream.o

OBJS=\
	$(SUMMUNIT_OBJS)\

clean clear:
	rm -rfv ratestream_test summ_unit_test $(OBJS) *.o *.core

