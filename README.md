# HydrOS

## About

HydrOS aims to be a simple yet well documented kernel for the x86-64 architecture.

## Why HydrOS

Hydros (that's why OS looks so nice there) is one of the Greek deities associated with water (specially in ancient texts), which is cool. But what is even cooler is the fact the Hydrogen is the first element in the periodic table, which means it is the lightest one.

## Building

Depending on your operating system you may need different packages. In case you run `build.sh` with a problem, simply install the package of the failed command. To build the kernel you need *clang*, *nasm* and the GNU linker (*ld*). Finally, to build the image you will need *xorriso*.

- Clone the repository with `git clone <url>`
- Run `build.sh` with the `build` or `run` options (use the `limine` option before that if you are building the kernel for the first time)

## TODO

Kernel:

- [x] ISRs and IRQs
- [x] Serial driver
- [x] PCI listing

Drivers and usage:

- [x] Framebuffer support
- [x] PS/2 Keyboard driver
- [ ] Mouse support

# License

MIT