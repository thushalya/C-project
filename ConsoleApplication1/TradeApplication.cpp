#include "TradeApplication.h"
#include "Constants.h"
mutex mtx;

Trade& Trade::getInstance() {
	/* The static keyword ensures that the variable instance is initialized only once during the program's lifetime.
		When the getInstance function is called for the first time, the instance is created.
		Subsequent calls to the function return the existing instance without reinitializing it. Also thread safe (C++11)*/
	static Trade instance; 
	return instance;
}

void Trade::insertTransactionQueue(FlowerOrder order, std::string exec_status, bool acquireLock = true)
{
	if (acquireLock) {
		mtx.lock();
	}
	order.exec_status = exec_status;
	this->trade_regular_queue.push(order);
	if (acquireLock) {
		mtx.unlock();
	}
	return;
}

std::pair<bool, std::string> Trade::validateOrder(const FlowerOrder& order) {
	// Check if order is valid based on your criteria
	// An order will be rejected if:
	// 1) It does not contain a required field
	// 2) It is for an invalid Instrument
	// 3) It contains an invalid side
	// 4) Its price is not greater than 0
	// 5) Its quantity is not a multiple of 10
	// 6) Its quantity is outside the range (min = 10 max = 1000)
	
	if (order.customer_id.empty()) {
		return std::make_pair(false, "Invalid customer order ID");
	}

	if (order.instrument.empty() || order.instrument == "" || (FlowerTrading::validInstruments.find(order.instrument) == FlowerTrading::validInstruments.end())) {
		return std::make_pair(false, "Invalid instrument");
	}

	if (order.side.empty() || (order.side != "1" && order.side != "2")) {
		return std::make_pair(false, "Invalid side");
	}

	if (order.quantity <= 0 || order.quantity % 10 != 0) {
		return std::make_pair(false, "Invalid size");
	}

	if (order.quantity >= 1000) {
		return std::make_pair(false, "Invalid size");
	}

	if (order.price <= 0) {
		return std::make_pair(false, "Invalid price");
	}

	return std::make_pair(true, ""); // Return true if all validations passed
}



void Trade::executeOrders(std::string flower)
{
	vector<FlowerOrder> flower_rows = this->order_map[flower];
	// order book for buy and sell side
	FlowerOrderBook order_book(flower);

	// read the each element of flower orders
	for (int i = 0; i < flower_rows.size(); i++)
	{
		FlowerOrder order = flower_rows[i]; // getting a row of a order
		string timestamp;

		// making the order id
		string order_id = "ord";
		order_id.append(to_string(static_cast<int>(order.order_flow)));
		order.order_id = order_id;

		// Validate the order
		std::pair<bool, std::string> validationResult = validateOrder(order);

		bool order_ok = validationResult.first;
		string cause = validationResult.second;

		if (order_ok) {
			if (order.side == "1")
			{ // buy order
				calcBuyOrders(order_book, order);
				if (order.quantity != 0)
				{
					order_book.addBuyArr(order); // decending order
				}
			}
			else
			{ // sell order
				calcSellOrders(order_book, order);
				if (order.quantity != 0)
				{
					order_book.addSellArr(order); // ascending order
				}
			}
		}
		else {
			order.reason = cause;
			this->insertTransactionQueue(order, "Reject");
		}
	}
}

