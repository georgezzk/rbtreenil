// class_stress.cpp — stress test for every public method of rbtreenil<T>.
// Build: clang++ -std=c++23 -O1 -g -static class_stress.cpp -o class_stress.exe
// Run one case per process: class_stress.exe <case> [seed] [trials]
// Cases: insert delete dupcount clone copy cmp find structure
#include "rbtreenil.hpp"
#include <algorithm>
#include <cstdio>
#include <functional>
#include <map>
#include <random>
#include <string>
#include <vector>
using namespace std;

// ---------- self-contained allocation tracker (leak + double-free, no tools) ----------
#include <atomic>
#include <cstdlib>
#include <new>
static std::atomic<long long> g_live{0}, g_new{0}, g_del{0};
static long long g_base=0;
static void* trk_alloc(size_t n){ void* p=std::malloc(n?n:1); if(p) g_live++; g_new++; return p; }
static void  trk_free(void* p){ if(p){ g_live--; g_del++; std::free(p);} }  // delete(nullptr) is a no-op
void* operator new(size_t n){ return trk_alloc(n); }
void* operator new[](size_t n){ return trk_alloc(n); }
void* operator new(size_t n,const std::nothrow_t&) noexcept{ return trk_alloc(n); }
void* operator new[](size_t n,const std::nothrow_t&) noexcept{ return trk_alloc(n); }
void  operator delete(void* p) noexcept{ trk_free(p); }
void  operator delete(void* p,size_t) noexcept{ trk_free(p); }
void  operator delete[](void* p) noexcept{ trk_free(p); }
void  operator delete[](void* p,size_t) noexcept{ trk_free(p); }
void  operator delete(void* p,const std::nothrow_t&) noexcept{ trk_free(p); }
void  operator delete[](void* p,const std::nothrow_t&) noexcept{ trk_free(p); }
struct LeakReport{
    ~LeakReport(){
        long long net=g_live-g_base;
        fprintf(stderr,"\n[leakcheck] base=%lld live=%lld net=%lld allocs=%lld frees=%lld -> %s\n",
            g_base,g_live.load(),net,g_new.load(),g_del.load(),
            net==0?"NO LEAK":(net>0?"*** LEAK ***":"*** OVER-FREE (double free / mismatched delete) ***"));
    }
} g_leakreport;

static bool lt_int(const int a,const int b){return a<b;}
static bool gt_int(const int a,const int b){return a>b;}

// ---------- content oracle ----------
// multiset of "present" values; inorderrb() must equal sorted expansion.
static bool oracle_ok(rbtreenil<int>& t,const map<int,int>& cnt){
    vector<int> exp;
    for(auto&[k,n]:cnt) for(int i=0;i<n;i++) exp.push_back(k);
    sort(exp.begin(),exp.end());
    vector<int> got=t.inorderrb();
    return exp==got;
}
// reverse-comparator oracle: inorder comes out descending.
static bool oracle_ok_desc(rbtreenil<int>& t,const map<int,int>& cnt){
    vector<int> exp;
    for(auto&[k,n]:cnt) for(int i=0;i<n;i++) exp.push_back(k);
    sort(exp.rbegin(),exp.rend());
    vector<int> got=t.inorderrb();
    return exp==got;
}

// ---------- internal RB invariant checker ----------
// Accesses the private Node through deduced pointers (public struct members).
// Detects: root not black, red node with red child, uneven black-height,
// broken BST order, parent-pointer corruption, sentinel loops (clone bug).
template<class Tree>
static bool check_rb(Tree* tree){
    auto* r=tree->root();
    if(r==nullptr) return true;
    using P=decltype(r);
    auto isNIL=[&](P p){return p->l==p && p->r==p;};
    if(!isNIL(r->f)) return false;              // root's parent must be the sentinel
    if(r->ir==true) return false;               // root must be black
    bool ok=true;
    function<int(P,P,int,int,int)> go;          // node,parent,lo_in? ,hi? ,depth -> blackheight, -1=fail
    // BST via explicit (min,max) with flags since values include INT_MIN etc: use parent relation instead
    function<int(P,int)> bh;                    // returns black height or -1 on failure
    bh=[&](P p,int depth)->int{
        if(depth>1000){ ok=false; return -1; }  // guard against clone/self loops
        if(isNIL(p)) return 1;
        // no red-red: if p is red, children must be black
        if(p->ir==true){
            if(!isNIL(p->l)&&p->l->ir==true) { ok=false; }
            if(!isNIL(p->r)&&p->r->ir==true) { ok=false; }
        }
        // parent pointers must point back
        if(!isNIL(p->l)&&p->l->f!=p){ ok=false; }
        if(!isNIL(p->r)&&p->r->f!=p){ ok=false; }
        int lb=bh(p->l,depth+1);
        int rb=bh(p->r,depth+1);
        if(lb==-1||rb==-1) return -1;
        if(lb!=rb){ ok=false; return -1; }
        return lb+(p->ir==false?1:0);           // count black node itself
    };
    int h=bh(r,0);
    return ok && h>=1;
}

