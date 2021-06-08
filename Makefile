# SYNTAX OF MAKEFILES:
# target: dependency1 dependency2 ... dependencyn
# 	command to get target from dependencies listed
# ${...} => use a variable already deifined using varName = value
# $@ => target
# $< => dependency1 (first dependency only)
# $^ => dependency1 ... dependencyn (all dependencies listed)
# %.o => all files matching .o

# USAGE IN THE TERMINAL:
# make target => run a specific part of makefile
# make => run all the commands in the makefile

# NOTE: Checks for the timestamps and runs the command iff the dependencies are updates to update the target accordingly.

# ----------------------------------------------------------------------------------------------------------------------------------------------------

# Any OS initially loads in 32-bit compatibility mode
# Specify to the compiler that there are no builtin memory managers, no stdlibs, no exception handlers etc.
# -I<folder_name> will tell the compiler that all .h files are in folder_name("include" in our case) and we can also use #include<filename.h> from folder_name instead of root of the project
GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

OBJECT_FILES =  obj/loader.o \
				obj/kernel.o \
				obj/drivers/driver.o \
				obj/drivers/vga.o \
				obj/hardware_communication/interrupts.o \
				obj/hardware_communication/pci.o \
				obj/drivers/mouse.o \
				obj/hardware_communication/interruptsAssembly.o \
				obj/gdt.o \
				obj/hardware_communication/port.o \
				obj/drivers/keyboard.o \
				obj/gui/desktop.o \
				obj/gui/widget.o \
				obj/gui/window.o \
				obj/multitasking.o \
				obj/memory_manager.o

obj/%.o: src/%.cpp
	# -p indicates to create new directiories only if they do not exist
	mkdir -p ${@D}
	g++ ${GPPPARAMS} -o $@ -c $<
	
obj/%.o : src/%.s
	mkdir -p ${@D}
	as ${ASPARAMS} -o $@ $<

kernel.bin: linker.ld ${OBJECT_FILES}
	ld ${LDPARAMS} -T $< -o $@ ${OBJECT_FILES}

 # Create an iso file to run on a vm 	
kernel.iso: kernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	# As we want only one entry for the OS to be displayed by the bootloader, set it to default and timeout to 0
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "myOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/kernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	# We no longer need the iso folder once we have the iso file:
	rm -rf iso
	# Add the OS to any vm using the .iso file like any other OS.  
	
# open vm from terminal once OS is setup on it for the first time
run: kernel.iso
	# kill all the previously opened VMs and wait for 1s until it closes and start a new one.
	(killall VirtualBoxVM && sleep 1) || true
	vboxmanage startvm "myOS"

# kill the running vm
stop: kernel.iso	
	killall VirtualBoxVM

# A phony target is one that is not really the name of a file; rather it is just a name for a recipe to be executed when you make an explicit request.
.PHONY: clean
clean:
	rm -rf obj kernel.bin kernel.iso





# ----------------------------------------------------------------------------------------------------------------------------------------------------
# TO BOOT ON A PC with linux's grub on it:	
# /boot contains all the OS. All OS are displayed on the bootloader(grub bootloader in my case)
# Step 1. "make install" => ls /boot/ => should have kernel.bin
# Step 2. "sudo <text-editor> /boot/grub/grub.cfg" => Add the following entry:
# ### BEGIN <any_name> ###
# menuentry '<some_name>' {
# 	multiboot /boot/kernel.bin
#	boot
# }
# ### END <any_name> ###
# On starting the PC, grub bootloader will display the different OS you can load, one among them will be <some_name> which you can select using arrow keys in the keyboard and by hitting enter. 
install: kernel.bin
	sudo cp $< /boot/kernel.bin
