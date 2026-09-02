#pragma once
#include <vector>
template<typename T>
class rbtreenil{
private:
	struct Node{
		Node* f=nullptr;
		Node* l=nullptr;
		Node* r=nullptr;
		T v{};
		int c=1;
		bool ir=true;
	};
	Node* NIL=new Node;
	Node* rooot=NIL;
	bool (*cmp)(const T,const T);
	Node* maken(Node* f,T v,bool il=true){
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
		} else if(root->f->f->r!=root->f&&root->f->f->r->ir==true){
			root->f->f->r->ir=false;
			root->f->ir=false;
			root->f->f->ir=true;
			insertfixup(root->f->f);
			return;
		}
		if(root->f->f->l==root->f&&root->f->r==root){
			root=root->f;
			leftspin(root);
		} else if(root->f->f->r==root->f&&root->f->l==root){
			root=root->f;
			rightspin(root);
		}
		if(root->f->f->l==root->f&&root->f->l==root){
			root->f->ir=false;
			root->f->f->ir=true;
			rightspin(root->f->f);
		} else if(root->f->f->r==root->f&&root->f->r==root){
			root->f->ir=false;
			root->f->f->ir=true;
			leftspin(root->f->f);
		}
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
	void destroyrb(Node* p) {
		if(p==NIL){
			return;
		}
		destroyrb(p->l);
		destroyrb(p->r);
		delete p;
	}
	void inorderrecur(Node* p,std::vector<T>& res) {
		if(p==NIL){
			return;
		}
		inorderrecur(p->l,res);
		for(int i=0;i<p->c;i++) {
			res.push_back(p->v);
		}
		inorderrecur(p->r,res);
	}
	void init(){
		NIL->f=NIL;
		NIL->l=NIL;
		NIL->r=NIL;
		NIL->c=0;
		NIL->v=T{};
		NIL->ir=false;
		rooot=NIL;
	}
	Node* findrbi(T v){
		Node* root=rooot;
		while(root!=NIL){
			if(cmp(v,root->v)){
				root=root->l;
			} else if(cmp(root->v,v)){
				root=root->r;
			} else{
				return root;
			}
		}
		return NIL;
	}
	Node* clonerbrecur(Node* root,Node* f,Node* NNIL) const{
		if(root==NIL){
			return NNIL;
		}
		Node* nroot=new Node{f,NNIL,NNIL,root->v,root->c,root->ir};
		nroot->l=clonerbrecur(root->l,nroot,NNIL);
		nroot->r=clonerbrecur(root->r,nroot,NNIL);
		return nroot;
	}
	rbtreenil<T> clonerbi(){
		rbtreenil<T> ntree(cmp);
		ntree.rooot=clonerbrecur(rooot,ntree.NIL,ntree.NIL);
		return ntree;
	}
public:
	rbtreenil():cmp([](const T a,const T b){return a<b;}){
		init();
	}
	rbtreenil(bool (*cmp_func)(const T,const T)):cmp(cmp_func){
		init();
	}
	~rbtreenil() {
		destroyrb(rooot);
		delete NIL;
	}
	rbtreenil(const rbtreenil& o):cmp(o.cmp){
		init();
		rooot=o.clonerbrecur(o.rooot,NIL,NIL);
	}
	rbtreenil& operator=(const rbtreenil& o){
		if(this!=&o){
			destroyrb(rooot);
			rooot=o.clonerbrecur(o.rooot,NIL,NIL);
			cmp=o.cmp;
		}
		return *this;
	}
	Node* root(){
		if(rooot==NIL){
			return nullptr;
		}
		return rooot;
	}
	Node* findrb(T v){
		Node* res=findrbi(v);
		if(res==NIL){
			return nullptr;
		}
		return res;
	}
	bool insertrb(T v){
		if(rooot==NIL){
			rooot=maken(NIL,v);
			rooot->ir=false;
			return true;
		}
		Node* root=rooot;
		Node* f=root;
		bool il=false;
		while(root!=NIL){
			f=root;
			if(cmp(v,root->v)){
				root=root->l;
				il=true;
			} else if(cmp(root->v,v)){
				root=root->r;
				il=false;
			} else{
				root->c++;
				return false;
			}
		}
		root=maken(f,v,il);
		insertfixup(root);
		rooot->ir=false;
		return true;
	}
	bool deleterb(T v){
		Node* root=findrbi(v);
		if(root==NIL){
			return false;
		}
		if(root->c>1){
			root->c--;
			return true;
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
		return true;
	}
	rbtreenil<T> clonerb(){
		return clonerbi();
	}
	std::vector<T> inorderrb() {
		std::vector<T> res;
		inorderrecur(rooot,res);
		return res;
	}
};