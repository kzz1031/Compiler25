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

## Git Graph

<img src="/Users/Zhuanz/Library/Application Support/typora-user-images/image-20250415183211129.png" alt="image-20250415183211129" style="zoom:50%;" />