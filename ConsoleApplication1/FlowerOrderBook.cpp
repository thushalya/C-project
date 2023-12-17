#include "FlowerOrderBook.h"
#include <utility>

FlowerOrderBook::FlowerOrderBook(std::string instrument) {
    this->instrument = std::move(instrument);
}

void FlowerOrderBook::addBuyArr(const FlowerOrder order)
{
    this->buy_orders.push(order);
}

void FlowerOrderBook::addSellArr(const FlowerOrder order)
{
    this->sell_orders.push(order);
}


