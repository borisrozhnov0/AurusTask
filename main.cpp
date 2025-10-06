#include "open_flight.h"

int main(int argv, char ** argc)
{
    open_flight::Parser parser("tests/Model_3_ver164.flt");
    parser.start();

    return 0;
}