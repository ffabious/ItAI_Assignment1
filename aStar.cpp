// This code is outdated, check aStarProper.cpp

#include <bits/stdc++.h>

using namespace std;

struct coord
{
    int x;
    int y;
};

struct move_t
{
    int x;
    int y;
    int fVal;
    int hVal;
};

bool sorter(move_t a, move_t b) {
    if (a.fVal == b.fVal) return a.hVal < b.hVal;
    return a.fVal < b.fVal;
}

class NeoAStar {

private:
    int perceptionVariant;
    int f[9][9];
    int g[9][9];
    int h[9][9];
    coord NeoPos = {0, 0};
    coord KMPos;
    bool keyFound = false;
    bool gameInProgress = false;
    vector <move_t> moves;

    void fillMaps() {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                g[i][j] = i + j;
                h[i][j] = abs(i - this->KMPos.y) + abs(j - this->KMPos.x);
                f[i][j] = g[i][j] + h[i][j];
            }
        }
    }

    void iterationRegular() {
        sort(moves.begin(), moves.end(), sorter);
    }

public:
    NeoAStar(int perceptionVariant, int KMx, int KMy) {
        this->perceptionVariant = perceptionVariant;
        this->KMPos.x = KMx;
        this->KMPos.y = KMy;

        this->fillMaps();
        this->moves.push_back({0, 0, f[0][0], h[0][0]});
        this->moves.push_back({2, 0, f[0][2], h[0][2]});
        this->moves.push_back({2, 1, f[1][2], h[1][2]});
    }

    coord getKMPos() {
        return this->KMPos;
    }

    coord getNeoPos() {
        return this->NeoPos;
    }

    void printMaps() {
        cout << "F:\n------------\n";
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) cout << f[i][j] << '\t';
            cout << '\n';
        }
        cout << "G:\n------------\n";
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) cout << g[i][j] << '\t';
            cout << '\n';
        }
        cout << "H:\n------------\n";
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) cout << h[i][j] << '\t';
            cout << '\n';
        }
    }

    void play() {
        iterationRegular();
    }
};

int main() {
    int perceptVar, KMx, KMy;

    cin >> perceptVar >> KMx >> KMy;

    NeoAStar neo(perceptVar, KMx, KMy);

    // neo.printMaps();
    neo.play();
}
