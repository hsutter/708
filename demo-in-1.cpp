
void copy_from(auto...) { }


//------------------------------------------------------------------------------
//  Today's "old" in-parameter implementation -- trivial -- one parameter
//------------------------------------------------------------------------------

void old_in(int i) {
    copy_from(i);
}


//------------------------------------------------------------------------------
//  Proposed "new" in-parameter implementation -- trivial -- one parameter
//------------------------------------------------------------------------------

void new_in(in int i) {
    copy_from(i);
}
