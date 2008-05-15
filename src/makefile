# @(#) Build the robotrade project
# Author: Andrew Wingorodov <http://andr.mobi/>
# Licence: GPLv3
# $Id: makefile 31 2008-05-14 18:39:24Z wingorodov $

.PHONY: all show clean clear

LIBCXX= ../../../libcxx/trunk/src

CXXFLAGS+=\
	-I.\
	-I/usr/local/include\
	-I/usr/local/include/libxml2\
	-I$(LIBCXX)/logostream\
	-I$(LIBCXX)/except\
	-I$(LIBCXX)/env\
	-I$(LIBCXX)/timeter\
	-L/usr/local/lib\

LDLIBS+= -lcurl -lxml2 -lboost_thread

ALL= rtd trade baklog

all: trade

#
$(LIBCXX)/logostream/logostream.o:
	cd $(LIBCXX)/logostream && make logostream.o
#
$(LIBCXX)/env/iron.o:
	cd $(LIBCXX)/env && make iron.o

#---------
ORDER_SRC=\
	order.h\
	order.cc\
	factor.h\
	request.h\
	delta_rate.h\
	$(LIBCXX)/logostream/logostream.hpp\
#
order.o: $(ORDER_SRC)
	$(CXX) $(CXXFLAGS) -c order.cc

#--------------
RATE_STATS_SRC=\
	factor.h\
	summ_unit.h\
	delta_rate.h\
	rate_stats.h\
	rate_stats.cc\
	$(LIBCXX)/logostream/logostream.hpp\
#
rate_stats.o: $(RATE_STATS_SRC)
	$(CXX) $(CXXFLAGS) -c rate_stats.cc

#----------
FACTOR_SRC=\
	factor.h\
	factor.cc\
#
factor.o: $(FACTOR_SRC)
	$(CXX) $(CXXFLAGS) -c factor.cc

#-------------
SUMM_UNIT_SRC=\
	order.h\
	average.hpp\
	summ_unit.h\
	summ_unit.cc\
	$(LIBCXX)/logostream/logostream.hpp\
#
summ_unit.o: $(SUMM_UNIT_SRC)
	$(CXX) $(CXXFLAGS) -c summ_unit.cc

#------------
MEMCHUNK_SRC=\
	memchunk.h\
	memchunk.cc\
	$(LIBCXX)/env/iron.h\
#
memchunk.o: $(MEMCHUNK_SRC)
	$(CXX) $(CXXFLAGS) -c memchunk.cc

#------------
REQUEST_SRC=\
	request.h\
	request.cc\
	url_easy.hpp\
	memparser.hpp\
	synchron.h\
#
request.o: $(REQUEST_SRC)
	$(CXX) $(CXXFLAGS) -c request.cc

#------------
SYNCHRON_SRC=\
	synchron.h\
	synchron.cc\
	$(LIBCXX)/logostream/logostream.hpp\
#
synchron.o: $(SYNCHRON_SRC)
	$(CXX) $(CXXFLAGS) -c synchron.cc

#---------
RATED_SRC=\
	rtd.cc\
	url_easy.hpp\
	memparser.hpp\
	$(LIBCXX)/env/iron.h\
	$(LIBCXX)/timeter/timeter.h\
	$(LIBCXX)/logostream/logostream.hpp\
#
rtd.o: $(RATED_SRC)
	$(CXX) $(CXXFLAGS) -c rtd.cc

#---------
RATED_OBJ=\
	rtd.o\
	memchunk.o\
	$(LIBCXX)/env/iron.o\
	$(LIBCXX)/logostream/logostream.o\
#
rtd: $(RATED_OBJ)
	$(CXX) $(CXXFLAGS) $(RATED_OBJ) -o $@ $(LDLIBS)

##--------
TRADE_SRC=\
	order.h\
	factor.h\
	synchron.h\
	request.h\
	memchunk.h\
	summ_unit.h\
	url_easy.hpp\
	rate_stats.h\
	ratestream.hpp\
	$(LIBCXX)/env/iron.h\
	$(LIBCXX)/except/er.h\
	$(LIBCXX)/timeter/timeter.h\
	$(LIBCXX)/logostream/logostream.hpp\
#
trade.o: $(TRADE_SRC)
	$(CXX) $(CXXFLAGS) -c trade.cc

##--------
TRADE_OBJ=\
	trade.o\
	order.o\
	rate_stats.o\
	factor.o\
	summ_unit.o\
	memchunk.o\
	synchron.o\
	request.o\
	$(LIBCXX)/logostream/logostream.o\
	$(LIBCXX)/env/iron.o\
#
trade: $(TRADE_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(TRADE_OBJ) $(LDLIBS)


##---------
BAKLOG_SRC=\
	 baklog.cc
BAKLOG_HDR=\
	 $(LIBCXX)/logostream/logostream.hpp\
	 $(LIBCXX)/env/iron.h\

BAKLOG_OBJ=\
	 $(LIBCXX)/logostream/logostream.o
#
baklog: $(BAKLOG_SRC) $(BAKLOG_HDR) $(BAKLOG_OBJ)
	$(CXX) $(CXXFLAGS) baklog.cc $(TRADE_OBJS) -o $@ $(LDLIBS)

#----
OBJS=\
	$(RATED_OBJ)\
	$(TRADE_OBJ)\
	$(BAKLOG_OBJ)\
#
clean clear:
	rm -rfv $(ALL) $(OBJS) *.core

#
res:
	@echo "`svn diff | grep + | wc -l` lines was added"
	@echo "`svn diff | grep - | wc -l` lines was removed"
