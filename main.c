#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef union {
  double number;
  unsigned char bytes[sizeof(double)];
} PunningHehe;

PunningHehe mask_big = {
  .bytes = {0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};
PunningHehe mask_little = {
  .bytes = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff}
};

unsigned char get_bit_from_left(
  unsigned char byte, unsigned char position
);
double reverse_byte_endianness(double num);
double convert_little(double num);
double convert_big(double num);
void print_binary(double num);
void check_conversion_little(
  double min, double max, double inc
);
void check_conversion_big(
  double min, double max, double inc
);

int main(void) {
  int n = 1;
  // little endian if true
  if (*(char*)&n == 1) {
    printf("Yippee\n");
    printf("Size of double %zu\n", sizeof(double));
  } else {
    printf("Bad!!\n");
  }

  double nums_up[] = {1, 2, 3, 4, 5, 6, 7, 8, 16, 32};
  printf("\n");

  for (int i = 0; i < 10; i++) {
    print_binary(nums_up[i]);
    printf("\n");
    print_binary(convert_little(nums_up[i]));
    printf("\n");
    printf("\n");
  }

  for (int i = 0; i < 10; i++) {
    print_binary(1.0 / nums_up[i]);
    printf("\n");
    print_binary(convert_little(1.0 / nums_up[i]));
    printf("\n");
    printf("\n");
  }

  check_conversion_little(0, pow(2, 8), pow(2, -8));
  check_conversion_big(0, pow(2, 8), pow(2, -8));

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

double convert_little(double num) {
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

double convert_big(double num) {
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

void print_binary(double num) {
  PunningHehe bad;
  bad.number = num;

  printf("%.3f:\t", bad.number);
  for (int byte = 0; byte < sizeof(double); byte++) {
    for (int bit = 0; bit < 8; bit++) {
      if (byte * 8 + bit == 1)
        printf("|");
      if (byte * 8 + bit == 12)
        printf("|");
      printf(
        "%d", get_bit_from_left(
                bad.bytes[sizeof(double) - byte - 1], bit
              )
      );
    }
  }
}

void check_conversion_little(
  double min, double max, double inc
) {
  for (double i = min; i < max; i += inc) {
    double target = pow(2, floor(log2(i)));
#ifdef __MYMACHINEISBIG__
    target = reverse_byte_endianness(target);
#endif
    if (target != convert_little(i)) {
      printf("Failed at %lf!\n", i);
      exit(1);
    }
  }
  printf("Little Endian Success!\n");
}
void check_conversion_big(
  double min, double max, double inc
) {
  for (double i = min; i < max; i += inc) {
    double target = pow(2, floor(log2(i)));
#ifndef __MYMACHINEISBIG__
    target = reverse_byte_endianness(target);
#endif
    if (target != convert_big(i)) {
      printf("Failed at %lf!\n", i);
      exit(1);
    }
  }
  printf("Big Endian Success!\n");
}
