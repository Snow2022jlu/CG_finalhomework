#include<bits/stdc++.h>
#pragma optimize("-O2")
#define sight(c) ('0'<=c&&c<='9')
#define Ma 2147483647
#define N 5000007
//#define getchar nc
inline char nc() {
    static char buf[1000000], * p1 = buf, * p2 = buf;
    return p1 == p2 && (p2 = (p1 = buf) + fread(buf, 1, 1000000, stdin), p1 == p2) ? EOF : *p1++;
}
inline void read(int& x) {
    static char c; static int b;
    for (b = 1, c = getchar(); !sight(c); c = getchar()) if (c == '-') b = -1;
    for (x = 0; sight(c); c = getchar()) x = x * 10 + c - 48;
    x *= b;
}
void write(int x) { if (x < 10) { putchar('0' + x); return; } write(x / 10); putchar('0' + x % 10); }
inline void writeln(int x) { if (x < 0) putchar('-'), x *= -1; write(x); putchar('\n'); }
using namespace std;
double x1[N], Y1[N], x2[N], y2[N];
struct Event {
    int id, type;
    double time;
    Event() {}
    Event(double _time, int _id, int _type) :time(_time), id(_id), type(_type) {}
    bool operator < (const Event& other) const {
        if (time != other.time) return time > other.time;
        if (type != other.type) return type < other.type;
        if (id != other.id) return id > other.id;
		return false; // 保证相同的事件不会被重复处理
    }
};
inline int Rand() {
    static int x = 23333;
    return x ^= x << 13, x ^= x >> 17, x ^= x << 5;
}
struct T {
    T* s[2]; int key, siz, val;
    T() {}
    T(int x) { s[0] = s[1] = NULL; key = x; siz = 1; val = Rand(); }
    void rub() {
        siz = 1;
        if (s[0]) siz += s[0]->siz;
        if (s[1]) siz += s[1]->siz;
    }
};
T* ro[N], * X, * Y, * Z;
#define eps 1e-9
#define Eps 1e-6
#define EPS 1e-3
//void spilt(T* now, int k, T*& x, T*& y) {
//    if (!now) { x = y = NULL; return; }
//    int cmp = now->s[0] ? now->s[0]->siz + 1 : 1;
//    if (k < cmp) y = now, spilt(y->s[0], k, x, y->s[0]);
//    else x = now, spilt(x->s[1], k - cmp, x->s[1], y);
//    now->rub();
//}
void Spilt(T* now, int k, T*& x, T*& y) {
    if (!now) { x = y = NULL; return; }
    int cmp = now->s[0] ? now->s[0]->siz + 1 : 1;
    if (k < cmp) { y = new T(); *y = *now; Spilt(y->s[0], k, x, y->s[0]); y->rub(); }
    else { x = new T(); *x = *now; Spilt(x->s[1], k-cmp, x->s[1], y); x->rub(); }
}
//void spilt2(T* now, int k, T*& x, T*& y) {
//    if (!now) { x = y = NULL; return; }
//    if (k <= now->key) y = now, spilt2(y->s[0], k, x, y->s[0]);
//    else x = now, spilt2(x->s[1], k, x->s[1], y);
//    now->rub();
//}
bool leq(int id1, int id2, double time) {
    if (id1 == Ma) return false;
    if (id2 == Ma) return true;
    double YY1 = Y1[id1] + (time - x1[id1]) * (y2[id1] - Y1[id1]) / (x2[id1] - x1[id1]);
    double YY2 = Y1[id2] + (time - x1[id2]) * (y2[id2] - Y1[id2]) / (x2[id2] - x1[id2]);
    return YY1 < YY2-eps || (fabs(YY1-YY2)<eps && (y2[id1] - Y1[id1]) / (x2[id1] - x1[id1])<= (y2[id2] - Y1[id2]) / (x2[id2] - x1[id2]));
}
void Spilt2(T* now, int k, T*& x, T*& y, double time) {
    if (!now) { x = y = NULL; return; }
    if (leq(k, now->key, time)) { y = new T(); *y = *now; Spilt2(y->s[0], k, x, y->s[0], time); y->rub(); }
    else { x = new T(); *x = *now; Spilt2(x->s[1], k, x->s[1], y, time); x->rub(); }
}
//T* merge(T* x, T* y) {
//    if (!x) return y; if (!y) return x;
//    if (x->val < y->val) { x->s[1] = merge(x->s[1], y); x->rub(); return x; }
//    y->s[0] = merge(x, y->s[0]); y->rub(); return y;
//}
T* Merge(T* x, T* y) {
    if (!x) return y; if (!y) return x;
    if (x->val < y->val) {
        T* g = new T(); *g = *x;
        g->s[1] = Merge(g->s[1], y); g->rub(); return g;
    }
    T* g = new T(); *g = *y;
    g->s[0] = Merge(x, g->s[0]); g->rub(); return g;
}
//int pre(T* now, int x) {
//    if (!now) return -Ma;
//    if (now->key >= x) return pre(now->s[0], x);
//    return max(now->key, pre(now->s[1], x));
//}
int net(T* now, int x, double time) {
    if (!now) return Ma;
    if (leq(now->key, x, time)) return net(now->s[1], x, time);
    int son = net(now->s[0], x, time);
    return leq(now->key, son, time) ? now->key : son;
}
int m, n, v, op, x, tot;
priority_queue <Event> Q;
double tim[N];
T* rt[N];

