BUILD = ./build
SRC = ./src
TEST = ./test
INFO = ./info
INC = $(SRC)/inc
# CFLAGS = -gdwarf-2 -O0 -c -m32 -I$(INC) -fno-pie -fpack-struct -fno-stack-protector -nostdlib -nostdinc -Wno-builtin-declaration-mismatch -Wno-int-to-pointer-cast -Wno-implicit-function-declaration -Wno-address-of-packed-member
CFLAGS =  -gdwarf-2 -O0 -c -m32  -march=i386 -fpack-struct -fno-leading-underscore -I$(INC) -I$(INC)/csos 
#-march=i686
# -fpack-struct
CCX = gcc

$(BUILD)/test/%.bin: $(TEST)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -g -f bin $< -o $@

$(BUILD)/%.o: $(SRC)/%.asm
	$(shell mkdir -p $(dir $@))
	echo building $<
	nasm -g -f obj -o $@ $< 

$(BUILD)/boot/boot2.bin: $(SRC)/boot/boot2.asm
# nasm -g -f elf $< -o $@.elf
# nasm -g -f win32 $< -o $@.win32
# nasm -g -f obj $< -o $@.obj
# ld -Ttext=0x7c00 $@.win32 -o $@
	nasm -g -f elf -d_nod_org -l $@.lst -o $@.elf $< 
#	nasm -g -f elf -l $@.lst -o $@.o2 $< 
#	nasm  -O0 -g -f bin $< -o $@
#	ld -mi386pe -Ttext=0x7c00 -g -e main -o $@ $@.o
#	objcopy -O binary $@.o $@ 
	nasm -g -f bin -o $@ $< 
#nasm -g -f bin $(SRC)/boot/boot2.asm -o $@

$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -g -f elf -d_nod_org $< -o $@.elf
	nasm -g -f bin $< -o $@

$(BUILD)/boot2.pe: $(BUILD)/boot/boot2.o
	$(shell mkdir -p $(dir $@))
	ld  -Ttext=0x7c00  $^ -o $(BUILD)/boot2.pe
# x86_64-elf-objcopy -O binary $(BUILD)/boot.elf $(BUILD)/boot.bin

$(BUILD)/%.o: $(SRC)/%.S
	$(shell mkdir -p $(dir $@))
	$(CCX) $(CFLAGS) $< -o $@

