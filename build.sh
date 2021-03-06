# --------------------------------------

LD="ld"
CC="gcc"
AS="nasm"
CFLAGS="-std=gnu99 -ffreestanding -fno-stack-protector -mno-red-zone -Wall -Wextra -c -Isrc/include"
AFLAGS="-f elf64"

linker="linker.ld"
kernel="kernel.bin"
iso="hydros.iso"

# --------------------------------------

clean() {
    rm *.o *.bin *.iso *.log
    rm -rf mnt
}

if [[ $1 == "build" || $1 == "run" ]]
then
    clean

    # --------------------------------------
    
    $CC $CFLAGS $(find src -name "*.c")

    # --------------------------------------

    $AS $AFLAGS src/kernel/__gdt.asm -o __gdt.o
    $AS $AFLAGS src/kernel/__int.asm -o __int.o
    $AS $AFLAGS src/task/__task.asm -o __task.o

    # --------------------------------------

    $LD -n -o $kernel -T $linker $(find . -name "*.o")

    # --------------------------------------

    mkdir -p mnt
    cp -v $kernel limine.cfg limine/limine.sys\
        limine/limine-cd.bin limine/limine-eltorito-efi.bin mnt

    xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table\
        --efi-boot limine-eltorito-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label\
        mnt -o $iso

    ./limine/limine-install $iso

    # --------------------------------------

    if [[ $1 == "run" ]]
    then
        if [[ $2 == "ne2k" ]]; then qemuflags+="-nic user,model=ne2k_pci"; fi
        if [[ $2 == "rtl" ]]; then qemuflags+="-nic user,model=rtl8139"; fi
        qemu-system-x86_64 -cdrom $iso $qemuflags
    fi

elif [[ $1 == "clean" ]]
then
    clean

elif [[ $1 == "limine" ]]
then
    rm -rf limine
    git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1
    make -C limine

else
    echo "(build), (run), (limine), (clean)"
    echo "(run ne2k) to load the ne2k device"
    echo "(run rtl)  to load the rtl8139 device"
fi