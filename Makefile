
NAME=main
PROGRAMMER=usbtiny
PART_SHORT=t44
PART_LONG=attiny44a

CC=avr-gcc
CFLAGS= -std=c11 -Os -DF_CPU=1000000 -mmcu=${PART_LONG}
OBJCOPY=avr-objcopy

.PHONY: install clean fuses

${NAME}.hex: ${NAME}.o
	${OBJCOPY} -j .text -j .data -O ihex $< $@

${NAME}.o: ${NAME}.c
	${CC} ${CFLAGS} -o $@ $<

install: ${NAME}.hex
	avrdude -c ${PROGRAMMER} -p ${PART_SHORT} -U flash:w:$<:i

clean:
	rm -f ${NAME}.hex ${NAME}.o

fuses:
	avrdude -F -V -c ${PROGRAMMER} -p ${PART_SHORT} -U lfuse:w:0x7d:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
