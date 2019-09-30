# Solver of LP problem library

## 1. How to Use
**Step 1:** Download the source code. For example,
~~~
$ git clone https://github.com/AtsuyaShono/simplex
~~~

**Step 2:** Include "simplex.hpp" file and use
```c++
#include "simplex.hpp"

...
simplex lp; //Declare class

lp.fileload("input file path"); //Input from "lp" file

lp.calc(); //Calculate and output the result

//Result
cout << "ANS" << endl;
for(const auto& e : lp.base)
        cout << e << " = " << lp.ans[e] << endl;

```

## 2. "Lp" file format
**Format:**
```example.lp
Maximize
 obj: 3 x_1 + x_2

 r_1: x_1 - x_2 <= -1
 r_2: - x_1 - x_2 <= -3
 r_3: 2 x_1 + x_2 <= 4
End
```
**Rules**
#### 1. The first line is "Maximize" or "Minimize"
#### 2. Objective function after "obj:"
#### 3. Restriction after “r_num:”
#### 4. In formulas, spaces are required between numbers, signs, and inequality signs
#### 5. Each variable is non-negative
#### 6. "End" is required at the end of the file

## 3. Dependencies

* Tested by Apple clang version 11.0.0 (clang-1100.0.33.8)
