# 配置测试

## 普通文本
&emsp;&emsp;hello world.  
&emsp;&emsp;你好世界。  

## 代码段
普通代码段
```
#include <stdio.h>

int main()
{
    printf("hello world");
    return 0;
}
```

代码段 - 指定语言
```c
#include <stdio.h>

int main()
{
    printf("hello world");
    return 0;
}
```

```python
import math

if __name__ == "__main__":
    print("hello world")
    print("pi={}".format(math.pi))
```

代码段 - 显示行号
```c linenums="1"
#include <stdio.h>

int main()
{
    printf("hello world");
    return 0;
}
```

代码段 - 高亮
```c linenums="1" hl_lines="5-6"
#include <stdio.h>

int main()
{
    printf("hello world");
    return 0;
}
```

## LaTex

使用 `\\(` 和 `\\)` 在行内插入: \\((x-a)^2+(y-b)^2 = r^2\\)  

使用 `$$` 插入多行:
$$
\begin{aligned}
& (x_1, y_1) = \lambda(x_2, y_2) \newline
\Rightarrow & \left\lbrace
\begin{aligned}
& x_1 = \lambda x_2 \newline
& x_y = \lambda x_2 \newline
\end{aligned}
\right. \newline
\Rightarrow & x_1 y_2 - x_2 y_1 = 0
\end{aligned}
$$

