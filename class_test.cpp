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
    auto v=tree.inorder();
    for(int x:v) {
        cout<<x<<" ";
    }
    cout<<endl;
    if(tree.findrb(5)!=nullptr) {
        cout<<"found 5"<<endl;
    }
    tree.deleterb(3);
    v=tree.inorder();
    for(int x:v) {
        cout<<x<<" ";
    }
    cout<<endl;
    return 0;
}