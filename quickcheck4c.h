/********************************************************************
 * Copyright (c) 2014, Andrea Zito
 * All rights reserved.
 *
 * License: BSD3
 ********************************************************************/

#ifndef QUICKCHECK4C_C
#define QUICKCHECK4C_C

#include <stdlib.h>
#include <stdarg.h>

#define QCC_getValue(vals, idx, type) ( (type) vals[idx]->value )

#define QCC_imply(precondition, property) precondition ? property : QCC_NOTHING

typedef enum { QCC_TRUE=1, QCC_FALSE=0 } QCC_Boolean;

typedef char* (*QCC_showValue)(void *value);
typedef void (*QCC_freeValue)(void *value);

typedef struct QCC_GenValue {
  void *value;
  int n;
  QCC_showValue show;
  QCC_freeValue free;
} QCC_GenValue;

typedef enum { QCC_OK=1, QCC_FAIL=0, QCC_NOTHING=-1 } QCC_TestStatus;

typedef struct QCC_Stamp {
  char *label;
  int n;
  struct QCC_Stamp *next;
} QCC_Stamp;

typedef struct QCC_Result {
  QCC_TestStatus status;
  QCC_Stamp *stamps;
  QCC_GenValue **arguments;
  int argumentsN;
} QCC_Result;

typedef void (*QCC_genRaw)(void *ptr);
typedef QCC_GenValue* (*QCC_gen)();
typedef QCC_TestStatus (*QCC_property)(QCC_GenValue **vals, int len, QCC_Stamp **stamps);

void QCC_init(int seed);
QCC_GenValue* QCC_initGenValue(void *value, int n, QCC_showValue show, QCC_freeValue free);

QCC_Result QCC_forAll(QCC_property prop, int genNum, ...); //QCC_gen *genLst,
QCC_Result QCC_vforAll(QCC_property prop, int genNum, va_list genLst); //QCC_gen *genLst,

void QCC_testForAll(int num, int maxFail, QCC_property prop, int genNum, ...);

void QCC_label(QCC_Stamp **stamps, char *label);

QCC_GenValue* QCC_genLong();
QCC_GenValue* QCC_genInt();
QCC_GenValue* QCC_genDouble();
QCC_GenValue* QCC_genFloat();
QCC_GenValue* QCC_genBoolean();
QCC_GenValue* QCC_genChar();

QCC_GenValue* QCC_genArrayOf(QCC_genRaw elemGen, size_t elemSize, QCC_showValue show, QCC_freeValue free);
QCC_GenValue* QCC_genString();
QCC_GenValue* QCC_genArrayLong();
QCC_GenValue* QCC_genArrayInt();
QCC_GenValue* QCC_genArrayDouble();
QCC_GenValue* QCC_genArrayFloat();
QCC_GenValue* QCC_genArrayBoolean();
QCC_GenValue* QCC_genArrayChar();
#endif
