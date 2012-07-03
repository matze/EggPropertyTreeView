CFLAGS=`pkg-config --cflags gtk+-2.0` -g -ggdb -Wall -Werror -std=c99
LDFLAGS=`pkg-config --libs gtk+-2.0`
DEPS=egg-property-cell-renderer.h
OBJ=tree-view-test.o egg-property-cell-renderer.o egg-property-tree-view.o

all: tree-view-test

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tree-view-test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ)
	rm -f renderer-test
