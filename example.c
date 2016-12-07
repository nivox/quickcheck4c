/********************************************************************
 * Copyright (c) 2014, Andrea Zito
 * All rights reserved.
 *
 * License: BSD3
 ********************************************************************/

#include "quickcheck4c.h"

#include <stdio.h>
#include <string.h>

QCC_GenValue* myRangedGenInt10() {
  return QCC_genIntR(-5, 5);
}

QCC_GenValue* mySpecificLengthGenString10() {
  return QCC_genStringL(10);
}

QCC_TestStatus mulIntCommutativity(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);
  int b = *QCC_getValue(vals, 1, int*);

  if (a == b) QCC_label(stamp, "equal");
  if (a == 1 || b == 1) QCC_label(stamp, "null");
  if (a < 0) QCC_label(stamp, "a-negative");
  if (b < 0) QCC_label(stamp, "b-negative");

  return a*b == b*a;
}

QCC_TestStatus mulIntAssociativity(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);
  int b = *QCC_getValue(vals, 1, int*);
  int c = *QCC_getValue(vals, 2, int*);

  if (a == b  && a == c) QCC_label(stamp, "equal");
  if (a == 0 || b == 0 || c == 0) QCC_label(stamp, "null");
  if (a < 0) QCC_label(stamp, "a-negative");
  if (b < 0) QCC_label(stamp, "b-negative");
  if (c < 0) QCC_label(stamp, "c-negative");

  return (a*b)*c == a*(b*c);
}

QCC_TestStatus mulFloatCommutativity(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  float a = *QCC_getValue(vals, 0, float*);
  float b = *QCC_getValue(vals, 1, float*);

  if (a == b) QCC_label(stamp, "equal");
  if (a == 1 || b == 1) QCC_label(stamp, "null");
  if (a < 0) QCC_label(stamp, "a-negative");
  if (b < 0) QCC_label(stamp, "b-negative");

  return a*b == b*a;
}

QCC_TestStatus mulFloatAssociativity(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  float a = *QCC_getValue(vals, 0, float*);
  float b = *QCC_getValue(vals, 1, float*);
  float c = *QCC_getValue(vals, 2, float*);

  if (a == b && a == c) QCC_label(stamp, "equal");
  if (a == 0 || b == 0 || c == 0) QCC_label(stamp, "null");
  if (a < 0) QCC_label(stamp, "a-negative");
  if (b < 0) QCC_label(stamp, "b-negative");
  if (c < 0) QCC_label(stamp, "c-negative");

  return (a*b)*c == a*(b*c);
}

QCC_TestStatus sumEvenInt(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);
  int b = *QCC_getValue(vals, 1, int*);

  return (a + b) % 2 == 0;
}

QCC_TestStatus sumEvenKindInt(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);
  int b = *QCC_getValue(vals, 1, int*);

  if (a % 2 == 0 && b % 2 == 0) QCC_label(stamp, "even");
  if (a % 2 != 0 && b % 2 != 0) QCC_label(stamp, "odd");

  return QCC_imply(a % 2 == b % 2,  (a + b) % 2 == 0);
}

QCC_TestStatus rangedIntGen(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);

  char *lbl = vals[0]->show(&a, vals[0]->n);
  QCC_label(stamp, lbl);
  free(lbl);

  return a >= -5 && a <= 5;
}

QCC_TestStatus specificLengthGen(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  char* str = QCC_getValue(vals, 0, char *);

  char *s = (char *) calloc(3, sizeof(char));
  snprintf(s, 3*sizeof(char), "%d", (int) strlen(str));
  QCC_label(stamp, s);

  return strlen(str) < 10;
}

QCC_TestStatus charGen(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  char str = *QCC_getValue(vals, 0, char *);

  char *s = (char *) calloc(2, sizeof(char));
  s[0] = str;
  s[1] = '\0';
  QCC_label(stamp, s);
  free(s);

  return 1;
}

QCC_TestStatus conjunction(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);
  int b = *QCC_getValue(vals, 1, int*);

  return QCC_and(QCC_imply(a % 2 == 0, (a * a) % 2 == 0),
                 QCC_imply(b % 2 != 0, (b * b) % 2 != 0));
}

QCC_TestStatus disjunction(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);
  int b = *QCC_getValue(vals, 1, int*);

  return QCC_or(QCC_imply(a % 2 == b % 2, (a + b) % 2 == 0),
                QCC_imply(b % 2 != b % 2, (a + b) % 2 != 0));
}

QCC_TestStatus xdisjunction(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);

  return  QCC_xor(QCC_imply(a > 0, a % 2 == 0),
                  QCC_imply(a > 0, a % 2 != 0));
}



int main(int argc, char **argv) {
  QCC_init(0);

  printf("Testing int multiplication commutativity\n");
  QCC_testForAll(100, 1000, mulIntCommutativity, 2, QCC_genInt, QCC_genInt);

  printf("Testing int multiplication associativity\n");
  QCC_testForAll(100, 1000, mulIntAssociativity, 3, QCC_genInt, QCC_genInt, QCC_genInt);

  printf("Testing float multiplication commutativity\n");
  QCC_testForAll(100, 1000, mulFloatCommutativity, 2, QCC_genFloat, QCC_genFloat);

  printf("Testing float multiplication associativity\n");
  QCC_testForAll(100, 1000, mulFloatAssociativity, 3, QCC_genFloat, QCC_genFloat, QCC_genFloat);

  printf("Testing int sum to even\n");
  QCC_testForAll(100, 1000, sumEvenInt, 2, QCC_genInt, QCC_genInt);

  printf("Testing int sum to even (same kind)\n");
  QCC_testForAll(1000, 10000, sumEvenKindInt, 2, QCC_genInt, QCC_genInt);

  printf("Testing ranged int generator\n");
  QCC_testForAll(100, 1000, rangedIntGen, 1, myRangedGenInt10);

  printf("Testing char generation\n");
  QCC_testForAll(100, 1000, charGen, 1, QCC_genChar);

  printf("Testing specific-length string generator\n");
  QCC_testForAll(100, 1000, specificLengthGen, 1, mySpecificLengthGenString10);

  printf("Testing property conjunction\n");
  QCC_testForAll(100, 1000, conjunction, 2, QCC_genInt, QCC_genInt);

  printf("Testing property disjunction\n");
  QCC_testForAll(100, 1000, disjunction, 2, QCC_genInt, QCC_genInt);

  printf("Testing property exclusive disjunction\n");
  QCC_testForAll(100, 1000, xdisjunction, 1, QCC_genInt);

  return 0;
}
