# LightweightCompiler

The compiler is in a fairly rough state at the moment - some classes are redundant/require intense refactoring, some interfaces are oddly constructed, the hierarchy might be confusing and seem unnatural.  
There are also countless problems with the ways I used pointers (overuse of unnecessary heap allocations, virtually no deallocations).  
**Please do keep in mind that I started this project a long time ago in an attempt to learn C++, and be merciful when reviewing it :)**
**Now that I am finished with the pathetic excuses, I will briefly explain how to use this compiler**  

# Instructions
You will need NASM installed on your machine & added to your system's PATH (ended up switching to NASM, MASM was a tad problematic).  

Source files must be written to a TXT file.  

After cloning the project, head into src/LightweightCompiler.cpp and update `sourceDir`, `outputDir` and `projectName`.  
`sourceDir` is the directory in which your project is written.  
`outputDir` is the directory in which you would like the compiler to compile. The compiler will create an OBJ and EXE file in that directory.  
`projectName` is the name of your project, meaning the name of the file that contains the code without the '.txt' extension.  

For example, if your source file is saved at `"E:\Projects\hello.txt"`, and you want the compiled files to be saved to `"E:\Projects\out\"`:
`sourceDir = "E:\Projects\"`, `outputDir = "E:\Projects\out\"`, `projectName = "hello"`

# Syntax #
The language is indent-sensitive, meaning that it does not use curly brackets to understand scopes, but rather indentations. Any empty line will need to follow the proper amount of indentations for its scope.   
**Bad** example:
```
if var == 5
<- indents end here
  print(17)
```
This will result in compilation error.  
**Good** example:
```
if var == 5
  <- indents end here
  print(17)
```
Here are some other examples of the syntax:
```
int a = 5

for i = 0, i < 5, i += 1
  int b = 20
	
  while b >= 1
    a += b
    b -= (i + 1)
	
  print(a)
```
```
bool a = true
bool b = true
int c = 30

while c > 0
  if a && b
    print(c / 3)
    
    <- indents end here
    if c < 15
      a = !b
  
  <- indents end here
  b = c % 3 == 0
  c -= 1
```
