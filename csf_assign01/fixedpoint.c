#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

#include <string.h>

// You can remove this once all of the functions are fully implemented
static Fixedpoint DUMMY;

Fixedpoint fixedpoint_create(uint64_t whole) {
  Fixedpoint fixedpoint = {whole, 0};
  return fixedpoint;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint fixedpoint = {whole, frac};
  return fixedpoint;  
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  // TODO: implement
  uint64_t whole = 0;
  uint64_t frac = 0;
  int len_hex = strlen(hex);

  for (int i = 0; i < len_hex; i++) {
    if (hex[i] == '.') {
      whole = hex_to_dec_whole(hex + i - 1, i);

      // hex_to_dec_frac not yet implemented
      frac = hex_to_dec_frac(hex + i + 1, len_hex - i - 1);

      return fixedpoint_create2(whole, frac);
    }
  }

  return fixedpoint_create(hex_to_dec_whole(hex, len_hex));

  // assert(0);
  // return DUMMY;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_zero(Fixedpoint val) {
  return (val.whole == 0 && val.frac == 0);
}

int fixedpoint_is_err(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_valid(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  // TODO: implement
  assert(0);
  char *s = malloc(20);
  strcpy(s, "<invalid>");
  return s;
}

uint64_t hex_to_dec_whole(char *hex, int len) {
  uint64_t whole = 0;
  int i = 0;
  for (int j = 0; j < len; ++j) {
    if (*hex >= '0' && *hex <= '9') whole += (1 << (4 * i)) * (*hex - '0');

    else if (*hex >= 'a' && *hex <= 'f') {
      switch (*hex) {
        case 'a':
          whole += (1 << (4 * i)) * 10;
          break;
        case 'b':
          whole += (1 << (4 * i)) * 11;
          break;
        case 'c':
          whole += (1 << (4 * i)) * 12;
          break;
        case 'd':
          whole += (1 << (4 * i)) * 13;
          break;
        case 'e':
          whole += (1 << (4 * i)) * 14;
          break;
        case 'f':
          whole += (1 << (4 * i)) * 15;
          break;
      }
    }
    --hex;
    ++i;
  }

  return whole;
}

uint64_t hex_to_dec_frac(char *hex, int len) {
   // TODO: implement
  assert(0);
  return 0;
}