$(BUILD)/%.o: $(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	$(CCX) $(CFLAGS) $< -o $@

$(BUILD)/kernel.pe: $(BUILD)/kernel/start.o \
	$(BUILD)/kernel/kernel.o \
	$(BUILD)/kernel/tty.o \
	$(BUILD)/kernel/memory.o \
	$(BUILD)/kernel/gdt.o \
	$(BUILD)/kernel/kernel32.o \
	$(BUILD)/lib/stdlib.o
	$(shell mkdir -p $(dir $@))
# ld -mi386pe -Ttext=0x8000 $^ -o $(BUILD)/kernel.pe
	ld -mi386pe $^ -o $(BUILD)/kernel.pe

$(BUILD)/kernel32.pe: $(BUILD)/kernel32/start.o \
	$(BUILD)/kernel32/kernel.o \
	$(BUILD)/kernel32/gdt32.o \
	$(BUILD)/kernel32/interrupt.o \
	$(BUILD)/kernel32/pic.o \
	$(BUILD)/kernel32/timer.o \
	$(BUILD)/kernel32/rtc.o \
	$(BUILD)/kernel32/time.o \
	$(BUILD)/kernel32/task/simple.o \
	$(BUILD)/kernel32/task/tss.o \
	$(BUILD)/kernel32/init/init_task_entry.o \
	$(BUILD)/kernel32/init/init_task.o \
	$(BUILD)/kernel32/sem.o \
	$(BUILD)/kernel32/mutex.o \
	$(BUILD)/kernel32/memory32.o \
	$(BUILD)/kernel32/kbd.o \
	$(BUILD)/kernel32/device.o \
	$(BUILD)/kernel32/device/tty.o \
	$(BUILD)/kernel32/device/disk.o \
		$(BUILD)/kernel32/task/tss2.o \
		$(BUILD)/kernel32/sysboundary/syscall_proxy.o \
		$(BUILD)/kernel32/sysboundary/syscall_stdio.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/lib/stdio.o \
	$(BUILD)/lib/stdlib.o \
	$(BUILD)/lib/logf.o \
	$(BUILD)/lib/list.o \
	$(BUILD)/lib/bitmap.o \
	$(BUILD)/lib/syscall.o \
	$(SRC)/kernel32.lds
	$(shell mkdir -p $(dir $@))
	rm -f $@
	ld -mi386pe --image-base=0x400000 -T$(SRC)/kernel32.lds $^ -o $@

.PHONY: test_chs
test_chs: $(BUILD)/test/read_disk_chs.bin
	dd if=$(BUILD)/test/read_disk_chs.bin of=master.img bs=512 count=1 conv=notrunc
	bochsdbg -q -f bochsrc.bxrc	

.PHONY: print
print: $(BUILD)/test/print.bin
	dd if=$(BUILD)/test/print.bin of=master.img bs=512 count=1 conv=notrunc
	bochsdbg -q -f bochsrc.bxrc	

$(BUILD)/pecpy.pe: $(BUILD)/boot/pecpy0.o \
	$(BUILD)/boot/pecpy1.o \
	$(BUILD)/boot/pecpy2.o \
	$(SRC)/boot/pecpy.lds
	ld -mi386pe $(BUILD)/boot/pecpy0.o $(BUILD)/boot/pecpy1.o $(BUILD)/boot/pecpy2.o \
	$(BUILD)/lib/stdio.o \
	 --image-base=0x1000 --file-alignment=512 --section-alignment=512 \
	-T $(SRC)/boot/pecpy.lds  \
	-o $(BUILD)/pecpy.pe
# 	perl $(SRC)/boot/fasm2nasm.pl

.PHONY: unlock
unlock:
	rm -f master.img.lock

x.img:
	dd if=/dev/zero of=x.img bs=1024 count=32760

master.out: $(BUILD)/boot/boot2.bin \
	$(BUILD)/boot/mode32.bin \
	$(BUILD)/boot/setup.bin \
	$(BUILD)/lib/stdio.o \
	$(BUILD)/kernel32.pe \
	$(BUILD)/pecpy.pe \
	$(SRC)/boot/init_task_relocation_output.txt
	dd if=$(BUILD)/boot/boot2.bin of=master.img bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/setup.bin of=master.img bs=512 count=1 seek=1 conv=notrunc
	dd if=$(BUILD)/boot/mode32.bin of=master.img bs=512 count=1 seek=2 conv=notrunc
	dd if=$(BUILD)/pecpy.pe of=master.img bs=512 count=20 seek=3 conv=notrunc

	dd if=$(BUILD)/pefromat.exe of=master.img bs=512 count=400 seek=35 conv=notrunc
	
	dd if=$(SRC)/boot/init_task_relocation_output.txt of=master.img bs=512 count=1 seek=499 conv=notrunc
	dd if=$(BUILD)/kernel32.pe of=master.img bs=512 count=400 seek=500 conv=notrunc
	
	dd if=$(BUILD)/boot/boot2.bin of=master.img bs=512 count=10 seek=1000 conv=notrunc
	rm -f master.out
	touch master.out

master: master.out
	echo building master

.PHONY: boot2
boot2: unlock master
	bochsdbg -q -f bochsrc.bxrc

# qemu: clean master
# -s -S
# -smp 4,cores=1
# -m 128M
.PHONY: qemu
qemu: unlock master
	qemu-system-i386 -s -S -m 128M -smp 4,cores=1 \
		-serial stdio -drive file=master.img,index=0,media=disk,format=raw \
		-drive file=disk.img,index=1,media=disk,format=raw \
		-audiodev id=sdl,driver=sdl -machine pcspk-audiodev=sdl

.PHONY: qemu2
qemu2: unlock
	qemu-system-i386w -g1234 -S -m 128M -drive file=master.img,index=0,media=disk,format=raw

.PHONY: testap
testap:
	nasm -g -f elf -d_nod_org -l $(BUILD)/boot/testap.lst -o $(BUILD)/boot/testap.bin.elf $(SRC)/boot/testap.asm
# nasm $(SRC)/boot/testap.asm -o $(SRC)/boot/testap.bin       
	nasm -g -f bin $(SRC)/boot/testap.asm -o $(BUILD)/boot/testap.bin       
# qemu-system-x86_64 -s -S -smp 4 -m 1M -drive file=$(SRC)/boot/testap.bin,format=raw  
	qemu-system-i386w -s -S -smp 4 -m 1M -drive file=$(BUILD)/boot/testap.bin,format=raw  

.PHONY: qemua
qemua:
	qemu-system-x86_64w -s -S -m 128M -drive file=master.img,index=0,media=disk,format=raw
 	