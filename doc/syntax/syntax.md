# Syntax

## Values
### Types
number, string, null, undefined, object, function, native-function
### Formats
number: `\d+\.?\d*`/`0[xob]\d+`, string: `".*""` or `'.+'`, null: `null`, undefined: `undefined`, object: [Development], function: [See define a function], native-function: None
### Expression
#### Numeric Representation in Other Numbers
##### Hexadecimal
We use header `0x` to express hexadecimal number;
##### Octal
We use header `0o` to express octal number;
##### Binary
We use header `0b` to express binary number;
#### Operators
=, :, ==, !=, <=, >=, <, >, ( ), [ ], { }, ., (comma), &, |, ^, ~, &&, ||, !, +, -, *, /, %, **, <<, >>, +=, -=, *-, /=, %=, **=, <<=, >>=, &=, |=, ^=, in, of, ~~
#### in/of operator
Format: `<expr> in <expr>`  
Used to detect whether the second expr has a value with the keyword expr1.

#### Operator ~~
Format: n~~m  
Returns all integers in an interval [n, m)  
_note_: If n, m is a floating point number, then the system automatically integrates the value of n, m downwards.

#### Monomial Express
Format: <number><variable>  
Can convert <number> <variable> to <number> * <variable> to calculate.  
Example:
```
import sys;
let a = 2;
sys.io.print(2a);// 4
```

#### Calling Functions
Syntax: `<function-name/function-getting-expression>(arg1, arg2, arg3...)`
Demo: 
```
// #1
hello("BerryMath");
```
```
// #2
Hello.hello("BerryMath");
```
And you can use operator= to specify setting arguments  
Demo: 
```
hello("BerryMath", id = 1234);
```
##### The Writing Method of Omitting Bracket
Condition: It is a single argument and the argument is a single face quantity or identifier (that is, there is no sign, nor can attributes such as A.B)  
Syntax: `<function-name/function-getting-expression> <single-face-quantity/identifier>`

#### Get properties in an object
Use operator. to get properties in an object  
Demo: 
```
let b = a.b;
```
Use operator[] also can get properties  
Demo: 
```
let b = a["b"];
```
#### Prioritization
"[" "." 15  
"(" 14  
"++" "--" "!" "~" "**" 13  
"*" "/" "%" 12  
"+" "-" 11  
"<<" ">>" 10  
">" ">=" "<" "<=" 9  
"==" "!=" 8  
"&" 7  
"^" 6  
"|" 5  
"&&" 4  
"||" "~~" 3  
"=" "*=" "/=" "%=" "+=" "-=" ">>=" "<<=" "^=" "&=" "|=" "in" "of" 2  
"," 1  

## Declares
### Variable
Use key word `let` to declare a variable.    
Demo:
```
let a = 1 + 3;
```
### Function
Use key word `def` to declare a function.  
Use `return` to return a result.
```
def expr() {
    return 1 * (2 + 3);
}
```
You can get some arguments, declare them in the brackets.
```
def add(a, b) {
    return a + b;
}
```
Use operator= with arguments can declare default value of the arguments.
```
def sub(a = 4 + 2, b = 5) {
    return a + b;
}
```
Each function contains a variable args to get a list of arguments.
```
def p() {
    return argv;
}
p(1, 2, 3, 4, 5);
```
#### If you want to define a lambda function, change the parentheses of the parameter definition to brackets
Demo:
```
def did(fun) {
    return fun(2);
}
did(def l[a] {
    return a ** 10;
});// 1024
```

### Reference variable
Use key word `refer` to declare a reference variable.  
**#: Initialization is necessary or the consequences are at your own expense.** 
**#: Reference variables are only applicable to variables, and reference assignments in the form of `refer a = b.c` are not accepted for the time being.**       
Demo:
```
let a = 0;
refer b = a;
b++;// a == 1
```
And you can also use operator `->` to declare a reference variable.  
Demo:
```
let a = 0;
let b -> a;
b++;// a == 1
```

### Reference arguments
You can define a reference parameter by adding the keyword `refer` before the parameter name. 
Demo:
```
def swap(refer a, refer b) {
    let tmp = a;
    a = b;
    b = a;
}
let a = 0;
let b = 1;
swap(a, b);// a = 1, b = 0
``` 
**#: All referenced variables/parameters are automatically de-referenced when the type changes!!!**    

## Process statement
### If
This statement controls the code that runs according to conditions  
Syntax:
```
if (<condition>) {
    <script>
} [elif(<condition>) {
    <script>
} [elif...
[else {
    <script>
}
```
Demo
```
let a = 1 + 2;
if (a == 3) {
    a++;
} elif (a == 5) {
    a--;
} else {
    a += 100;
}
```

