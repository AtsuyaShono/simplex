# Solver of LP problem library

## 1. How to Use
**Step 1:** Download the source code. For example,
~~~
$ git clone https://github.com/AtsuyaShono/simplex
~~~

**Step 2:** Include this file and use
```c++
#include "simplex.h"
...
simplex lp;

lp.fileload("input file path"); //If reading in the "lp" file format
lp.selfinput(); //If input from the keyboard

lp.calc(); //Calculate and output the result
```


### 1.1. Dependencies

* [GCC](https://gcc.gnu.org/) (must can compile in c++11) or other working c++ compliers
