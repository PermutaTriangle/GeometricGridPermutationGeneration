import sys
sys.setrecursionlimit(99999999)

# All grids
n = 2
m = 2
k = 4
grid = [ [ None for j in range(m) ] for i in range(n) ]

output = '%dx%d_%d.txt' % (n,m,k)

# 2x3 up to length 8
# 3x3 up to length 8

# A specific grid
# grid = [[1,-1,1],[1,-1,-1],[1,-1,-1]]
# n = len(grid)
# m = len(grid[0])
# k = 10


# EPS = 1e-9
EPS = 0.001 # k*EPS < 1

class Node:
    def __init__(self, id, parent=None):
        self.id = id
        self.left = None
        self.right = None
        self.parent = parent

class BinarySearchTree:

    def __init__(self):
        self.root = None
        self.node = {}

    def attach_right(self, id, node):
        while node.right is not None:
            node = node.right
        node.right = self.node[id] = Node(id, node)

    def attach_left(self, id, node):
        while node.left is not None:
            node = node.left
        node.left = self.node[id] = Node(id, node)

    def insert_before(self, id, right_id):
        assert right_id is None or right_id in self.node
        if right_id is None:
            if self.root is None:
                self.root = self.node[id] = Node(id)
            else:
                self.attach_right(id, self.root)
        else:
            right_node = self.node[right_id]
            if right_node.left is None:
                right_node.left = self.node[id] = Node(id, right_node)
            else:
                self.attach_right(id, right_node.left)

        # trav = self.inorder_traversal()
        # if right_id is None:
        #     assert trav[-1] == id
        # else:
        #     assert trav.index(id) + 1 == trav.index(right_id)

    def insert_after(self, id, left_id):
        assert left_id is None or left_id in self.node
        if left_id is None:
            if self.root is None:
                self.root = self.node[id] = Node(id)
            else:
                self.attach_left(id, self.root)
        else:
            left_node = self.node[left_id]
            if left_node.right is None:
                left_node.right = self.node[id] = Node(id, left_node)
            else:
                self.attach_left(id, left_node.right)

        # trav = self.inorder_traversal()
        # if left_id is None:
        #     assert trav[0] == id
        # else:
        #     assert trav.index(id) - 1 == trav.index(left_id)

    def remove(self, id):
        node = self.node[id]
        del self.node[id]
        assert node.left is None
        assert node.right is None
        if node.parent is None:
            self.root = None
        elif node.parent.left == node:
            node.parent.left = None
        elif node.parent.right == node:
            node.parent.right = None
        else:
            assert False

    def inorder_traversal(self):
        res = []
        def dfs(v):
            if v is None: return
            dfs(v.left)
            res.append(v.id)
            dfs(v.right)
        dfs(self.root)
        return res

def construct_permutation(rows, cols):
    n = len(rows)
    m = len(cols)
    l = sum( len(c) for c in cols )
    idx = [-1]*l
    perm = [-1]*l
    at = 0
    for i in range(m):
        for j in range(len(cols[i])):
            idx[cols[i][j]] = at
            at += 1

    at = 1
    for i in range(n-1,-1,-1):
        for j in range(len(rows[i])-1,-1,-1):
            perm[idx[rows[i][j]]] = at
            at += 1

    return tuple(perm)

def is_valid(rows, cols):
    # return True
    p = MixedIntegerLinearProgram()
    n = len(rows)
    m = len(cols)
    l = sum( len(c) for c in cols )
    if l == 0:
        return True

    colorder = []
    for i in range(m):
        for j in range(len(cols[i])):
            colorder.append(cols[i][j])

    rowidx = [-1]*l
    colidx = [-1]*l
    idx = [-1]*l
    at = 0
    for i in range(m):
        for j in range(len(cols[i])):
            idx[cols[i][j]] = at
            at += 1
    for i in range(m):
        for j in range(len(cols[i])):
            colidx[cols[i][j]] = i
    for i in range(n):
        for j in range(len(rows[i])):
            rowidx[rows[i][j]] = i

    var = p.new_variable(real=True,nonnegative=True)
    p.set_objective(var[0])

    for i in range(l):
        y = n-rowidx[colorder[i]]-1
        x = colidx[colorder[i]]
        p.add_constraint(x+EPS <= var[colorder[i]] <= x+1-EPS)

    for i in range(l-1):
        p.add_constraint(var[colorder[i]]+EPS <= var[colorder[i+1]])

    order = []
    for i in range(n):
        for j in range(len(rows[i])):
            order.append(rows[i][j])

    for i in range(l-1):
        a = order[i]
        b = order[i+1]

        aj = n-rowidx[a]-1
        ai = colidx[a]

        bj = n-rowidx[b]-1
        bi = colidx[b]

        f = lambda x: (aj - (x - ai) + 1) if grid[rowidx[a]][colidx[a]] == -1 else (aj + x - ai)
        g = lambda x: (bj - (x - bi) + 1) if grid[rowidx[b]][colidx[b]] == -1 else (bj + x - bi)

        p.add_constraint(f(var[a]) - EPS >= g(var[b]))

    try:
        # TODO: don't solve linear program, just check feasibility
        p.solve()
        return True
    except:
        return False

