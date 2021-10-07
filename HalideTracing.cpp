#include "Halide.h"
#include <stdio.h>

using namespace::Halide;

int main(int argc, char** argv) {
    Var x("x"), y("y");

    //Print out value of functions when they compute
    {
        Func gradient("gradient");
        gradient(x, y) = x + y;

        gradient.trace_stores();

        printf("\nEvaluating gradient\n");
        Buffer<int> output = gradient.realize(8,8);

        Func parallel_gradient("parallel gradient");
        parallel_gradient(x, y) = x + y;

        parallel_gradient.trace_stores();
        //parallelize loop over y dimension
        parallel_gradient.parallel(y);
        printf("\nEvaluation parallel_gradient\n");
        parallel_gradient.realize(8 ,8);
    }

    //Print out indiviual expressions
    {
        Func ff("ff");

        ff(x,y) = print_when(x == 20 && y == 20, sin(x), " <- this is the sin of x when x = ", x, "and y = ", y)
        + print(cos(y), " <- this is the cos of y = ", y, " x = ", x);

        printf("/nEvaluating ff/n");
        ff.realize(40, 40);

        Func gg("gg");
        Expr e = sin(x) + cos(y);

        gg (x, y) = print_when(e < 0, e, "A interesting point when e < 0 at x = ", x, " y = ", y);

        printf("/nEvaluating gg/n");
        gg.realize(8, 8);
    }

    // Printing expressions at compile-time.
    {
        Var fizz("fizz"), buzz("buzz");
        Expr e = 1;
        for (int i = 2; i < 100; i++) {
            if (i % 3 == 0 && i % 5 == 0) {
                e += fizz * buzz;
            } else if (i % 3 == 0) {
                e += fizz;
            } else if (i % 5 == 0) {
                e += buzz;
            } else {
                e += i;
            }
        }
        std::cout << "Printing a complex Expr: " << e << "\n";
    }

    return 0;
}