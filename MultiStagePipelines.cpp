#include "Halide.h"
#include <stdio.h>

#include "halide_image_io.h"

using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char** argv) {
    Var x("x"), y("y"), c("c");

    {
        Buffer<uint8_t> input = load_image("images/rgb.png");

        Func input_16("input_16");
        input_16(x, y, c) = cast<uint16_t>(input(x, y, c));

        Func blur_x("blur_x");
        blur_x(x, y, c) = (input_16(x - 1, y, c) + 2 * input_16(x, y, c) + input_16(x + 1, y , c)) / 4;

        Func blur_y("blue_y");
        blur_y(x, y, c) = (blur_x(x, y - 1, c) + 2 * blur_x(x, y, c) + blur_x(x, y + 1, c)) / 4;

        Func output("output");
        output(x, y, c) = cast<uint8_t>(blur_y(x, y, c));

        //This wont work because out of bound
        //Buffer<uint8_t> result = output.realize(input.width(), input.height(), 3);

        Buffer<uint8_t> result(input.width() - 2, input.height() - 2, 3);
        result.set_min(1, 1);
        output.realize(result);

        save_image(result, "blurry_parrot_1.png");
    }

    //same pipeline, we can also add condition on input boundaries
    {
        Func clamped("clamped");
        Buffer<uint8_t> input = load_image("images/rgb.png");

        Expr clamp_x = clamp(x, 0, input.width() - 1);
        Expr clamp_y = clamp(y, 0, input.height() - 1);

        clamped(x, y, c) = input(clamp_x, clamp_y, c);

        Func input_16("input_16");
        input_16(x, y, c) = cast<uint16_t>(clamped(x, y, c));

        Func blur_x("blur_x");
        blur_x(x, y, c) = (input_16(x - 2, y, c) + input_16(x - 1, y, c) + 2 * input_16(x, y, c) + input_16(x + 1, y , c) + input_16(x + 2, y, c)) / 6;

        Func blur_y("blue_y");
        blur_y(x, y, c) = (blur_x(x, y - 2, c) + blur_x(x, y - 1, c) + 2 * blur_x(x, y, c) + blur_x(x, y + 1, c) + blur_x(x, y + 2, c)) / 6;

        Func output("output");
        output(x, y, c) = cast<uint8_t>(blur_y(x, y, c));

        //This wont work because out of bound
        //Buffer<uint8_t> result = output.realize(input.width(), input.height(), 3);

        Buffer<uint8_t> result(input.width(), input.height(), 3);
        output.realize(result);

        save_image(result, "blurry_parrot_2.png");
    }

    return 0;
}