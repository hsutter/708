#include <https://raw.githubusercontent.com/hsutter/misc/master/hst.h>
#include <string>
#include <iostream>

void copy_from(auto...) { }

using String = hst::noisy<std::string>;


//------------------------------------------------------------------------------
//  Today's "old" in-parameter implementation -- simple -- one parameter
//------------------------------------------------------------------------------

void old_in(const String& s) {
    copy_from(s);
}

void old_in(String&& s) {
    copy_from(std::move(s));
}


//------------------------------------------------------------------------------
//  Proposed "new" in-parameter implementation -- simple -- one parameter
//------------------------------------------------------------------------------

void new_in(in String s) {
    copy_from(s);
}


//------------------------------------------------------------------------------
//
//  Compare current and proposed "in" parameter styles... both implement this:
//
//      void f( /*in String s */ ) {
//          //...
//          copy_from(s);
//          //...
//      }
//
//  where "old_in" does it today's way, and "new_in" uses an "in" parameter.
//
//------------------------------------------------------------------------------

void compare(auto name, auto f1, auto f2) {
    std::cout << name << "\n  old: " << hst::run_history(f1)
                      << "\n  new: " << hst::run_history(f2) << "\n\n";
}

int main() {
    compare("nontrivial lvalue",
            []{ String x;   new_in(x);            }, 
            []{ String x;   old_in(x);            });

    compare("nontrivial xvalue",
            []{ String x;   new_in(std::move(x)); }, 
            []{ String x;   old_in(std::move(x)); });

    compare("nontrivial prvalue",
            []{             new_in(String());     }, 
            []{             old_in(String());     });
}
