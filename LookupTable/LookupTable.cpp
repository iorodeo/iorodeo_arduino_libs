#include "LookupTable.h"
#include "Streaming.h"

LookupTable::LookupTable() {
    size = 0;
}

bool LookupTable::setTable(int _table[][2], unsigned int _size) {
    bool rtnVal = true;
    table = _table;
    size = _size;
    for (int i=1; i<size; i++) {
        if (table[i-1][0] > table[i][0]) {
            rtnVal = false;
        }
    }
    return rtnVal;
}


int LookupTable::getValue(int x) {
    int rtnVal;

    // handle x values outside of range - return nearest end point value.
    if (x <= table[0][0]) {
        rtnVal = table[0][1];
    }
    else if (x >= table[size-1][0]) {
        rtnVal = table[size-1][1];
    }
    else {
        // x value is inside table - interpolate
        for (int i=1; i<size; i++) {
            if (x < table[i][0]) {
                rtnVal = map(x,table[i-1][0], table[i][0], table[i-1][1], table[i][1]);
            }
        }
    }
    return rtnVal;
}



