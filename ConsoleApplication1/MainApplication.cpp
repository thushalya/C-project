#include <iostream>
#include "TradeApplication.h"
#include "CSVProcess.h"
#include <thread>

using namespace std;

int main()
{
    // Get the input file path from the user
    std::cout << "Enter the path of the input CSV file: ";
    std::string inputFilePath;
    std::getline(std::cin, inputFilePath);

    // Read the CSV file and store the orders in the unordered map tradeOrders
    CSV readFile(inputFilePath);

    // Get an instance of the Trade class
    Trade& tradeInstance = Trade::getInstance();

    // Convert CSV to order map and set up the order map or any other configurations
    std::unordered_map<std::string, std::vector<FlowerOrder>> newOrderMap = readFile.convertCsvToOrderMap(tradeInstance);
    tradeInstance.orderMap = newOrderMap;

    // Get the list of all instruments from the CSV
    std::vector<std::string> allInstruments = readFile.getInstruments();

    // Threads for each instrument and rejected orders
    const int numThreads = allInstruments.size();
    std::thread threads[6]; // Assuming there are always 6 instruments

    // Start threads for each instrument
    for (int i = 0; i < numThreads; i++)
    {
        threads[i] = std::thread(&Trade::processOrders, &tradeInstance, allInstruments[i]);
    }

    // Wait until threads are completed
    for (int i = 0; i < numThreads; ++i)
    {
        threads[i].join();
    }

    // Create the final CSV file
    CSV writeFile("execution_report.csv");
    writeFile.writeToCsv(tradeInstance.tradeQueue);

    return 0;
}
