/********************************************************************
 * Copyright (c) 2014, Andrea Zito
 * All rights reserved.
 *
 * License: BSD3
 ********************************************************************/

#include "quickcheck4c.h"

#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

void QCC_init(int seed) {
  if (seed) srandom(seed);
  else srandom(time(NULL));
}

/***********************************************************************
 *  Generators helper functions
 ***********************************************************************/
enum QCC_deref_type { NONE, LONG, INT, FLOAT, DOUBLE, CHAR };
static char* QCC_showSimpleValue(void *value, enum QCC_deref_type dt, int maxsize, const char *format) {
  char *vc = malloc(sizeof(char) * maxsize+1);

  switch (dt) {
  case LONG:
    snprintf(vc, maxsize, format, *(long*)value);
    break;
  case INT:
    snprintf(vc, maxsize, format, *(int*)value);
    break;
  case FLOAT:
    snprintf(vc, maxsize, format, *(float*)value);
    break;
  case DOUBLE:
    snprintf(vc, maxsize, format, *(double*)value);
    break;
  case CHAR:
    snprintf(vc, maxsize, format, *(char*)value);
    break;
  default:
    snprintf(vc, maxsize, format, *(int*)value);
    break;
  }
  vc[maxsize]=0;
  return vc;
}

static void QCC_freeSimpleValue(void *value) {
  free(value);
}

QCC_GenValue* QCC_initGenValue(void *value, int n, QCC_showValue show, QCC_freeValue free) {
  QCC_GenValue *gv = malloc(sizeof(QCC_GenValue));
  *gv = (QCC_GenValue) { .value=value, .n=n, .show=show, .free=free };

  return gv;
}


/***********************************************************************
 *  Generators implementations
 ***********************************************************************/

static char* QCC_showLong(void *value) {
  return QCC_showSimpleValue(value, LONG, 21, "%ld");
}

void QCC_genLongAt(long *l) {
  *l = random() * (random() % 2 ? 1 : -1);
}

QCC_GenValue* QCC_genLong() {
  long *v = malloc(sizeof(long));
  QCC_genLongAt(v);

  return QCC_initGenValue(v, 1, QCC_showLong, QCC_freeSimpleValue);
}

static char* QCC_showInt(void *value) {
  return QCC_showSimpleValue(value, INT, 11, "%d");
}

void QCC_genIntAt(int *i) {
  *i = (int) (random() % INT_MAX) * (random() % 2 ? 1 : -1);
}

QCC_GenValue* QCC_genInt() {
  int *v = malloc(sizeof(int));
  QCC_genIntAt(v);

  return QCC_initGenValue(v, 1, QCC_showInt, QCC_freeSimpleValue);
}

static char* QCC_showDouble(void *value) {
  return QCC_showSimpleValue(value, DOUBLE, 50, "%.12e");
}

void QCC_genDoubleAt(double *d) {
  *d = (double)random() / (double) RAND_MAX;
  *d += (int) (random() % INT_MAX);
  *d *= random() % 2 ? 1 : -1;
}

QCC_GenValue* QCC_genDouble() {
  double *v = malloc(sizeof(double));
  QCC_genDoubleAt(v);
  return QCC_initGenValue(v, 1, QCC_showDouble, QCC_freeSimpleValue);
}

static char* QCC_showFloat(void *value) {
  return QCC_showSimpleValue(value, FLOAT, 50, "%.12e");
}

void QCC_genFloatAt(float *f) {
  *f = (float)random() / (float) RAND_MAX;
  *f += (int) (random() % INT_MAX);
  *f *= random() % 2 ? 1 : -1;
}

QCC_GenValue* QCC_genFloat() {
  float *v = malloc(sizeof(float));
  QCC_genFloatAt(v);

  return QCC_initGenValue(v, 1, QCC_showFloat, QCC_freeSimpleValue);
}

static char* QCC_showBoolean(void *value) {
  QCC_Boolean *b = (QCC_Boolean *)value;

  if (b) return "TRUE";
  else return "FALSE";
}

void QCC_genBooleanAt(QCC_Boolean *b) {
  double r = (double)random() / (double) RAND_MAX;
  *b = r > 0.5 ? QCC_TRUE : QCC_FALSE;
}

QCC_GenValue* QCC_genBoolean() {
  QCC_Boolean *v = malloc(sizeof(QCC_Boolean));
  QCC_genBooleanAt(v);

  return QCC_initGenValue(v, 1, QCC_showBoolean, QCC_freeSimpleValue);
}

static char* QCC_showChar(void *value) {
  return QCC_showSimpleValue(value, CHAR, 1, "%c");
}

void QCC_genCharAt(char *c) {
  *c = (char) (random() % 93) + 33;
}

QCC_GenValue* QCC_genChar() {
  char *v = malloc(sizeof(char));
  QCC_genCharAt(v);

  return QCC_initGenValue(v, 1, QCC_showChar, QCC_freeSimpleValue);
}


/***********************************************************************
 *  Array generators implementations
 ***********************************************************************/

