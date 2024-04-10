# Truncating 64 bit floating point numbers to their nearest power of using bit magic
## It is just as horrible (if not more) as it sounds
I just wanted to check whether it was possible to this sort of thing without
using any math libs, that generally would be required as
such sort of truncation requires usage of flooring, exponentiation and logarithms.

I pray to all known gods that I have not hit any UBs along the way, but there are some bold assumptions:
1) Your doubles are IEEE754 doubles, I do not want to hear anything about other doubles
2) Your byte is 8 bits long
3) Bit Endianness was not taken into account
4) Actually no assumptions, it works on my machine, but if it doesn't on yours, would gladly learn about why
5) You will NOT get inspired by this in any way, shape or form, this repo is an intrusive thought that won and nothing more.

More explanations on how that works are in the main.c file.

## Build and run
### For little endian machines
```
cc main.c -lm -o ieee754cringe
./ieee754cringe
```
### For big endian machines
```
cc main.c -lm -D__MYMACHINEISBIG__ -o ieee754cringe
./ieee754cringe
```
`Failed at 0.003906` means either that the endianness given was wrong, or something forgotten on my part.
