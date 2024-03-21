CC = gcc
PY = python3
RM = rm -rf

OBJDIR = build
DEPDIR = .deps
DATADIR = data
TMPDIR = tmp

FLAGS = -lm
DEPFLAGS = -MMD -MP -MF $(DEPDIR)/$*.d
PYFLAGS = -c

SRCS = main.c
LOAD_DATA = src/load_data.c
GRAPHIC_FILE = src.graphic
FUNC_GRAPHIC = gera_grafico

OBJS = $(patsubst %, $(OBJDIR)/%.exe, $(basename $(SRCS)))
$(shell mkdir -p $(dir $(OBJS)) > /dev/null)

DEPS = $(patsubst %, $(DEPDIR)/%.d, $(basename $(SRCS)))
$(shell mkdir -p $(dir $(DEPS)) > /dev/null)

$(shell mkdir -p $(DATADIR) > /dev/null)
$(shell mkdir -p $(TMPDIR) > /dev/null)

all: $(OBJS)

$(OBJDIR)/%.exe: src/%.c $(DEPDIR)/%.d
	$(CC) $(DEPFLAGS) -o $(OBJDIR)/program $(LOAD_DATA) $< $(FLAGS)

$(DEPS):


-include $(DEPS)

train:
	./$(OBJDIR)/*

graphic:
	$(PY) $(PYFLAGS) "from $(GRAPHIC_FILE) import $(FUNC_GRAPHIC); $(FUNC_GRAPHIC)()"

exec: all train graphic

clean:
	$(RM) $(OBJDIR) $(DEPDIR) $(DATADIR) $(TMPDIR) src/__pycache__/ && clear