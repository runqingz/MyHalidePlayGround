#include "Halide.h"

#include "halide_image_io.h"
using namespace Halide::Tools;
using namespace std;

int main(int argc, char** argv) {
    Halide::Buffer<uint8_t> input = load_image("images/rgb.png");

    Halide::Func brighter;

    Halide::Var x,y,c;

    Halide::Expr value = Halide::cast<uint8_t>(Halide::min(Halide::cast<float>(input(x,y,c)) * 1.5f, 255.0f));

    brighter(x, y, c) = value;

    vector<int> vect{input.width(), input.height(), input.channels()};
    Halide::Buffer<uint8_t> output = brighter.realize(vect);

    save_image(output, "brighter.png");

    return 0;
}