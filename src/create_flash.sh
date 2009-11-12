#!/bin/sh

FLASH_IMAGE_NAME=flash-image

rm -f $FLASH_IMAGE_NAME
if [ -e x-boot255 ]; then
    dd of=$FLASH_IMAGE_NAME bs=1k count=16k if=/dev/zero
    dd of=$FLASH_IMAGE_NAME bs=1k conv=notrunc if=x-boot255
    dd of=$FLASH_IMAGE_NAME bs=1k conv=notrunc seek=29 if=pxacore_image.bin
else
    echo "Please invoke 'make' to get one."
    exit 1
fi

