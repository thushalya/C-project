#include "FlowerOrder.h"
#ifndef FLOWER_ORDER_H
#define FLOWER_ORDER_H

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
//#include "utils.cpp"
using namespace std;

class FlowerOrder
{
public:
    string customer_id;
    string instrument;
    string side;
    int quantity;
    int price;
    string order_id;
    string exec_status;
    double order_flow; // use for make the correct ordering
    string reason;

    // Constructor
    FlowerOrder(vector<string>& row);

private:
    static int nextOrderFlow;

};



#endif 

int FlowerOrder::nextOrderFlow = 1;

FlowerOrder::FlowerOrder(vector<std::string>& row) {

    order_flow = nextOrderFlow++;;
    customer_id = row[0];
    instrument = row[1];
    side = row[2];
    quantity = stoi(row[3]);
    price = stoi(row[4]);
    reason = "";
};
