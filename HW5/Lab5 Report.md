# Lab5 Report

22300240022 王镜凯

## 核心函数

### `generate_class_table(AST_Semant_Map* semant_map)`

- **功能**：遍历类信息，记录变量和方法的偏移位置；
- **重点逻辑**：
  - 为每个类的变量和方法分配唯一偏移值；
  - 使用var_pos_map和method_pos_map分别存储变量和方法的偏移；
  - 通过class_name和变量/方法名组合形成唯一标识；
  - 维护独立的变量和方法偏移计数器。

### `generate_method_var_table(string class_name, string method_name, Name_Maps* nm, Temp_map* tm)`
- **功能**：为方法创建变量表，记录变量的临时寄存器和类型信息
- **重点逻辑**：
  - 处理this指针（非main方法）；
  - 添加局部变量，为每个变量分配临时寄存器；
  - 处理方法参数，记录参数的类型信息；
  - 根据变量类型（INT/PTR）设置对应的类型映射。

## `ASTToTreeVisitor::visit(fdmj::VarDecl* node)`

这个函数处理**变量声明**，支持三种类型：

### 1. **普通整数变量（INT）**

```c++
if(node->type->typeKind == TypeKind::INT)
```

- 如果变量初始化了：

  ```c++
  auto init_int = std::get<IntExp*>(node->init);
  ```

  则生成一条 `Move` 指令，将值赋给对应的临时变量：

  ```c++
  new tree::Move(
      new tree::TempExp(tree::Type::INT, var_temp), 
      new tree::Const(value)
  )
  ```

- 如果没有初始化（即 `node->init.index() == 0`），则不生成代码。

------

### 2. **数组变量（ARRAY）**

```c++
if(node->type->typeKind == TypeKind::ARRAY)
```

#### 分配数组空间：

- 通过 `malloc` 申请空间，大小为 `(array_size + 1) * 4` 字节（+1 是为了存储数组长度）。
- 使用 `ExtCall("malloc", ...)` 生成外部调用。

#### 存储数组长度到数组首地址：

```c++
new tree::Mem(... new tree::TempExp(...)) = new tree::Const(array_size);
```

#### 如果有初始值：

- 将初始值依次写入数组后续地址（`base + 4, base + 8, ...`）。

------

### 3. **类类型变量（CLASS）**

```c++
if(node->type->typeKind == TypeKind::CLASS)
```

#### 步骤说明：

1. **类层级解析**：
   - 收集当前类及其所有父类（用于继承的初始化顺序）。
2. **对象分配**：
   - 根据变量、方法总数计算对象大小，调用 `malloc`。
3. **父类到子类顺序初始化成员变量**：
   - INT 类型变量若有初始值则生成 `Move` 语句；
   - ARRAY 类型变量分配内存并存值，与普通数组处理类似。
4. **初始化（方法表）**：
   - 依照 offset，将 `Name(label)` 写入对象对应位置，形成一个类似 vtable 的结构。

###  `ASTToTreeVisitor::visit(fdmj::MethodDecl* node)`

这个函数负责处理**方法声明**，即类中的函数。

1. **变量绑定：**
   - 创建该方法的变量映射表（`current_mvt`），用于记录变量名到临时变量的映射。
   - `this` 指针和形参变量注册进 args 向量。

###  `ASTToTreeVisitor::visit(fdmj::BinaryOp* node)`
1. 获取数组长度
```cpp
stms->push_back(new tree::Move(...));
```
取 left_tr->exp 和 right_tr->exp 的内存中开头（偏移为 0）的数据，作为长度，分别存到 left_len_temp 和 right_len_temp 中。

2. 检查两个数组长度是否一致
```cpp
tree::Cjump(..., error_label, ok_label)
```
比较两个数组长度是否相等，如果不等就跳转到 error_label，然后调用 exit(-1) 终止程序。

3. 分配结果数组空间
```cpp
new tree::ExtCall(tree::Type::PTR, "malloc", ...)
```
计算所需空间大小为 (length + 1) * 4 字节，并调用 malloc 分配新数组，首元素存长度，后面是数据。

4. 初始化循环
```cpp
index_temp = 4
bound_temp = (length + 1) * 4
```
因为第一个 4 字节存储的是长度，所以循环从偏移 4 开始，到整个数组结束。

5. 进入循环体
```cpp
new tree::LabelStm(loop_body);
```
循环每次迭代：
从左右两个数组的当前偏移位置读取值
执行 +, -, *, / 操作
把结果写入新数组的对应位置
这些通过：

```cpp
new tree::Mem(...), new tree::Binop(...), ...
```

来组合 IR 表达式。

6. 更新索引 & 退出
每次偏移加 4，直到到达 bound_temp，然后跳出循环。

7. 返回新数组
```cpp
tr_exp = new Tr_ex(new tree::Eseq(...));
```
整个操作表示为一个 ESEQ：先执行指令序列，再返回 result_array。

###  `ASTToTreeVisitor::visit(fdmj::ArrayExp* node)`
1. 获取数组基地址 array_tr
```cpp
node->arr->accept(*this);
Tr_ex* array_tr = dynamic_cast<Tr_ex*>(tr_exp);
```
访问 arr 这个表达式得到它的中间表示。如果不是 Tr_ex 类型（表达式），可能是 Tr_cx（条件表达式），需要 unEx() 转换。

2. 获取索引表达式 index_tr
```cpp
node->index->accept(*this);
Tr_ex* index_tr = dynamic_cast<Tr_ex*>(tr_exp);
```
同样地，对 i 表达式求值。如果是条件表达式，也转成表达式类型。

3. 临时变量准备（array 和 index）
根据是否是常量表达式决定是否将中间结果存储到临时变量中：

```cpp
if(node->arr->getASTKind() != ASTKind::IdExp) // 非常量
```
如果 arr 不是变量（如 someFunc()[i]），就把它的值存入临时变量 array_temp。
如果是变量名，就直接从 var_temp_map 中取变量绑定的临时变量。
对索引 i 也做类似处理。

4. 下标越界检查
```cpp
check_stms->push_back(new tree::Cjump(">=", index, len, error, ok));
```

执行内容：
读取数组长度：len = *(arr + 0)
检查：如果 i >= len 跳转到 error_label
否则跳转到 ok_label

error_label 下调用 exit(-1) 终止程序

这些检查代码都被包进了一个 tree::Seq 语句列表中。

5. 计算真实访问地址
```cpp
tree::Binop(... "+", array_base, offset)
```
计算偏移量 4 * (i + 1)，即跳过数组的长度字段（数组头占4字节），访问 i 所在的数据位置。

最终访问地址为：

```cpp
addr = array_base + 4 * (i + 1)
```

6. 是否生成额外语句序列
```cpp
if ((index 非常量) || (arr 非常量))
```
如果 arr 或 index 是复杂表达式，则需要先执行临时语句来求出中间值。

把这些语句用 tree::Seq 包起来，配上越界检查，用 tree::Eseq 构建完整表达式。

如果 arr 和 index 都是简单变量或常量，直接生成访问地址并取值。

## 修复bug
- callexp和callstm中obj不是idexp的情况

## 部分测试通过截图

![alt text](image-1.png)

除变量label不一样外，其他都一样

## Git Graph

![alt text](image.png)