// Kirill Greshnov B23-DSAI-03
#include <bits/stdc++.h>

using namespace std;

// coord_t struct holds the coordinates of a cell
struct coord_t {
    int x;
    int y;

    bool operator ==(coord_t &b) {
        return x == b.x && y == b.y;
    }
};

/*
    node_a struct represents a node/cell object

    Variables:
    pos - coordinates of a node
    fVal, gVal, hVal - values of f(n), g(n), h(n) functions
        for this node
    danger - 'true' if this node is dangerous to the Actor
        (Enemy or Perseption Zone of the Enemy)
    parent - pointer to the node the algorithm moved from
        to this node
*/
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

/*
    'sorter' function is responsible for correctly sorting the vector of nodes (type node_a) based on the values
    of functions fVal & hVal: we prioritize the smaller total path-length fVal and, in case they are equal for 2 nodes,
    we choose one with the smallest hVal, meaning that the node is closer to the end point
*/
bool sorter(node_a *a, node_a *b) {
    if (a->fVal == b->fVal) return a->hVal < b->hVal;
    return a->fVal < b->fVal;
}

/*
    NeoAStar class constains the implementation of A* algorithm for this particular assignment

    Variables:
    perceptionVariant - perception variant of the actor for this test
    map[][] - matrix of node objects (Environment of the game)
    neoPos - current position of the Actor
    kmPos - position of Keymaker (end point)
    bdkPos - position of the Backdoor Key, if found
    sentinel - pointer to the node containing the Sentinel, if found
    agents - vector of node pointers to known Agents
    keyFound - 'true' if the Backdoor Key was found
    gameInProgress - 'true' if the game is not over
    moves - vector of node objects representing the list of nodes for the
        algorithm to check next
    n - length of a shortest path to Keymaker, or -1 if no valid path

    Functions:
    (Comments are given above function)
*/
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

    /*
        'fillMapValues' fills initial values for the node objects in
        the map
    */
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

    /*
        Given 2 nodes 'node' & 'temp', 'fixx' function is used in the following
        'fixValuesAroundNode' function to change f, g, h values for node temp
        when making a move node->temp. Then 'fixx' adds 'temp' node to 'moves'
        and sets 'temp's' parent to be 'node'
    */
    void fixx(node_a *node, node_a *temp) {
        temp->gVal = node->gVal + 1;
        temp->fVal = temp->gVal + temp->hVal;
        moves.push_back(temp);
        temp->parent = node;
    }

    /*
        Given node 'node', 'fixValuesAroundNode' function uses 'fixx' to update the
        4 nodes adjacent to 'node'.
        It check whether the adjacent node exists (that it's not outside of the map),
        whether it can improve g value of the adjacent node.
        Adjacent node is not considered if it is "dangerous".
    */
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

    /*
        'recreatePath' function is used to recreate the final shortest path from (0;0)
        to Keymaker and counts the number of steps 'n' it took
    */
    void recreatePath() {
        node_a *currentNode = map[neoPos.y][neoPos.x];
        n = 0;
        while (currentNode != map[0][0]) {
            currentNode = currentNode->parent;
            n++;
        }
    }

    /*
        'handleAgent' function changes 'danger' variable of nodes in perception range of
        the Agent and the node containing Agent itself.
        If the Backdoor Key is found, then all of the above mentioned nodes are no longer dangerous
        and their values are changes accordingly.
        If the Key is not found yet, 'handleAgent' sets 'danger' values of the above mentioned nodes
        to 'true'
    */
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

    /*
        Similarly to 'handleAgent', 'handleSentinel' changes the 'danger' values of the node
        containing the Sentinel itself, as well as nodes in its perception range.
        If the Key is found, then, given from the assignment statement, the perception range
        of the Sentinel is increased.
        Otherwise, the initial 'danger' values for nodes in Sentinel's perception range are
        set to 'true'.
    */
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

    /*
        'dataAwait' function get information about nodes in Actor's perception
        range after every move, it calls handlers for Agents and Sentinel &
        changes 'danger' values of nodes if they are said to be 'P'erception Zone
        of the Enemy
    */
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

    /*
        'isClose' function simply returns 'true' if the two given
        nodes 'start' & 'end' are adjacent (or equal) to each other
    */
    bool isClose(node_a *start, node_a *end) {
        return (abs(start->pos.x - end->pos.x) + abs(start->pos.y - end->pos.y) == 1) || (start == end);
    }

    /*
        'findIntersection' is a helper function for the following
        'aToB' function & returns a node - intersection of paths
        from a & from b.

        Detailed explanation if comments to 'aToB' function.
    */
    node_a *findIntersection(node_a *a, node_a *b) {
        vector<node_a*> aToStart;
        vector<node_a*> bToStart;

        node_a *curr = a;
        while (curr != map[0][0]) {
            aToStart.push_back(curr);
            curr = curr->parent;
        }
        aToStart.push_back(map[0][0]);

        curr = b;
        while (curr != map[0][0]) {
            bToStart.push_back(curr);
            curr = curr->parent;
        }
        bToStart.push_back(map[0][0]);

        for (auto node : aToStart) {
            if (find(bToStart.begin(), bToStart.end(), node) != bToStart.end()) {
                return node;
            }
        }
        return map[0][0];
    }

    /*
        The biggest change from the theoretical A* algorithm is that
        in this assignment the Actor cannot simply 'teleport' around the map
        to check the next node from 'moves'.

        'aToB' function finds a path from node a to node b by considering
        paths from a to (0;0) & from b to (0;0) using 'parent' pointers.
        Afterwards, it finds the intersection of these paths and constructs an
        way to get from node a to node b, while also calling 'dataAwait' after
        every move.
    */
    void aToB(node_a *a, node_a *b) {
        vector <node_a*> aToIntersection;
        vector <node_a*> intersectionToB;
        node_a *intersection = findIntersection(a, b);

        node_a *curr = a;
        while (curr != intersection) {
            curr = curr->parent;
            aToIntersection.push_back(curr);
        }
        curr = b;
        while (curr != intersection) {
            curr = curr->parent;
            intersectionToB.push_back(curr);
        }
        reverse(intersectionToB.begin(), intersectionToB.end());

        aToIntersection.insert(aToIntersection.end(), intersectionToB.begin() + 1, intersectionToB.end());
        for (auto el : aToIntersection) {
            printf("m %d %d\n", el->pos.x, el->pos.y);
            dataAwait();
        }
    }

    /*
        'iteration' function is responsible for choosing a next
        node to check and end the program if the path is found or
        if there are no more moves (-1).
        The 'moves' list is sorted at the start of 'iteration'. The next 
        node to move to is 'chosen' which is the 0th element of 'moves'.
        If the next node is adjacent to the current position of the Actor,
        then the algorithm simply moves to the next node.
        Otherwise, it calls 'aToB'.
        After, the 'chosen' move is removed from the 'moves' list.
    */
    void iteration() {
        if (moves.empty()) {
            gameInProgress = false;
            return;
        }
        sort(moves.begin(), moves.end(), sorter);
        node_a *chosen = moves[0];
        if (neoPos == kmPos) {
            gameInProgress = false;
            recreatePath();
            return;
        }
        if (isClose(map[neoPos.y][neoPos.x], chosen)) {
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
        }
        else {
            aToB(map[neoPos.y][neoPos.x], chosen);
        }
        moves.erase(moves.begin());
        neoPos = chosen->pos;
    }

public:

    /*
        Public constructor gets perception variant for this test
        & coordinates of the Keymaker.
        Then it calls 'fillMapValues' and adds the (0;0) node
        to the 'moves' list as a starting point of the search.
    */
    NeoAStar(int perceptionVariant, int kmX, int kmY) {
        this->perceptionVariant = perceptionVariant;
        this->kmPos = {kmX, kmY};

        this->fillMapValues();
        this->moves.push_back(map[0][0]);
    }


    /*
        The following getters & 'printMapValues' function were used while writing the code
    */
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

    /*
        Public function 'play' calls 'iteration' while the
        game is in progress (indicated by 'gameInProgress' variable).
        Afterwards, it prints the final answer.
    */
    void play() {
        while (gameInProgress) {
            iteration();
        }
        printf("e %d\n", n);
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

    NeoAStar neo(perceptVar, kmX, kmY);
    neo.play();
}