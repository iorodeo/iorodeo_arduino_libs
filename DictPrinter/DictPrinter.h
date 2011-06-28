#ifndef DictPrinter_h
#define DictPrinter_h

#define DP_DOUBLE_PREC 12
#define DP_STR_LEN 30

class DictPrinter {
    public:
        DictPrinter();
        void start();
        void stop();
        void addStrItem(char *key, char *value);
        void addFltItem(char *key, float value);
        void addDblItem(char *key, double value);
        void addIntItem(char *key, int value);
        void addLongItem(char *key, long value);
        int len();
    private:
        int numberOfItems;
};
#endif
