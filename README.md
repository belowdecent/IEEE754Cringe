# Truncating 64 bit floating point numbers to their nearest power of using bit magic
## It is just as horrible (if not more) as it sounds
I just wanted to check whether it was possible to this sort of thing without
using any math libs, that generally would be required as
such sort of truncation requires usage of flooring, exponentiation and logarithms.

I pray to all known gods that I have not hit any UBs along the way, but there are some bold assumptions:
1) Your doubles are IEEE754 doubles, I do not want to hear anything about other doubles
2) Your byte is 8 bits long
3) Actually no assumptions, it works on my machine, but if it doesn't on yours, would gladly learn about why
4) You will NOT get inspired by this in any way, shape or form, this repo is an intrusive thought that won and nothing more.

More explanations on how that works are in the main.c file.

Also don't forget to link math.h
