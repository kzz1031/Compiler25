# lab3-1: FDMJ2025

# Syntax

```fdmj2025
Prog -> MainMethod ClassDeclList
  MainMethod -> public int main '(' ')' '{' VarDeclList StmList '}'
    VarDeclList -> ε | VarDecl VarDeclList
      VarDecl -> class id id ';' // id <=> [a-z_A-Z][a-z_A-Z0-9]*
               | int id ';' | int id '=' Const ';' 
               | int '[' ']' id ';' | int '[' ']' id '=' '{' ConstList '}' ';'
               | int '[' NUM ']' id ';' | int '[' NUM ']' id '=' '{' ConstList '}' ';' // NUM <=> [1-9][0-9]*|0
        Const -> NUM | '-' NUM
        ConstList -> ε | Const ConstRest
          ConstRest -> ε | ',' Const ConstRest
    StmList -> ε | Stm StmList
      Stm -> '{' StmList '}' 
           | if '(' Exp ')' Stm else Stm | if '(' Exp ')' Stm 
           | while '(' Exp ')' Stm | while '(' Exp ')' ';'
           | Exp '=' Exp ';' 
           | Exp '.' id '(' ExpList ')' ';' 
           | continue ';' | break ';' 
           | return Exp ';' 
           | putint '(' Exp ')' ';' | putch '(' Exp ')' ';'
           | putarray '(' Exp ',' Exp ')' ';'
           | starttime '(' ')' ';' | stoptime '(' ')' ';'
      Exp -> NUM | true | false | length '(' Exp ')'
           | getint '(' ')' | getch '(' ')'
           | getarray '(' Exp ')'
           | id | this
           | Exp op Exp | '!' Exp | '-' Exp // op stands for "+, - , * , / , || , && , < , <= , > , >= , == , != ". When applied to arrays, the operations are element-wise applied. Boolean binary operations (|| and &&) follow the "shortcut" semantics.
           | '(' Exp ')' | '(' '{' StmList '}' Exp ')'
           | Exp '.' id
           | Exp '.' id '(' ExpList ')'
           | Exp '[' Exp ']'
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

# Overall Structure

```java
/* main method */
public int main() {
  // vars
  // stms
}

/* classes */
public class A {
  // vars
  // methods
}
// other classes
```

Comments are not treated as part of the program. There are 2 kinds of comments:

1. `//...` in 1 line
2. `/*...*/` in n line(s)

# Notes

## Variable

All variables are **local**, there are no global variables.

There are 3 kinds of variables:

1. **class variable**: defined in a class, which only “exists” with a class instance (object). A class variable `b` of class `A` can only be accessed using `this.b` inside class, using `A.b` outside class.
2. **formal parameter**: defined in the formal list of a method
3. **method variable**: defined in a method

In a method:

1. The same kind of variables **cannot** have the same id (redifined).
2. Different kinds of variables **can** have the same id, and the following priority is used:
   1. class variable with `this` prefix
   2. method variable
   3. formal parameter
   4. class variable without `this` prefix

Naming: Although not a good practice, class ids, method ids and variable ids **do not** need to be different (e.g., m may be a class name, and at the same time a method name and a variable name), as they can be syntactically distinguished from each other.

## Array

An array variable **holds a pointer** to a block (a sequence of integers) stored in the memory, and **the arity is associated with the block** instead of the variable. Hence, when an array is assigned to an array variable, only the pointer is passed (while the block of integers is not moved). For example:

- `int[] a`: a point to a block with size of 0
- `int[0] a`: a point to a block with size of 0
- `int[666] a`: a point to a block with size of 666
- `int[] a = {}`: a point to a block with size of 0
- `int[] a = {666,888}`: a point to a block with size of 2
- `int[666] a = {666,888}`: a point to a block with size of 2
- `a = {666,888}`: a point to a block with size of 2 (assume a is declared before with arbitary size)

When two arrays are in a binary operation, they must have the **same arity**. All operations on arrays are **element-wise**. The validity of array operations can sometimes be checked at compile time, but **many needs to be checked at run time** (because they regard the arities of the involved memory block). For example (a is {1,2,3}, b is {4,5,6}, c is {1,2,3}, d is {6,6,6,6}):

- `a == b`: false
- `a == c`: true
- `a+b`: yield `{5,7,9}`
- `a*b`: yield `{4,10,18}`
- `a*d`: error

## Class

The keyword “extends” means subclass. A subclass inherits the class variables and methods from all its ancestor classes. To simplify, we assume that:

1. A variable declared in a subclass **cannot** use the same id as any variable declared in any of its ancestor classes (so no override of variables is allowed), but may have include more variables.
2. A subclass **can** override methods declared in its ancestor classes, but must have the **same signature** (i.e., the same list of types, albeit may use different id’s, and the same return type).

Semantically, a class variable is very different from an array variable. However, they are similar in the way that they both hold pointers to a block of memory, although of different content. The “content” of an object includes not only values (int, array, or object), but also some methods. Both class variables and array variables are `new` (allocate memory) during declaration, so we **do not** have `new` expression (`new int '[' Exp ']' | new id '(' ')'`). Notice the "upcasting" of the class variable during assign statement.
