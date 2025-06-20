#include <iostream>
#include "../../src/coingecko_fetcher.hpp" // Adjust include as needed

int main() {
    try {
        CoingeckoFetcher fetcher;
        // Example: fetch the price of bitcoin (adjust to your actual API)
        // You may need to adapt method names/parameters.
        double price = fetcher.fetch_price("bitcoin"); // Replace with real method
        std::cout << "Bitcoin price: " << price << std::endl;

        // Optionally, add a basic check
        if (price > 0) {
            std::cout << "Test passed." << std::endl;
            return 0;
        } else {
            std::cerr << "Test failed: price not positive." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 2;
    }
}
