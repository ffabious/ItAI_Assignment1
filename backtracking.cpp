#include <bits/stdc++.h>

using namespace std;

struct coord_t {
    int x;
    int y;

    bool operator==(coord_t &b) {
        return x == b.x && y == b.y;
    }
};

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

class NeoBacktracking {
private:
    int perceptionVariant;
    coord_t neoPos = {0, 0};
    coord_t kmPos;
    node_b* map[9][9] = {nullptr};
    int pathLength;
    queue <node_b*> bfsQ;

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

    void handleAgent(node_b *agentNode) {
        endangerNode(agentNode);
        for (int i = max(agentNode->pos.y - 1, 0); i < min(agentNode->pos.y + 1, 9); i++)
            for (int j = max(agentNode->pos.x - 1, 0); j < min(agentNode->pos.x + 1, 9); j++) {
                endangerNode(map[i][j]);
            }
    }

    void handleSentinel(node_b *sentinelNode) {
        endangerNode(sentinelNode);
        if (sentinelNode->pos.x > 0) endangerNode(map[sentinelNode->pos.y][sentinelNode->pos.x - 1]);
        if (sentinelNode->pos.x < 8) endangerNode(map[sentinelNode->pos.y][sentinelNode->pos.x + 1]);
        if (sentinelNode->pos.y > 0) endangerNode(map[sentinelNode->pos.y - 1][sentinelNode->pos.x]);
        if (sentinelNode->pos.y < 8) endangerNode(map[sentinelNode->pos.y + 1][sentinelNode->pos.x]);
    }

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

    bool isClose(node_b *a, node_b *b) {
        return abs(a->pos.x - b->pos.x) + abs(a->pos.y - b->pos.y) <= 1;
    }

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

                // while (next != map[0][0]) {
                //     printf("%d %d -- ", next->pos.x, next->pos.y);
                //     next = next->parent;
                // }

                exit(0);
            }

            // if (next->order > node->order + 1) {
            //     next->parent = node;
            //     next->order = node->order + 1;
            // }

            // if (!next->explored) {
            //     next->explored = true;
            //     if (next->order < pathLength) {
            //         for (node_b *el : next->adjacent) {
            //             if (!el->explored) {
            //                 el->parent = next;
            //                 el->order = next->order + 1;
            //                 s.push(el);
            //             }
            //         }
            //     }
            // }
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
    
    void fullSearch() {
        dfs(map[0][0]);
        bfs();
    }

public:
    NeoBacktracking(int perceptionVariant, int kmX, int kmY) : perceptionVariant(perceptionVariant), kmPos({kmX, kmY}) {
        pathLength = kmX + kmY;
        initNodes();
    }

    void play() {
        fullSearch();
    }
};

int main() {
    int perceptVar, kmX, kmY;
    cin >> perceptVar >> kmX >> kmY;

    NeoBacktracking neo(perceptVar, kmX, kmY);

    neo.play();
}