#ifndef FLOWER_CSV_H
#define FLOWER_CSV_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "FlowerOrder.h"
#include "TradeApplication.h"
using namespace std;

class CSV
{
public:
    // Constructor
    explicit CSV(string filename) : filename(std::move(filename)) {}

    //----------------------------------- READ THE CSV FILE-----------------------------------------------------
    unordered_map<string, vector<FlowerOrder>> convertCsvToOrderMap(Trade& tradeInstance);

    //----------------------------------- WRITE TO THE CSV FILE-----------------------------------------------------
    void writeToCsv(queue<FlowerOrder> trade_queue);

    std::vector<std::string> getInstruments() const;

private:
    string filename;
    std::vector<std::string> instruments;

};


#endif //FLOWER_CSV_H
