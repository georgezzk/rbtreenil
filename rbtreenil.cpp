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
		leftspin(root);
	}else if(root->f->f->r==root->f&&root->f->l==root){
		root=root->f;
		rightspin(root);
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
void deletefixup(Node* root,Node* f,bool dil){
	rooot->ir=false;
	if(root!=NIL&&root->ir==true){
		root->ir=false;
		return;
	}
	if(f==NIL){
		return;
	}
	if(dil==true){
		if(f->r->ir==true){
			f->r->ir=false;
			f->ir=true;
			leftspin(f);
		}
		if(f->r->ir==false&&f->r->r->ir==false&&f->r->l->ir==false){
			f->r->ir=true;
			root=f;
			dil=(f->f->l==f);
			f=f->f;
			deletefixup(root,f,dil);
			return;
		}
		if(f->r->ir==false&&f->r->r->ir==false){
			f->r->l->ir=false;
			f->r->ir=true;
			rightspin(f->r);
		}
		if(f->r->ir==false&&f->r->r->ir==true){
			f->r->ir=f->ir;
			f->ir=false;
			f->r->r->ir=false;
			leftspin(f);
			return;
		}
	} else{
		if(f->l->ir==true){
			f->l->ir=false;
			f->ir=true;
			rightspin(f);
		}
		if(f->l->ir==false&&f->l->r->ir==false&&f->l->l->ir==false){
			f->l->ir=true;
			root=f;
			dil=(f->f->l==f);
			f=f->f;
			deletefixup(root,f,dil);
			return;
		}
		if(f->l->ir==false&&f->l->l->ir==false){
			f->l->r->ir=false;
			f->l->ir=true;
			leftspin(f->l);
		}
		if(f->l->ir==false&&f->l->l->ir==true){
			f->l->ir=f->ir;
			f->ir=false;
			f->l->l->ir=false;
			rightspin(f);
			return;
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
	bool db=!root->ir,dil=false;
	Node* debt=NIL;
	Node* f=NIL;
	if(root->l==NIL&&root->r==NIL){
		debt=NIL;
		f=root->f;
		if(root->f->l==root&&root!=rooot){
			root->f->l=NIL;
			dil=true;
		} else if(root->f->r==root&&root!=rooot){
			root->f->r=NIL;
			dil=false;
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
		db=!min->ir;
		debt=min->r;
		f=min->f;
		if(min->r==NIL){
			if(min->f->l==min){
				min->f->l=NIL;
				dil=true;
			} else if(min->f->r==min){
				min->f->r=NIL;
				dil=false;
			}
		} else{
			if(min->f->l==min){
				min->f->l=min->r;
				dil=true;
			} else if(min->f->r==min){
				min->f->r=min->r;
				dil=false;
			}
			min->r->f=min->f;
		}
		delete min;
	} else if(root->r!=NIL){
		debt=root->r;
		f=root->f;
		if(root->f->l==root&&root!=rooot){
			root->f->l=root->r;
			dil=true;
		} else if(root->f->r==root&&root!=rooot){
			root->f->r=root->r;
			dil=false;
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
			dil=true;
		} else if(root->f->r==root&&root!=rooot){
			root->f->r=root->l;
			dil=false;
		} else{
			rooot=root->l;
		}
		root->l->f=root->f;
		delete root;
	}
	if(db==true){
		deletefixup(debt,f,dil);
	}
	rooot->ir=false;
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
int main(){
	NIL->f=NIL;
	NIL->l=NIL;
	NIL->r=NIL;
	NIL->c=0;
	NIL->v=0;
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
#endif
	inorder(rooot);
	cout<<endl;
	cin>>n;
	for(int i=0;i<n;i++){
		int v;
		cin>>v;
		deleterb(v);
	}
	inorder(rooot);
#ifdef RBDEBUG
	if(check_rbtree(rooot)==false){
		cout<<"rbtree invalid"<<endl;
	} else{
		cout<<"no panic"<<endl;
	}
	bh=blackheight(rooot);
	if(bh==-1){
		cout<<"black height invalid"<<endl;
	} else{
		cout<<"no black panic"<<endl;
	}
#endif
}