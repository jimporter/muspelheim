CXXFLAGS := -std=c++1y
PREFIX := /usr

-include config.mk

ifndef TMPDIR
  TMPDIR := /tmp
endif

SOURCES := $(wildcard *.cpp)

.PHONY: all
all: muspelheim

# Include all the existing dependency files for automatic #include dependency
# handling.
-include $(SOURCES:.cpp=.d)

# Build .o files and the corresponding .d (dependency) files. For more info, see
# <http://scottmcpeak.com/autodepend/autodepend.html>.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@
	$(eval TEMP := $(shell mktemp $(TMPDIR)/muspelheim-XXXXXX))
	@$(CXX) $(CXXFLAGS) -MM -Iinclude $< > $(TEMP)
	@sed -e 's|.*:|$*.o:|' < $(TEMP) > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(TEMP) | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $(TEMP)

muspelheim: $(SOURCES:.cpp=.o)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -lpng -o $@

.PHONY: install
install: all
	cp muspelheim $(PREFIX)/bin/muspelheim

.PHONY: clean
clean:
	rm -f muspelheim *.o *.d
