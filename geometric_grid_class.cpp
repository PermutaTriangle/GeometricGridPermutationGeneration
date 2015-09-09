#include <bits/stdc++.h>
#include "gurobi_c++.h"
using namespace std;
typedef long long ll;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;
typedef vector<vi> vvi;
template <class T> int size(const T &x) { return x.size(); }
const int INF = 2147483647;
#define rep(i,a,b) for (__typeof(a) i=(a); i<(b); ++i)
#define iter(it,c) for (__typeof((c).begin()) it = (c).begin(); it != (c).end(); ++it)

typedef vector<char> Permutation;

const int n = 3,
          m = 3,
          k = 9;

#define INCR 1
#define DECR -1
#define EMPTY 0
#define UNSPEC 2
int grid[n][m];
// int grid[n][m] = {
//     {0,-1},
//     {-1,0}
// };

double EPS = 0.001;
// double EPS = 1e-100;

GRBEnv env;

struct Node {
    int id;
    Node *left, *right, *parent;
    Node(int id, Node *parent=NULL) {
        this->id = id;
        this->left = NULL;
        this->right = NULL;
        this->parent = parent;
    }
    ~Node() {
        if (this->left) delete this->left;
        if (this->right) delete this->right;
        this->left = this->right = this->parent = NULL;
    }
};

struct BinarySearchTree {
    Node *root;
    map<int, Node*> node;

    BinarySearchTree() {
        this->root = NULL;
    }

    ~BinarySearchTree() {
        if (this->root != NULL) {
            delete this->root;
            this->root = NULL;
        }
        this->node.clear();
    }

    void attach_right(int id, Node *node) {
        while (node->right != NULL)
            node = node->right;
        node->right = this->node[id] = new Node(id, node);
    }

    void attach_left(int id, Node *node) {
        while (node->left != NULL)
            node = node->left;
        node->left = this->node[id] = new Node(id, node);
    }

    void insert_before(int id, int right_id) {
        if (right_id == -1) {
            if (this->root == NULL) {
                this->root = this->node[id] = new Node(id);
            } else {
                this->attach_right(id, this->root);
            }
        } else {
            Node* right_node = this->node[right_id];
            if (right_node->left == NULL) {
                right_node->left = this->node[id] = new Node(id, right_node);
            } else {
                this->attach_right(id, right_node->left);
            }
        }
    }

    void insert_after(int id, int left_id) {
        if (left_id == -1) {
            if (this->root == NULL) {
                this->root = this->node[id] = new Node(id);
            } else {
                this->attach_left(id, this->root);
            }
        } else {
            Node *left_node = this->node[left_id];
            if (left_node->right == NULL) {
                left_node->right = this->node[id] = new Node(id, left_node);
            } else {
                this->attach_left(id, left_node->right);
            }
        }
    }

    void remove(int id) {
        Node* node = this->node[id];
        this->node.erase(id);
        if (node->parent == NULL) {
            this->root = NULL;
        } else if (node->parent->left == node) {
            node->parent->left = NULL;
        } else if (node->parent->right == node) {
            node->parent->right = NULL;
        } else {
            assert(false);
        }
        node->parent = NULL;
        delete node;
    }

    void _dfs(Node *v, Permutation &res) {
        if (v == NULL) {
            return;
        }
        _dfs(v->left, res);
        res.push_back(v->id);
        _dfs(v->right, res);
    }

    Permutation inorder_traversal() {
        Permutation res;
        _dfs(this->root, res);
        return res;
    }
};

Permutation construct_permutation(const vector<Permutation> &rows, const vector<Permutation> &cols) {
    int n = size(rows);
    int m = size(cols);
    int l = 0;
    rep(i,0,m) {
        l += size(cols[i]);
    }
    Permutation idx(l,-1),
                perm(l,-1);
    int at = 0;
    rep(i,0,m) {
        rep(j,0,size(cols[i])) {
            idx[cols[i][j]] = at;
            at += 1;
        }
    }

    at = 1;
    for (int i = n-1; i >= 0; i--) {
        for (int j = size(rows[i]) - 1; j >= 0; j--) {
            perm[idx[rows[i][j]]] = at;
            at += 1;
        }
    }

    return perm;
}

