# Solver of LP problem library

## 1. How to Use
**Step 1:** Download the source code. For example,
~~~
$ git clone https://github.com/AtsuyaShono/simplex
~~~

**Step 2:** Include this file and use
```c++
#include "simplex.hpp"

...
simplex lp;

lp.fileload("input file path"); //Input from "lp" file

lp.calc(); //Calculate and output the result

//結果出力
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


### 1.1. Dependencies

* Tested by Apple clang version 11.0.0 (clang-1100.0.33.8)
