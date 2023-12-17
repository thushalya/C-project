#include "FlowerOrder.h"

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