QCC_GenValue* QCC_genArrayOf(QCC_genRaw elemGen, size_t elemSize, QCC_showValue show, QCC_freeValue free) {
  int n = (int) random() % 100;

  uint8_t *arr = malloc(n*elemSize);

  int p,i;
  for(i=0, p=0; i<n; i++, p += elemSize) {
    elemGen(arr+p);
  }

  return QCC_initGenValue(arr, n, show, free);
}

static char *QCC_showString(void *value) {
  return QCC_showSimpleValue(value, NONE, 100, "%s");
}

QCC_GenValue* QCC_genString() {
  QCC_GenValue *s = QCC_genArrayOf((QCC_genRaw) QCC_genCharAt, sizeof(char), QCC_showString, QCC_freeSimpleValue);
  ((char *)s->value)[s->n-1] = '\0';
  return s;
}

/***********************************************************************
 *  Categorization function
 ***********************************************************************/
void QCC_freeStamp(QCC_Stamp *stamps) {
  QCC_Stamp *tstamps;
  QCC_Stamp *curr;
  for (tstamps=stamps; tstamps != NULL;) {
    curr = tstamps;
    tstamps = tstamps->next;
    free(curr->label);
    free(curr);
  }
}

void QCC_label(QCC_Stamp **stamps, char *label) {
  QCC_Stamp *ptr = *stamps;
  QCC_Stamp *pre = NULL;
  QCC_Stamp *new;

  while(ptr) {
    if (strcmp(ptr->label, label) == 0) {
      (ptr->n)++;
      return;
    }
    pre = ptr;
    ptr = ptr->next;
  }

  new = malloc(sizeof(QCC_Stamp));
  *new = (QCC_Stamp) { .label = strdup(label), .n=1, .next = NULL };

  if (pre) pre->next = new;
  else *stamps = new;
}

/***********************************************************************
 *  Testing functions
 ***********************************************************************/
void QCC_freeGenValues(QCC_GenValue **arguments, int argumentsN) {
  int i;
  for (i=0; i<argumentsN; i++) {
    arguments[i]->free(arguments[i]->value);
    free(arguments[i]);
  }
  if (arguments) free(arguments);
}

QCC_Result QCC_vforAll(QCC_property prop, int genNum, va_list genP) { //QCC_gen *genLst,
  QCC_GenValue **vals = NULL;
  if (genNum) {
    int i;
    vals = malloc(sizeof(QCC_GenValue*) * genNum);
    for (i=0; i<genNum; i++) {
      QCC_gen gen = va_arg(genP, QCC_gen);
      vals[i] = gen();
    }
  }

  QCC_Stamp *stamps = NULL;
  QCC_TestStatus status = prop(vals, genNum, &stamps);
  return (QCC_Result) {
    .status = status,
      .stamps = stamps,
      .arguments = vals,
      .argumentsN = genNum
      };
}

QCC_Result QCC_forAll(QCC_property prop, int genNum, ...) { //QCC_gen *genLst,
  va_list genP;
  QCC_Result res;
  va_start(genP, genNum);
  res = QCC_vforAll(prop, genNum, genP);
  va_end(genP);
  return res;
}

void QCC_testForAll(int num, int maxFail, QCC_property prop, int genNum, ...) {
  va_list genP;
  int succ = 0;
  int fail = 0;

  QCC_Result res = { .status=QCC_OK };
  QCC_Stamp *stamps = NULL;
  QCC_Stamp *tstamps;
  while (succ < num && fail < maxFail) {
    va_start(genP, genNum);
    res = QCC_vforAll(prop, genNum, genP);
    va_end(genP);

    if (res.status == QCC_FAIL) {
      break;
    } else {
      if (res.status == QCC_OK) {
        succ++;
        for (tstamps=res.stamps; tstamps != NULL; tstamps=tstamps->next)
          QCC_label(&stamps, tstamps->label);
      } else fail++;

      QCC_freeStamp(res.stamps);
      QCC_freeGenValues(res.arguments, res.argumentsN);
    }
  }

  if (succ == num) {
    printf("%d test passed (%d)!\n", succ, fail);
    for (tstamps=stamps; tstamps != NULL; tstamps=tstamps->next)
      printf("%.2f%%\t%s\n", (tstamps->n/(float)succ)*100, tstamps->label);
    if (stamps) QCC_freeStamp(stamps);
  } else if (res.status == QCC_FAIL) {
    int i;
    printf("Falsifiable after %d test\n", succ+1);
    for (i=0; i<res.argumentsN; i++) {
      printf("%s\n", res.arguments[i]->show(res.arguments[i]->value));
    }
    QCC_freeStamp(res.stamps);
    QCC_freeGenValues(res.arguments, res.argumentsN);
  } else if (fail >= maxFail) {
    printf("Gave up after %d tests!\n", succ);
    for (tstamps=stamps; tstamps != NULL; tstamps=tstamps->next)
      printf("%.2f%%\t%s\n", (tstamps->n/(float)succ)*100, tstamps->label);
    if (stamps) QCC_freeStamp(stamps);
  }
}
