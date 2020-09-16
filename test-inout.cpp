#include <https://raw.githubusercontent.com/hsutter/misc/master/hst.h>
#include <iostream>


//------------------------------------------------------------------------------
//  "Inout" tests

//  Helper, just to try a different kind of copy than initializing/assigning a
//  local variable (in case the difference matters).
template<typename T>
void copy_from(T) { }

//  Helper, to test side effects
template<typename T>
    requires std::is_arithmetic_v<T>
void modify(T& t) { ++t; }

//- Built-in type --------------------------------------------------------------

//  Passing a small trivial type should be by pointer
void int_inout(inout int t, int* p) {
    hst::history += &t==p ? "pass-by-pointer " : "pass-by-copy ";
    modify(t);
}

//- Nontrivial concrete type ---------------------------------------------------

using String = hst::noisy<std::string>;
void modify(String& t) { t.t.append("xyzzy "); }

//  Just plain "inout" with no attempt to copy, function just reads its param
void string_inout(inout String t) {
    (void)t;
    modify(t);
}

//  "inout+copy" where the only thing in the body is an attempt to copy
void string_inout_copy(inout String t) {
    auto local = t;         // should always be a copy
    modify(t);
}

//  "inout+copy" with a more complex path, where the last use is a copy attempt
void string_inout_copy_last(inout String t) {
    if (rand()%2) {
        auto local  = t;    // should always be a copy
    } else {
        auto local2 = t;    // should always be a copy
    }
    String last_use;
    last_use = t;           // should always be a copy
    modify(t);
}

//- Template -------------------------------------------------------------------

//  Just plain "inout" with no attempt to copy, function just reads its param
template<typename T>
void t_inout(inout T t) {
    (void)t;
    modify(t);
}

//  "inout+copy" where the only thing in the body is an attempt to copy
template<typename T>
void t_inout_copy(inout T t) {
    copy_from(t);       // should always be a copy
    modify(t);
}

//  "inout+copy" with a more complex path, where the last use is a copy attempt
template<typename T>
void t_inout_copy_last(inout T t) {
    if (rand()%2) {
        copy_from(t);   // should always be a copy
    } else {
        copy_from(t);   // should always be a copy
    }
    copy_from(t);       // should always be a copy
    modify(t);
}

//- Comparison with traditional ------------------------------------------------

template<typename T>
void traditional_inout(T& t) {
    hst::history += "pass-by-pointer ";
    copy_from(t);
    modify(t);
}

template<typename T>
void new_inout(inout T t, T* p = nullptr) {  // p is &arg or null
    if (p) hst::history += (&t == p) ? "pass-by-pointer " : "pass-by-copy ";
    copy_from(t);
    modify(t);
}

//------------------------------------------------------------------------------
//  Test cases: inout

