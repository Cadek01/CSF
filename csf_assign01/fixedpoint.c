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

// Create a Fixedpoint value representing an integer.
//
// Parameters:
//   whole - the value of the whole part of the representation
//
// Returns:
//   the Fixedpoint value
Fixedpoint fixedpoint_create(uint64_t whole) {
  // define this.whole as whole, this.frac as 0
  // define the remaining fields as false (neg, err, overflow tags)
  Fixedpoint fixedpoint = {whole, 0, 0, 0, 0, 0, 0, 0};
  return fixedpoint;
}

// Create a Fixedpoint value from specified whole and fractional values.
//
// Parametrs:
//   whole - the value of the whole part of the representation
//   frac - the value of the fractional part of the representation, where
//          the highest bit is the halves (2^-1) place, the second highest
//          bit is the fourths (2^-2) place, etc.
//
// Returns:
//   the Fixedpoint value
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

// Get the whole part of the given Fixedpoint value.
//
// Parameters:
//   val - a valid Fixedpoint value
//
// Returns:
//   a uint64_t value which is the whole part of the Fixedpoint value
uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole;
}

// Get the fractional part of the given Fixedpoint value.
//
// Parameters:
//   val - a valid Fixedpoint value
//
// Returns:
//   a uint64_t value which is the fractional part of the Fixedpoint value
uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac;
}

// Compute the sum of two valid Fixedpoint values.
//
// Parameters:
//   left - the left Fixedpoint value
//   right - the right Fixedpoint value
//
// Returns:
//   if the sum left + right is in the range of values that can be represented
//   exactly, the sum is returned;
//   if the sum left + right is not in the range of values that can be
//   represented, then a value for which either fixedpoint_is_overflow_pos or
//   fixedpoint_is_overflow_neg returns true is returned (depending on whether
//   the overflow was positive or negative)
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

// Compute the difference of two valid Fixedpoint values.
//
// Parameters:
//   left - the left Fixedpoint value
//   right - the right Fixedpoint value
//
// Returns:
//   if the difference left - right is in the range of values that can be represented
//   exactly, the difference is returned;
//   if the difference left - right is not in the range of values that can be
//   represented, then a value for which either fixedpoint_is_overflow_pos or
//   fixedpoint_is_overflow_neg returns true is returned (depending on whether
//   the overflow was positive or negative)
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

// Negate a valid Fixedpoint value.  (I.e. a value with the same magnitude but
// the opposite sign is returned.)  As a special case, the zero value is considered
// to be its own negation.
//
// Parameters:
//   val - a valid Fixedpoint value
//
// Returns:
//   the negation of val
Fixedpoint fixedpoint_negate(Fixedpoint val) {
  if (fixedpoint_is_zero(val)) return val; // if value is equal to zero, do nothing
  
  // set positive value to negative, negative values to positive
  val.neg = val.neg ? 0 : 1;
  return val;
}

// Return a Fixedpoint value that is exactly 1/2 the value of the given one.
//
// Parameters:
//   val - a valid Fixedpoint value
//
// Return:
//   a Fixedpoint value exactly 1/2 of the given one, if it can be represented exactly;
//   otherwise, a Fixedpoint value for which either fixedpoint_is_underflow_pos
//   or fixedpoint_is_underflow_neg returns true (depending on whether the
//   computed value would have been positive or negative)
Fixedpoint fixedpoint_halve(Fixedpoint val) {
  uint64_t halved_whole = 0, halved_frac = 0; // initalize values
  int whole_is_odd = val.whole & 1; // mark as odd if trailing bit is 1

  // underflow occurs if the 64th bit of fraction is a 1 (the value is lost)
  if (val.frac & 1) {
	  if (fixedpoint_is_neg(val)) val.neg_under = 1; // determine sign of underflow (halving yields same sign)
	  else val.pos_under = 1;
  }
  // divide whole and fraction by 2
  halved_whole = val.whole >> 1;
  halved_frac = val.frac >> 1;

  // carry over 1 to the halves bit if necessary
  // this will not affect underflow since halved fraction must have lead bit as 0
  if (whole_is_odd) halved_frac += 1UL << 63;

  // assign appropriate underflow/sign values and return
  Fixedpoint half = fixedpoint_create2(halved_whole, halved_frac);
  half.neg_under = val.neg_under;
  half.pos_under = val.pos_under;
  half.neg = val.neg;
  return half;
}

// Return a Fixedpoint value that is exactly twice the value of the given one.
//
// Parameters:
//   val - a valid Fixedpoint value
//
// Return:
//   a Fixedpoint value exactly twice the given one, if it can be represented exactly;
//   otherwise, a Fixedpoint value for which either fixedpoint_is_overflow_pos
//   or fixedpoint_is_overflow_neg returns true (depending on whether the
//   computed value would have been positive or negative)
Fixedpoint fixedpoint_double(Fixedpoint val) {
  return fixedpoint_add(val, val); // add val to itself to double
}

// Compare two valid Fixedpoint values.
//
// Parameters:
//   left - the left Fixedpoint value
//   right - the right Fixedpoint value
//
// Returns:
//    -1 if left < right;
//     0 if left == right;
//     1 if left > right
int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  // if values are equal, return 0
  if ( (left.whole == right.whole) && (left.frac == right.frac) && (left.neg == right.neg)) return 0;

  // if left is positive and right is negative, left is greater, return 1
  if (!left.neg) {
    if (right.neg) return 1;
    // for both >= 0, left is greater if it has greater whole, or greater frac and equal whole
    if ( (left.whole > right.whole) || ( (left.whole == right.whole) && (left.frac > right.frac) )) return 1;
    // otherwise it is less
    return -1;
  }

  // if right is positive, (at this point left must be negative), so right is greater
  if (!right.neg) return -1;

  // both values are negative, left is greater if its whole is less, or whole is equal and frac is less
  if ( (left.whole < right.whole) || ( (left.whole == right.whole) && (left.frac < right.frac) )) return 1;
  // otherwise right is greater
  return -1;
}

