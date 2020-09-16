#include <https://raw.githubusercontent.com/hsutter/misc/master/hst.h>
#include <iostream>

template<typename T> void copy_from(T) { }

using X = hst::noisy<std::string>;
void f_old(X&& x) { copy_from(std::move(x)); }
void f_new(move X x) { copy_from(x); }

int main() {
    hst::tester test("move parameter cases");

    test.run(
        "move test", 
        []{
            X x;
            f_new(std::move(x));
        }, 
        []{
            X x;
            f_old(std::move(x));
        });

    std::cout << test.summary();

}
