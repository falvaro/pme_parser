MAGICK=`Magick++-config --cppflags --cxxflags --ldflags --libs`

ifeq ($(mode),debug)
   FLAGS = -lm -g -DVERBOSE -ansi -Wall -pedantic $(MAGICK)
else
   FLAGS = -lm -O3 -Wall -Wno-unused-result $(MAGICK)
endif

parser: parser.cc production.o grammar.o sample.o recNN.o mfset.o cyktable.o logspace.o gparser.o
	g++ -o parser parser.cc production.o grammar.o sample.o recNN.o mfset.o cyktable.o logspace.o gparser.o $(FLAGS)

production.o: production.h production.cc
	g++ -c production.cc $(FLAGS)

grammar.o: grammar.h grammar.cc production.o recNN.o cyktable.o logspace.o gparser.o
	g++ -c grammar.cc $(FLAGS)

gparser.o: gparser.h gparser.cc
	g++ -c gparser.cc $(FLAGS)

sample.o: sample.h sample.cc mfset.o cyktable.o
	g++ -c sample.cc $(FLAGS)

recNN.o: recNN.h recNN.cc
	g++ -c recNN.cc $(FLAGS)

mfset.o: mfset.h mfset.cc
	g++ -c mfset.cc $(FLAGS)

cyktable.o: cyktable.h cyktable.cc
	g++ -c cyktable.cc $(FLAGS)

logspace.o: logspace.h logspace.cc cyktable.o
	g++ -c logspace.cc $(FLAGS)

clean:
	rm -rf *.o *~ \#*\#
