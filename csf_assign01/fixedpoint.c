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
  Fixedpoint fixedpoint = {whole, 0, 0, 1};
  return fixedpoint;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint fixedpoint = {whole, frac, 0, 1};
  return fixedpoint;  
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  uint64_t whole = 0, frac = 0;
  int neg = 0;
  int len_hex = strlen(hex);

  if (*hex == '-') {
    neg = 1;
    ++hex;
  }

  for (int i = 0; i < len_hex; i++) {
    if (hex[i] == '.') {
      whole = hex_to_dec(hex + i - 1, i, 1);
      frac = hex_to_dec(hex + i + 1, len_hex - i - 1, 0);

      Fixedpoint fixedpoint = fixedpoint_create2(whole, frac);
      fixedpoint.neg = neg;
      return fixedpoint;
    }
  }

  Fixedpoint fixedpoint = fixedpoint_create(hex_to_dec(hex, len_hex, 1));
  fixedpoint.neg = neg;
  return fixedpoint;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  /*
  // get sum of left and right as an int
  int sumAsInt = fixedpoint_to_int(left) + fixedpoint_to_int(right);
  // get the highest number of decimal places contained in either left or right
  int lengthLeftFrac = std::to_string(left.frac).length();
  int lengthRightFrac = std::to_string(right.frac).length();
  int digitsInFrac = (lengthLeftFrac >= lengthRightFrac) ?
	  lengthLeftFrac : lengthRightFrac;
  // get whole and frac 
  // (whole = digits to right of 10^digitsInFrac place, frac = other digits)
  uint64_t whole = sumAsInt / get_powerOf10(digitsInFrac);
  uint64_t frac = sumAsInt % whole;
  // make Fixedpoint, return it
  return fixedpoint_create2(whole, frac); */
  // TODO: implement
  assert(0);
  return DUMMY;
}

int fixedpoint_to_int(Fixedpoint fixedpoint) { 
  /* 
  // get length of frac
  int lengthOfFrac = length(fixedpoint.frac);
  // exponentiate 10 by lengthOfFrac
  int powerOf10 = getPowerOf10(lengthOfFrac)
  // finish the conversion, whole * powerOf10 + frac
  return ((fixedpoint.whole * powerOf10) + fixedpoint.frac); */

  // TODO: implement
  assert(0);
  return 0;
}

int get_powerOf10(int exponent) {
  int powerOf10 = 1;
  for (int i = 0; i < exponent; i++) {
     powerOf10 = powerOf10 * 10;
  }
  return powerOf10;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  if (fixedpoint_is_zero(val)) return val;
  
  val.neg = val.neg ? 0 : 1;
  return val;
  // TODO: implement
  // assert(0);
  // return DUMMY;
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
  return val.neg;
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

uint64_t hex_to_dec(const char *hex, int len, int is_whole) {
  uint64_t val = 0;
  int i = is_whole ? 0 : 15;
  for (int j = 0; j < len; ++j) {
    if (*hex >= '0' && *hex <= '9') {
      val += ((uint64_t) 1 << (4 * i)) * (*hex - '0');
    }

    else if (*hex >= 'a' && *hex <= 'f') {
      val += ((uint64_t) 1 << (4 * i)) * (*hex - 'a' + 10);
    }

    if (is_whole) { --hex; ++i; }
    else { ++hex; --i; }
  }

  return val;
}