// BST order check independent of structure validity (uses inorder which is order-safe for a valid tree).
template<class Tree>
static bool bst_sorted(Tree* tree){
    auto v=tree->inorderrb();
    return is_sorted(v.begin(),v.end());
}

// Deep structural equality: same shape, same values, same counts, same colors.
template<class Tree>
static bool struct_equal(Tree* a,Tree* b){
    using P=decltype(a->root());
    auto isNIL=[&](P p){return p->l==p && p->r==p;};
    function<bool(P,P)> eq;
    eq=[&](P x,P y)->bool{
        bool xn=isNIL(x),yn=isNIL(y);
        if(xn!=yn) return false;
        if(xn) return true;
        return x->v==y->v && x->c==y->c && x->ir==y->ir
            && eq(x->l,y->l) && eq(x->r,y->r);
    };
    auto* ra=a->root(); auto* rb=b->root();
    if(!ra||!rb) return ra==nullptr && rb==nullptr;
    return eq(ra,rb);
}

// Memory independence: collect every address each tree touches (node addresses
// plus their l/r/f child pointers, which include the sentinel). Two correct trees
// must share NOTHING. A clone that links to the source's sentinel, or a shallow
// copy sharing nodes, produces an overlap -> returns false. This catches aliasing
// that the self-loop sentinel check cannot distinguish on its own.
template<class Tree>
static bool disjoint(Tree* a,Tree* b){
    using P=decltype(a->root());
    auto isNIL=[&](P p){return p->l==p && p->r==p;};
    auto collect=[&](Tree* t){
        vector<P> s;
        function<void(P)> dfs=[&](P p){
            s.push_back(p); s.push_back(p->l); s.push_back(p->r); s.push_back(p->f);
            if(!isNIL(p->l)) dfs(p->l);
            if(!isNIL(p->r)) dfs(p->r);
        };
        auto* r=t->root();
        if(r) dfs(r);
        return s;
    };
    vector<P> sa=collect(a), sb=collect(b);
    for(P x:sb) if(find(sa.begin(),sa.end(),x)!=sa.end()) return false;
    return true;
}

