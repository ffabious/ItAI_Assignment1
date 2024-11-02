// Kirill Greshnov B23-DSAI-03
#include <bits/stdc++.h>

using namespace std;

// coord_t struct holds the coordinates of a cell
struct coord_t {
    int x;
    int y;

    bool operator==(coord_t &b) {
        return x == b.x && y == b.y;
    }
};

/*
    node_b struct represents a node/cell object

    Variables:
    pos - coordinates of a node
    danger - 'true' if this node is dangerous to the Actor
        (Enemy or Perseption Zone of the Enemy)
    parent - pointer to the node the algorithm moved from
        to this node
    explored - 'true' if this node has been "explored" by the BFS
        part of the algorithm
    order - index of the node in a path from (0;0) to itself;
        (0;0) gets order=0, which is incremented for next nodes
        in the path
    adjacent - vector of pointers to nodes, which are adjacent
        to this node
*/
struct node_b {
    coord_t pos;
    bool danger = false;
    node_b *parent = nullptr;
    bool explored = false;
    int order = -1;

    vector <node_b*> adjacent;
    
    bool operator==(node_b &b) {
        return pos == b.pos;
    }
};

/*
    NeoBacktracking class constains the implementation of Backtracking algorithm
        for this particular assignment

    Explanation: consider that x and y are the coordinates of the
        Keymaker, then, notice that, the shortest path to the Keymaker
        cannot be shorter than (x + y). Therefore, initially, we can run a
        modification of DFS algorithm, based of the rules of this assigment,
        to find all paths of length (x + y). If the path of length (x + y) is
        found, then it is the shortest and we can end the execution and output
        the answer. If we haven't reaches Keymaker by following the above-
        mentioned paths, then we can run a modified BFS algorithm from the ends
        of those paths (nodes with order=(x + y)), to find the shortest path, if
        is exists.

    Variables:
    perceptionVariant - perception variant of the actor for this test
    map[][] - matrix of node objects (Environment of the game)
    neoPos - current position of the Actor
    kmPos - position of Keymaker (end point)
    pathLength - length of the possible shortest path; this
        value changes during the execution of the algorithm
    bfsQ - queue of node pointers for the BFS part of this
        algorithm

    Functions:
    (Comments are given above function)
*/
class NeoBacktracking {
private:
    int perceptionVariant;
    node_b* map[9][9] = {nullptr};
    coord_t neoPos = {0, 0};
    coord_t kmPos;
    int pathLength;
    queue <node_b*> bfsQ;

    /*
        The 'initAdj' adds the given node 'node' to its neighbours'
        'adjacent' list.
    */
    void initAdj(node_b *node) {
        if (node->pos.x > 0) {
            node->adjacent.push_back(map[node->pos.y][node->pos.x - 1]);
        }
        if (node->pos.x < 8) {
            node->adjacent.push_back(map[node->pos.y][node->pos.x + 1]);
        }
        if (node->pos.y > 0) {
            node->adjacent.push_back(map[node->pos.y - 1][node->pos.x]);
        }
        if (node->pos.y < 8) {
            node->adjacent.push_back(map[node->pos.y + 1][node->pos.x]);
        }
    }

