# lab3-2: AST Type Checking

# Types

## Data Type (3)

> FDMJ is static typed

- basic type (integer)
- array type (of integers)
- class type (also called an object type, with a class name)
  - class variables
    - Class-wide variables
      - Using static keyword (e.g. class c {static int a;…}) in Java
      - **FDMJ don’t have these**
    - Object (instance) variables
      - Act more like a record (e.g. class c {int a;}) in Java
      - **FDMJ only have public object variables**
  - methods
    - Java methods also have class level or object level (static method; Non-static method)
    - **FDMJ only have non-static public methods**
  - Inheritance
    - Java
      - A sub-class inherits the variables and the methods of the (super) class. This inheritance relationship is transitive.
      - Can override (public) variables and methods
        - variables: overridden by same name.
        - methods: overridden by the same name and signature (polymorphism).
    - **FDMJ**
      - 子类继承父类的变量，但不能重复声明同名变量
      - 子类继承父类的方法，可以重写方法的实现，但方法的签名(返回值和参数列表)不能改变。例如父类是 `int f(int a, int[] b)`，子类只能是 `int f(int, int[])`的签名，参数名可以不一样但函数名、类型、参数类型顺序必须一样。
      - 只能单继承，不允许多继承，不允许成环
        - FDMJ语法中，只有1个extend，自然只有单继承、不允许多继承
        - FDMJ加入如下限制：若 `a extends b`，则b必须事先定义。因此保证了继承关系的方向性（父类先于子类存在），而且单继承形成的是树结构，没有回溯的可能性，因此不会存在循环继承的问题

## Value Type (4)

> In FDMJ “everything represents a value”, useful in type checking

- basic value (abbreviated as “value”)
  - 只有int的变量或常量才有value
  - 变量如：int a;
  - 常量如：1
- pointer value (abbreviated as “pointer”)
  - 只有数组或类的对象或匿名对象才有pointer
  - 对象如：int[] a;（数组对象），class C;（类对象）
  - 匿名对象如：{1, 2, 3}（匿名数组对象）
- location value (of all data types, abbreviated as “location”)
  - For a memory location (either in the stack or in heap space).
  - Each location is dedicated for a specific data type and holds 32 bits (for this class).
  - 只有左值（变量或对象）有location，右值（常量或匿名对象）没有location
- method value
  - Representing function entrance location (instruction space)
  - Note we don’t have goto statements in FDMJ.

## Name Type (3)

>  FDMJ uses name equivalence

1. variable name:
   - Associated with 1 of the 3 data types
   - Carries 2 values: a location value and an basic/pointer value (if defined)
   - A variable name stands for either: (e.g. variable name is a)
     - a basic type has location value and integer value (e.g. int a;)
     - an array type has location value and pointer value (e.g. int[] a;)
     - a class type has location value and pointer value (e.g. class A a;)
   - A variable is only in the scope where it is declared
     - In a method, or （包括传入的，和声明的；FDMJ传值(integer/pointer)，不传引用(location)）
     - In a class
2. class name
   - Is an alias for class type
   - Is not associated with any value
   - A class name is used for defining a variable of a class type (e.g. “class A o;”)
3. method name:
   - Is a pointer value, which is the “address” of a “function” (or a method). No location value is associated.
   - The exact “address” of a method
     - depends on the object on which it is called (also based on the inheritance hierarchy).
     - Or it’s determined at link time (if it’s an external function).

## Casting

> FDMJ allow implicit basic type casting and class type upcasting during assignment

- implicit basic type casting
  - 隐式类型转换，比如int a = true + 2;最终a赋值为3
- class type upcasting (inheritance)!
  - upcasting相当于丢掉子类的一些性质变成父类
  - 比如，允许类赋值操作：class Father f; class Son s; f = s;
  - f的location和value都是Father，因此，此后f只能调用父类中存在的变量和方法；但如果调用了子类重写的方法，则调用子类的实现
  - 在typechecking里，我们不管它调用什么函数，只检查右边是否是左边的类型或左边的孩子
  - 那我们怎么f.m()知道调用是父类还是子类的方法实现呢？在赋值时，entry会记录method到底指向父类还是子类的method实现。这是runtime做的事情（dynamic method）

