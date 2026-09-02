#include "rbtreenil.hpp"
#include <iostream>
using namespace std;
int main() {
    rbtreenil<int> tree;
    tree.insertrb(5);
    tree.insertrb(3);
    tree.insertrb(7);
    tree.insertrb(1);
    tree.insertrb(9);
    auto v=tree.inorderrb();
    for(int x:v) {
        cout<<x<<" ";
    }
    cout<<endl;
    if(tree.findrb(5)!=nullptr) {
        cout<<"found 5"<<endl;
    }
    rbtreenil<int> treee=tree.clonerb();
    treee.deleterb(3);
    v=treee.inorderrb();
    for(int x:v) {
        cout<<x<<" ";
    }
    cout<<endl;
    return 0;
}