int main(int argc,char** argv){
    if(argc<2){ printf("usage: %s <case> [seed] [trials]\n",argv[0]); return 2; }
    string mode=argv[1];
    unsigned seed=argc>2?stoul(argv[2]):1u;
    int trials=argc>3?stoi(argv[3]):200;
    mt19937 rng(seed);

    // warmup: settle lazy CRT/stdio allocations, then snapshot the live baseline
    { rbtreenil<int> w; w.insertrb(1); w.insertrb(2); w.insertrb(2); w.deleterb(2); (void)w.inorderrb(); }
    fprintf(stderr,"");
    g_base=g_live;

    if(mode=="selfleak"){
        // negative control: leak on purpose, LeakReport must flip to *** LEAK ***
        for(int i=0;i<5;i++){ int* p=new int(i); (void)p; }          // leak 5 ints
        rbtreenil<int>* t=new rbtreenil<int>(); t->insertrb(9);      // leak a tree
        printf("selfleak: intentionally leaked 6 objects\n");
        return 0;                                                     // dtor prints net=+6 expected
    }

    if(mode=="insert"){
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> tr;
            map<int,int> cnt;
            int n=1+(rng()%40);
            uniform_int_distribution<int> vd(0,30);
            for(int i=0;i<n;i++){ int x=vd(rng); tr.insertrb(x); cnt[x]++; }
            if(!oracle_ok(tr,cnt)||!check_rb(&tr)||!bst_sorted(&tr)){ bad++;
                printf("insert FAIL t=%d n=%d\n",t,n); }
        }
        printf("insert: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    if(mode=="delete"){
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> tr;
            map<int,int> cnt;
            vector<int> ins;
            int n=1+(rng()%35);
            uniform_int_distribution<int> vd(0,40);
            for(int i=0;i<n;i++){ int x=vd(rng); tr.insertrb(x); cnt[x]++; ins.push_back(x); }
            shuffle(ins.begin(),ins.end(),rng);
            int k=rng()%(n+1);
            for(int i=0;i<k;i++){
                int x=ins[i];
                bool present=cnt[x]>0;
                bool ret=tr.deleterb(x);
                if(ret!=present){ bad++; printf("delete ret FAIL t=%d x=%d present=%d ret=%d\n",t,x,(int)present,(int)ret); }
                if(present){ cnt[x]--; if(cnt[x]==0) cnt.erase(x); }
                if(!oracle_ok(tr,cnt)||!check_rb(&tr)){ bad++; printf("delete STATE FAIL t=%d x=%d\n",t,x); return 1; }
            }
            if(!oracle_ok(tr,cnt)||!check_rb(&tr)||!bst_sorted(&tr)){ bad++; printf("delete END FAIL t=%d\n",t); }
        }
        printf("delete: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    if(mode=="dupcount"){
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> tr;
            map<int,int> cnt;
            for(int j=0;j<20;j++){ int x=cnt.size()? (int)(rng()%3) : 0; tr.insertrb(x); cnt[x]++; }
            if(!oracle_ok(tr,cnt)||!check_rb(&tr)){ bad++; }
            // delete duplicates one at a time; node must survive until last
            for(int rep=0;rep<3;rep++){
                int x=(int)(rng()%3);
                bool present=cnt[x]>0;
                tr.deleterb(x);
                if(present){ cnt[x]--; if(cnt[x]==0) cnt.erase(x); }
                if(!oracle_ok(tr,cnt)||!check_rb(&tr)){ bad++; printf("dupcount FAIL t=%d rep=%d\n",t,rep); }
            }
        }
        printf("dupcount: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    if(mode=="clone"){
        // BUG-2 probe: cloned leaves reference the ORIGINAL's sentinel.
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> orig;
            map<int,int> cnt;
            int n=1+(rng()%25);
            uniform_int_distribution<int> vd(0,30);
            for(int i=0;i<n;i++){ int x=vd(rng); orig.insertrb(x); cnt[x]++; }
            rbtreenil<int> c=orig.clonerb();
            if(!oracle_ok(c,cnt)){ bad++; printf("clone CONTENT FAIL t=%d\n",t); }
            if(!check_rb(&c)){ bad++; printf("clone STRUCT FAIL t=%d\n",t); }
            // mutate clone; original must be untouched
            vector<int> keys=c.inorderrb(); shuffle(keys.begin(),keys.end(),rng);
            map<int,int> cc=cnt;
            if(!keys.empty()){ int x=keys[0]; c.deleterb(x); cc[x]--; if(cc[x]==0)cc.erase(x); }
            if(!oracle_ok(c,cc)){ bad++; printf("clone MUT FAIL t=%d\n",t); }
            if(!oracle_ok(orig,cnt)){ bad++; printf("clone ORIG CLOBBERED t=%d\n",t); return 1; }
            // destructors run here — expected crash if sentinel shared
        }
        printf("clone: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    if(mode=="copy"){
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> a;
            map<int,int> acnt;
            int n=1+(rng()%30);
            uniform_int_distribution<int> vd(0,40);
            for(int i=0;i<n;i++){ int x=vd(rng); a.insertrb(x); acnt[x]++; }
            {
                rbtreenil<int> b=a;                 // copy constructor (deep)
                map<int,int> bcnt=acnt;
                if(!struct_equal(&a,&b)){ bad++; printf("copy EQFAIL t=%d\n",t); return 1; }
                if(!disjoint(&a,&b)){ bad++; printf("copy ALIAS t=%d\n",t); return 1; }
                if(!check_rb(&b)||!oracle_ok(b,bcnt)){ bad++; printf("copy CCTOR t=%d\n",t); return 1; }
                // operator= into an already-populated b from a fresh a2
                rbtreenil<int> a2; map<int,int> a2cnt;
                int m=1+(rng()%20);
                for(int i=0;i<m;i++){ int x=vd(rng); a2.insertrb(x); a2cnt[x]++; }
                b=a2; bcnt=a2cnt;                    // assign over b's existing tree
                if(!struct_equal(&a2,&b)||!disjoint(&a2,&b)){ bad++; printf("copy ASSIGN t=%d\n",t); return 1; }
                if(!check_rb(&b)||!oracle_ok(b,bcnt)){ bad++; printf("copy ASSIGNSTATE t=%d\n",t); return 1; }
                b=b;                                 // self-assignment guard
                if(!check_rb(&b)||!oracle_ok(b,bcnt)){ bad++; printf("copy SELFASSIGN t=%d\n",t); return 1; }
                // divergent mutation on b must not touch a
                vector<int> bk=b.inorderrb(); shuffle(bk.begin(),bk.end(),rng);
                for(int i=0;i<m/2 && i<(int)bk.size();i++){ int x=bk[i]; b.deleterb(x); if(bcnt[x]>0){bcnt[x]--; if(!bcnt[x])bcnt.erase(x);} }
                b.insertrb(1000); bcnt[1000]++;
                if(!oracle_ok(b,bcnt)||!check_rb(&b)){ bad++; printf("copy BMUT t=%d\n",t); return 1; }
                if(!oracle_ok(a,acnt)||!check_rb(&a)){ bad++; printf("copy A CLOBBERED t=%d\n",t); return 1; }
                // destructors run here for b and a2 — must not double-free
            }
            if(!oracle_ok(a,acnt)){ bad++; printf("copy A-DTOR t=%d\n",t); return 1; }
        }
        printf("copy: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    if(mode=="cmp"){
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> tr(gt_int);      // reverse comparator
            map<int,int> cnt;
            int n=1+(rng()%30);
            uniform_int_distribution<int> vd(0,40);
            for(int i=0;i<n;i++){ int x=vd(rng); tr.insertrb(x); cnt[x]++; }
            if(!oracle_ok_desc(tr,cnt)){ bad++; printf("cmp ORDER FAIL t=%d\n",t); }
            if(!check_rb(&tr)){ bad++; printf("cmp STRUCT FAIL t=%d\n",t); }
            // find must respect the same order
            vector<int> keys; for(auto&[k,_]:cnt) keys.push_back(k);
            for(int x:keys) if(tr.findrb(x)==nullptr){ bad++; printf("cmp FIND FAIL t=%d x=%d\n",t,x); }
        }
        printf("cmp: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    if(mode=="find"){
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> tr;
            map<int,int> cnt;
            uniform_int_distribution<int> vd(0,50);
            for(int i=0;i<25;i++){ int x=vd(rng); tr.insertrb(x); cnt[x]++; }
            for(int x=0;x<60;x++){
                bool want=cnt[x]>0;
                bool got=tr.findrb(x)!=nullptr;
                if(want!=got){ bad++; printf("find FAIL x=%d want=%d got=%d\n",x,(int)want,(int)got); }
            }
        }
        printf("find: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    if(mode=="structure"){
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> tr;
            map<int,int> cnt;
            int n=1+(rng()%40);
            uniform_int_distribution<int> vd(0,50);
            vector<int> ins;
            for(int i=0;i<n;i++){ int x=vd(rng); tr.insertrb(x); cnt[x]++; ins.push_back(x); }
            shuffle(ins.begin(),ins.end(),rng);
            int k=rng()%(n+1);
            for(int i=0;i<k;i++){ int x=ins[i]; tr.deleterb(x); if(cnt[x]>0){cnt[x]--; if(!cnt[x])cnt.erase(x);} }
            if(!check_rb(&tr)||!oracle_ok(tr,cnt)){ bad++; printf("structure FAIL t=%d\n",t); return 1; }
        }
        printf("structure: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    if(mode=="selftest"){
        rbtreenil<int> a;
        for(int i=0;i<12;i++) a.insertrb(i*3%11);
        rbtreenil<int> c=a.clonerb();
        bool self_dj=disjoint(&a,&a);       // a tree shares every address with itself -> must be FALSE
        bool self_eq=struct_equal(&a,&a);   // trivially TRUE
        bool clone_dj=disjoint(&a,&c);      // real deep copy -> must be TRUE
        bool clone_eq=struct_equal(&a,&c);
        bool pass=(!self_dj && self_eq && clone_dj && clone_eq);
        printf("selftest: self_disjoint=%d(want0) clone_disjoint=%d(want1) -> %s\n",
               (int)self_dj,(int)clone_dj, pass?"OK":"FAIL");
        return pass?0:1;
    }

    if(mode=="cloneedge"){
        int bad=0;
        // empty tree clone
        { rbtreenil<int> e; rbtreenil<int> c=e.clonerb();
          if(!disjoint(&e,&c)||!c.inorderrb().empty()){ bad++; printf("cloneedge EMPTY fail\n"); } }
        // single node (root->f must be the clone's own sentinel)
        { rbtreenil<int> o; o.insertrb(42); map<int,int> cnt{{42,1}};
          rbtreenil<int> c=o.clonerb();
          if(!struct_equal(&o,&c)||!disjoint(&o,&c)||!check_rb(&c)||!oracle_ok(c,cnt)){ bad++; printf("cloneedge SINGLE fail\n"); } }
        // all duplicates (count field survives clone)
        { rbtreenil<int> o; map<int,int> cnt;
          for(int i=0;i<7;i++){ o.insertrb(5); cnt[5]++; }
          rbtreenil<int> c=o.clonerb();
          if(!struct_equal(&o,&c)||!disjoint(&o,&c)||!oracle_ok(c,cnt)){ bad++; printf("cloneedge DUP fail\n"); } }
        // clone then fully empty the clone; original must be untouched
        { rbtreenil<int> o; map<int,int> cnt; vector<int> keys;
          uniform_int_distribution<int> vd(0,40);
          for(int i=0;i<20;i++){ int x=vd(rng); o.insertrb(x); cnt[x]++; keys.push_back(x); }
          rbtreenil<int> c=o.clonerb(); map<int,int> ccnt=cnt;
          for(int x:keys){ c.deleterb(x); if(ccnt[x]>0){ccnt[x]--; if(!ccnt[x])ccnt.erase(x);} }
          map<int,int> empty;
          if(!oracle_ok(o,cnt)||!check_rb(&o)){ bad++; printf("cloneedge ORIG-CLOBBERED fail\n"); }
          if(!oracle_ok(c,empty)||!check_rb(&c)){ bad++; printf("cloneedge CLONE-NOT-EMPTIED fail\n"); } }
        printf("cloneedge: %d bad\n",bad); return bad?1:0;
    }

    if(mode=="clonehard"){
        int bad=0;
        for(int t=0;t<trials;t++){
            rbtreenil<int> orig;
            map<int,int> ocnt;
            int n=1+(rng()%30);
            uniform_int_distribution<int> vd(0,40);
            for(int i=0;i<n;i++){ int x=vd(rng); orig.insertrb(x); ocnt[x]++; }
            rbtreenil<int> c=orig.clonerb();
            map<int,int> ccnt=ocnt;
            // 1) clone must be a faithful, structurally-valid, memory-disjoint copy
            if(!struct_equal(&orig,&c)){ bad++; printf("clonehard EQFAIL t=%d\n",t); return 1; }
            if(!disjoint(&orig,&c)){ bad++; printf("clonehard ALIAS t=%d (shared nodes/sentinel)\n",t); return 1; }
            if(!check_rb(&c)||!check_rb(&orig)){ bad++; printf("clonehard STRUCT t=%d\n",t); return 1; }
            if(!oracle_ok(c,ccnt)||!oracle_ok(orig,ocnt)){ bad++; printf("clonehard CONTENT t=%d\n",t); return 1; }
            // 2) divergent mutation: edits must stay independent
            for(int i=0;i<n;i++){
                int a=vd(rng), b=vd(rng);
                orig.insertrb(a); ocnt[a]++;
                c.insertrb(b);    ccnt[b]++;
            }
            vector<int> ok=orig.inorderrb(); shuffle(ok.begin(),ok.end(),rng);
            vector<int> ck=c.inorderrb();   shuffle(ck.begin(),ck.end(),rng);
            for(int i=0;i<n/2 && i<(int)ok.size();i++){ int x=ok[i]; orig.deleterb(x); if(ocnt[x]>0){ocnt[x]--; if(!ocnt[x])ocnt.erase(x);} }
            for(int i=0;i<n/2 && i<(int)ck.size();i++){ int x=ck[i]; c.deleterb(x);   if(ccnt[x]>0){ccnt[x]--; if(!ccnt[x])ccnt.erase(x);} }
            if(!oracle_ok(orig,ocnt)||!check_rb(&orig)){ bad++; printf("clonehard ORIGDRIFT t=%d\n",t); return 1; }
            if(!oracle_ok(c,ccnt)||!check_rb(&c)){ bad++; printf("clonehard CLONEDIFF t=%d\n",t); return 1; }
            if(!disjoint(&orig,&c)){ bad++; printf("clonehard ALIAS2 t=%d\n",t); return 1; }
            // 3) clone-of-clone
            rbtreenil<int> c2=c.clonerb();
            if(!struct_equal(&c,&c2)||!disjoint(&c,&c2)||!oracle_ok(c2,ccnt)){
                bad++; printf("clonehard CHAIN t=%d\n",t); return 1;
            }
        }
        printf("clonehard: %d/%d bad\n",bad,trials); return bad?1:0;
    }

    printf("unknown case %s\n",mode.c_str());
    return 2;
}
