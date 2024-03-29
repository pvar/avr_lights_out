
CC      = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size

APP = lightsout
LIB =

DEV = atmega328
CLK = 8000000
STD = -std=gnu99
WRN = -Wall -Wstrict-prototypes -Wno-pointer-to-int-cast
TUN = -O1 -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -fshort-enums

CFLAGS = -mmcu=$(DEV) -DF_CPU=$(CLK) $(TUN) $(STD) $(WRN)

all: $(APP)

$(LIB):
	@echo Building library: $@

$(APP): $(LIB)
	@echo Building program: $@
	@mkdir -p obj
	@$(CC) $(CFLAGS) -o obj/$@.o app/$@.c
	@$(AVRSIZE) obj/$@.o
	@$(OBJCOPY) -j .text  -j .data -O ihex obj/$@.o $@.hex
	@$(OBJDUMP) -d -S obj/$@.o >obj/$@.lss

.PHONY:	clean
clean:
	rm *.hex obj/*.o obj/*.lss
