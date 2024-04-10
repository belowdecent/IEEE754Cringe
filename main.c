#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Note: despite me using sizeof(double), i'm using it
// mostly to avoid random eights around the code THIS DOES
// NOT MAKE IT GENERIC FOR OTHER SIZES OF DOUBLE EVERYTHING
// WILL BREAK, YOUR PC WILL EXPLODE AND I AM NOT TAKING
// RESPONSIBILITY FOR THAT

// Allows to treat a double as an array of 8 bytes and vice
// versa Fairly certain unions are the way to do this in C
// But things like that are forbidden in C++
typedef union {
  double number;
  unsigned char bytes[sizeof(double)];
} PunningHehe;

// SO
// What is a 64-bit floating point number, also known by
// many as "double" (for the sake of demonstration we are
// using the big endian byte order)
//
// To oversimplify, it's a way to cram a scientific notation
// of number into 64 bits The implied form is a * 2^n
//
// To demonstrate how this works
//
// This is 64 bits
// 0000000000000000000000000000000000000000000000000000000000000000
//
// We split the regions of this number
// |------------- Sign (1 bit)
// |  |------------------ Exponent (11 bits)
// |  |            |---------- Mantissa (52 bits)
// 0  00000000000  0000000000000000000000000000000000000000000000000000
//
// SIGN:
// Sign is sign, 1 - number is negative, 0 - number is
// positive
//
// EXPONENT:
// Exponent is the power to which we raise the 2 in our case
// BUT, we also subtract 1023 from the exponent
// to make sense of this:
// given a = 1:
// 00000000000 - this gives us the number 1 * 2^(0-1023), or
// 1/2^1023 10000000000 - this is 2^(1024-1023) or 2
// 01111111111 - this is 2^(1023-1023) or 1
//
// MANTISSA
// The kinky part
// Mantissa determines the a in our case
// because we work in binary, scientific notation always
// wants our a to be greater than 1 Mantissa becomes the
// fractional part of our a, or in other words 1 + Mantissa
// = a The way it is written it looks simply as a fractional
// part of a number in binary like: .5 in decimal  is .1,
// .25 in decimal is .01,
// .75 in decimal is .11
//
//
// Now combining all of our knowledge lets try to write 30
// in FP64 let's start with the sign, 30 is positive so we
// write 0
//
// 0 xxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
// Now the exponent:
// Nearest power of 2 to 30 that is smaller than 30 is 16
// that is 2^4
// Our exponent is 4, now we need to account for the -1023
// For that we simply 1023 back
// We get 1027, which in binary is 10000000011:
// 0 10000000011 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
// And for the finale, the mantissa
// Let's try to express 30 in scientific notation base 2
// 30 / 16 = 1.875
// => 1.875 * 2^4
// The fractional part is .875, which in binary is 0.111
// (0.5 + 0.25 + 0.125) giving us the full number 
// 0 10000000011 1110000000000000000000000000000000000000000000000000
//
//
// (Note 2: THERE ARE A LOT MORE TO FLOATING POINT NUMBERS,
// THAN JUST THAT. But when working with relatively normal FP numbers,
// it's pretty much just that afaik)
//
//
// HOW TO ABUSE THIS TO TRUNCATE THE NUMBER TO THE LAST
// POWER OF TWO It's actually simple Just bitwise AND away
// the fractional part and voila, you got yourself the last
// power of two

PunningHehe mask_big = {
  .bytes = {0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

// For little endian bytes:
//
// Byte 8   Byte 7   Byte 6   Byte 5   Byte 4   Byte 3 Byte
// 2   Byte 1 00000000 00000000 00000000 00000000 00000000
// 00000000 11110000 11111111 My PC and likely yours is
// Little Endian
PunningHehe mask_little = {
  .bytes = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff}
};

// We assume that the byte size is 8 bits, bold assumption,
// I know
unsigned char get_bit_from_left(
  unsigned char byte, unsigned char position
);
// nth byte becomes (8-n)th byte, little->big, big->little
double reverse_byte_endianness(double num);

// those functions truncate the number to the nearest power
// of two it does not behave well with subnormals,
// infinities, nans and basically any doubles that are weird
// and ugly

// if the __MYMACHINEISBIG__ flag is set, reverses the byte
// order of num within the function and then truncates it
double truncate_little(double num);
// if the __MYMACHINEISBIG__ flag is NOT set, reverses the
// byte order of num within the function and then truncates
// it
double truncate_big(double num);

// those functions simply check whether
// 2^(floor(log2(n)) [also known as the sane way to truncate
// numbers] yields the same result as my little bit magic
// done on n

// if the __MYMACHINEISBIG__ flag is set, reverses the byte
// order of 2^(floor(log2(n))), because truncate_little
// returns a little endian number
void check_truncation_little(
  double min, double max, double inc
);

// if the __MYMACHINEISBIG__ flag is NOT set, reverses the
// byte order of 2^(floor(log2(n))), because truncate_big
// returns a big endian number
void check_truncation_big(
  double min, double max, double inc
);


// here we do the checks
int main(void) {
  check_truncation_little(0, pow(2, 8), pow(2, -8));
  check_truncation_big(0, pow(2, 8), pow(2, -8));

  return EXIT_SUCCESS;
}


unsigned char get_bit_from_left(
  unsigned char byte, unsigned char position
) {
  unsigned int num = byte;
  unsigned int mask = 1 << (7 - position);
  num = num & mask;
  num = num >> (7 - position);
  if (num == 1)
    return 1;
  if (num == 0)
    return 0;
  exit(1);
}


// slow but does the job
double reverse_byte_endianness(double num) {
  PunningHehe bad;
  bad.number = num;

  unsigned char buffer[sizeof(double)] = {0};
  for (int i = 0; i < sizeof(double); i++) {
    buffer[i] = bad.bytes[sizeof(double) - 1 - i];
  }
  for (int i = 0; i < sizeof(double); i++) {
    bad.bytes[i] = buffer[i];
  }

  return bad.number;
}


double truncate_little(double num) {
  PunningHehe bad;
#ifdef __MYMACHINEISBIG__
  bad.number = reverse_byte_endianness(num);
#else
  bad.number = num;
#endif

  for (int i = 0; i < sizeof(double); i++) {
    bad.bytes[i] &= mask_little.bytes[i];
  }

  return bad.number;
}


double truncate_big(double num) {
  PunningHehe bad;
#ifndef __MYMACHINEISBIG__
  bad.number = reverse_byte_endianness(num);
#else
  bad.number = num;
#endif

  for (int i = 0; i < sizeof(double); i++) {
    bad.bytes[i] &= mask_big.bytes[i];
  }

  return bad.number;
}


void check_truncation_little(
  double min, double max, double inc
) {
  for (double i = min; i < max; i += inc) {
    double target = pow(2, floor(log2(i)));
#ifdef __MYMACHINEISBIG__
    target = reverse_byte_endianness(target);
#endif
    if (target != truncate_little(i)) {
      printf("Failed at %lf!\n", i);
      exit(1);
    }
  }
  printf("Little Endian Success!\n");
}


void check_truncation_big(
  double min, double max, double inc
) {
  for (double i = min; i < max; i += inc) {
    double target = pow(2, floor(log2(i)));
#ifndef __MYMACHINEISBIG__
    target = reverse_byte_endianness(target);
#endif
    if (target != truncate_big(i)) {
      printf("Failed at %lf!\n", i);
      exit(1);
    }
  }
  printf("Big Endian Success!\n");
}