// Determine whether a Fixedpoint value is equal to 0.
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   1 if val is a valid Fixedpoint value equal to 0;
//   0 is val is not a valid Fixedpoint value equal to 0
int fixedpoint_is_zero(Fixedpoint val) {
  return (val.whole == 0 && val.frac == 0); // true if val is 0
}

// Determine whether a Fixedpoint value is an "error" value resulting
// from a call to fixedpoint_create_from_hex for which the argument
// string was invalid.
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   1 if val is the result of a call to fixedpoint_create_from_hex with
//   an invalid argument string;
//   0 otherwise
int fixedpoint_is_err(Fixedpoint val) {
  return val.err; // true if error from invalid hex-string
}

// Determine whether a Fixedpoint value is negative (less than 0).
//
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   1 if val is a valid value less than 0;
//   0 otherwise
int fixedpoint_is_neg(Fixedpoint val) {
  return val.neg && fixedpoint_is_valid(val); // true if val is negative and valid
}

// Determine whether a Fixedpoint value is the result of negative overflow.
// Negative overflow results when a sum, difference, or product is negative
// and has a magnitude that is too large to represent.
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   1 if val is the result of an operation where negative overflow occurred;
//   0 otherwise
int fixedpoint_is_overflow_neg(Fixedpoint val) {
  return val.neg_over;
}

// Determine whether a Fixedpoint value is the result of positive overflow.
// Positive overflow results when a sum, difference, or product is positive
// and has a magnitude that is too large to represent.
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   1 if val is the result of an operation where positive overflow occurred;
//   0 otherwise
int fixedpoint_is_overflow_pos(Fixedpoint val) {
  return val.pos_over;
}

// Determine whether a Fixedpoint value is the result of negative underflow.
// Negative underflow occurs when a division (i.e., fixedpoint_halve)
// produces a value that is negative, and can't be exactly represented because
// the fractional part of the representation doesn't have enough bits.
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   1 if val is the result of an operation where negative underflow occurred;
//   0 otherwise
int fixedpoint_is_underflow_neg(Fixedpoint val) {
  return val.neg_under;
}

// Determine whether a Fixedpoint value is the result of positive underflow.
// Positive underflow occurs when a division (i.e., fixedpoint_halve)
// produces a value that is positive, and can't be exactly represented because
// the fractional part of the representation doesn't have enough bits.
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   1 if val is the result of an operation where positive underflow occurred;
//   0 otherwise
int fixedpoint_is_underflow_pos(Fixedpoint val) {
  return val.pos_under;
}

// Determine whether a Fixedpoint value represents a valid negative or non-negative number.
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   1 if val represents a valid negative or non-negative number;
//   0 otherwise
int fixedpoint_is_valid(Fixedpoint val) {
  // true if any occurence of any over/underflow or hex-string error
  return !(val.err || val.pos_over || val.neg_over || val.pos_under || val.neg_under);
}

// Return a dynamically allocated C character string with the representation of
// the given valid Fixedpoint value.  The string should start with "-" if the
// value is negative, and should use the characters 0-9 and a-f to represent
// each hex digit of the whole and fractional parts. As a special case, if the
// Fixedpoint value represents an integer (i.e., the fractional part is 0),
// then no "decimal point" ('.') should be included.
//
// Parameters:
//   val - the Fixedpoint value
//
// Returns:
//   dynamically allocated character string containing the representation
//   of the Fixedpoint value
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

// Return a 64 bit value (computed in decimal) converted from a hexadecimal string
// Leading 0s in a whole number representation are ignored while trailing 0s are
// accounted for. The inverse is true for fractionally represented values
//
// Parameters:
//   hex - the hexadecimal string
//   len - the length of the string
//   is_whole - the representation of the string (1 - whole, 0 - fraction)
//   *err - the error state of the string
//
// Returns: appropriate decimal value for the inputted hexadecimal string
uint64_t hex_to_dec(const char *hex, int len, int is_whole, int* err) {
  uint64_t val = 0; // initialize value
  int i = is_whole ? 0 : 15; // start power at 0 for whole (right to left) and 15 for frac (left to right)

  // if either whole or frac longer than 16, denote error
  if (len > 16) *err = 1;

  // for each char in string, translate alphanumeric chars (0-9, a-f/A-F) into integer
  // shift the bits (multiply by powers of 2 in in/decrements of 4)
  for (int j = 0; j < len; ++j) {
    if (*hex >= '0' && *hex <= '9') val += (1UL << (4 * i)) * (*hex - '0');
    else if (*hex >= 'a' && *hex <= 'f') val += (1UL << (4 * i)) * (*hex - 'a' + 10);
    else if (*hex >= 'A' && *hex <= 'F') val += (1UL << (4 * i)) * (*hex - 'A' + 10);

    // if char is not expected alphanumeric (0-9, a-f/A-F), denote error
    else *err = 1;

    // if whole, move the pointer forward; if frac, backward - increment or decrement power
    // (ex:12345, if whole, pointer would start on 5 and frac would start on 1)
    if (is_whole) { --hex; ++i; }
    else { ++hex; --i; }
  }

  // return appropriate value
  return val;
}

// Append a hexadecimal string from a 64 bit value (computed in decimal) 
// to an existing dynamically allocated hexadecimal string
//
// Parameters:
//   val - 64 bit value
//   hex - dymanically allocated hex string
//   index - index at which to insert the converted string
//   whole - the representation of the value (1 - whole, 0 - fraction)
// 
// Returns: (void)
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
