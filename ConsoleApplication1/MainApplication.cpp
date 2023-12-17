#include <iostream>
#include "TradeApplication.h"
#include "CSVProcess.h"
#include <thread>

using namespace std;

int main()
{
     // Get the input file path from the user
    cout << "Enter the path of the input CSV file: ";
    string INPUT_FILE_PATH;
    getline(cin, INPUT_FILE_PATH);

    // Read the CSV file and store the orders in the unordered map flower_fields
    CSV read_file(INPUT_FILE_PATH);

    // Get an instance of the Trade class
    Trade& tradeInstance = Trade::getInstance();

    // Convert CSV to order map and set up the order map or any other configurations
    unordered_map<string, vector<FlowerOrder>> new_order_map = read_file.convertCsvToOrderMap(tradeInstance);
    tradeInstance.order_map = new_order_map;

    // Get the list of all instruments from the CSV
    vector<string> allInstruments = read_file.getInstruments();

    // Threads for each instrument and rejected orders
    const int num_threads = allInstruments.size();
    thread threads[6];

    // Start threads for each instrument
    for (int i = 0; i < num_threads; i++)
    {
        threads[i] = thread(&Trade::executeOrders, &tradeInstance, allInstruments[i]);
    }
   // threads[5] = thread(&Trade::addRejectedOrders, &tradeInstance);

    // Wait until threads are completed
    for (int i = 0; i < num_threads; ++i)
    {
        threads[i].join();
    }

    // Create the final CSV file
    CSV write_file("execution_rep.csv");
    write_file.writeToCsv(tradeInstance.trade_regular_queue);

    return 0;
}