# Rules

```fdmj2025
Prog -> MainMethod ClassDeclList
  MainMethod -> public int main '(' ')' '{' VarDeclList StmList '}'
    VarDeclList -> ε | VarDecl VarDeclList
      VarDecl -> class id id ';' // id <=> [a-z_A-Z][a-z_A-Z0-9]*
               | int id ';' | int id '=' Const ';' 
               | int '[' ']' id ';' | int '[' ']' id '=' '{' ConstList '}' ';' // 数组长度只由[]内的数字决定，若为空则是0
               | int '[' NUM ']' id ';' | int '[' NUM ']' id '=' '{' ConstList '}' ';' // NUM <=> [1-9][0-9]*|0
        Const -> NUM | '-' NUM
        ConstList -> ε | Const ConstRest
          ConstRest -> ε | ',' Const ConstRest
    StmList -> ε | Stm StmList
      Stm -> '{' StmList '}' 
           | if '(' Exp ')' Stm else Stm | if '(' Exp ')' Stm // 检查Exp为int
           | while '(' Exp ')' Stm | while '(' Exp ')' ';' // 检查Exp
           | Exp '=' Exp ';' // 检查类型是否兼容，允许upcast，允许int和bool之间的隐式转换
           | Exp '.' id '(' ExpList ')' ';' // 检查Exp为class，Exp有id方法，ExpList匹配方法参数
           | continue ';' | break ';' // 检查在while里
           | return Exp ';' // 检查Exp为函数返回值
           | putint '(' Exp ')' ';' | putch '(' Exp ')' ';' // 检查Exp为int
           | putarray '(' Exp ',' Exp ')' ';' // 检查第一个Exp为int，第二个Exp为array
           | starttime '(' ')' ';' | stoptime '(' ')' ';'
      Exp -> NUM | true | false // 返回int
           | length '(' Exp ')' // 返回int，检查Exp是array
           | getint '(' ')' | getch '(' ')' // 返回int
           | getarray '(' Exp ')' // 返回int，检查Exp是array
           | id // 返回变量类型
           | this // 返回class
           | Exp op Exp | '!' Exp | '-' Exp // 检查Exp是int或array，返回Exp类型。不用检查两个数组的长度是否相同，因为有些数组长度是runtime才能确定的（比如getarray()）
           | '(' Exp ')' | '(' '{' StmList '}' Exp ')' // 返回Exp类型
           | Exp '.' id // 检查Exp为class，检查Exp是否有id，返回id类型
           | Exp '.' id '(' ExpList ')' // 检查Exp为class，检查Exp是否有id，检查ExpList是否匹配参数列表，返回方法类型
           | Exp '[' Exp ']' // 检查第一个Exp为array，第二个Exp为int
      ExpList -> ε | Exp ExpRest
        ExpRest -> ε | ',' Exp ExpRest
  ClassDeclList -> ε | ClassDecl ClassDeclList
    ClassDecl -> public class id '{' VarDeclList MethodDeclList '}' 
               | public class id extends id '{' VarDeclList MethodDeclList '}'
      MethodDeclList -> ε | MethodDecl MethodDeclList
      MethodDecl -> public Type id '(' FormalList ')' '{' VarDeclList StmList '}'
        Type -> class id | int | int '[' ']'
          FormalList -> ε | Type id FormalRest
            FormalRest -> ε | ',' Type id FormalRest
```

# Checking

需要多次遍历AST。以下是遍历2遍的示例：

1. 预处理阶段：
   - 记录类及其extend关系
   - 记录类内变量与方法
   - 检查类内变量与方法是否重定义
2. 类型检查阶段：
   - 检查类内变量，如果与父类中的同名，报错重定义
   - 检查类内方法，如果与父类中的同名，检查签名是否相同(返回值和参数列表类型)，若不同则报错
   - 检查方法内部，按照规则
