#ifndef ORDER_H
#define ORDER_H
#include "FlowerOrder.h"


struct BuyOrderComparator {
    bool operator()(const FlowerOrder& lhs, const FlowerOrder& rhs) const {
        return lhs.price < rhs.price; // Change to > for descending order
    }
};

struct SellOrderComparator {
    bool operator()(const FlowerOrder& lhs, const FlowerOrder& rhs) const {
        return lhs.price > rhs.price; // Change to < for ascending order
    }
};

#endif // ORDER_H