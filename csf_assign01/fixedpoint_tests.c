/*
 * Fixedpoint functions testing for fixedpoint libary
 * CSF Assignment 1 MS2
 * M.Albert and R.Sivananthan
 * malber20@jhu.edu and rsivana1@jhu.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "tctest.h"

// Test fixture object, has some useful values for testing
typedef struct {
  Fixedpoint zero;
  Fixedpoint one;
  Fixedpoint one_half;
  Fixedpoint one_fourth;
  Fixedpoint large1;
  Fixedpoint large2;
  Fixedpoint min_magnitude;
  Fixedpoint max;
  Fixedpoint min;
} TestObjs;

// functions to create and destroy the test fixture
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_whole_part(TestObjs *objs);
void test_frac_part(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_is_err(TestObjs *objs);

void test_double(TestObjs *objs);
void test_halve(TestObjs *objs);

int main(int argc, char **argv) {
  // if a testname was specified on the command line, only that
  // test function will be executed
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_whole_part);
  TEST(test_frac_part);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_negate);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_is_err);

  TEST(test_double);
  TEST(test_halve);

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));

  objs->zero = fixedpoint_create(0UL);
  objs->one = fixedpoint_create(1UL);
  objs->one_half = fixedpoint_create2(0UL, 0x8000000000000000UL);
  objs->one_fourth = fixedpoint_create2(0UL, 0x4000000000000000UL);
  objs->large1 = fixedpoint_create2(0x4b19efceaUL, 0xec9a1e2418UL);
  objs->large2 = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
  objs->max = fixedpoint_create2(0xffffffffffffffffUL, 0xffffffffffffffffUL);

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_whole_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
}

void test_frac_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_create_from_hex(TestObjs *objs) {
  (void) objs;

  // given test
  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");
  ASSERT(fixedpoint_is_valid(val1));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));

  // uppercase
  Fixedpoint val2 = fixedpoint_create_from_hex("F3A5265.00F5");
  ASSERT(fixedpoint_is_valid(val2));
  ASSERT(0xf3a5265UL == fixedpoint_whole_part(val2));
  ASSERT(0x00f5000000000000UL == fixedpoint_frac_part(val2));

  // only frac
  val1 = fixedpoint_create_from_hex(".092f");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0x092f000000000000UL == fixedpoint_frac_part(val1));
  ASSERT(!fixedpoint_is_err(val1));

  // only whole
  val1 = fixedpoint_create_from_hex("4cda6b");
  ASSERT(0x4cda6bUL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // whole + .
  val1 = fixedpoint_create_from_hex("9cdaaaa6b23.");
  ASSERT(0x9cdaaaa6b23UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // too long whole
  val1 = fixedpoint_create_from_hex("1234abcd5678ef123.a2");
  ASSERT(fixedpoint_is_err(val1));

  // max length whole
  val1 = fixedpoint_create_from_hex("1234abcd5678ef12.a2");
  ASSERT(0x1234abcd5678ef12UL == fixedpoint_whole_part(val1));
  ASSERT(0xa200000000000000UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // too long frac
  val1 = fixedpoint_create_from_hex(".1234abcd5678ef123");
  ASSERT(fixedpoint_is_err(val1));

  // max length frac
  val1 = fixedpoint_create_from_hex("fffffff.1234abcd5678ef12");
  ASSERT(0xfffffffUL == fixedpoint_whole_part(val1));
  ASSERT(0x1234abcd5678ef12UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // too long both
  val1 = fixedpoint_create_from_hex("ffffffffFfffFfffffff.aaaaaaaaaaaaaaaaaaaa");
  ASSERT(fixedpoint_is_err(val1));

  // max length both
  val1 = fixedpoint_create_from_hex("fFffffffffffffff.aaaaaaaaaaaaaAAa");
  ASSERT(0xffffffffffffffffUL == fixedpoint_whole_part(val1));
  ASSERT(0xaaaaaaaaaaaaaaaaUL == fixedpoint_frac_part(val1));
  
  // "" is valid and generates 0.0
  val1 = fixedpoint_create_from_hex("");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // "." is valid and generates 0.0
  val1 = fixedpoint_create_from_hex(".");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // "0" is valid and generates 0.0
  val1 = fixedpoint_create_from_hex("0");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // "0.0" is valid and generates 0.0
  val1 = fixedpoint_create_from_hex("0.0");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // "0." is valid and generates 0.0
  val1 = fixedpoint_create_from_hex("0.");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));

  // ".  " is valid and gererates 0.0
  val1 = fixedpoint_create_from_hex(". ");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_err(val1));

  // "  " is invalid
  val1 = fixedpoint_create_from_hex("  ");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_err(val1));

  // "0.0.0" is invalid
  val1 = fixedpoint_create_from_hex("0.0.0");
  ASSERT(fixedpoint_is_err(val1));
  
  // "0..0" is invalid
  val1 = fixedpoint_create_from_hex("0..0");
  ASSERT(fixedpoint_is_err(val1));

  // "-0.0" is valid and generates 0.0
  val1 = fixedpoint_create_from_hex("-0.0");
  ASSERT(0UL == fixedpoint_whole_part(val1));
  ASSERT(0UL == fixedpoint_frac_part(val1));
  ASSERT(fixedpoint_is_valid(val1));
}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  // given tests

  s = fixedpoint_format_as_hex(objs->zero);
  ASSERT(0 == strcmp(s, "0"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one);
  ASSERT(0 == strcmp(s, "1"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_half);
  ASSERT(0 == strcmp(s, "0.8"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_fourth);
  ASSERT(0 == strcmp(s, "0.4"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large1);
  ASSERT(0 == strcmp(s, "4b19efcea.000000ec9a1e2418"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large2);
  ASSERT(0 == strcmp(s, "fcbf3d5.00004d1a23c24faf"));
  free(s);
}

void test_negate(TestObjs *objs) {
  // none of the test fixture objects are negative
  ASSERT(!fixedpoint_is_neg(objs->zero));
  ASSERT(!fixedpoint_is_neg(objs->one));
  ASSERT(!fixedpoint_is_neg(objs->one_half));
  ASSERT(!fixedpoint_is_neg(objs->one_fourth));
  ASSERT(!fixedpoint_is_neg(objs->large1));
  ASSERT(!fixedpoint_is_neg(objs->large2));

  // negate the test fixture values
  Fixedpoint zero_neg = fixedpoint_negate(objs->zero);
  Fixedpoint one_neg = fixedpoint_negate(objs->one);
  Fixedpoint one_half_neg = fixedpoint_negate(objs->one_half);
  Fixedpoint one_fourth_neg = fixedpoint_negate(objs->one_fourth);
  Fixedpoint large1_neg = fixedpoint_negate(objs->large1);
  Fixedpoint large2_neg = fixedpoint_negate(objs->large2);

  // zero does not become negative when negated
  ASSERT(!fixedpoint_is_neg(zero_neg));

  // all of the other values should have become negative when negated
  ASSERT(fixedpoint_is_neg(one_neg));
  ASSERT(fixedpoint_is_neg(one_half_neg));
  ASSERT(fixedpoint_is_neg(one_fourth_neg));
  ASSERT(fixedpoint_is_neg(large1_neg));
  ASSERT(fixedpoint_is_neg(large2_neg));

  // magnitudes should stay the same
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_add(TestObjs *objs) {
  (void) objs;
  
  // given test
  Fixedpoint lhs, rhs, sum; //use these initializiations for following tests
  lhs = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
  rhs = fixedpoint_create_from_hex("d09079.1e6d601");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_neg(sum));
  ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum));
  ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum));

  // normal addition test
  lhs = fixedpoint_create_from_hex("a1.8");
  rhs = fixedpoint_create_from_hex("1.c");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(0xa3UL == fixedpoint_whole_part(sum));
  ASSERT(0x4000000000000000UL  == fixedpoint_frac_part(sum));
  ASSERT(!fixedpoint_is_neg(sum));

  // positive and positive, positive overflow
  lhs = fixedpoint_create_from_hex("ffffffffffffffff.0");
  rhs = fixedpoint_create_from_hex("ffffffffffffffff.0");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  // positive and negative
  lhs = fixedpoint_create_from_hex("ffffffffffffffff.0");
  rhs = fixedpoint_create_from_hex("-fffffffffffffffe.0");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(0x0000000000000001UL == fixedpoint_whole_part(sum));
  ASSERT(0UL == fixedpoint_frac_part(sum));
  ASSERT(!fixedpoint_is_neg(sum));


  // negative and positive
  lhs = fixedpoint_create_from_hex("-ffffffffffffffff.0");
  rhs = fixedpoint_create_from_hex("fffffffffffffffe.0");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(0x0000000000000001UL == fixedpoint_whole_part(sum));
  ASSERT(fixedpoint_is_neg(sum));

  // negative and negative 
  lhs = fixedpoint_create_from_hex("-1.0");
  rhs = fixedpoint_create_from_hex("-2.0");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(0x0000000000000003UL == fixedpoint_whole_part(sum));
  ASSERT(0UL == fixedpoint_frac_part(sum));
  ASSERT(fixedpoint_is_neg(sum));

  // negative and negative, negative overflow
  lhs = fixedpoint_create_from_hex("-ffffffffffffffff.0");
  rhs = fixedpoint_create_from_hex("-ffffffffffffffff.0");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_overflow_neg(sum));
}


void test_sub(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs, rhs, diff;

  // given test
  lhs = fixedpoint_create_from_hex("-ccf35aa3a04a3b.b105");
  rhs = fixedpoint_create_from_hex("f676e8.58");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_neg(diff));
  ASSERT(0xccf35aa496c124UL == fixedpoint_whole_part(diff));
  ASSERT(0x0905000000000000UL == fixedpoint_frac_part(diff));

  // 0 - 0 = 0
  lhs = fixedpoint_create_from_hex("0.0");
  rhs = fixedpoint_create_from_hex("0.0");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(!fixedpoint_is_neg(diff));
  ASSERT(fixedpoint_is_zero(diff));
  ASSERT(0UL == fixedpoint_whole_part(diff));
  ASSERT(0UL == fixedpoint_frac_part(diff));

  // positive - positive
  lhs = fixedpoint_create_from_hex("ce8ae.2b7");
  rhs = fixedpoint_create_from_hex("c7ad8.7a730");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(0x6DD5 == fixedpoint_whole_part(diff));
  ASSERT(0XB0FD000000000000 == fixedpoint_frac_part(diff));
  
  // positive - negative
  lhs = fixedpoint_create_from_hex("65.9b85a0a14fc6");
  rhs = fixedpoint_create_from_hex("-ab34f892357ec2.2419b3");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(0xAB34F892357F27 == fixedpoint_whole_part(diff));
  ASSERT(0xBF9F53A14FC60000 == fixedpoint_frac_part(diff));

  // positive - negative
  lhs = fixedpoint_create_from_hex("8.74e77ff82d1ea3e");
  rhs = fixedpoint_create_from_hex("87be5.e22");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(0x87BDD == fixedpoint_whole_part(diff));
  ASSERT(0x6D388007D2E15C20 == fixedpoint_frac_part(diff));

  // positive - negative w/ overflow
  lhs = fixedpoint_create_from_hex("ffffffffffffffff");
  rhs = fixedpoint_create_from_hex("-0000000000000001");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_overflow_pos(diff));
  ASSERT(!fixedpoint_is_overflow_neg(diff));

  // positive - negative w/ pos overflow because of carrying-over
  lhs = fixedpoint_create_from_hex("ffffffffffffffff.f");
  rhs = fixedpoint_create_from_hex("-0000000000000000.1");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_overflow_pos(diff));
  ASSERT(!fixedpoint_is_overflow_neg(diff));

  // negative - positive
  lhs = fixedpoint_create_from_hex("-587ed665bfc.5cfbe37");
  rhs = fixedpoint_create_from_hex("78fac8.20b2c76");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_neg(diff));
  ASSERT(0x587EDDF56C4 == fixedpoint_whole_part(diff));
  ASSERT(0x7DAEAAD000000000 == fixedpoint_frac_part(diff));
  
  // negative - positive w/ overflow
  lhs = fixedpoint_create_from_hex("-0000000000000001");
  rhs = fixedpoint_create_from_hex("ffffffffffffffff");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(!fixedpoint_is_overflow_pos(diff));
  ASSERT(fixedpoint_is_overflow_neg(diff));

  // negative - positive w/ neg overflow because of carrying-over
  lhs = fixedpoint_create_from_hex("-ffffffffffffffff.f");
  rhs = fixedpoint_create_from_hex("0000000000000000.1");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_overflow_neg(diff));
  ASSERT(!fixedpoint_is_overflow_pos(diff));
  
  // negative - negative
  
}

void test_is_err(TestObjs *objs) {
  (void) objs;

  // too many characters
  Fixedpoint err1 = fixedpoint_create_from_hex("88888888888888889.6666666666666666");
  ASSERT(fixedpoint_is_err(err1));

  // too many characters
  Fixedpoint err2 = fixedpoint_create_from_hex("6666666666666666.88888888888888889");
  ASSERT(fixedpoint_is_err(err2));

  // this one is actually fine
  Fixedpoint err3 = fixedpoint_create_from_hex("-6666666666666666.8888888888888888");
  ASSERT(fixedpoint_is_valid(err3));
  ASSERT(!fixedpoint_is_err(err3));

  // whole part is too large
  Fixedpoint err4 = fixedpoint_create_from_hex("88888888888888889");
  ASSERT(fixedpoint_is_err(err4));

  // fractional part is too large
  Fixedpoint err5 = fixedpoint_create_from_hex("7.88888888888888889");
  ASSERT(fixedpoint_is_err(err5));

  // invalid hex digits in whole part
  Fixedpoint err6 = fixedpoint_create_from_hex("123xabc.4");
  ASSERT(fixedpoint_is_err(err6));

  // invalid hex digits in fractional part
  Fixedpoint err7 = fixedpoint_create_from_hex("7.0?4");
  ASSERT(fixedpoint_is_err(err7));
}

void test_double(TestObjs *objs) {
  (void) objs;

  Fixedpoint val, doubled_val;

  // test simple doubling
  ASSERT(fixedpoint_whole_part(fixedpoint_double(objs->one_fourth))
		 == fixedpoint_whole_part(objs->one_half));
  ASSERT(fixedpoint_frac_part(fixedpoint_double(objs->one_fourth))
		 == fixedpoint_frac_part(objs->one_half));
  
  // test carrying over
  ASSERT(fixedpoint_whole_part(fixedpoint_double(objs->one_half))
		 == fixedpoint_whole_part(objs->one));
  ASSERT(fixedpoint_frac_part(fixedpoint_double(objs->one_half))
		 == fixedpoint_frac_part(objs->one)); 
  
  // test overflow positive
  doubled_val = fixedpoint_double(objs->max);
  ASSERT(fixedpoint_is_overflow_pos(doubled_val));
  ASSERT(!fixedpoint_is_overflow_neg(doubled_val));
  
  // test overflow negative
  val = fixedpoint_negate(objs->max);
  doubled_val = fixedpoint_double(val);
  ASSERT(!fixedpoint_is_overflow_pos(doubled_val));
  ASSERT(fixedpoint_is_overflow_neg(doubled_val));
  
}

void test_halve(TestObjs *objs) {
  (void) objs;

  Fixedpoint val, halved_val;

  // test simple halving
  ASSERT(fixedpoint_whole_part(fixedpoint_halve(objs->one_half)) ==
		  fixedpoint_whole_part(objs->one_fourth));
  ASSERT(fixedpoint_frac_part(fixedpoint_halve(objs->one_half)) ==
		  fixedpoint_frac_part(objs->one_fourth));
  
  // test 'carrying under' 
  ASSERT(fixedpoint_whole_part(fixedpoint_halve(objs->one)) ==
		  fixedpoint_whole_part(objs->one_half));
  ASSERT(fixedpoint_frac_part(fixedpoint_halve(objs->one)) ==
		fixedpoint_frac_part(objs->one_half));
  
  // test underflow positive
  val = fixedpoint_create_from_hex("0.0000000000000001");
  halved_val = fixedpoint_halve(val);
  ASSERT(fixedpoint_is_underflow_pos(halved_val));
  ASSERT(!fixedpoint_is_underflow_neg(halved_val));
  
  // test underflow negative
  val = fixedpoint_negate(fixedpoint_create_from_hex("0.0000000000000001"));
  halved_val = fixedpoint_halve(val);
  ASSERT(!fixedpoint_is_underflow_pos(halved_val));
  ASSERT(fixedpoint_is_underflow_neg(halved_val));
  
}

