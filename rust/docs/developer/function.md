# function call rule

The order that values be pushed into the stack is the same as the order that you defined in your code.

So you can get function arguments like this:

If arguments is `a,b,c`

You should let valc = stack.pop().let valb = stack.pop(),let vala = stack.pop();

The order you read is opposite to the order you define.

You can define an interface class and use it to define your class.

by this way,we call the fuction by read the vtable.

We will store the functions in a table,no matter what is the kind of your function.

because we just allow one-depth class,so we just store a table like this:

A large table of different interface.A global alloc id of any class is the difference of this,

So we should first find the right interface of this.Then we should call the right function of the right interface.

By the following instructions,LOAD_INTERFACE XXX and CALL_METHOD XXX to call the right function.

The common method is just called by CALL_METHOD,so virtual function is slower than common function.
