#include <https://raw.githubusercontent.com/hsutter/misc/master/hst.h>
#include <string>
#include <iostream>

void copy_from(auto...) { }

using String = hst::noisy<std::string>;


//------------------------------------------------------------------------------
//  Today's "old" in-parameter implementation -- simple -- two parameters
//------------------------------------------------------------------------------

void old_in(const String& s1, const String& s2) {
    copy_from(s1);
    copy_from(s2);
}

void old_in(String&& s1, const String& s2) {
    copy_from(std::move(s1));
    copy_from(s2);
}

void old_in(const String& s1, String&& s2) {
    copy_from(s1);
    copy_from(std::move(s2));
}

void old_in(String&& s1, String&& s2) {
    copy_from(std::move(s1));
    copy_from(std::move(s2));
}


//------------------------------------------------------------------------------
//  Proposed "new" in-parameter implementation -- simple -- two parameters
//------------------------------------------------------------------------------

void new_in(in String s1, in String s2) {
    copy_from(s1);
    copy_from(s2);
}


//------------------------------------------------------------------------------
//
//  Compare current and proposed "in" parameter styles... both implement this:
//
//      void f( /*in String s1, in String s2 */ ) {
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
                      << "\n  new: " << hst::run_history(f2)
                      << "\n\n";
}

int main() {
    compare("lvalue + lvalue",
            []{ String x, y;   old_in(x, y);            }, 
            []{ String x, y;   new_in(x, y);            });

    compare("lvalue + rvalue",
            []{ String x;      old_in(x, String());     }, 
            []{ String x;      new_in(x, String());     });

    compare("rvalue + lvalue",
            []{ String x;      old_in(String(), x);     }, 
            []{ String x;      new_in(String(), x);     });

    compare("rvalue + rvalue",
            []{                old_in(String(), String()); }, 
            []{                new_in(String(), String()); });
}
