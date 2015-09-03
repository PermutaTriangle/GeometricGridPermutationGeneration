import sys
sys.setrecursionlimit(99999999)

n = 1
m = 2
grid = [ [ 0 for j in range(m) ] for i in range(n) ]
cnt = [ [ 0 for j in range(m) ] for i in range(n) ]
res = { }

def bt(k,x,y):
    # print('A', k,x,y)

    if y == m:
        bt(k, x + 1, 0)
        return
    elif x == n:
        # print(grid, cnt)
        gridt = tuple([ tuple(row) for row in grid ])
        res.setdefault(gridt, [])
        res[gridt].append([ [ c for c in r ] for r in cnt ])
        return

    vertical = 0
    horizontal = 0

    def bt2(k,i,j):
        # print('B', k,i,j)

        # end = (-1 if grid[x][y] == 1 else horizontal+1)
        # if j == vertical + 1:
        #     bt2(k, i-grid[x][y], 0)
        #     return
        # elif end == i:
        #     bt(k, x, y + 1)
        #     return
        #
        # # put something
        # if k > 0:
        #     cnt[x][y] += 1
        #     bt2(k-1, i, j)
        #     cnt[x][y] -= 1
        #
        # if j+1 < horizontal:
        #     # put nothing, go right
        #     bt2(k, i, j+1)
        #
        # if i-grid[x][y] != end:
        #     # put nothing, go direction
        #     bt2(k, i-grid[x][y], j)

        # don't put anything more down
        bt(k, x, y + 1)

        # put one more
        if k > 0:
            di = -grid[x][y]
            dj = 1

            for up in range( i+1 if grid[x][y] == 1 else vertical+1 - i ):
                for right in range(horizontal+1 - j):
                    cnt[x][y] += 1
                    bt2(k - 1, i + up * di, j + right * dj)
                    cnt[x][y] -= 1


    # increasing at (x,y)
    grid[x][y] = 1
    bt2(k, horizontal, 0)

    # decreasing at (x,y)
    grid[x][y] = -1
    bt2(k, 0, 0)

    # empty at (x,y)
    grid[x][y] = 0
    bt(k, x, y + 1)

bt(3, 0, 0)

for gridt in res:
    print('GRID', gridt)
    for cnts in res[gridt]:
        print(cnts)