void Draw(T* now) {
    if (now == NULL) return;
    printf("[");
    Draw(now->s[0]);
    printf(" %d ", now->key);
    Draw(now->s[1]);
    printf("]");
}
int Rnd(int n){
	return ((rand()<<12)^rand())%n+1;
}
int main() {
//	freopen("input.txt", "r", stdin);
    scanf("%d", &n);
    srand((unsigned int)time(NULL));
    for (int i = 1; i <= n; i++) {
    	x1[i]=Rnd(2*n); x2[i]=Rnd(2*n);
    	Y1[i]=Rnd(2*n); y2[i]=Rnd(2*n);
//        scanf("%lf%lf%lf%lf", &x1[i], &Y1[i], &x2[i], &y2[i]);
        if (x1[i] > x2[i]) swap(x1[i], x2[i]), swap(Y1[i], y2[i]);
        x2[i] -= EPS;
        //x1[i] -= Eps, Y1[i] -= Eps, x2[i] += Eps, y2[i] += Eps;
        Q.push(Event{ x1[i],i,1 });
        Q.push(Event{ x2[i],i,-1 });
    }
    auto start = std::chrono::high_resolution_clock::now();
    while (Q.size()) {
        auto event = Q.top();
//        printf("%d %d %lf", event.id, event.type, event.time);
        Q.pop();
        if (event.type == 1) {
            Spilt2(rt[tot], event.id, X, Y, event.time);
            rt[++tot] = Merge(Merge(X, new T(event.id)), Y);
//            Draw(rt[tot]);
        }
        else {
            Spilt2(rt[tot], event.id, X, Y, event.time);
            //Draw(X); puts("");
			//Draw(Y); puts("");
            Spilt(Y, 1, Y, Z);
			//Draw(Y); puts("");
            //Draw(Z); puts("");
            rt[++tot] = Merge(X, Z);
//            Draw(rt[tot]);
        }
        tim[tot] = event.time;
//        puts("");
    }
    auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "耗时: " << duration.count() << " 微秒" << std::endl;
//    for (int i = 1; i <= tot; i++) {
//        Draw(rt[i]); puts("");
//    }
    int q, ans;
    double x, y;
    q=1000000;
//    scanf("%d", &q);
	start = std::chrono::high_resolution_clock::now();
    while (q--) {
    	x=Rnd(2*n); y=Rnd(2*n);
//        scanf("%lf%lf", &x, &y);
        int id = upper_bound(tim + 1, tim + tot + 1, x) - tim;
        if (id == 0 or tim[tot]<x) {
//            printf("%d\n", 2147483647);
            continue;
        }
        id--;
        if (id <= 0) 
		x1[0] = 0; Y1[0] = y-eps; x2[0] = 1e9; y2[0] = y-eps;
        ans = net(rt[id], 0, x);
//        printf("%d\n", ans);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "耗时: " << duration.count() << " 微秒" << std::endl;
    //	freopen("testdata.in","r",stdin);
    //	freopen("rr.out","w",stdout);
    //    read(m);
    //    while (m--) {
    //        read(v); read(op); read(x);
    //        switch (op) {
    //            case 1: Spilt2(ro[v],x,X,Y); 
    //              ro[++tot]=merge(merge(X,new T(x)),Y); break;
    //            case 2: Spilt2(ro[v],x,X,Y);
    //              spilt(Y,1,Y,Z); 
    //			  if (Y&&Y->key^x) Z=merge(Y,Z);
    //              ro[++tot]=merge(X,Z); break;
    //            case 3: spilt2(ro[v],x,X,Y); 
    //              writeln((X?X->siz:0)+1); 
    //              ro[v]=merge(X,Y); ro[++tot]=ro[v];break;
    //            case 4: 
    //            spilt(ro[v],x,X,Y); 
    //            spilt(X,x-1,X,Z);
    //              writeln(Z->key); 
    //			  X=merge(X,Z); 
    //			  ro[v]=merge(X,Y);
    //			  ro[++tot]=ro[v]; 
    //			  break;
    //            case 5: writeln(pre(ro[v],x));ro[++tot]=ro[v]; break;
    //            case 6: writeln(net(ro[v],x));ro[++tot]=ro[v]; break;	
    //        }
    //    } return 0;
}
