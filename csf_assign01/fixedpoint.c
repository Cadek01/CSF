#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

// You can remove this once all of the functions are fully implemented
static Fixedpoint DUMMY;

Fixedpoint fixedpoint_create(uint64_t whole) {
  Fixedpoint fixedpoint = {whole, 0, 1, 0, 0, 0, 0, 0, 0};
  return fixedpoint;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint fixedpoint = {whole, frac, 1, 0, 0, 0, 0, 0, 0};
  return fixedpoint;  
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  uint64_t whole = 0, frac = 0;
  int neg = 0, err = 0, is_frac = 0;
  int *err_ptr = &err;
  int len_hex = strlen(hex);

  if (*hex == '-') {
    neg = 1;
    ++hex;
    --len_hex;
  }

  for (int i = 0; i < len_hex; i++) {
    if (hex[i] == '.') {
      is_frac = 1;
      whole = hex_to_dec(hex + i - 1, i, 1, err_ptr);
      frac = hex_to_dec(hex + i + 1, len_hex - i - 1, 0, err_ptr);
    }
  }

  if (!is_frac) whole = hex_to_dec(hex, len_hex, 1, err_ptr);

  Fixedpoint fixedpoint = fixedpoint_create2(whole, frac);
  fixedpoint.neg = neg;
  fixedpoint.err = err;
  fixedpoint.valid = !err;
  return fixedpoint;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  if (left.neg && !right.neg) {
    left = fixedpoint_negate(left);
    return fixedpoint_sub(right, left);
  }

  if (!left.neg && right.neg) {
    right = fixedpoint_negate(right);
    return fixedpoint_sub(left, right);
  }

  if (left.neg && right.neg) {
    left = fixedpoint_negate(left);
    right = fixedpoint_negate(right);
    return fixedpoint_negate( fixedpoint_add(left, right));
  }

  // start with frac:
  // create pointer to track any carry-over needed
  uint64_t carry_over = 0;
  uint64_t* carry_over_ptr = &carry_over;
  // get frac_sum, modified cary_over pointer
  uint64_t frac_sum = bitwise_sum(carry_over_ptr, left.frac, right.frac);
  // move on to whole:
  uint64_t whole_sum = carry_over + left.whole + right.whole;
  // return overall fixedpoint
  Fixedpoint sum = fixedpoint_create2(whole_sum, frac_sum);

  // OVERFLOW CHECK
  if (!fixedpoint_is_zero(left) && !fixedpoint_is_zero(right)) {
    if ( (fixedpoint_compare(left, sum) != -1) || (fixedpoint_compare(right, sum) != -1)) {
      sum.pos_over = 1;
    }
  }
  return sum;
}   

uint64_t bitwise_sum(uint64_t* carry_over_ptr, uint64_t addend1, uint64_t addend2) {
        // do bitwise sum
        uint64_t local_carry_over = 0;
        uint64_t local_sum = 0;
        while (addend2 != 0) {
                // if 64th bit of both addend 1 & addend2 are a 1, need to carry over into whole
                // change state of carry_over_ptr to reflect this intention, change 64th bith of both addends to be 0
                if (((addend1 >> 63) & 1 ) && ((addend2 >> 63) & 1)) {
                        *carry_over_ptr = 1;
                        addend1 = addend1 ^ ((uint64_t) 1 << 63);
                        addend2 = addend2 ^ ((uint64_t) 1 << 63);
                }
                local_carry_over = (addend1 & addend2) << 1;
                local_sum = addend1 ^ addend2;
                addend1 = local_sum;
                addend2 = local_carry_over;
        }
        // return sum, which is stored in addend1
        return addend1;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  Fixedpoint diff, temp;
  uint64_t whole_diff = 0, frac_diff = 0;
  int neg = 0;

  if (!left.neg && right.neg) {
    right = fixedpoint_negate(right);
    return fixedpoint_add(left, right);
  }

  if (left.neg && !right.neg) {
    left = fixedpoint_negate(left);
    return fixedpoint_negate( fixedpoint_add(left, right) );
  }

  if (left.neg && right.neg) {
    temp = fixedpoint_negate(right);
    right = fixedpoint_negate(left);
    left = temp;
    neg = !neg;
  }


  // CHANGE TO COMPARE
  if (!fixedpoint_compare(left, right)) return fixedpoint_create(0);

  if ((left.whole < right.whole) || ((left.whole == right.whole) && (left.frac < right.frac))) {
    neg = !neg;
    temp = right;
    right = left;
    left = temp;
  }

  whole_diff = left.whole - right.whole;

  if (right.frac > left.frac) {
    frac_diff = right.frac - left.frac;
    whole_diff--;
  }

  else frac_diff = left.frac - right.frac;

  diff = fixedpoint_create2(whole_diff, frac_diff);

  if (neg) diff = fixedpoint_negate(diff);

  return diff;
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  int temp;
  if (fixedpoint_is_zero(val)) return val;
  val.neg = val.neg ? 0 : 1;

  if (val.pos_over || val.neg_over) {
    temp = val.pos_over;
    val.pos_over = val.neg_over;
    val.neg_over = temp;
  }

  if (val.pos_under || val.neg_under) {
    temp = val.pos_under;
    val.pos_under = val.neg_under;
    val.neg_under = temp;
  }
  return val;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  return fixedpoint_add(val, val);
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  if ( (left.whole == right.whole) && (left.frac == right.frac) && (left.neg == right.neg)) return 0;

  if (!left.neg) {
    if (right.neg) return 1;
    if ( (left.whole > right.whole) || ( (left.whole == right.whole) && (left.frac > right.frac) )) return 1;
    return -1;
  }

  if (!right.neg) return -1;
  if ( (left.whole < right.whole) || ( (left.whole == right.whole) && (left.frac < right.frac) )) return 1;
  return -1;
}

int fixedpoint_is_zero(Fixedpoint val) {
  return (val.whole == 0 && val.frac == 0);
}

int fixedpoint_is_err(Fixedpoint val) {
  return val.err;
}

int fixedpoint_is_neg(Fixedpoint val) {
  return val.neg;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  return val.neg_over;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  return val.pos_over;
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
  return val.valid;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  // TODO: implement
  assert(0);
  char *s = malloc(20);
  strcpy(s, "<invalid>");
  return s;
}

uint64_t hex_to_dec(const char *hex, int len, int is_whole, int* err) {
  uint64_t val = 0;
  int i = is_whole ? 0 : 15;

  if (len > 16) *err = 1;

  for (int j = 0; j < len; ++j) {
    if (*hex >= '0' && *hex <= '9') {
      val += ((uint64_t) 1 << (4 * i)) * (*hex - '0');
    }

    else if (*hex >= 'a' && *hex <= 'f') {
      val += ((uint64_t) 1 << (4 * i)) * (*hex - 'a' + 10);
    }

    else *err = 1;

    if (is_whole) { --hex; ++i; }
    else { ++hex; --i; }
  }

  return val;
}
