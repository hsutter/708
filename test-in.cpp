#include <https://raw.githubusercontent.com/hsutter/misc/master/hst.h>
#include <iostream>


//------------------------------------------------------------------------------
//  "In" tests

//  Helper, just to try a different kind of copy than initializing/assigning a
//  local variable (in case the difference matters).
template<typename T>
void copy_from(T) { }   

//- Built-in type --------------------------------------------------------------

//  Passing a small trivial type should be a copy
void int_in(in int t, int* p) {
    hst::history += &t==p ? "pass-by-pointer " : "pass-by-copy ";
}

//- Nontrivial concrete type ---------------------------------------------------

using String = hst::noisy<std::string>;

//  Just plain "in" with no attempt to copy, function just reads its param
void string_in(in String t) {
    (void)t;
}

//  "in+copy" where the only thing in the body is an attempt to copy
//  (which should invoke move if arg is an rvalue)
void string_in_copy(in String t) {
    String local;
    local = t;    // should be a move if arg is an rvalue
}

//  "in+copy" with a more complex path, where the last use is a copy attempt
//  (which should invoke move if arg is an rvalue)
void string_in_copy_last(in String t) {
    if (rand()%2) {
        String local;
        local = t;   // should always be a copy
    } else {
        String local2;
        local2 = t;   // should always be a copy
    }
    String last_use;
    last_use = t;       // should be a move assignment if arg is an rvalue
}

//- Template -------------------------------------------------------------------

//  Just plain "in" with no attempt to copy, function just reads its param
template<typename T>
void t_in(in T t) {
    (void)t;
}

//  "in+copy" where the only thing in the body is an attempt to copy
//  (which should invoke move if arg is an rvalue)
template<typename T>
void t_in_copy(in T t) {
    copy_from(t);       // should be a move if arg is an rvalue
}

//  "in+copy" with a more complex path, where the last use is a copy attempt
//  (which should invoke move if arg is an rvalue)
template<typename T>
void t_in_copy_last(in T t) {
    if (rand()%2) {
        copy_from(t);   // should always be a copy
    } else {
        copy_from(t);   // should always be a copy
    }
    copy_from(t);       // should be a move if arg is an rvalue
}

//- Comparison with traditional ------------------------------------------------

template<typename T> constexpr bool should_pass_by_value_v
    = std::is_trivially_copyable_v<T> && sizeof(T) < 8;

template<typename T>
    requires should_pass_by_value_v<T>
void traditional_in(T t) {
    hst::history += "pass-by-copy ";
    copy_from(t);
}
template<typename T>
    requires (!should_pass_by_value_v<T>)
void traditional_in(const T& t) {
    hst::history += "pass-by-pointer ";
    copy_from(t);
}
template<typename T>
    requires (   !should_pass_by_value_v<T>
              && !std::is_reference_v<T>) // don’t grab non-const lvalues
void traditional_in(T&& t) {
    copy_from(forward<T>(t));
}

template<typename T>
void new_in(in T t, T* p = nullptr) {  // p is &arg or null
    if (p) hst::history += (&t == p) ? "pass-by-pointer " : "pass-by-copy ";
    copy_from(t);
}


//------------------------------------------------------------------------------
//  Test cases: in

