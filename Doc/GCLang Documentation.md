<head>
 <style>
  body{
   font-family:"SF Pro Display", "Helvetica Neue", "Helvetica", "sans-serif";
  }
  code{
   font-family:"SF Mono", "consolas", "monospace";
  }
 </style>
</head>
<body>
 <h1>
  <u>
   <b>
    GCLang
   </b>
  </u>
 </h1>
 <h3>
  <u>
   A concatenative, stack based language
  </u>
 </h3>
 <p>
  Every operation in GCLang is performed via one of four stacks. These are like hardware stacks, which can have a value pushed on to them, and a value popped off of them.
 </p>
 
 <p>
  <b>
   <u>
    For example:
   </u>
  </b>
 </p>
 <code>
  8 15 +
 </code>
 <p>
  will push <code>8</code> on to the top of the active stack. The same can be said of <code>15</code>. This leaves the stack as such:
 </p>
 <code>
  [8, 15]
 </code>
 <p>
  From here, the <code>+</code> operand takes the top two values off of the active stack, adds them together, and pushes the result back onto the top of the active stack, leaving the stack as such:
 </p>
 <code>
  [23]
 </code>
 
 <p>
  The same behaviour is true of every operation, though they may take a different number of values off of the top of the active stack, and leave a different number of values on the top of the active stack.
 </p>
 
 <br>
 
 <h3>
  <b>
   <u>
    Stack Switching
   </u>
  </b>
 </h3>
 <p>
  Through the use of the <code>&</code> operator, the active stack can be switched to one of four stacks. This means that only the active stack will be modified by operations, and the other three will be preserved when they are not the active stack.
 </p>
 <p>
  <b>
   <u>
    For example:
   </u>
  </b>
 </p>
 <code>
  0 & 9  <br>
  1 & 10  <br>
  2 & 11  <br>
  3 & 12
 </code>
 <p>
  will first push <code>0</code> on to the top of the active stack, which is by default 0. It then calls the <code>&</code> operator, which takes the value off of the top of the active stack that we have just pushed, and switches the active stack to stack number 0. We then push <code>2</code> on to the active stack, in this case 0. This procedure is then repeated three more times with a switch to stack 1, then pushing 4; a switch to stack 2, and pushing 6; and a switch to stack 3, and pushing 8.
 </p>
 <p>
  This leaves us with the stacks as follows:
 </p>
 <code>
  0 -> [9]  <br>
  1 -> [10]  <br>
  2 -> [11]  <br>
  3 -> [12]
 </code>
 
 
 <br>
 
 <h3>
  <b>
   <u>
    Built In Operators
   </u>
  </b>
 </h3>
 
 <p>
  There are multiple built in operators, most of which are self descriptive. These all operate on the stacks as has been previously described.
 </p>
 
 <table>
  <tr>
   <th>
    Operator
   </th>
   <th>
    Description
   </th>
   <th>
    Example
   </th>
  </tr>
  <tr>
   <td style="text-align:center">
    <code>+</code>
   </td>
   <td style="text-align:justify">
    Takes the top two elements from the active stack, adds them together, then pushed the result back on to the active stack.
   </td>
   <td style="text-align:center">
    <code>
     "5 3 +" <br>
     becomes <br>
     "8"
    </code>
   </td>
  </tr>
  <tr>
   <td style="text-align:center">
    <code>-</code>
   </td>
   <td style="text-align:justify">
    Subtracts the top element of the active stack from the second-top element of the active stack, then pushes the result back on to the active stack
   </td>
   <td style="text-align:center">
    <code>
     "5 3 -" <br>
     becomes <br>
     "2"
    </code>
   </td>
  </tr>
  <tr>
   <td style="text-align:center">
    <code>=</code>
   </td>
   <td style="text-align:justify">
    Takes the top two elements from the active stack, and compares them. If they are equal, then push 1 on to the active stack, else push 0.
   </td>
   <td style="text-align:center">
    <code>
     "5 3 =" <br>
     becomes <br>
     "0"
     <br>
     <br>
     "5 5 =" <br>
     becomes <br>
     "1"
    </code>
   </td>
  </tr>
  <tr>
   <td style="text-align:center">
    <code>!</code>
   </td>
   <td style="text-align:justify">
    Takes the top element of the active stack, and if it is <code>0</code>, then pushes <code>1</code> on to the top of the active stack. In the case that the top of the active stack is not <code>0</code>, then <code>0</code> is pushed in to the top of the active stack instead.
   </td>
   <td style="text-align:center">
    <code>
     "5 !"   <br>
     becomes <br>
     "0"     <br>
     <br>
     "0 !"   <br>
     becomes <br>
     "1"
    </code>
   </td>
  </tr>
  <tr>
   <td style="text-align:center">
    <code>dup</code>
   </td>
   <td style="text-align:justify">
    Takes the top element of the active stack, and copies it onto the top of the same active stack.
    <br>
   </td>
   <td style="text-align:center">
    <code>
     "5 dup" <br>
     becomes <br>
     "5 5"
    </code>
   </td>
  </tr>
  <tr>
   <td style="text-align:center">
    <code>drop</code>
   </td>
   <td style="text-align:justify">
    Takes the top element of the active stack, and discards it.
    <br>
   </td>
   <td style="text-align:center">
    <code>
     "5 drop" <br>
     becomes <br>
     ""
    </code>
   </td>
  </tr>
  <tr>
   <td style="text-align:center">
    <code>@</code>
   </td>
   <td style="text-align:justify">
    Calls an external function from the file <code>stdLib.cpp</code>
    <br>
    Aliases for these functions can be viewed in <code>stdLib.alias</code>
    <br>
    See &quot;Standard Library.md&quot; for details.
   </td>
   <td style="text-align:center">
    <code>
     "@0" <br>
     calls <br>
     "_extern_0"
    </code>
   </td>
  </tr>
 </table>
</body>