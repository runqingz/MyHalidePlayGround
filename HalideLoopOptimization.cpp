#include "Halide.h"
#include <algorithm>
#include <stdio.h>

using namespace Halide;

int main(int argc, char** argv) {
    Var x("x"), y("y");

    //Default looping
    {
        Func gradient("gradient");
        gradient(x, y) = x + y;

        gradient.trace_stores();

        printf("\n Evaluating gradient...\n");
        gradient.realize(4,4);

        printf("\nPseudo-code for the schedule:\n");
        gradient.print_loop_nest();
    }

    //Reordered looping
    {
        Func gradient("gradient_col_major");
        gradient(x, y) = x + y;

        gradient.trace_stores();

        gradient.reorder(y,x);
        printf("\n Evaluating gradient...\n");
        gradient.realize(4,4);

        printf("\nPseudo-code for the schedule:\n");
        gradient.print_loop_nest();
    }

    //Split
    {
        Func gradient("gradient_split");
        gradient(x, y) = x + y;

        gradient.trace_stores();

        Var x_outer, x_inner;
        gradient.split(x, x_outer, x_inner, 2);

        printf("\n Evaluating gradient...\n");
        gradient.realize(4,4);

        printf("\nPseudo-code for the schedule:\n");
        gradient.print_loop_nest();
    }

    //Fuse
    {
        Func gradient("gradient_fused");
        gradient(x, y) = x + y;

        gradient.trace_stores();

        Var fused;
        gradient.fuse(x, y, fused);

        printf("\n Evaluating gradient...\n");
        gradient.realize(4,4);

        printf("\nPseudo-code for the schedule:\n");
        gradient.print_loop_nest();
    }

    //Tiles
    {
        Func gradient("gradient_tiled");
        gradient(x, y) = x + y;

        gradient.trace_stores();

        Var x_inner, x_outter, y_inner, y_outter;
        gradient.tile(x, y, x_outter, y_outter, x_inner, y_inner, 4, 4);
        // This is same as
        // gradient.split(x, x_outter, x_inner, 4);
        // gradient.split(y, y_outter, y_inner, 4);
        // gradient.reorder(x_inner, y_inner, x_outter, y_outter);

        printf("\n Evaluating gradient...\n");
        gradient.realize(8,8);

        printf("\nPseudo-code for the schedule:\n");
        gradient.print_loop_nest();
    }

    //Vectorize
    {
        Func gradient("gradient_in_vectors");
        gradient(x, y) = x + y;
        gradient.trace_stores();

        gradient.vectorize(x, 4);
        //This is the same as
        //gradient.split(x, x, x_inner, 4)
        //gradient.vectorize(x_inner);

        printf("\n Evaluating gradient...\n");
        gradient.realize(8, 4);
        printf("\nPseudo-code for the schedule:\n");
        gradient.print_loop_nest();     
    }

    //Unrolling a loop.
    {
        Func gradient("gradient_unroll");
        gradient(x, y) = x + y;
        gradient.trace_stores();

        gradient.unroll(x, 2);
        //This is the same as
        //gradient.split(x, x_outer, x_inner, 2);
        //gradient.unroll(x_inner);

        printf("\n Evaluating gradient...\n");
        gradient.realize(4, 4);

        gradient.print_loop_nest();
    }

    //Splitting by factors that dont divide the content
    {
        Func gradient("gradient_split_7x2");
        gradient(x, y) = x + y;

        gradient.trace_stores();


        Var x_outer, x_inner;
        gradient.split(x, x_outer, x_inner, 3);

        //In this case, same points get evaluated MULTIPLE times.
        //This can be either safe or unsafe depends on the context.
        //For example, in this coding case it is safe. However in a context
        //like UPDATE, this is UNSAFE. It is PROGRAMMER's resposibility to 
        //make sure to handle same points being evaluated multiple times.
        printf("\n Evaluating gradient...\n");
        gradient.realize(7,2);

        printf("\nPseudo-code for the schedule:\n");
        gradient.print_loop_nest();
    }

    //Fuse, tile, parallel
    {
        Func gradient("gradient_fused_tile");
        gradient(x, y) = x + y;
        gradient.trace_stores();


        //Fuse helps when parallelize through multiple dimention with out
        //nested parallelism. Nested parallelism is supported however, it is
        //not as fast as fuse into a single parallel fro loop
        Var x_outer, y_outer, x_inner, y_inner, title_index;
        gradient.tile(x,y, x_outer, y_outer, x_inner, y_inner, 4, 4);
        gradient.fuse(x_outer, y_outer, title_index);
        gradient.parallel(title_index);

        printf("\n Evaluating gradient...\n");
        gradient.realize(8,8);

        printf("\nPseudo-code for the schedule:\n");
        gradient.print_loop_nest();
    }

    //Combine everything together
    {
        Func gradient_fast("gradient_fast");
        gradient_fast(x, y) = x + y;

        //First divide output into 64*64 tiles in parallel
        Var x_outer, y_outer, x_inner, y_inner, tile_index;

        gradient_fast.tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64)
            .fuse(x_outer, y_outer, tile_index)
            .parallel(tile_index);

        //For each tile, divide into 4*2 subtile to vectorize and also we 
        //can look at 2 scan line at the same time by unroll y;

        Var x_inner_outer, y_inner_outer, x_vectorize, y_unroll;
        gradient_fast.tile(x_inner, y_inner, x_inner_outer, y_inner_outer, x_vectorize, y_unroll, 4, 2)
            .vectorize(x_vectorize)
            .unroll(y_unroll);

        Buffer<int> output = gradient_fast.realize(350, 250);

        printf("\nPseudo-code for the schedule:\n");
        gradient_fast.print_loop_nest();
    }

    return 0;
}