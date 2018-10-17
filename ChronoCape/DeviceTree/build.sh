#!/bin/bash

DTC=/usr/bin/dtc
INCLUDE=/opt/source/bb.org-overlays/include
INSTALLDIR=/lib/firmware

for SRC in *.dts; do
	NAME=$(basename "$SRC" .dts)
	TMP=${NAME}.tmp.dts
	DST=${NAME}.dtbo
	echo "Processing $NAME"
	
	cpp -nostdinc -I ${INCLUDE} -undef -x assembler-with-cpp $SRC > $TMP
	${DTC} -i ${INCLUDE} -@ -O dtb -b 0 -o $DST $TMP
	rm $TMP
	sudo cp $DST ${INSTALLDIR}
done
