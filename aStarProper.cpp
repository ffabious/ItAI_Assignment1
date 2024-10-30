#include <bits/stdc++.h>

using namespace std;

struct coord_t {
    int x;
    int y;

    bool operator ==(coord_t &b) {
        return x == b.x && y == b.y;
    }
};

struct node_a {
    coord_t pos;
    int fVal;
    int gVal;
    int hVal;
    bool danger = false;
    node_a *parent = nullptr;

    bool operator ==(node_a *b) {
        return pos == b->pos;
    }
};

bool sorter(node_a *a, node_a *b) {
    if (a->fVal == b->fVal) return a->hVal < b->hVal;
    return a->fVal < b->fVal;
}

class NeoAStar {
private:
    int perceptionVariant;
    node_a *map[9][9] = {nullptr};
    coord_t neoPos = {0, 0};
    coord_t kmPos;
    coord_t bdkPos = {-1, -1};
    node_a *sentinel = nullptr;
    vector <node_a*> agents;
    bool keyFound = false;
    bool gameInProgress = true;
    vector <node_a*> moves;
    int n = -1;

    void fillMapValues() {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                int f, g, h;
                h = abs(i - this->kmPos.y) + abs(j - this->kmPos.x);
                map[i][j] = new node_a{{j, i}, -1, -1, h};
            }
        }
        map[0][0]->gVal = 0;
        map[0][0]->fVal = map[0][0]->gVal + map[0][0]->hVal;
    }

    void fixx(node_a *node, node_a *temp) {
        temp->gVal = node->gVal + 1;
        temp->fVal = temp->gVal + temp->hVal;
        moves.push_back(temp);
        temp->parent = node;
    }

    void fixValuesAroundNode(node_a *node) {

        int x = node->pos.x, y = node->pos.y;

        if (x > 0 && (map[y][x - 1]->gVal > node->gVal + 1 || map[y][x - 1]->gVal < 0) && !map[y][x - 1]->danger) {
            fixx(node, map[y][x - 1]);
        }
        if (y > 0 && (map[y - 1][x]->gVal > node->gVal + 1 || map[y - 1][x]->gVal < 0) && !map[y - 1][x]->danger) {
            fixx(node, map[y - 1][x]);
        }
        if (x < 8 && (map[y][x + 1]->gVal > node->gVal + 1 || map[y][x + 1]->gVal < 0) && !map[y][x + 1]->danger) {
            fixx(node, map[y][x + 1]);
        }
        if (y < 8 && (map[y + 1][x]->gVal > node->gVal + 1 || map[y + 1][x]->gVal < 0) && !map[y + 1][x]->danger) {
            fixx(node, map[y + 1][x]);
        }
    }

    void recreatePath() {
        node_a *currentNode = map[neoPos.y][neoPos.x];
        n = 0;
        while (currentNode != map[0][0]) {
            // printf("(%d;%d)--", currentNode->pos.x, currentNode->pos.y);
            currentNode = currentNode->parent;
            n++;
        }
        // printf("(0;0)\n");
    }

    void handleAgent(node_a *node) {
        if (!keyFound) {
            for (int i = max(node->pos.y - 1, 0); i < min(node->pos.y + 1, 8); i++)
                for (int j = max(node->pos.x - 1, 0); j < min(node->pos.x + 1, 8); j++) {
                    map[i][j]->danger = true;
                }
            return;
        }
        for (int i = max(node->pos.y - 1, 0); i < min(node->pos.y + 1, 8); i++)
            for (int j = max(node->pos.x - 1, 0); j < min(node->pos.x + 1, 8); j++) {
                map[i][j]->danger = false;
            }
    }

    void handleSentinel(node_a *node) {
        if (!keyFound) {
            if (node->pos.x > 0) {
                map[node->pos.y][node->pos.x - 1]->danger = true;
            }
            if (node->pos.x < 8) {
                map[node->pos.y][node->pos.x + 1]->danger = true;
            }
            if (node->pos.y > 0) {
                map[node->pos.y - 1][node->pos.x]->danger = true;
            }
            if (node->pos.y < 8) {
                map[node->pos.y + 1][node->pos.x]->danger = true;
            }
            return;
        }
        for (int i = max(node->pos.y - 2, 0); i < min(node->pos.y + 2, 8); i++) {
            map[i][node->pos.x]->danger = true;
        }
        for (int j = max(node->pos.x - 2, 0); j < min(node->pos.x + 2, 8); j++) {
            map[node->pos.y][j]->danger = true;
        }
        for (int i = max(node->pos.y - 1, 0); i < min(node->pos.y + 1, 8); i++)
                for (int j = max(node->pos.x - 1, 0); j < min(node->pos.x + 1, 8); j++) {
                    map[i][j]->danger = true;
                }
    }

    void dataAwait() {
        int n_inputs;
        cin >> n_inputs;
        for (int i = 0; i < n_inputs; i++) {
            int xi, yi;
            char ii;
            cin >> xi >> yi >> ii;
            if (ii == 'P') {
                map[yi][xi]->danger = true;
            }
            else if (ii == 'A') {
                map[yi][xi]->danger = true;
                agents.push_back(map[yi][xi]);
                handleAgent(map[yi][xi]);
            }
            else if (ii == 'S') {
                map[yi][xi]->danger = true;
                handleSentinel(map[yi][xi]);
            }
            else if (ii == 'B') {
                bdkPos = {xi, yi};
            }
            else continue;
        }
    }

    bool isClose(node_a *start, node_a *end) {
        return abs(start->pos.x - end->pos.x) + abs(start->pos.y - end->pos.y) == 1;
    }

    node_a *findIntersection(node_a *a, node_a *b) {
        
    }

    void aToB(node_a *a, node_a *b) {
        vector <node_a*> aToIntersection;
        vector <node_a*> intersectionToB;
        node_a *intersection = findIntersection(a, b);

        node_a *curr = a;
        while (curr != intersection) {
            aToIntersection.push_back(curr);
            curr = curr->parent;
        }
        aToIntersection.push_back(curr);
        curr = b;
        while (curr != intersection) {
            intersectionToB.push_back(curr);
            curr = curr->parent;
        }
        reverse(intersectionToB.begin(), intersectionToB.end());

    }

    void iteration() {
        if (moves.empty() && keyFound) {
            gameInProgress = false;
            return;
        }
        sort(moves.begin(), moves.end(), sorter);
        node_a *chosen = moves[0];
        neoPos = chosen->pos;
        if (neoPos == kmPos) {
            gameInProgress = false;
            recreatePath();
            return;
        }
        if (isClose(map[neoPos.y][neoPos.y], chosen)) {
            printf("m %d %d\n", chosen->pos.x, chosen->pos.y);
            dataAwait();
            fixValuesAroundNode(chosen);
            if (bdkPos == chosen->pos) {
                keyFound = true;
                for (auto &agent: agents) {
                    handleAgent(agent);
                }
                if (sentinel != nullptr) handleSentinel(sentinel);
            }
            moves.erase(moves.begin());
        }
        else {

        }
    }

public:
    NeoAStar(int perceptionVariant, int kmX, int kmY) {
        this->perceptionVariant = perceptionVariant;
        this->kmPos = {kmX, kmY};

        this->fillMapValues();
        this->moves.push_back(map[0][0]);
    }

    coord_t getKMPos() {
        return this->kmPos;
    }

    coord_t getNeoPos() {
        return this->neoPos;
    }

    void printMapValues() {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (!map[i][j]->danger) {
                    printf("(%d;%d;%d)   \t", map[i][j]->fVal, map[i][j]->gVal, map[i][j]->hVal);
                }
                else {
                    cout << "(DANGER)   \t";
                }
            }
            cout << '\n';
        }
    }

    void play() {
        while (gameInProgress) {
            iteration();
        }
        printf("e %d\n", n);
    }
};

int main() {
    int perceptVar, kmX, kmY;
    cin >> perceptVar >> kmX >> kmY;

    NeoAStar neo(perceptVar, kmX, kmY);
    // neo.printMapValues();
    neo.play();
}