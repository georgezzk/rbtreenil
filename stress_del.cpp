#define main original_main
#include "rbtreenil.cpp"
#undef main

// External harness: same stdin protocol as the shipped main, but also
// prints the post-delete inorder so content can be verified.
// Output shape per run:
//   RB1 <OK|BAD>
//   BH1 <OK|BAD>
//   IN1 <sorted values>|
//   RB2 <OK|BAD>
//   BH2 <OK|BAD>
//   IN2 <sorted values>|
int main(){
    NIL->f=NIL;
    NIL->l=NIL;
    NIL->r=NIL;
    NIL->c=0;
    NIL->v=0;
    NIL->ir=false;
    rooot=NIL;
    int n;
    if(!(cin>>n)) return 2;
    for(int i=0;i<n;i++){int v;cin>>v;insertrb(v);}
    cout<<(check_rbtree(rooot)?"RB1 OK":"RB1 BAD")<<endl;
    cout<<(blackheight(rooot)!=-1?"BH1 OK":"BH1 BAD")<<endl;
    cout<<"IN1 ";inorder(rooot);cout<<"|"<<endl;
    if(!(cin>>n)) return 2;
    for(int i=0;i<n;i++){int v;cin>>v;deleterb(v);}
    cout<<(check_rbtree(rooot)?"RB2 OK":"RB2 BAD")<<endl;
    cout<<(blackheight(rooot)!=-1?"BH2 OK":"BH2 BAD")<<endl;
    cout<<"IN2 ";inorder(rooot);cout<<"|"<<endl;
    return 0;
}
