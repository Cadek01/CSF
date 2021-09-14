/*
 * Fixedpoint functions implementation for fixedpoint libary
 * CSF Assignment 1 MS2
 * M.Albert and R.Sivananthan
 * malber20@jhu.edu and rsivana1@jhu.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

Fixedpoint fixedpoint_create(uint64_t whole) {
  // define this.whole as whole, this.frac as 0
  // define the remaining fields as false (neg, err, overflow tags)
  Fixedpoint fixedpoint = {whole, 0, 0, 0, 0, 0, 0, 0};
  return fixedpoint;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  // define this.whole as whole, this.frac as frac
  // define the remaining fields as false (neg, err, overflow tags)
  Fixedpoint fixedpoint = {whole, frac, 0, 0, 0, 0, 0, 0};
  return fixedpoint;  
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  // initialize all fields to zero
  uint64_t whole = 0, frac = 0;
  int neg = 0, err = 0, is_frac = 0;
  int *err_ptr = &err; // initialize error pointer
  int len_hex = strlen(hex); // get length of string

  // return erroneous fixedpoint for empty string
  if (len_hex == 0) {
	  Fixedpoint fixedpoint = fixedpoint_create2(0, 0);
	  fixedpoint.err = 1;
	  return fixedpoint;
  }

  // determine the sign of the fixedpoint, move forward in the parsing of the string
  // (decrement length to be iterated through and move char pointer ahead)
  if (*hex == '-') {
    neg = 1;
    ++hex;
    --len_hex;
  }

  // find decimal in the string. convert left side to whole value and right side to frac
  for (int i = 0; i < len_hex; i++) {
    if (hex[i] == '.') {
      is_frac = 1;
      whole = hex_to_dec(hex + i - 1, i, 1, err_ptr);
      frac = hex_to_dec(hex + i + 1, len_hex - i - 1, 0, err_ptr);
    }
  }

  // convert string to whole value if there is no decimal
  if (!is_frac) whole = hex_to_dec(hex + len_hex - 1, len_hex, 1, err_ptr);

  // create fixedpoint with passed on negative and error values
  Fixedpoint fixedpoint = fixedpoint_create2(whole, frac);
  fixedpoint.neg = neg;
  fixedpoint.err = err;
  
  // ensure 0 fixedpoint is marked as non-negative
  if (fixedpoint_is_zero(fixedpoint)) {
    fixedpoint.neg = 0;
  }
  return fixedpoint; // return fixedpoint
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole; // return whole value
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac; // return fractional value
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  Fixedpoint sum; // declare sum
  	
  // -|left| + |right| = |right| - |left|
  if (left.neg && !right.neg) return fixedpoint_sub(right, fixedpoint_negate(left));

  // |left| + -|right| = |left| - |right|
  if (!left.neg && right.neg) return fixedpoint_sub(left, fixedpoint_negate(right));

  // -|left| + -|right| = -(|left| + |right|)
  if (left.neg && right.neg) {
    sum = fixedpoint_add(fixedpoint_negate(left), fixedpoint_negate(right));
    sum.neg = 1;
    sum.neg_over = sum.pos_over;
    sum.pos_over = 0;
    return sum;
  }

  // a + b where both a > 0 and b > 0
  uint64_t frac_sum = 0, whole_sum = 0;
  int is_overflow = 0;

  // increment whole_sum if fractional sum overflows (sum is less than an addend)
  frac_sum = left.frac + right.frac;
  if (frac_sum < right.frac) whole_sum += 1UL;

  // mark as overflow if whole overflows from carried 1 (same logic as fraction)
  whole_sum += left.whole;
  if (whole_sum < left.whole) is_overflow = 1;

  // mark as overflow if whole overflows from increment by right whole
  whole_sum += right.whole;
  if (whole_sum < right.whole) is_overflow = 1;

  // return sum with proper overflow
  sum = fixedpoint_create2(whole_sum, frac_sum);
  sum.pos_over = is_overflow;
  return sum;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  Fixedpoint diff; // declare/initialize variables
  uint64_t whole_diff = 0, frac_diff = 0;

  // |left| - -|right| = |left| + |right|
  if (!left.neg && right.neg) return fixedpoint_add(left, fixedpoint_negate(right));

  // -|left| - |right| = -(|left| + |right|)
  if (left.neg && !right.neg) {
    diff = fixedpoint_add(fixedpoint_negate(left), right);
    diff.neg = 1;
    diff.neg_over = diff.pos_over;
    diff.pos_over = 0;
    return diff;
  }

  // -|left| - -|right| = |right| - |left|
  if (left.neg && right.neg) {
    return fixedpoint_sub(fixedpoint_negate(right), fixedpoint_negate(left));
  }

  // return 0 if difference is 0
  if (!fixedpoint_compare(left, right)) return fixedpoint_create(0);

  // if left - right < 0, compute right - left and negate it
  if ((left.whole < right.whole) || ((left.whole == right.whole) && (left.frac < right.frac))) {
    diff = fixedpoint_sub(right, left);
    diff.neg = 1;
    return diff;
  }

  whole_diff = left.whole - right.whole; // compute whole difference

  // invert bits and add least significant bit to borrow 1 from whole if necessary
  if (right.frac > left.frac) {
    frac_diff = right.frac - left.frac;
    frac_diff = (~0UL) - frac_diff + 1;
    whole_diff--;
  }

  else frac_diff = left.frac - right.frac; // compute fractional difference if no borrow needed

  // return difference
  return fixedpoint_create2(whole_diff, frac_diff);
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  int temp;

  // if value is equal to zero, do nothing
  if (fixedpoint_is_zero(val)) return val;
  
  // set positive value to negative, negative values to positive
  val.neg = val.neg ? 0 : 1;

  // switch positive underflow and negative underflow status
  if (val.pos_under || val.neg_under) {
    temp = val.pos_under;
    val.pos_under = val.neg_under;
    val.neg_under = temp;
  }
  return val;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  uint64_t halved_whole = 0, halved_frac = 0;
  int whole_is_odd = val.whole & 1;

  // complication #1 with base case: frac looses information when shifted over by 1 bit because already 64th bit had information in it
  // need to mark as underflow (pos if val is pos, neg is val is neg)
  if (val.frac & 1) {
	  if (fixedpoint_is_neg(val)) val.neg_under = 1;
	  else val.pos_under = 1;
  }
  // base case: whole can get halved, fraction can get halved indepedently of each other (no carrying over)
  halved_whole = val.whole >> 1;
  halved_frac = val.frac >> 1;

  // complication #2 with base case: when whole get cut, 0.5 may need to be carried over to frac
  if (whole_is_odd) halved_frac += 1UL << 63;

  // else, base case, do nothing
  // return halved fixedpoint
  Fixedpoint half = fixedpoint_create2(halved_whole, halved_frac);

  // half inherits underflow statuses of its progenitor fixedpoint and negative status
  half.neg_under = val.neg_under;
  half.pos_under = val.pos_under;
  half.neg = val.neg;
  return half;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  // return fixedpoint_add(val, val);

  // create tracking for carrying and overflow
  uint64_t carry = 0, overflow = 0;

  // recognize need to carry over if last bit of frac is 1
  if (val.frac & (1UL << 63)) {
    carry = 1UL;
  }

  // recognize that overflow will occur if last bit of whole is 1
  if (val.whole & (1UL << 63)) {
    overflow = 1UL;
  }

  // do bit shifting to double
  val.frac = val.frac << 1;
  val.whole = val.whole << 1;

  // add carry to whole;
  val.whole += carry;

  // set overflow statuses, either positive or neg based on neg status of value doubled
  if (overflow) {
    if (fixedpoint_is_neg(val)) val.neg_over = 1;
    else val.pos_over = 1;
  }

  // return val
  return val;
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  // if values are equal, return 0
  if ( (left.whole == right.whole) && (left.frac == right.frac) && (left.neg == right.neg)) return 0;

  // if left is positive and right is negative, left is greater, return 1
  if (!left.neg) {
    if (right.neg) return 1;
    // is both values are pos, and |left| > |right|, left is greater, return 1
    if ( (left.whole > right.whole) || ( (left.whole == right.whole) && (left.frac > right.frac) )) return 1;
    // else, right is greater, return -1
    return -1;
  }

  // if right is positive, only option left is that left is negative. right is greater, return -1 
  if (!right.neg) return -1;

  // both values must be neg to reach this point, if |left| < |right|, left is greater, return 1
  if ( (left.whole < right.whole) || ( (left.whole == right.whole) && (left.frac < right.frac) )) return 1;
  // else return -1
  return -1;
}

int fixedpoint_is_zero(Fixedpoint val) {
  return (val.whole == 0 && val.frac == 0); // true if val is 0
}

int fixedpoint_is_err(Fixedpoint val) {
  return val.err; // true if error from invalid hex-string
}

int fixedpoint_is_neg(Fixedpoint val) {
  return val.neg; // true if val is negative
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  return val.neg_over; // true if occurence of negative overflow
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  return val.pos_over; // true if occurence of positive overflow
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  return val.neg_under; // true if occurence of negative underflow
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  return val.pos_under; // true if occurence of positive underflow
}

int fixedpoint_is_valid(Fixedpoint val) {
  // true if any occurence of any over/underflow or hex-string error
  return !(val.err || val.pos_over || val.neg_over || val.pos_under || val.neg_under);
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  char* hex = calloc(35, 1); // allocate memory for maximum length valid hex-string (16 whole, 16 frac, 1 '.', 1 '-')
  unsigned int index = 0; // initialize variables
  unsigned int *i_ptr = &index;

  if (fixedpoint_is_zero(val)) { // return '0' + null if zero
    hex[index] = '0';
    hex[index + 1] = '\0';
    return hex;
  }

  if (fixedpoint_is_neg(val)) { // insert '-' if negative - increment pointer
    hex[index] = '-';
    index++;
  }
  
  // convert whole to hex string and append - pointer incremented in function
  if (val.whole) dec_to_hex(val.whole, hex, i_ptr, 1);
  else { // append 0 - increment pointer
    hex[index] = '0';
    index++;
  }

  // convert fraction to hex string and append - pointer incremented in function (as well as '.' added)
  if (val.frac) dec_to_hex(val.frac, hex, i_ptr, 0);
  hex[index] = '\0'; // append null

  return hex; // return hex-string
}

void dec_to_hex(uint64_t val, char *hex, unsigned int* index, int whole) {
  int hex_int, non_zero = 0; // initialize and declare variables
  char hex_char;
  uint64_t remaining = val;

  if (!whole) { // append '.' if this val is fractional and increment pointer
    hex[*index] = '.';
    (*index)++;
  }

  for (int j = 15; j >= 0; j--) { // iterate through all 16 'groups of 4 bits'
    if (remaining == 0 && !whole) return; // return if there is no remaining value to convert to hexadecimal

    // shift 'group of 4' furthest right and take & with 15 (1111) - this gets rid of all bits further left (1111 has them as 0)
    // and retains the 4 bits now furthest on the right
    hex_int = (remaining >> (4 * j)) & 15;

    // subtract from the remainder the value shifted right then left to remove the hexdigits just added (left most)
    remaining -= (remaining >> (4 * j)) << (4 * j);

    // mark the val as non-zero at first instance of non-zero value. this way we ignore leading 0s
    if (!non_zero && hex_int) non_zero = 1;
    if (hex_int > 9) hex_char = 'a' + hex_int - 10; // convert int to hexadecimal character
    else hex_char = '0' + hex_int;

    if (non_zero || !whole) { // only append the character if it is not a leading 0 in the whole part
      hex[*index] = hex_char;
      (*index)++; // increment index
    }
  }
}

uint64_t hex_to_dec(const char *hex, int len, int is_whole, int* err) {
  uint64_t val = 0;
  int i = is_whole ? 0 : 15;

  // if empty sting was passed to this function, interpret as 0 and mark error
  if(len == 0) {
    *err=1;
    return 0;
  } 

  // if string is longer than 16, mark error
  if (len > 16) *err = 1;

  // for each char in string, translate alphanumeric chars (0-9, a-f/A-F) into bits
  for (int j = 0; j < len; ++j) {
    if (*hex >= '0' && *hex <= '9') val += (1UL << (4 * i)) * (*hex - '0');
    else if (*hex >= 'a' && *hex <= 'f') val += (1UL << (4 * i)) * (*hex - 'a' + 10);
    else if (*hex >= 'A' && *hex <= 'F') val += (1UL << (4 * i)) * (*hex - 'A' + 10);

    // if char is not expected alphanumeric (0-9, a-f/A-F), mark error
    else *err = 1;

    // if whole, move the pointer up; if frac, do opposite
    // (ex:12345, if whole, pointer would start on 5 and frac would start on 1)
    if (is_whole) { --hex; ++i; }
    else { ++hex; --i; }
  }

  // return created value
  return val;
}
