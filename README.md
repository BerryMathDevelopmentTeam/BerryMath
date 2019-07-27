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
=======
## Welcome to GitHub Pages

You can use the [editor on GitHub](https://github.com/BerryMathDevelopmentTeam/BerryMath/edit/master/README.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/BerryMathDevelopmentTeam/BerryMath/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
>>>>>>> ec68307536e436fd148afb3d9210ac96591d8a85
