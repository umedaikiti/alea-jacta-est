LIBSENT=../libsent
LIBJULIUS=../libjulius

CC=g++
CFLAGS=-g -O2 -std=c++11

PKGBIN=../bin

# if you want to use system-installed Julius libraries,
# delete all "$(PKGBIN)/" in the followings.

CPPFLAGS=`$(PKGBIN)/libsent-config --cflags` `$(PKGBIN)/libjulius-config --cflags`
LDFLAGS= `$(PKGBIN)/libjulius-config --libs` `$(PKGBIN)/libsent-config --libs`

####
#### When using system-installed libraries
####
# CPPFLAGS=`libjulius-config --cflags` `libsent-config --cflags`
# LDFLAGS=`libjulius-config --libs` `libsent-config --libs`

####
#### When using within-package libraries
####
# CPPFLAGS=-I$(LIBJULIUS)/include -I$(LIBSENT)/include  `$(LIBSENT)/libsent-config --cflags` `$(LIBJULIUS)/libjulius-config --cflags`
# LDFLAGS= -L$(LIBJULIUS) `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs`

############################################################

all: aleajactaest

aleajactaest: aleajactaest.cpp shogi.o
	$(CC) $(CFLAGS) $(CPPFLAGS) shogi.o -o aleajactaest aleajactaest.cpp $(LDFLAGS) `pkg-config --cflags --libs alsa`

test: test.cpp shogi.o
	g++ -g shogi.o test.cpp -o test

shogi.o: shogi.cpp shogi.h
	g++ -g -c shogi.cpp

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) test
