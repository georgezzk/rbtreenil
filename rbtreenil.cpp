#define RBDEBUG 114514
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
using namespace std;
struct Node{
	Node* f=nullptr;
	Node* l=nullptr;
	Node* r=nullptr;
	int v=0,c=1;
	bool ir=true;
};
Node* NIL=new Node;
Node* rooot=NIL;
#ifdef RBDEBUG
vector<Node*> pool;
bool check_rbtree(Node* root){
	if(root==NIL){
		return true;
	}
	if(root->l!=NIL&&root->l->f!=root){
		return false;
	}
	if(root->r!=NIL&&root->r->f!=root){
		return false;
	}
	if(root->f!=NIL&&root->f->l!=root&&root->f->r!=root){
		return false;
	}
	if(root->ir==true){
		if(root->l!=NIL&&root->l->ir==true){
			return false;
		}
		if(root->r!=NIL&&root->r->ir==true){
			return false;
		}
	}
	bool leftok=check_rbtree(root->l);
	if(leftok==false){
		return false;
	}
	bool rightok=check_rbtree(root->r);
	if(rightok==false){
		return false;
	}
	return true;
}
int blackheight(Node* root){
	if(root==NIL){
		return 1;
	}
	int leftbh=blackheight(root->l);
	int rightbh=blackheight(root->r);
	if(leftbh==-1||rightbh==-1){
		return -1;
	}
	if(leftbh!=rightbh){
		return -1;
	}
	if(root->ir==false){
		return leftbh+1;
	} else {
		return leftbh;
	}
}
#endif
Node* maken(Node* f,int v,bool il=true){
	Node* root=new Node;
	root->ir=true;
	root->f=f;
	root->v=v;
	root->c=1;
	root->r=NIL;
	root->l=NIL;
	if(f!=NIL){
		if(il){
			f->l=root;
		} else{
			f->r=root;
		}
	}
#ifdef RBDEBUG
	pool.push_back(root);
#endif
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
Node* rightspin(Node* root){
	if(root==NIL||root->l==NIL){
		return NIL;
	}
	Node* temp=NIL;
	if(root->l->r!=NIL){
		temp=root->l->r;
	}
	if(root->f->l==root){
		root->f->l=root->l;
	}
	if(root->f->r==root){
		root->f->r=root->l;
	}
	root->l->r=root;
	root->l->f=root->f;
	root->f=root->l;
	root->l=temp;
	if(root->l!=NIL){
		root->l->f=root;
	}
	if(root->f->f==NIL){
		rooot=root->f;
		root->f->ir=false;
	}
	return root->f;
}
void insertfixup(Node* root){
	rooot->ir=false;
	if(root->f->ir==false){
		return;
	}
	if(root->f->f->l!=root->f&&root->f->f->l->ir==true){
		root->f->f->l->ir=false;
		root->f->ir=false;
		root->f->f->ir=true;
		insertfixup(root->f->f);
		return;
	}else if(root->f->f->r!=root->f&&root->f->f->r->ir==true){
		root->f->f->r->ir=false;
		root->f->ir=false;
		root->f->f->ir=true;
		insertfixup(root->f->f);
		return;
	}
	if(root->f->f->l==root->f&&root->f->r==root){
		root=root->f;
		root=leftspin(root);
	}else if(root->f->f->r==root->f&&root->f->l==root){
		root=root->f;
		root=rightspin(root);
	}
	if(root->f->f->l==root->f&&root->f->l==root){
		root->f->ir=false;
		root->f->f->ir=true;
		rightspin(root->f->f);
	}else if(root->f->f->r==root->f&&root->f->r==root){
		root->f->ir=false;
		root->f->f->ir=true;
		leftspin(root->f->f);
	}
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
	insertfixup(root);
	rooot->ir=false;
}
Node* findrb(int v){
	Node* root=rooot;
	while(root!=NIL){
		if(v<root->v){
			root=root->l;
		} else if(v>root->v){
			root=root->r;
		} else{
			return root;
		}
	}
	return NIL;
}
void deletefixup(Node* root,Node* f){
	if(f==NIL){
		return;
	}
	if(root==NIL){
		if(f->l!=NIL){

		}else if(f->r!=NIL){

		}
	}
}
void deleterb(int v){
	Node* root=findrb(v);
	if(root==NIL){
		return;
	}
	if(root->c>1){
		root->c--;
		return;
	}
	bool shitcodermrf=!root->ir;
	Node* debt=NIL;
	Node* f=NIL;
	if(root->l==NIL&&root->r==NIL){
		debt=NIL;
		f=root->f;
		if(root->f->l==root&&root!=rooot){
			root->f->l=NIL;
		} else if(root->f->r==root&&root!=rooot){
			root->f->r=NIL;
		} else{
			rooot=NIL;
		}
		delete root;
	} else if(root->l!=NIL&&root->r!=NIL){
		Node* min=root->r;
		while(min->l!=NIL){
			min=min->l;
		}
		root->v=min->v;
		root->c=min->c;
		shitcodermrf=!min->ir;
		debt=min->r;
		f=min->f;
		if(min->r==NIL){
			if(min->f->l==min){
				min->f->l=NIL;
			} else if(min->f->r==min){
				min->f->r=NIL;
			}
		} else{
			if(min->f->l==min){
				min->f->l=min->r;
			} else if(min->f->r==min){
				min->f->r=min->r;
			}
			min->r->f=min->f;
		}
		delete min;
	} else if(root->r!=NIL){
		debt=root->r;
		f=root->f;
		if(root->f->l==root&&root!=rooot){
			root->f->l=root->r;
		} else if(root->f->r==root&&root!=rooot){
			root->f->r=root->r;
		} else{
			rooot=root->r;
		}
		root->r->f=root->f;
		delete root;
	} else if(root->l!=NIL){
		debt=root->l;
		f=root->f;
		if(root->f->l==root&&root!=rooot){
			root->f->l=root->l;
		} else if(root->f->r==root&&root!=rooot){
			root->f->r=root->l;
		} else{
			rooot=root->l;
		}
		root->l->f=root->f;
		delete root;
	}
	if(shitcodermrf==true){
		deletefixup(debt,f);
	}
}
void inorder(Node* p) {
	if(p==NIL){
		return;
	}
	inorder(p->l);
	for(int i=0;i<p->c;i++){
		cout<<p->v;
		if(p->ir==true){
			cout<<"R";
		} else{
			cout<<"B";
		}
		cout<<" ";
	}
	inorder(p->r);
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
#ifdef RBDEBUG
void clearrbpool(){
	while(!pool.empty()){
		delete(pool.front());
		pool.erase(pool.begin());
	}
}
#endif
int main(){
	NIL=maken(NIL,0);
	NIL->ir=false;
	rooot=NIL;
	int n;
	cin>>n;
	for(int i=0;i<n;i++){
		int v;
		cin>>v;
		insertrb(v);
	}
#ifdef RBDEBUG
	if(check_rbtree(rooot)==false){
		cout<<"rbtree invalid"<<endl;
	} else{
		cout<<"no panic"<<endl;
	}
	int bh=blackheight(rooot);
	if(bh==-1){
		cout<<"black height invalid"<<endl;
	} else{
		cout<<"no black panic"<<endl;
	}
#endif()
	inorder(rooot);
#ifdef RBDEBUG
	clearrbpool();
#endif
}