void inout_tests() {
    hst::tester test("inout parameter cases");

    //------------------------------------------------------------------------------
    // Pass trivial lvalue: Should pass by pointer

    test.run(
        "inout with trivial lvalue", 
        []{
            int i = 0;
            int_inout(i, &i);
            hst::history += std::to_string(i);
        }, 
        "pass-by-pointer 1");

    //------------------------------------------------------------------------------
    // Pass nontrivial lvalue: Should pass by ptr/ref, then copy inside string_inout_copy*

    test.run(
        "inout with nontrivial lvalue", 
        []{
            String s;
            string_inout(s);
            hst::history += s.t;
        }, 
        "default-ctor xyzzy dtor ");

    test.run(
        "inout_copy with nontrivial lvalue", 
        []{
            String s;
            string_inout_copy(s);
            hst::history += s.t;
        }, 
        "default-ctor copy-ctor dtor xyzzy dtor ");

    test.run(
        "inout_copy_last with nontrivial lvalue", 
        []{
            String s;
            string_inout_copy_last(s);
            hst::history += s.t;
        }, 
        "default-ctor copy-ctor dtor default-ctor copy-assign dtor xyzzy dtor ");

    test.run(
        "templated inout with nontrivial lvalue", 
        []{
            String s;
            t_inout(s);
            hst::history += s.t;
        }, 
        "default-ctor xyzzy dtor ");

    test.run(
        "templated inout_copy with nontrivial lvalue", 
        []{
            String s;
            t_inout_copy(s);
            hst::history += s.t;
        }, 
        "default-ctor copy-ctor dtor xyzzy dtor ");

    test.run(
        "templated inout_copy_last with nontrivial lvalue", 
        []{
            String s;
            t_inout_copy_last(s);
            hst::history += s.t;
        }, 
        "default-ctor copy-ctor dtor copy-ctor dtor xyzzy dtor ");

    //------------------------------------------------------------------------------
    // Pass nontrivial xvalue: Should be rejected (which we detect by using the 'in' overload)

    test.run(
        "inout with nontrivial xvalue", 
        []{ 
            String s;
            HST_CAN_INVOKE(string_inout)(move(s));
        }, 
        "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    test.run(
        "inout_copy with nontrivial xvalue", 
        []{ 
            String s;
            HST_CAN_INVOKE(string_inout_copy)(move(s));
        }, 
        "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    test.run(
        "inout_copy_last with nontrivial xvalue", 
        []{ 
            String s;
            HST_CAN_INVOKE(string_inout_copy_last)(move(s));
        }, 
        "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    // test.run(
    //     "templated inout with nontrivial xvalue", 
    //     []{ 
    //         String s;
    //         HST_CAN_INVOKE(t_inout<String>)(move(s));
    //     }, 
    //     "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    // test.run(
    //     "templated inout_copy with nontrivial xvalue", 
    //     []{ 
    //         String s;
    //         HST_CAN_INVOKE(t_inout_copy<String>)(move(s));
    //     }, 
    //     "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    // test.run(
    //     "templated inout_copy_last with nontrivial xvalue", 
    //     []{ 
    //         String s;
    //         HST_CAN_INVOKE(t_inout_copy_last<String>)(move(s));
    //     }, 
    //     "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    //------------------------------------------------------------------------------
    // Pass nontrivial prvalue: Should be rejected (which we detect by using the 'in' overload)

    test.run(
        "inout_copy with nontrivial prvalue", 
        []{ 
            HST_CAN_INVOKE(string_inout)(String());
        }, 
        "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    test.run(
        "inout_copy with nontrivial prvalue", 
        []{ 
            HST_CAN_INVOKE(string_inout_copy)(String());
        }, 
        "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    test.run(
        "inout_copy_last with nontrivial prvalue", 
        []{ 
            HST_CAN_INVOKE(string_inout_copy_last)(String());
        }, 
        "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    // test.run(
    //     "templated inout_copy with nontrivial prvalue", 
    //     []{ 
    //         HST_CAN_INVOKE(t_inout<String>)(String());
    //     }, 
    //     "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    // test.run(
    //     "templated inout_copy with nontrivial prvalue", 
    //     []{ 
    //         HST_CAN_INVOKE(t_inout_copy<String>)(String());
    //     }, 
    //     "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile

    // test.run(
    //     "templated inout_copy_last with nontrivial prvalue", 
    //     []{ 
    //         HST_CAN_INVOKE(t_inout_copy_last<String>)(String());
    //     }, 
    //     "default-ctor cannot-invoke dtor ");    // i.e., without 'in' overload would fail to compile


    //------------------------------------------------------------------------------
    // Compare traditional_inout and new_inout

    // test.run(
    //     "inout equivalence with traditional, trivial lvalue", 
    //     []{ 
    //         int i = 0;
    //         new_inout(i, &i);    //  TODO: this is currently ambiguous, but should not be
    //         hst::history += std::to_string(i);
    //     }, 
    //     []{ 
    //         int i = 0;
    //         traditional_inout(i);
    //         hst::history += std::to_string(i);
    //     });
    // }

    test.run(
        "inout equivalence with traditional, nontrivial lvalue", 
        []{ 
            String s;
            new_inout(s, &s);
            hst::history += s.t;
        }, 
        []{ 
            String s;
            traditional_inout(s);
            hst::history += s.t;
        });

    // test.run(
    //     "inout equivalence with traditional, nontrivial xvalue", 
    //     []{ 
    //         String s;
    //         HST_CAN_INVOKE(new_inout)(move(s));
    //     }, 
    //     []{ 
    //         String s;
    //         HST_CAN_INVOKE(traditional_inout)(move(s));
    //     });

    // test.run(
    //     "inout equivalence with traditional, nontrivial prvalue", 
    //     []{ 
    //         HST_CAN_INVOKE(new_inout)(String());
    //     }, 
    //     []{ 
    //         HST_CAN_INVOKE(traditional_inout)(String());
    //     });

    std::cout << test.summary();

}

//------------------------------------------------------------------------------
//  One main to run them all

int main() {
    inout_tests();
}
