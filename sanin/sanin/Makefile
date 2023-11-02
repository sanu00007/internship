CC=g++
#CC=clang++
CFLAGS=-Wall -I. -std=c++11 -g
#CFLAGS=-Wall -I. -std=c++11 -O3
#CFLAGS=-Wall -I$(IDIR) -I. -std=c++11 -g -fsanitize=address -fno-omit-frame-pointer
ODIR=obj

LIBS=-levent -lpthread -ljansson -lwkhtmltox 
#LIBS=-levent -lpthread -ljansson -lwkhtmltox -fsanitize=address -fno-omit-frame-pointer

DEPS = $(wildcard *.h)

CPP_FILES := $(wildcard *.cpp)
OBJ:= $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

report_handler: $(OBJ) $(OBJLIBS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core report_handler $(OBJLIBS) 
force_look :
	true 
