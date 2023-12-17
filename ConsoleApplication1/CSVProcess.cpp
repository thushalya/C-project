#include "CSVProcess.h"


unordered_map<string, vector<FlowerOrder>> CSV::convertCsvToOrderMap(Trade& tradeInstance)
{
	unordered_map<string, vector<FlowerOrder>> order_map;
	ifstream inputFile(filename);
	if (!inputFile.is_open())
	{
		cerr << "Error opening file." << endl;
		return order_map;
	}

	string line;
	int count = 0;
	getline(inputFile, line); // Skip the first line (header)
	while (getline(inputFile, line))
	{
		istringstream lineStream(line);
		string field;
		int rejected = 0; 
		vector<string> row = {  };

		while (getline(lineStream, field, ','))
		{
			row.push_back(field); // Store each field
		}

		FlowerOrder order(row);
		string flowerName = order.instrument;
		order_map[flowerName].push_back(order);
		
		// Check for invalid flower name
		if (order.instrument.empty()) {
			// making the order id
			string order_id = "ord";
			order_id.append(to_string(static_cast<int>(order.order_flow)));
			order.order_id = order_id;

			order.reason = "Invalid Instrument";
			tradeInstance.insertTransactionQueue(order, "Reject", true);
		}

		if (std::find(instruments.begin(), instruments.end(), flowerName) == instruments.end())
		{
			instruments.push_back(flowerName);
		}
	}

	inputFile.close(); // Close the file
	return order_map;
}

void CSV::writeToCsv(queue<FlowerOrder> trade_queue)
{
	ofstream outputFile(filename);
	if (!outputFile.is_open())
	{
		cerr << "Error opening output file." << endl;
		return;
	}

	// Check if at least one order in trade_queue has a non-empty msg
	bool includeReason = false;
	queue<FlowerOrder> tempQueue = trade_queue; // Create a temporary queue

	while (!tempQueue.empty())
	{
		if (!tempQueue.front().reason.empty())
		{
			includeReason = true;
			break;
		}
		tempQueue.pop();
	}

	// Adds the header row first
	vector<string> trade_arr = { "Order ID", "Client Order ID", "Instrument", "Side",
								"Exec Status", "Quantity", "Price" };

	// Add "Reason" if at least one order has a non-empty msg
	if (includeReason)
	{
		trade_arr.push_back("Reason");
	}

	for (size_t i = 0; i < trade_arr.size(); ++i)
	{
		outputFile << trade_arr[i];
		if (i < trade_arr.size() - 1)
		{
			outputFile << ",";
		}
	}
	outputFile << endl;

	if (includeReason) {
		// Write each Order object as a CSV row
		while (!trade_queue.empty())
		{
			FlowerOrder top_order = trade_queue.front();
			trade_queue.pop();
			outputFile
				<< top_order.order_id << ","
				<< top_order.customer_id << ","
				<< top_order.instrument << ","
				<< top_order.side << ","
				<< top_order.exec_status << ","
				<< top_order.quantity << ","
				<< top_order.price << ","
				<< top_order.reason << ","
				<< endl;
		}
	}
	else {
		// Write each Order object as a CSV row
		while (!trade_queue.empty())
		{
			FlowerOrder top_order = trade_queue.front();
			trade_queue.pop();
			outputFile
				<< top_order.order_id << ","
				<< top_order.customer_id << ","
				<< top_order.instrument << ","
				<< top_order.side << ","
				<< top_order.exec_status << ","
				<< top_order.quantity << ","
				<< top_order.price << ","
				<< endl;
		}
	}

	outputFile.close();
	cout << "CSV file created successfully." << endl;
}

std::vector<std::string> CSV::getInstruments() const
{
	return instruments;
}







