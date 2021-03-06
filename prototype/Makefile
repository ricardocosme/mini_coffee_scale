MCU_TARGET=attiny85
STD_CXX=c++17
OPTIMIZE=-Os

CXX=avr-g++
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
INCLUDE=-I../../att85/include -I../../hx711/include
CXXFLAGS=-std=$(STD_CXX) -g -mmcu=$(MCU_TARGET) -Wall $(OPTIMIZE) -DF_CPU=1000000 $(INCLUDE)

demos = scale

all: $(demos:%=%.elf) $(demos:%=%.lst) $(demos:%=%.s)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(demos:%=%.d)

%.s: %.cpp
	$(CXX) $(CXXFLAGS) -S $^ 

%.elf: %.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

.PHONY: flash_%
flash-%: %.hex
	avrdude -p t85 -c usbasp -P usb  -U flash:w:$<

size:
	avr-size $(demos:%=%.elf)

.PHONY: clean
clean:
	rm -f *.hex *.lst *.elf *.o *.d *.s

