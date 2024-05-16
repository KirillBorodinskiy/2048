CXX := g++

CPPFLAGS := -MD -MP
CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic -O3 -g -I/usr/include/SDL2
LDFLAGS := -lm -lSDL2 -lSDL2_image

SOURCES = main.cpp
TARGETS = $(SOURCES:%.cpp=%)

all: $(TARGETS)

# proměnné:
# $@   target
# $^   seznam všech závislostí
# $<   první závislost ze seznamu

# pravidlo pro překlad zdrojových souborů
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

# pravidlo pro linkování
$(TARGETS): % : %.o
	$(CXX) -o $@ $< $(LDFLAGS)

clean:
	$(RM) *.[od] $(TARGETS)

-include $(SOURCES:%.cpp=%.d)