bool is_valid(const vector<Permutation> &rows, const vector<Permutation> &cols) {
    // return true;

    GRBModel p(env);
    // GRBVar v = model.addVar(0.0, 1.0, -0.5, GRB_CONTINUOUS, "test");
    // GRBVar v = model.addVar(2.0, 1.0, -0.5, GRB_CONTINUOUS, "test");
    // model.update();
    // model.optimize();
    // cout << model.get(GRB_IntAttr_SolCount) << endl;
    // int res = model.get(GRB_IntAttr_Status);
    // assert(res == 2 || res == 3);
    // return res == 2;
    // cout << model.getVarByName("test").get(GRB_DoubleAttr_X) << endl;

    // return 0;

    int n = size(rows),
        m = size(cols),
        l = 0;
    rep(i,0,size(cols)) {
        l += size(cols[i]);
    }

    vi colorder;
    rep(i,0,m) {
        rep(j,0,size(cols[i])) {
            colorder.push_back(cols[i][j]);
        }
    }

    vi rowidx(l, -1),
       colidx(l, -1),
       idx(l, -1);
    int at = 0;
    rep(i,0,m) {
        rep(j,0,size(cols[i])) {
            idx[cols[i][j]] = at;
            at += 1;
        }
    }
    rep(i,0,m) {
        rep(j,0,size(cols[i])) {
            colidx[cols[i][j]] = i;
        }
    }
    rep(i,0,n) {
        rep(j,0,size(rows[i])) {
            rowidx[rows[i][j]] = i;
        }
    }

    vector<GRBVar> var(l);
    rep(i,0,l) {
        int y = n-rowidx[colorder[i]]-1,
            x = colidx[colorder[i]];
        var[colorder[i]] = p.addVar(x+EPS, x+1-EPS, 1.0, GRB_CONTINUOUS);
    }

    p.update();

    rep(i,0,l-1) {
        p.addConstr(var[colorder[i]]+EPS <= var[colorder[i+1]]);
    }

    vi order;
    rep(i,0,n) {
        rep(j,0,size(rows[i])) {
            order.push_back(rows[i][j]);
        }
    }
    rep(i,0,l-1) {
        int a = order[i],
            b = order[i+1];

        int aj = n-rowidx[a]-1,
            ai = colidx[a];

        int bj = n-rowidx[b]-1,
            bi = colidx[b];

        // f = lambda x: (aj - (x - ai) + 1) if grid[rowidx[a]][colidx[a]] == -1 else (aj + x - ai)
        // g = lambda x: (bj - (x - bi) + 1) if grid[rowidx[b]][colidx[b]] == -1 else (bj + x - bi)

        GRBLinExpr e1, e2;
        if (grid[rowidx[a]][colidx[a]] == -1) {
            e1 = aj - var[a] + ai + 1;
        } else {
            e1 = aj + var[a] - ai;
        }
        if (grid[rowidx[b]][colidx[b]] == -1) {
            e2 = bj - var[b] + bi + 1;
        } else {
            e2 = bj + var[b] - bi;
        }

        // f = lambda x: aj - x + ai + 1 if grid[rowidx[a]][colidx[a]] == -1 else aj + x - ai
        // g = lambda x: bj - (x - bi) + 1) if grid[rowidx[b]][colidx[b]] == -1 else (bj + x - bi)
        // f = lambda x: aj - var[a] + ai + 1 if grid[rowidx[a]][colidx[a]] == -1 else aj + var[a] - ai
        // g = lambda x: bj - var[b] + bi + 1 if grid[rowidx[b]][colidx[b]] == -1 else bj + var[b] - bi

        // p.add_constraint(f(var[a]) - EPS >= g(var[b]))
        p.addConstr(e1 - EPS, GRB_GREATER_EQUAL, e2);
    }

    p.update();
    p.optimize();
    int res = p.get(GRB_IntAttr_Status);
    assert(res == 2 || res == 3);
    return res == 2;
}

BinarySearchTree* rows[n];
BinarySearchTree* cols[m];
map<vector<vector<char> >, set<Permutation> > res;

struct Backtrack {

    #define OTHERSTUFF x,y,horizontal,vertical,hlines,vlines

