This is Homework 10, which is to do a simple optimization (over Quad Program in SSA form).

The basic idea is from Tiger Book Chapter 19.3 for conditional constant propagation. Basically, each temp is given a Run-Time Value (RtValue in opt.hh) that is either NO_VALUE, ONE_VALUE, or MANY_VALUES (when ONE_VALUE, also includes the specific value). We iterate over the statements to find the fixed point (starting with all temp having NO_VALUE, and all blocks non-executable (except for the entry block)).

The above should be implemented in the function "void calculateBT()" in Opt class. You need to implement the code for this function.

The "void modifyFunc()" is to remove constant temps (replaced with constants), unnecessary statements, and unexecutable blocks. You need to implement the code for this function.

The main.cc is the main program you need to use.

There are some input files under test/input_example (with test/fmj_example for your reference), and the corresponding sample output files under test/output_example.