### For loop
This statement can be used for loop operation. The first expression is used for initialization, the second is used for loop condition judgment, and the third is used for loop variables operation.  
Syntax:
```
for (<expr>; <condition>; <expr>) {
    <script>
}
```
Demo
```
let res = 0;
for (let i = 1; i <= 100; i++) {
    res += i;
}
```
In the for loop condition, an in/of expression (`var > in < expr > `or `var > of < expr > `) can also be used to traverse an object in / of with only one expression.   
Syntax:
```
for (<in/of-expr>) {
    <script>
}
```
Demo  
[1]
```
import sys;
using sys.io;

let arr = {
    "a": "a",
    "b": "b"
};
for (i in arr) {
    print(i, arr[i]);
}
```
[2]
```
import sys;
using sys.io;

let arr = {
    "a": "a",
    "b": "b"
};
for (i of arr) {
    print(i);
}
```

### While loop
This statement can be used for loop operation. This expression is used for conditional judgment. If it does not conform, it exits.  
Syntax:
```
while (<condition>) {
    <script>
}
```
Demo
```
let res = 0;
let i = 1;
while (i <= 100) {
    res += i++;
}
```

### Do...while loop
This statement can be used for loop operation. This expression is used for conditional judgment. If it does not conform, it exits.  
And it run script first conditional judgment second.  
Syntax:
```
do {
    <script>
} while (<condition>);
```
Demo
```
let res = 0;
let i = 1;
do {
    res += i++;
} while (i <= 100);
```

### Break statement
To break loop and you can set breaking layer count.  
Syntax:
```
break [<expr>;
```
Demo
```
for (let i = 0; i < 10; i++) {
    for (let j = 0; j < 10; j++) {
        if (i == 8 && j == 9) {
            break 2;
        } elif (i == 7 && j == 4) {
            break;
        }
    }
}
```

### Continue statement
To continue loop.  
Syntax:
```
continue
```
Demo
```
for (let i = 0; i < 10; i++) {
    for (let j = 0; j < 10; j++) {
        if (i == 8 && j == 9) {
            break 2;
        } elif (i == 7 && j == 4) {
            continue;
        }
    }
}
```

## Modules
### Import
To import a module.
Syntax:
```
import "<module-path>" as <export-name>;
```
Demo:
```
import "sys" as sys;
sys.io.print("Hello world!");
```
And you can also import a module with:
```
import <module-name>
```
Like:
```
import sys;
```
### Export
To export values in a module script
Syntax:
```
export <variable-name>
export <function-name>
```
Demo:
```
let a = 0;
def fun() {
    return a++;
}

export a;
export fun;
```

## Pass
Pass is a meaningless statement, which is used to occupy space.  
Syntax:
```
pass
```
Demo:
```
let a = 0;
pass;
a++;
```

## Using
The using statement can import all attributes of an object into the same-name variable in the current scope.    
Syntax:
```
using <expr>;
```
Demo:
```
import "sys" as sys;
using sys.io;
print("Hello world!");
```

## Class
Used to define a class  
Syntax: `class <class-name> { /* definitions */ }`  
Definitions in class(_Defining the prototype of a class_):  
- Defining function: Just like defining functions directly, write directly in this class: `def <method-name>(<args>) { /* scripts */ }`
- Defining static function: Just like defining methods, write directly in this class: `static <method-name>(<args>) { /* scripts */ }`
- Overload operators: Just like defining methods, write directly in this class: `operator<operator>(<args>) { /* scripts */ }`
- Defining property: Write the name of this property directly followed by a semicolon: `<property-name>;`
- Defining static property: Add `<class-name>.<property-name> = <value>` after class definition
Type Declarations(_Set how prototypes are displayed (public or private)_):  
- Defining public prototypes: Add `public:`
- Defining private prototypes: Add `private:`
### A constructor is a member function named `ctor`
### Access object uses identifier `this`
Demo:
```
import sys;
using sys.io;

class Vector3 {
public:
    def ctor(x = 0, y = 0, z = 0) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
    operator+(b) {
        return new Vector3(this.x + b.x, this.y + b.y, this.x + b.z);
    }
    operator-(b) {
        return new Vector3(this.x - b.x, this.y - b.y, this.x - b.z);
    }
    static Zero() {
        return new Vector3();
    }
private:
    x; y; z;
};
Vector3.version = "v-1.0.0";

let a = new Vector3(1, 2);
let b = Vector3.Zero();
print Vector3;
print a;
print b;
print(a + b);
print(a - b);
print(Vector3.version);
```
