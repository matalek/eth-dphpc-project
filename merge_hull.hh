#include <vector>
#include "geometric_helpers.hh"
#include <stdio.h>
#include <algorithm>


using namespace std;

//TODO handle more poinst with the same x coordinates

//TODO: use same method with different comparator
int findRightmostPoint(vector <POINT*> &hull) {
    int index = 0;
    for (unsigned int i = 0; i < hull.size(); i++) {
        if (OrderXY(hull[index], hull[i])) {
            index = i;
        }
    }
    return index;
}

int findLeftmostPoint(vector <POINT*> &hull) {
    int index = 0;
    for (unsigned int i = 0; i < hull.size(); i++) {
        if (OrderXY(hull[i], hull[index])) {
            index = i;
        }
    }
    return index;
}

int turnRight(POINT a, POINT b, POINT c) {
    return Det(a,b,c) <= 0;
}

int turnLeft(POINT a, POINT b, POINT c) {
    return Det(a,b,c) >= 0;
}

int goCounterClockwise(int index, vector <POINT*> &hull) {
    return (index + 1) % hull.size();
}

int goClockwise(int index, vector <POINT*> &hull) {
    return index > 0 ? index - 1 : hull.size()-1;
}

int lowerTangentA(vector <POINT*> &hullA, vector <POINT*> &hullB, int aIndex, int bIndex) {
    POINT a = (*hullA[aIndex]);
    POINT u = (*hullA[goCounterClockwise(aIndex,hullA)]);
    POINT w = (*hullA[goClockwise(aIndex,hullA)]);
    POINT b = (*hullB[bIndex]);
    return !(turnLeft(b, a, u) || turnLeft(b, a, w));
}

// A is always the left hull, B is the right one
int upperTangentA(vector <POINT*> &hullA, vector <POINT*> &hullB, int aIndex, int bIndex) {
    POINT a = (*hullA[aIndex]);
    POINT u = (*hullA[goCounterClockwise(aIndex,hullA)]);
    POINT w = (*hullA[goClockwise(aIndex,hullA)]);
    POINT b = (*hullB[bIndex]);
    return !(turnRight(b, a, u) || turnRight(b, a, w));
}

int lowerTangentB(vector <POINT*> &hullA, vector <POINT*> &hullB, int aIndex, int bIndex) {
    POINT a = (*hullA[aIndex]);
    POINT u = (*hullB[goCounterClockwise(bIndex,hullB)]);
    POINT w = (*hullB[goClockwise(bIndex,hullB)]);
    POINT b = (*hullB[bIndex]);
    return !(turnRight(a, b, u) || turnRight(a, b, w));
    //return upperTangentA(hullB, hullA, bIndex, aIndex);
}

int upperTangentB(vector <POINT*> &hullA, vector <POINT*> &hullB, int aIndex, int bIndex) {
    POINT a = (*hullA[aIndex]);
    POINT u = (*hullB[goCounterClockwise(bIndex,hullB)]);
    POINT w = (*hullB[goClockwise(bIndex,hullB)]);
    POINT b = (*hullB[bIndex]);
    return !(turnLeft(a, b, u) || turnLeft(a, b, w));
}

int findLowerTangentA(vector <POINT*> &hullA, vector <POINT*> &hullB, int aIndex, int bIndex) {
    while (!lowerTangentA(hullA, hullB, aIndex, bIndex)) {
        aIndex = goClockwise(aIndex, hullA);
    }
    return aIndex;
}

int findLowerTangentB(vector <POINT*> &hullA, vector <POINT*> &hullB, int aIndex, int bIndex) {
    while (!lowerTangentB(hullA, hullB, aIndex, bIndex)) {
        bIndex = goCounterClockwise(bIndex, hullB);
    }
    return bIndex;
}

int findUpperTangentA(vector<POINT*>& hullA, vector <POINT*> &hullB, int aIndex, int bIndex) {
    while (!upperTangentA(hullA, hullB, aIndex, bIndex)) {
        aIndex = goCounterClockwise(aIndex, hullA);
    }
    return aIndex;
}

int findUpperTangentB(vector<POINT*>& hullA, vector <POINT*> &hullB, int aIndex, int bIndex) {
    while (!upperTangentB(hullA, hullB, aIndex, bIndex)) {
        bIndex=goClockwise(bIndex, hullB);
    }
    return bIndex;
}

//TODO: make it more general
pair<int,int> findLowerT(vector<POINT*>& hullA, vector <POINT*> &hullB) {
    pair<char, double> returnValues;
    int aIndex = findRightmostPoint(hullA);
    int bIndex = findLeftmostPoint(hullB);
    while (!(lowerTangentA(hullA, hullB, aIndex, bIndex) && lowerTangentB(hullA, hullB, aIndex, bIndex))) {
        aIndex = findLowerTangentA(hullA, hullB, aIndex, bIndex);
        bIndex = findLowerTangentB(hullA, hullB, aIndex, bIndex);
    }
    returnValues.first = aIndex;
    returnValues.second = bIndex;
    return returnValues;
}

pair<int,int> findUpperT(vector <POINT*> &hullA, vector <POINT*> &hullB) {
    pair<char, double> returnValues;
    int aIndex = findRightmostPoint(hullA);
    int bIndex = findLeftmostPoint(hullB);

    while (!(upperTangentA(hullA, hullB, aIndex, bIndex) && upperTangentB(hullA, hullB, aIndex, bIndex))) {
        aIndex = findUpperTangentA(hullA, hullB, aIndex, bIndex);
        bIndex = findUpperTangentB(hullA, hullB, aIndex, bIndex);
    }
    returnValues.first = aIndex;
    returnValues.second = bIndex;
    return returnValues;
}


vector<POINT*>  mergeVectors(vector<POINT*>& hullA, vector <POINT*> &hullB){
    vector<POINT*> AB;
    AB.reserve( hullA.size() + hullA.size() ); // preallocate memory
    AB.insert( AB.end(), hullA.begin(), hullA.end() );
    AB.insert( AB.end(), hullB.begin(), hullB.end() );
    return AB;
}

void removeInnerPointsCounterClockwise(vector <POINT*> &hull, int from, int to) {
    if(to == from) {
        POINT * preservePoint = hull[from];
        hull.resize(1);
        hull[0] = preservePoint;
    }
    else if(to - from > 1) {
        hull.erase(hull.begin() + from + 1, hull.begin() + to);
    }
    else if (from > to) {
        hull.resize(from+1);
        hull.erase(hull.begin(), hull.begin() + to);
    }
}

vector<POINT*> mergeHulls(vector<POINT*>& hullA, vector <POINT*> &hullB) {
    pair <int, int> lowT = findLowerT(hullA, hullB);
    pair <int,int> upperT = findUpperT(hullA, hullB);
    removeInnerPointsCounterClockwise(hullA, lowT.first, upperT.first);
    removeInnerPointsCounterClockwise(hullB, upperT.second, lowT.second);
    return mergeVectors(hullA, hullB);
}