rows = [ BinarySearchTree() for i in range(n) ]
cols = [ BinarySearchTree() for i in range(m) ]
res = {}

def bt(k,x,y, id):
    if y == m:
        bt(k, x + 1, 0, id)
        return
    elif x == n:
        # print(grid)
        # for r in range(n):
        #     print('row', r, rows[r].inorder_traversal())
        # for c in range(m):
        #     print('col', c, cols[c].inorder_traversal())
        # gridt = tuple([ tuple(row) for row in grid ])
        # print('xxx')

        roworders = [ row.inorder_traversal() for row in rows ]
        colorders = [ col.inorder_traversal() for col in cols ]
        gridt = tuple([ tuple(row) for row in grid ])
        perm = construct_permutation(roworders, colorders)
        res.setdefault(gridt,set())
        # if not perm:
        #     print('empty', perm not in res[gridt], is_valid(roworders, colorders))
        #     print(roworders)
        #     print(colorders)
        if perm not in res[gridt] and is_valid(roworders, colorders):
            res[gridt].add(perm)
        return

    hlines = rows[x].inorder_traversal()
    vlines = cols[y].inorder_traversal()

    horizontal = len(hlines)
    vertical = len(vlines)

    def bt2(k,i,j, id, must):

        if k > 0:
            # put one more here
            if grid[x][y] == -1:
                rows[x].insert_before(id, hlines[i] if i < len(hlines) else None)
            else:
                rows[x].insert_after(id, hlines[i-1] if 0 <= i-1 else None)
            cols[y].insert_before(id, vlines[j] if j < len(vlines) else None)
            bt2(k - 1, i, j, id + 1, False)
            rows[x].remove(id)
            cols[y].remove(id)

        if not must:
            # don't put anything more down
            bt(k, x, y + 1, id)

            # jump somewhere else
            if k > 0:
                di = -grid[x][y]
                dj = 1

                for up in range( i+1 if grid[x][y] == 1 else horizontal+1 - i ):
                    for right in range(vertical+1 - j):

                        # don't jump here again...
                        if up == 0 and right == 0:
                            continue

                        bt2(k, i + up * di, j + right * dj, id, True)

    lo = -1
    hi = 1
    if grid[x][y] is not None:
        lo = hi = grid[x][y]

    for t in range(lo,hi+1):
        grid[x][y] = t

        # if x == 0 and y <= 1:
        #     print(x,y,t,k)

        if t == 1:
            # increasing at (x,y)
            bt2(k, horizontal, 0, id, False)
        elif t == -1:
            # decreasing at (x,y)
            bt2(k, 0, 0, id, False)
        else:
            # empty at (x,y)
            bt(k, x, y + 1, id)

    if lo < hi:
        grid[x][y] = None

def main():
    global k
    bt(k, 0, 0, 0)

    with open(output, 'w') as f:
    # with sys.stdout as f:
        for gridt in sorted(res.keys()):
            f.write('[%s]' % ','.join( '[%s]' % ','.join(map(str,row)) for row in gridt ))
            # print('GRID', gridt)
            cnt = {}
            for perm in sorted(res[gridt], key=lambda p: (len(p), p)):
                f.write(':')
                f.write(','.join( str(x) for x in perm ))

            f.write('\n')
                # cnt.setdefault(len(perm), 0)
                # cnt[len(perm)] += 1
            # for k,v in sorted(cnt.items()):
            #     print(k,v)
            # for perm, cnt in sorted(extra[gridt].items(), key=lambda p: (len(p[0]), p[0])):
            #     if perm in res[gridt]: continue
            #     print(perm, cnt)
            #     # if len(perm) == 5:
            #         # print('Extra', perm)
            #

main()
# import cProfile
# cProfile.run('main()')
