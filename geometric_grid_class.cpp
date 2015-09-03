#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;
template <class T> int size(const T &x) { return x.size(); }
const int INF = 2147483647;
#define rep(i,a,b) for (__typeof(a) i=(a); i<(b); ++i)
#define iter(it,c) for (__typeof((c).begin()) it = (c).begin(); it != (c).end(); ++it)

const int N = 2;
const int M = 3;
const int K = 6;

int grid[N][M];

struct Node {
    int id;
    Node *parent, *left, *right;
    Node(int id, Node *parent=NULL) {
        this->id = id;
        this->left = this->right = NULL;
        this->parent = parent;
    }
};

struct BinarySearchTree {
    Node *root;
    unordered_map<int, Node*> node;
    BinarySearchTree() {
        this->root = NULL;
    }

    void attach_right(int id, Node *node) {
        while (node->right != NULL) {
            node = node->right;
        }
        node->right = this->node[id] = new Node(id, node);
    }

    void insert_before(int id, int right_id) {
        assert(right_id == -1 || node.find(right_id) != node.end());
        if (right_id == -1) {
            if (root == NULL) {
                root = node[id] = new Node(id);
            } else {
                attach_right(id, root);
            }
        } else {
            Node *right_node = node[right_id];
            if (right_node->left == NULL) {
                right_node->left = node[id] = new Node(id, right_node);
            } else {
                attach_right(id, right_node->left);
            }
        }
    }

    void remove(int id) {
        Node *node = this->node[]
    }
};

int main() {
    return 0;
}

