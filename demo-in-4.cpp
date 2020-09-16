#include <https://raw.githubusercontent.com/hsutter/misc/master/hst.h>
#include <string>
#include <iostream>

void copy_from(auto...) { }

using String = hst::noisy<std::string>;


//------------------------------------------------------------------------------
//  Today's "old" in-parameter implementation -- advanced -- one parameter
//------------------------------------------------------------------------------

template<typename T> constexpr bool should_pass_by_value_v
    = std::is_trivially_copyable_v<T> && sizeof(T) < 8;

template<typename T>
    requires should_pass_by_value_v<T>
void old_in(T t) {
    copy_from(t);
}

template<typename T>
    requires (!should_pass_by_value_v<T>)
void old_in(const T& t) {
    copy_from(t);
}

template<typename T>
    requires (   !should_pass_by_value_v<T>
              && !std::is_reference_v<T>) // don’t grab non-const lvalues
void old_in(T&& t) {
    copy_from(std::forward<T>(t));        // means 'std::move'
}


//------------------------------------------------------------------------------
//  Proposed "new" in-parameter implementation -- advanced -- one parameter
//------------------------------------------------------------------------------

void new_in(in auto t) {
    copy_from(t);
}

//  Yes, this is a fair comparison because using the cleaner non-"template<...>"
//  syntax is natural for new_in, but infeasible for old_in. If you want to try
//  writing old_in with the non-"template<...>" syntax, note that it's not
//  correct to just replace each "<T>" with "<decltype(t)>".

//  Anyway, even if we write new_in the verbose way, it's still better:
//
//    template<typename T>
//    void new_in(in T t) {
//        copy_from(t);
//    }


//------------------------------------------------------------------------------
//
//  Compare current and proposed "in" parameter styles... both implement this:
//
//      template<typename T>
//      void f( /*in T t */ ) {
//          //...
//          copy_from(t);
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
    compare("trivial lvalue",
            []{ int x = 0;  new_in(x);            }, 
            []{ int x = 0;  old_in(x);            });

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
