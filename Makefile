CXXFLAGS := -std=c++1y
PREFIX := /usr

-include config.mk

ifndef TMPDIR
  TMPDIR := /tmp
endif

SOURCES := $(wildcard src/*.cpp)
GALLERY := $(patsubst %.cpp,%,$(wildcard src/gallery/*.cpp))

.PHONY: all
all: gallery

# Include all the existing dependency files for automatic #include dependency
# handling.
-include $(SOURCES:.cpp=.d)
-include $(GALLERY:=.d)

# Build .o files and the corresponding .d (dependency) files. For more info, see
# <http://scottmcpeak.com/autodepend/autodepend.html>.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -Iinclude -MMD -MF $*.d -c $< -o $@
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d

$(GALLERY): LDFLAGS += -lpng -lpthread -lboost_program_options
$(GALLERY): %: %.o $(SOURCES:.cpp=.o)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

.PHONY: gallery
gallery: $(GALLERY)

.PHONY: install
install: all
	cp muspelheim $(PREFIX)/bin/muspelheim

.PHONY: clean
clean:
	rm -f $(GALLERY)
	find . -name "*.[od]" -exec rm -f {} +

.PHONY: gitignore
gitignore:
	@echo $(GALLERY) | sed -e 's|src/gallery/||g' -e 's/ /\n/g' > \
	  src/gallery/.gitignore
