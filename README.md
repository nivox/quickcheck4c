quickcheck4c: A QuickCheck for C
================================
Quickcheck4c is a property based testing framework inspired at [QuickCheck](http://www.cse.chalmers.se/~rjmh/QuickCheck/).
The main goal of the project is to provide a convenient enough API without the need for too many dependencies.

You can just copy *quickcheck4c.c* and *quickcheck4c.h* in your source tree and have all you need to use the framework.

License
-------
Quickcheck4c is released under the BSD3 license, a full copy of the license can be found in the *LICENSE* file.

Features
--------
The currently supported features are:
* Automated property testing with multiple value generators
* Value generators for base types: long, int, double, float, boolean, char
* Value generators for array types: string, long, int double, float, boolean, char
* Easy implementation of ranged/sized value generators
* Possibility to add custom value generators for complex types
* Categorization of test case via labels for coverage statistics

Usage example
-------------
Using quickcheck4c is as simple as:
```C
#include "quickcheck4c.h"

QCC_TestStatus mulCommutativity(QCC_GenValue **vals, int len, QCC_Stamp **stamp) {
  int a = *QCC_getValue(vals, 0, int*);
  int b = *QCC_getValue(vals, 1, int*);

  return a*b == b*a;
}

int main(int argc, char **argv) {
  QCC_init(0);
  QCC_testForAll(100, 1000, mulCommutativity, 2, QCC_genInt, QCC_genInt);
}
```

The example test 100 times the commutative property of multiplication by supplying the property function with random integer *a* and *b*.
Compiling and running the program will yield the following output:
```
$ gcc test.c quickcheck4c.c -o test 
$ ./test
100 test passed (0)!
```

Still reading?
--------------
If you are still reading for more examples or documentation please refer to *example.c* for the former and to *quickcheck4c.h* for the latter.

Going forward
-------------
The project is in a state where is useful enough that I can start using it to test my other project.
There are lots of interesting features that might be added, like increasing the generated values "size" as the test progresses or implement argument shrinking to find the smallest arguments set which falsifies a property.
I'll spent more time on such features if a practical use case for them come up in my work or if enough interest is shown.

In any case issue reports and/or contribution are welcome.
