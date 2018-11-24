BUILD_DIR = build
SUBDIR = communication

.PHONY: all clean $(SUBDIR)

all: $(SUBDIR)

$(SUBDIR):
	$(MAKE) -C $@

clean:
	rm -rf build \
	rm harness
