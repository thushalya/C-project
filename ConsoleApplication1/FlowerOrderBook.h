
#ifndef FLOWER_ORDERBOOK_H
#define FLOWER_ORDERBOOK_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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
#include "Comparators.cpp"

using namespace std;




class FlowerOrderBook
{
public:
    std::string instrument;
    std::queue<FlowerOrder> buy_orders;
    std::queue<FlowerOrder> sell_orders;

    // Constructor
    FlowerOrderBook(std::string instrument);

    void addSellArr(const FlowerOrder& order);

    void addBuyArr(const FlowerOrder& order);
};


#endif //FLOWER_ORDERBOOK_H
