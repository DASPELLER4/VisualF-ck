# V*sualFuck
<h2>A visual esolang inspired by BrainFuck</h2>
<hr>
<h2>Basic Explanation</h2>
* This programming language operates on a single row of pixels in a bmp image.<br>
* It has reserved keywords in the form of specific 24bit colours<br>
* Variables are declared and initialized by referncing them outside a function call of course they cannot be one of the reserved colours<br>
* Function calls consist of {[args],function} where the function is one of the afore mentioned keywords<br>
* Some functions also allow the length of the function colour as an argument like in increment for example where arg 1 is incremented by the function colour length<br>
* Of course these functions also allow for standard function calls<br>
<h2>All Functions and their arguments</h2>
<b>Bold</b> functions allow their last argument to be the function colour length<br>
1. <b>Increment</b> - takes two arguments {arg1: colour to increment, arg2: value to increment by} <b>#a249a4</b><br>
2. <b>Decrement</b> - takes two arguments {arg1: colour to decrement, arg2: value to decrement by} <b>#4a90e2</b><br>
3. Print Char - takes one argument {arg1: ASCII character to print}<b>#b7e61d</b><br>
4. Print Int - takes one argument {arg1: integer to print} <b>#7cd6d6</b><br>
5. <b>Multiply</b> - takes two arguments {arg1: colour to multiply, arg2: value to multiply by} <b>#ec277c</b><br>
6. <b>Divide</b> - takes two arguments {arg1: colour to divide, arg2: value to divide by} <b>#fffa4f</b><br>
7. <b>Jump</b> - takes one argument {arg1: where to jump (starts counting at 1)} <b>#6e120e</b><br>
8. If Equal - takes four arguments {arg1: comparison left value, arg2: comparison right value, arg3: where to jump if true, arg4: where to jump if false} <b>#00ff12</b><br>
9. If Less Than - takes four arguments takes four arguments {arg1: comparison left value, arg2: comparison right value, arg3: where to jump if true, arg4: where to jump if false} <b>#9aa6ad</b><br>
10. If Greater Than - takes four arguments takes four arguments {arg1: comparison left value, arg2: comparison right value, arg3: where to jump if true, arg4: where to jump if false} <b>#397463</b><br>
11. End Program - not a function but just a white pixel ends the program <b>#ffffff</b><br>
<h2>Example with explanation</h2>
<img src="https://raw.githubusercontent.com/DASPELLER4/Unnamed-esolang/main/code%20run%20through.png"></img>
<h2>Colour Key</h2>
<img src="https://raw.githubusercontent.com/DASPELLER4/Unnamed-esolang/main/color%20key.bmp"></img>
<h2>Todo</h2>
* User input<br>
* Random numbers between a range
