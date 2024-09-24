#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <string>
#include <cstring> 
#include <iomanip>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
using namespace std;

#pragma region Settings

#define DRAW_EVERY_STEP false

#define FILE_NAME "labyrinth.1.txt"

// If you think something went wrong, then use ASCII fork
#define BORDER  L"██" //"##"
#define FINISH  L"ST"
#define START   L"FN"

#define WHITE   230, 230, 230
#define RED     255,  50,  50
#define GREEN    50, 255,  50
#define BLUE     80,  80, 255

#pragma endregion

struct vec2 {
    int x = 0;
    int y = 0;
    vec2(int x, int y) : x(x), y(y) {}
    vec2() {}
};

vec2 lsize;
vec2 startPos, endPos;
char** Labirint;
int** DistaceMap;

class seeker {
public:
    vec2 pos;
    int distance = 0;

    seeker(vec2 pos) : pos(pos) { DistaceMap[pos.x][pos.y] = distance; }
    seeker(vec2 pos, int distance) : pos(pos), distance(distance) { DistaceMap[pos.x][pos.y] = distance; }
    void move();
};

queue<seeker> Seekers;
string fileName;

void seeker::move() {
    if (Labirint[pos.x - 1][pos.y] != '#' && (DistaceMap[pos.x - 1][pos.y] == -1 || DistaceMap[pos.x - 1][pos.y] > distance + 1))
        Seekers.push(seeker(vec2(pos.x - 1, pos.y), distance + 1));
    if (Labirint[pos.x + 1][pos.y] != '#' && (DistaceMap[pos.x + 1][pos.y] == -1 || DistaceMap[pos.x + 1][pos.y] > distance + 1))
        Seekers.push(seeker(vec2(pos.x + 1, pos.y), distance + 1));
    if (Labirint[pos.x][pos.y - 1] != '#' && (DistaceMap[pos.x][pos.y - 1] == -1 || DistaceMap[pos.x][pos.y - 1] > distance + 1))
        Seekers.push(seeker(vec2(pos.x, pos.y - 1), distance + 1));
    if (Labirint[pos.x][pos.y + 1] != '#' && (DistaceMap[pos.x][pos.y + 1] == -1 || DistaceMap[pos.x][pos.y + 1] > distance + 1))
        Seekers.push(seeker(vec2(pos.x, pos.y + 1), distance + 1));
    //delete this;
}

wstring color(int r, int g, int b) {
    return L"\033[38;2;" + to_wstring(r) + L";" + to_wstring(g) + L";" + to_wstring(b) + L"m";
}

vec2 getLabirintSize() {
    ifstream file(fileName);
    string line;
    vec2 size;

    if (file.is_open()) {
        while (getline(file, line)) {
            size.x = line.length();
            size.y++;
        }
        file.close();
    }
    return size;
}

void getData() {
    ifstream file(fileName);
    string line;

    int y = 0;
    if (file.is_open()) {
        while (getline(file, line)) {
            for (int x = 0; x < line.length(); x++) {
                Labirint[x][y] = line[x];
                if (line[x] == 'S') {
                    startPos.x = x;
                    startPos.y = y;
                }
                if (line[x] == 'F') {
                    endPos.x = x;
                    endPos.y = y;
                }
            }
            y++;
        }
        file.close();
    }
}

void drawLabirint() {
    for (int y = 0; y < lsize.y; y++) {
        for (int x = 0; x < lsize.x; x++) {
            if (Labirint[x][y] == '#') wcout << color(WHITE) << BORDER;
            if (Labirint[x][y] == ' ') {
                if (DistaceMap[x][y] != -1) wcout << color(BLUE) << setw(2) << DistaceMap[x][y] % 100;
                else wcout << color(WHITE) << "  ";
            }
            if (Labirint[x][y] == 'S') wcout << color(GREEN) << START;
            if (Labirint[x][y] == 'F') wcout << color(RED) << FINISH;
        }
        wcout << endl;
    }
}

void summonStartSeekers() {
    for (int y = 0; y < lsize.y; y++) {
        for (int x = 0; x < lsize.x; x++) {
            if (Labirint[x][y] == 'S') Seekers.push(seeker(vec2(x, y)));
        }
    }
}

void drawBestWay() {
    vec2 wayFinderPos = endPos;
    int** BestWayMap = new int* [lsize.x];
    for (int x = 0; x < lsize.x; x++) {
        BestWayMap[x] = new int[lsize.y];
        for (int y = 0; y < lsize.y; y++) BestWayMap[x][y] = -1;
    }

    while (!(wayFinderPos.x == startPos.x && wayFinderPos.y == startPos.y)) {
        //cout << wayFinderPos.x << " " << wayFinderPos.y << endl;
        if (DistaceMap[wayFinderPos.x][wayFinderPos.y] == DistaceMap[wayFinderPos.x + 1][wayFinderPos.y] + 1)
            wayFinderPos.x++;
        else if (DistaceMap[wayFinderPos.x][wayFinderPos.y] == DistaceMap[wayFinderPos.x - 1][wayFinderPos.y] + 1)
            wayFinderPos.x--;
        else if (DistaceMap[wayFinderPos.x][wayFinderPos.y] == DistaceMap[wayFinderPos.x][wayFinderPos.y + 1] + 1)
            wayFinderPos.y++;
        else if (DistaceMap[wayFinderPos.x][wayFinderPos.y] == DistaceMap[wayFinderPos.x][wayFinderPos.y - 1] + 1)
            wayFinderPos.y--;

        BestWayMap[wayFinderPos.x][wayFinderPos.y] = DistaceMap[wayFinderPos.x][wayFinderPos.y];
    }

    for (int y = 0; y < lsize.y; y++) {
        for (int x = 0; x < lsize.x; x++) {
            if (Labirint[x][y] == '#') wcout << color(WHITE) << BORDER;
            if (Labirint[x][y] == ' ') {
                if (BestWayMap[x][y] != -1) wcout << color(BLUE) << setw(2) << BestWayMap[x][y] % 100;
                else wcout << color(WHITE) << "  ";
            }
            if (Labirint[x][y] == 'S') wcout << color(GREEN) << START;
            if (Labirint[x][y] == 'F') wcout << color(RED) << FINISH;
        }
        wcout << endl;
    }
}



int main() {
    //SetConsoleOutputCP(CP_UTF16);
    _setmode(_fileno(stdout), _O_U16TEXT);

    fileName = FILE_NAME;

    lsize = getLabirintSize();
    Labirint = new char* [lsize.x];
    DistaceMap = new int* [lsize.x];
    for (int x = 0; x < lsize.x; x++) {
        Labirint[x] = new char[lsize.y];
        DistaceMap[x] = new int[lsize.y];
        for (int y = 0; y < lsize.y; y++) DistaceMap[x][y] = -1;
    }

    getData();
    drawLabirint(); wcout << endl;
    summonStartSeekers();
    while (!Seekers.empty()) {
        for (int count = Seekers.size(); count != 0; count--) {
            Seekers.front().move();
            Seekers.pop();
        }
        if (DRAW_EVERY_STEP) {drawLabirint(); wcout << endl;}
    }
    drawLabirint(); wcout << endl;
    drawBestWay();

}