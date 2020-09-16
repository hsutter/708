#include <https://raw.githubusercontent.com/hsutter/misc/master/hst.h>
#include <string>
#include <algorithm>
#include <iostream>

void copy_from(auto...) { }

using String = hst::noisy<std::string>;


//------------------------------------------------------------------------------
//  Proposed "new" in-parameter implementation -- scalable
//------------------------------------------------------------------------------

void new_in(in auto a, in auto b, in auto c, in auto d, in auto e, in auto f) {
    copy_from(a, b);
    copy_from(c);
    copy_from(d, e, f);
}


int main() {
    int i = 0;
    String s, s2, s3;
    hst::history = {}; // clear history

    new_in(i, s, std::move(s2), s3, 42, String());
    //     a  b
    //           c
    //                          d   e   f

    std::cout << hst::history;
}
