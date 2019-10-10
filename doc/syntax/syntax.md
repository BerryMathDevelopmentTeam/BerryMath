# Syntax

## Values
### Types
number, string, null, undefined, object, function, native-function
### Formats
number: `\d+\.?\d*`, string: `".*""` or `'.+'`, null: `null`, undefined: `undefined`, object: [Development], function: [See define a function], native-function: None
### Expression
#### Operators
=, :, ==, !=, <=, >=, <, >, ( ), [ ], { }, ., (comma), &, |, ^, ~, &&, ||, !, +, -, *, /, %, **, <<, >>, +=, -=, *-, /=, %=, **=, <<=, >>=, &=, |=, ^=, in, of, ~~

#### Operator ~~
Format: n~~m  
Returns all integers in an interval [n, m)  
_note_: If n, m is a floating point number, then the system automatically integrates the value of n, m downwards.

#### Calling Functions
Syntax: `<function-name>(arg1, arg2, arg3...)`
Demo: 
```
hello("BerryMath");
```
And you can use operator= to specify setting arguments  
Demo: 
```
hello("BerryMath", id = 1234);
```
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