    /*
        The 'initNodes' function creates node objects and adds them
        to the 'map'. It also sets order of (0;0) node to 0.
    */
    void initNodes() {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                map[i][j] = new node_b{{j, i}};
            }
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                initAdj(map[i][j]);
            }
        }
        map[0][0]->order = 0;
    }

    /*
        The 'endangerNode' is a helper function which sets 'danger' value
        of the node to 'true' and removes this node from the 'adjacent'
        lists of its neighbours.
    */
    void endangerNode(node_b *node) {
        if (node->danger) return;
        node->danger = true;
        node->adjacent.clear();
        if (node->pos.x > 0) {
            node_b *neighbour = map[node->pos.y][node->pos.x - 1];
            auto iter = find(neighbour->adjacent.begin(), neighbour->adjacent.end(), node);
            if (iter != neighbour->adjacent.end()) {
                neighbour->adjacent.erase(iter);
            }
        }
        if (node->pos.x < 8) {
            node_b *neighbour = map[node->pos.y][node->pos.x + 1];
            auto iter = find(neighbour->adjacent.begin(), neighbour->adjacent.end(), node);
            if (iter != neighbour->adjacent.end()) {
                neighbour->adjacent.erase(iter);
            }
        }
        if (node->pos.y > 0) {
            node_b *neighbour = map[node->pos.y - 1][node->pos.x];
            auto iter = find(neighbour->adjacent.begin(), neighbour->adjacent.end(), node);
            if (iter != neighbour->adjacent.end()) {
                neighbour->adjacent.erase(iter);
            }
        }
        if (node->pos.y < 8) {
            node_b *neighbour = map[node->pos.y + 1][node->pos.x];
            auto iter = find(neighbour->adjacent.begin(), neighbour->adjacent.end(), node);
            if (iter != neighbour->adjacent.end()) {
                neighbour->adjacent.erase(iter);
            }
        }
    }

    /*
        'handleAgent' function calls 'endangerNode' for the 'agentNode', as
        well as for the nodes in this Agent's perception range
    */
    void handleAgent(node_b *agentNode) {
        endangerNode(agentNode);
        for (int i = max(agentNode->pos.y - 1, 0); i < min(agentNode->pos.y + 1, 9); i++)
            for (int j = max(agentNode->pos.x - 1, 0); j < min(agentNode->pos.x + 1, 9); j++) {
                endangerNode(map[i][j]);
            }
    }

    /*
        The 'handleSentinel' function works similarly to 'handleAgent
    */
    void handleSentinel(node_b *sentinelNode) {
        endangerNode(sentinelNode);
        if (sentinelNode->pos.x > 0) endangerNode(map[sentinelNode->pos.y][sentinelNode->pos.x - 1]);
        if (sentinelNode->pos.x < 8) endangerNode(map[sentinelNode->pos.y][sentinelNode->pos.x + 1]);
        if (sentinelNode->pos.y > 0) endangerNode(map[sentinelNode->pos.y - 1][sentinelNode->pos.x]);
        if (sentinelNode->pos.y < 8) endangerNode(map[sentinelNode->pos.y + 1][sentinelNode->pos.x]);
    }

    /*
        'dataAwait' function get information about nodes in Actor's perception
        range after every move, it calls handlers for 'A'gents and 'S'entinel &
        changes 'danger' values of nodes if they are said to be 'P'erception Zone
        of the Enemy
    */
    void dataAwait() {
        int k;
        cin >> k;
        for (int i = 0; i < k; i++) {
            int xi, yi;
            char ii;
            cin >> xi >> yi >> ii;

            if (ii == 'P') {
                endangerNode(map[yi][xi]);
            }
            else if (ii == 'A') {
                handleAgent(map[yi][xi]);
            }
            else if (ii == 'S') {
                handleSentinel(map[yi][xi]);
            }
            else continue;
        }
    }

    /*
        'isClose' function simply returns 'true' if the two given
        nodes 'start' & 'end' are adjacent (or equal) to each other
    */
    bool isClose(node_b *a, node_b *b) {
        return abs(a->pos.x - b->pos.x) + abs(a->pos.y - b->pos.y) <= 1;
    }

    /*
        The 'performMovementDfs' function handle movement of the Actor
        during the DFS part of the algorithm. If the 'next' node is adjacent
        to the current position of the Actor, then it will simply move to 'next'.
        Otherwise, it will recreate a path from current node to 'next' using 'parent'
        pointers to find these nodes' common node-ancestor and move through it,
        while calling 'dataAwait' after every move.
    */
    void performMovementDfs(node_b *next) {
        if (!isClose(next, map[neoPos.y][neoPos.x])) {
            node_b *currNode = map[neoPos.y][neoPos.x];
            while (currNode != next->parent && currNode->parent != nullptr) {
                currNode = currNode->parent;
                neoPos = currNode->pos;
                printf("m %d %d\n", currNode->pos.x, currNode->pos.y);
                dataAwait();
            }
        }
        printf("m %d %d\n", next->pos.x, next->pos.y);
        dataAwait();
        neoPos = next->pos;
    }

    /*
        The 'performMoveBfs' works very similarly to 'performMoveDfs'
    */
    void performMoveBfs(node_b *next) {
        if (!isClose(next, map[neoPos.y][neoPos.x])) {
            node_b *currNode = map[neoPos.y][neoPos.x];
            while (currNode != next->parent && currNode->parent != nullptr) {
                currNode = currNode->parent;
                neoPos = currNode->pos;
                printf("m %d %d\n", currNode->pos.x, currNode->pos.y);
                dataAwait();
            }
        }
        node_b *extraNode = next;
        vector <node_b*> extraArray;
        while (extraNode != map[neoPos.y][neoPos.x]) {
            extraNode = extraNode->parent;
            extraArray.push_back(extraNode);
        }
        extraArray.erase(extraArray.begin() + extraArray.size() - 1);
        reverse(extraArray.begin(), extraArray.end());
        for (node_b *el : extraArray) {
            neoPos = el->pos;
            printf("m %d %d\n", el->pos.x, el->pos.y);
            dataAwait();
        }
        printf("m %d %d\n", next->pos.x, next->pos.y);
        dataAwait();
        neoPos = next->pos;
    }

    /*
        DFS part of the algorithm implemented using a stack 's'.
        The function adds nodes with order x + y (coordinates of
        the Keymaker) to 'bfsQ'
    */
    void dfs(node_b *node) {
        stack<node_b*> s;
        s.push(node);
        node->explored = true;
        while (!s.empty()) {
            node_b *next = s.top();
            s.pop();
            
            performMovementDfs(next);
            next->explored = true;

            if (next == map[kmPos.y][kmPos.x]) {
                printf("e %d", pathLength);
                exit(0);
            }
            if (next->order < pathLength) {
                for (node_b *el : next->adjacent) {
                    if (el->order > next->order + 1 || el->order == -1) {
                        el->parent = next;
                        el->order = next->order + 1;
                        s.push(el);
                    }
                }
            }
            else bfsQ.push(next);
        }
    }


    /*
        BFS part of the algorithm, slightly modified to
        start not from one specific node, but already having
        some paths done by DFS & a filled queue.
        This function also returns "e -1" if the path doesn't
        exist.
    */
    void bfs() {
        while (!bfsQ.empty()) {
            node_b *next = bfsQ.front();
            bfsQ.pop();

            performMoveBfs(next);

            if (next == map[kmPos.y][kmPos.x]) {
                printf("e %d", next->order);
                exit(EXIT_SUCCESS);
            }

            for (node_b *el : next->adjacent) {
                if (!el->explored) {
                    el->explored = true;
                    el->parent = next;
                    el->order = next->order + 1;
                    bfsQ.push(el);
                }
            }
        }
        printf("e -1");
        exit(0);
    }

public:

    /*
        Public constructor gets perception variant for this test
        & coordinates of the Keymaker.
        Then it calls 'initNodes' to create node objects and
        add them to the 'map'.
    */
    NeoBacktracking(int perceptionVariant, int kmX, int kmY) : perceptionVariant(perceptionVariant), kmPos({kmX, kmY}) {
        pathLength = kmX + kmY;
        initNodes();
    }

    /*
        Public function 'play' calls DFS & BFS parts of the
        algorithm in order.
    */
    void play() {
        dfs(map[0][0]);
        bfs();
    }
};

/*
    Input of perception variant and coordinates of the Keymaker
    &
    creation of instance 'neo' of 'NeoAStar' class
    &
    call of 'play' from 'neo'.
*/
int main() {
    int perceptVar, kmX, kmY;
    cin >> perceptVar >> kmX >> kmY;

    NeoBacktracking neo(perceptVar, kmX, kmY);

    neo.play();
}