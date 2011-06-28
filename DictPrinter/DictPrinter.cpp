#include "WProgram.h"
#include "Streaming.h"
#include "DictPrinter.h"

DictPrinter::DictPrinter() {
    numberOfItems = 0;
}

void DictPrinter::start() {
    Serial << "{";
    numberOfItems = 0;
}

void DictPrinter::stop() {
    Serial << "}\n";
    numberOfItems = 0;
}

void DictPrinter::addIntItem(char *key, int value) {
    Serial << "'" << key << "'" << ":" <<  _DEC(value) << ",";
    numberOfItems++;
}

void DictPrinter::addLongItem(char *key, long value) {
    Serial << "'" << key << "'" << ":" << _DEC(value) << ",";
}

void DictPrinter::addStrItem(char *key, char *value) {
    Serial << "'" << key << "'" << ":" << "'" << value << "'" << ",";
    numberOfItems++;
}

void DictPrinter::addDblItem(char *key, double value) {
    char valueStr[DP_STR_LEN];
    dtostre(value,valueStr,DP_DOUBLE_PREC,0);
    Serial << "'" << key << "'" << ":" <<  valueStr << ",";
}

void DictPrinter::addFltItem(char *key, float value) {
    char valueStr[DP_STR_LEN];
    dtostre((double)value, valueStr, DP_DOUBLE_PREC,0);
    Serial << "'" << key << "'" << ":" <<  valueStr << ",";
}

int DictPrinter::len() {
    return numberOfItems;
}

