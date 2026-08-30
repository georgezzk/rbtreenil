#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
using namespace std;
void panic(){
	cout<<"panic"<<endl;
	cout<<"panic"<<endl;
	cout<<"panic"<<endl;
	cout<<"panic"<<endl;
	cout<<"panic"<<endl;
}
void scared(){
	cout<<"scared"<<endl;
}
struct Node{
	Node* f=nullptr;
	Node* l=nullptr;
	Node* r=nullptr;
	int v=0,c=1;
	bool ir=true;
};
vector<Node*> pool;
Node* NIL=new Node{NIL,NIL,NIL,0,false};
Node* rooot=NIL;
Node* maken(Node* f,int v,bool il=true){
	Node* root=new Node;
	root->ir=true;
	root->f=f;
	root->v=v;
	root->r=NIL;
	root->l=NIL;
	if(f!=NIL){
		if(il){
			f->l=root;
		} else{
			f->r=root;
		}
	}
	pool.push_back(root);
	return root;
}
Node* leftspin(Node* root){
	if(root==NIL||root->r==NIL){
		return NIL;
	}
	Node* temp=NIL;
	if(root->r->l!=NIL){
		temp=root->r->l;
	}
	if(root->f->l==root){
		root->f->l=root->r;
	}
	if(root->f->r==root){
		root->f->r=root->r;
	}
	root->r->l=root;
	root->r->f=root->f;
	root->f=root->r;
	root->r=temp;
	if(root->r!=NIL){
		root->r->f=root;
	}
	if(root->f->f==NIL){
		rooot=root->f;
		root->f->ir=false;
	}
	return root->f;
}
void insertrb(int v){
	if(rooot==NIL){
		rooot=maken(NIL,v);
		rooot->ir=false;
		return;
	}
	Node* root=rooot;
	Node* f=root;
	bool il=false;
	while(root!=NIL){
		f=root;
		if(v<root->v){
			root=root->l;
			il=true;
		}else if(v>root->v){
			root=root->r;
			il=false;
		}else if(v==root->v){
			root->c++;
			return;
		}
	}
	root=maken(f,v,il);
	pool.push_back(root);
}
void inorder(Node* p) {
	if(p==NIL){
		return;
	}
	inorder(p->l);
	cout<<p->v<<" ";
	inorder(p->r);
}
void clearrbpool(){
	while(!pool.empty()){
		delete(pool.at(0));
		pool.erase(pool.begin());
	}
}
vector<string> splitstr(string str){
	stringstream ss(str);
	vector<string> arr;
	string temp;
	while(ss>>temp){
		arr.push_back(temp);
	}
	return arr;
}
int main(){
	
	clearrbpool();
}