    void bt2(int k, int i, int j, int id, bool must, int x, int y, int horizontal, int vertical, const Permutation &hlines, const Permutation &vlines) {
        if (k > 0) {
            // put one more here
            if (grid[x][y] == -1) {
                rows[x]->insert_before(id, i < size(hlines) ? hlines[i] : -1);
            } else {
                rows[x]->insert_after(id, 0 <= i-1 ? hlines[i-1] : -1);
            }
            cols[y]->insert_before(id, j < size(vlines) ? vlines[j] : -1);
            bt2(k - 1, i, j, id + 1, false, OTHERSTUFF);
            rows[x]->remove(id);
            cols[y]->remove(id);
        }

        if (!must) {
            // don't put anything more down
            bt(k, x, y + 1, id);

            // jump somewhere else
            if (k > 0) {
                int di = -grid[x][y],
                    dj = 1;

                rep(up,0, grid[x][y] == 1 ? i+1 : horizontal+1 - i) {
                    rep(right, 0, vertical+1 - j) {
                        // don't jump here again...
                        if (up == 0 && right == 0) {
                            continue;
                        }

                        bt2(k, i + up * di, j + right * dj, id, true, OTHERSTUFF);
                    }
                }
            }
        }
    }

    void bt(int k, int x, int y, int id) {

        if (y == m) {
            bt(k, x + 1, 0, id);
            return;
        } else if (x == n) {
            vector<Permutation> roworders, colorders;
            rep(i,0,n) {
                roworders.push_back(rows[i]->inorder_traversal());
            }
            rep(i,0,m) {
                colorders.push_back(cols[i]->inorder_traversal());
            }
            vector<vector<char> > gridt(n, vector<char>(m));
            rep(i,0,n) {
                rep(j,0,m) {
                    gridt[i][j] = grid[i][j];
                }
            }
            Permutation perm = construct_permutation(roworders, colorders);
            // if (size(perm) != 2) return;
            // if (perm[0] != 1) return;
            // if (perm[1] != 2) return;
            // cout << "perm = ";
            // rep(i,0,size(perm)) {
            //     cout << perm[i] << " ";
            // }
            // cout << ", valid: " << (is_valid(roworders, colorders) ? "yes" : "no");
            // cout << endl;
            if (res[gridt].find(perm) == res[gridt].end() && is_valid(roworders, colorders)) {
                res[gridt].insert(perm);
            }
            return;
        }

        Permutation hlines = rows[x]->inorder_traversal(),
        vlines = cols[y]->inorder_traversal();

        int horizontal = size(hlines);
        int vertical = size(vlines);

        int lo = -1,
            hi = 1;

        if (grid[x][y] != UNSPEC) {
            lo = hi = grid[x][y];
        }

        rep(t,lo,hi+1) {
            grid[x][y] = t;

            if (x == 0 && y <= 1) {
                printf("%d %d %d %d\n", x, y, t, k);
            }

            if (t == 1) {
                // increasing at (x,y)
                bt2(k, horizontal, 0, id, false, OTHERSTUFF);
            } else if (t == -1) {
                // decreasing at (x,y)
                bt2(k, 0, 0, id, false, OTHERSTUFF);
            }
            else {
                // empty at (x,y)
                bt(k, x, y + 1, id);
            }
        }

        if (lo < hi) {
            grid[x][y] = UNSPEC;
        }
    }
};


bool cmp(const Permutation &a, const Permutation &b) {
    if (size(a) != size(b))
        return size(a) < size(b);
    return a < b;
}

int main() {
    env.set(GRB_IntParam_OutputFlag, 0);

    rep(i,0,n) rep(j,0,m) grid[i][j] = UNSPEC;
    rep(i,0,n) rows[i] = new BinarySearchTree();
    rep(i,0,m) cols[i] = new BinarySearchTree();

    Backtrack bt;
    bt.bt(k, 0, 0, 0);

    stringstream outf;
    outf << n << "x" << m << "_" << k << "_fast.txt";
    ofstream outs(outf.str().c_str());

    iter(it, res) {
        outs << "[";
        rep(i,0,n) {
            if (i != 0) {
                outs << ",";
            }
            outs << "[";
            rep(j,0,m) {
                if (j != 0) {
                    outs << ",";
                }
                outs << static_cast<int>(it->first[i][j]);
            }
            outs << "]";
        }
        outs << "]";
        vector<Permutation> here;
        iter(p,it->second) here.push_back(*p);
        sort(here.begin(), here.end(), cmp);
        iter(p,here) {
            outs << ":";
            rep(i,0,size(*p)) {
                if (i != 0) {
                    outs << ",";
                }
                outs << static_cast<int>((*p)[i]);
            }
        }
        outs << endl;
    }

    rep(i,0,n) delete rows[i];
    rep(i,0,m) delete cols[i];

    return 0;
}

