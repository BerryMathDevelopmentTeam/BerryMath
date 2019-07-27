# BerryMath

BerryMath is a simple programming language.
We'll keep it be stronger, keep it to grow up.
I hope this language will great at math, AI.
Of course, if you just want it as a gadget rather than a systemic programming language, it can be used to quickly develop a small program.

## Build
We use cmake to manage the project.
If you don't have cmake, you must first install cmake.
``` shell
cmake .
```
or
``` shell
cmake -DSHARED=OFF .
```
These shell scripts can also compile it into an executable.

And use this shell script:
``` shell
cmake -DSHARED=ON .
```
can compile it into an shared library.

## Usage
When you had built this project, open the executable can run the script by BerryMath.
If you want to use BerryMath Interpreter in your project, you can include this by shared library. Header files and dynamic link libraries in Linux and Mac are installed in the `/usr/BM/dev` directory, and windows is in `C:\BM\dev`.

Use `BM::Object` can create a BerryMath object, use `BM::Number`/`BM::String`/`BM::Function`/`BM::NativeFunction` to create a number, a string, a function or a native function.
Use `BM::Variable` to create a BerryMath variable.
Use `BM::scope` to create a scope to save variables.
Use `BM::AST` to build BerryMath script Abstract Syntax Tree(AST).
Use `BM::Interpreter` to run the BM script or compile the BM script into a .bmast file.
...

## License
[Apache 2.0 License](LICENSE)