void in_tests() {
    hst::tester test("in parameter cases");

    //------------------------------------------------------------------------------
    // Pass trivial lvalue: Should pass by copy

    test.run(
        "in with trivial lvalue", 
        []{
            int i = 0;
            int_in(i, &i);
        }, 
        "pass-by-copy ");

    //------------------------------------------------------------------------------
    // Pass nontrivial lvalue: Should pass by ptr/ref, then copy inside string_in_copy*

    test.run(
        "in with nontrivial lvalue", 
        []{
            String s;
            string_in(s);
        }, 
        "default-ctor dtor ");

    test.run(
        "in_copy with nontrivial lvalue", 
        []{
            String s;
            string_in_copy(s);
        }, 
        "default-ctor default-ctor copy-assign dtor dtor ");

    test.run(
        "in_copy_last with nontrivial lvalue", 
        []{
            String s;
            string_in_copy_last(s);
        }, 
        "default-ctor default-ctor copy-assign dtor default-ctor copy-assign dtor dtor ");

    test.run(
        "templated in with nontrivial lvalue", 
        []{
            String s;
            t_in(s);
        }, 
        "default-ctor dtor ");

    test.run(
        "templated in_copy with nontrivial lvalue", 
        []{
            String s;
            t_in_copy(s);
        }, 
        "default-ctor copy-ctor dtor dtor ");

    test.run(
        "templated in_copy_last with nontrivial lvalue", 
        []{
            String s;
            t_in_copy_last(s);
        }, 
        "default-ctor copy-ctor dtor copy-ctor dtor dtor ");

    //------------------------------------------------------------------------------
    // Pass nontrivial xvalue: Should pass by ptr/ref, then move inside string_in_copy*

    test.run(
        "in with nontrivial xvalue", 
        []{ 
            String s;
            string_in(move(s));
        }, 
        "default-ctor dtor ");

    test.run(
        "in_copy with nontrivial xvalue", 
        []{ 
            String s;
            string_in_copy(move(s));
        }, 
        "default-ctor default-ctor move-assign dtor dtor ");

    test.run(
        "in_copy_last with nontrivial xvalue", 
        []{ 
            String s;
            string_in_copy_last(move(s));
        }, 
        "default-ctor default-ctor copy-assign dtor default-ctor move-assign dtor dtor ");

    test.run(
        "templated in with nontrivial xvalue", 
        []{ 
            String s;
            t_in(move(s));
        }, 
        "default-ctor dtor ");

    test.run(
        "templated in_copy with nontrivial xvalue", 
        []{ 
            String s;
            t_in_copy(move(s));
        }, 
        "default-ctor move-ctor dtor dtor ");

    test.run(
        "templated in_copy_last with nontrivial xvalue", 
        []{ 
            String s;
            t_in_copy_last(move(s));
        }, 
        "default-ctor copy-ctor dtor move-ctor dtor dtor ");

    //------------------------------------------------------------------------------
    // Pass nontrivial prvalue: Should pass by ptr/ref, then move inside string_in_copy*

    test.run(
        "in_copy with nontrivial prvalue", 
        []{ 
            string_in(String());
        }, 
        "default-ctor dtor ");

    test.run(
        "in_copy with nontrivial prvalue", 
        []{ 
            string_in_copy(String());
        }, 
        "default-ctor default-ctor move-assign dtor dtor ");

    test.run(
        "in_copy_last with nontrivial prvalue", 
        []{ 
            string_in_copy_last(String());
        }, 
        "default-ctor default-ctor copy-assign dtor default-ctor move-assign dtor dtor ");

    test.run(
        "templated in_copy with nontrivial prvalue", 
        []{ 
            t_in(String());
        }, 
        "default-ctor dtor ");

    test.run(
        "templated in_copy with nontrivial prvalue", 
        []{ 
            t_in_copy(String());
        }, 
        "default-ctor move-ctor dtor dtor ");

    test.run(
        "templated in_copy_last with nontrivial prvalue", 
        []{ 
            t_in_copy_last(String());
        }, 
        "default-ctor copy-ctor dtor move-ctor dtor dtor ");


    //------------------------------------------------------------------------------
    // Compare traditional_in and new_in

    test.run(
        "in equivalence with traditional, trivial lvalue", 
        []{ 
            int i = 0;
            new_in(i, &i);
        }, 
        []{ 
            int i = 0;
            traditional_in(i);
        });

    test.run(
        "in equivalence with traditional, nontrivial lvalue", 
        []{ 
            String s;
            new_in(s, &s);
        }, 
        []{ 
            String s;
            traditional_in(s);
        });

    test.run(
        "in equivalence with traditional, nontrivial xvalue", 
        []{ 
            String s;
            new_in(move(s));
        }, 
        []{ 
            String s;
            traditional_in(move(s));
        });

    test.run(
        "in equivalence with traditional, nontrivial prvalue", 
        []{ 
            int i = 0;
            new_in(String());
        }, 
        []{ 
            traditional_in(String());
        });

    std::cout << test.summary();

}

//------------------------------------------------------------------------------
//  One main to run them all

int main() {
    in_tests();
}
