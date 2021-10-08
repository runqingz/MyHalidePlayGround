#include "Halide.h"

#include<stdio.h>

int main(int agrc, char** argv) {

    //Func object is a pipeline stage. A pure function define which each pixel value is.
    Halide::Func gradient;

    //Names that used in func as a variable; 
    Halide::Var x, y;


    // Funcs are defined at any integer coordinate of its variables as
    // an Expr in terms of those variables and other functions.
    Halide::Expr e = x + y;

    //Add a defintion for func. Each pixel at x, y will have a value of expression e
    gradient(x, y) = e;
    
    //'realise' the function. Which JIT compile some code base on the definitin and runs it.
    //We also need to specific the domain over which Halide will evaluate
    Halide::Buffer<int32_t> output = gradient.realize(800, 600);
    //Another way to realise in arbitrary dimention
    //Buffer<int32_t> result(8, 8)
    //gradient.realize(result)

    //we can also do
    //result.set_min(4, 4) to tell the top left corner, NOTE: this result in
    //for (int y = 4; y < 12; y++)...
    //which can result in out of bound

    /*
    Halide does type inference for you. Var objects represent
    32-bit integers, so the Expr object 'x + y' also represents a
    32-bit integer, and so 'gradient' defines a 32-bit image, and
    so we got a 32-bit signed integer image out when we call
    'realize'. Halide types and type-casting rules are equivalent
    to C.
    */

   for(int j = 0; j < output.height(); j++) {
       for(int i = 0; i < output.width(); i++) {
           if (output(i, j) != i + j) {
                printf("Something went wrong!\n"
                        "Pixel %d, %d was supposed to be %d, but instead it's %d\n",
                        i, j, i + j, output(i, j));
                return -1;
           }
       }
    }

    printf("Success!\n");

    return 0;
}