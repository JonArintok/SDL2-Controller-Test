
primaryTarget = controllerTest

src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = -lSDL2 -lm

$(primaryTarget): $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(primaryTarget)