void Trade::calcBuyOrders(FlowerOrderBook& order_book, FlowerOrder& order)
{
	priority_queue<FlowerOrder, std::vector<FlowerOrder>, SellOrderComparator>* sell_book = &order_book.sell_orders;

	if (sell_book->size() == 0)
	{ // there are nothing to sell

		this->insertTransactionQueue(order, "New");
	}
	else
	{
		if (sell_book->top().price > order.price) 
		{
			this->insertTransactionQueue(order, "New");
		}
		else
		{
			while (order.quantity != 0)
			{
				if (sell_book->top().price > order.price)
				{
					break;
				}
				int sell_quantity = (sell_book->top().quantity);
				int buy_quantity = (order.quantity);
				int row_price = order.price;
				int sell_price = sell_book->top().price;

				order.price = sell_price;

				if (sell_quantity > buy_quantity)
				{
					FlowerOrder top_sell_order = sell_book->top();
					top_sell_order.quantity = buy_quantity; 
					sell_book->pop();
					sell_book->push(top_sell_order);

					// Acquiring the lock here because both Fill and PFill records should be inserted together
					mtx.lock();
					this->insertTransactionQueue(order, "Fill", false);
					order.quantity = 0;
					this->insertTransactionQueue(sell_book->top(), "PFill", false);
					mtx.unlock();

					top_sell_order = sell_book->top();
					top_sell_order.quantity = sell_quantity - buy_quantity;
					sell_book->pop();
					sell_book->push(top_sell_order);;
					break;
				}
				else if (sell_quantity < buy_quantity)
				{
					order.quantity = sell_quantity;
					
					// Acquiring the lock here because both Fill and PFill records should be inserted together
					mtx.lock();
					this->insertTransactionQueue(order, "PFill", false);
					order.price = row_price; 
					order.quantity = buy_quantity - sell_quantity;
					this->insertTransactionQueue(sell_book->top(), "Fill", false);
					mtx.unlock();

					sell_book->pop();;
				}
				else
				{ // sell quantity is equal to buy quantity

					// Acquiring the lock here because both Fill and PFill records should be inserted together
					mtx.lock();
					this->insertTransactionQueue(order, "Fill", false);
					this->insertTransactionQueue(sell_book->top(), "Fill", false);
					mtx.unlock();

					sell_book->pop(); // remove the completed order from the order book
					order.quantity = 0;                 // order completed, quantity is zero
					break;
				}
			}
		}

	}
}

void Trade::calcSellOrders(FlowerOrderBook& order_book, FlowerOrder& order)
{
	priority_queue<FlowerOrder, vector<FlowerOrder>, BuyOrderComparator>* buy_book = &order_book.buy_orders;

	if (buy_book->size() == 0)
	{
		this->insertTransactionQueue(order, "New");
	}
	else
	{
		if (buy_book->top().price < order.price)
		{ // buy price is lesser than to sell price

			this->insertTransactionQueue(order, "New");
		}
		else
		{
			while (order.quantity != 0)
			{ // order quantity is not zero

				if (buy_book->size() == 0)
				{ // there are nothing to buy
					break;
				}
				if (buy_book->top().price < order.price)
				{ // buy price is lesser than to sell price
					break;
				}
				int buy_quantity = buy_book->top().quantity;
				int sell_quantity = (order.quantity);
				int row_price = order.price;
				int buy_price = buy_book->top().price;
				// update the transaction price of the order according to the order book
				order.price = buy_price;

				if (buy_quantity > sell_quantity)
				{					// set the transaction quantity
					FlowerOrder top_buy_order = buy_book->top();
					top_buy_order.quantity = sell_quantity;
					buy_book->pop();
					buy_book->push(top_buy_order);
					this->insertTransactionQueue(order, "Fill");
					order.quantity = 0;
					this->insertTransactionQueue(buy_book->top(), "PFill");
					top_buy_order = buy_book->top();
					top_buy_order.quantity = buy_quantity - sell_quantity;
					buy_book->pop();
					buy_book->push(top_buy_order);
					break;
				}
				else if (buy_quantity < sell_quantity)
				{
					order.quantity = buy_quantity;
					this->insertTransactionQueue(order, "PFill");
					order.price = row_price;
					order.quantity = sell_quantity - buy_quantity;
					this->insertTransactionQueue(buy_book->top(), "Fill");
					buy_book->pop();

				}
				else
				{ // buy quantity is equal to sell quantity
					this->insertTransactionQueue(order, "Fill");
					this->insertTransactionQueue(buy_book->top(), "Fill");
					buy_book->pop(); // remove the completed order from the order book
					order.quantity = 0;               // order completed, quantity is zero
					break;
				}
			}
		}

	}
}



