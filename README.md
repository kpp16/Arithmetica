<h1 align="center">Arithmetica: Simple Math Interpreter</h1>
<hr>

## About
An interpreter built using C++ to evaluate mathematical expressions.


## Key Features
1. Define and change variables
2. The only supported datatype right now is a C++ `double`
3. Runs using the scanner (lexing) and the parser (will change the codebase to add AST later)


## How To Use
1. Run the executable file

### Syntax:
1. **Print statements**:

	Prints variables, numbers and mathematical expressions into the console.
	
	Usage:
	```
	print 2+3;
	print x;
	print x + y - (z + 3) * a;
	```
	

2. **Defining variables**:

	A variable (identifier) is a set of alphanumeric ASCII characters used to represent a `double`.
	
	Usage:
	```
	rad = 10.45;
	pi = 3.1415;
	circum = 2 * pi * rad;
	area = pi * rad * rad;
	```
	
3. **Evaluating expressions**:

	An expression is defined as arithmetical expression that involves variables and numbers.
	
	Examples
	
	`3 * (4/a - 2*b) - (2/a)`


**End of each statement is followed by a semicolon `;`**
  
<br>

![Example Usage](https://github.com/kpp16/Arithmetica/blob/main/imageExample.PNG)
  


## Building
To build and run this application you will need a C++ compiler.

Using the GNU GCC compiler:
```Shell
g++ -g main.cpp -o main
./main
```

## TODO:
1. Fix bugs.
2. Add ASTs and make a proper interpreter.
3. Use LLVM and generate IR code.
4. Add more datatypes and more keywords.
