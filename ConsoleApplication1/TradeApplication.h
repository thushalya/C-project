#ifndef FLOWER_TRADE_H
#define FLOWER_TRADE_H

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
#include "FlowerOrderBook.h"

using namespace std;
extern mutex mtx;

class Trade
{
public:
    queue<FlowerOrder> trade_regular_queue;
    unordered_map<string, vector<FlowerOrder>> order_map;

    static Trade& getInstance();

    void insertTransactionQueue(FlowerOrder order, std::string exec_status, bool acquireLock);

    void executeOrders(string flower);

private:
    Trade() = default;  // Private constructor to prevent instantiation.

    // Declare these to prevent copies of the singleton.
    Trade(const Trade&) = delete;
    Trade& operator=(const Trade&) = delete;

    void calcBuyOrders(FlowerOrderBook& order_book, FlowerOrder& order);

    void calcSellOrders(FlowerOrderBook& order_book, FlowerOrder& order);

    std::pair<bool, std::string> validateOrder(const FlowerOrder& order);
};


#endif 
