#include <spargel/base/output_stream.h>

int main() {
    auto& out = spargel::base::OutputStream::Stdout();
    out << 'h' << "ello, world!\n";
    out << spargel::base::OutputColor::Black << "this is black\n";
    out << spargel::base::OutputColor::Red << "this is red\n";
    out << spargel::base::OutputColor::Green << "this is green\n";
    out << spargel::base::OutputColor::Yellow << "this is yellow\n";
    out << spargel::base::OutputColor::Blue << "this is blue\n";
    out << spargel::base::OutputColor::Magenta << "this is magenta\n";
    out << spargel::base::OutputColor::Cyan << "this is cyan\n";
    out << spargel::base::OutputColor::White << "this is white\n";
    out << spargel::base::OutputColor::Reset << "reset!\n";
    return 0;
}
