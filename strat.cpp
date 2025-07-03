#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <stdexcept>

using namespace std;


vector<double> readCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return {};
    }
    
    vector<double> prices;
    string line;
    

    if (getline(file, line)) {
        cout << "Header: " << line << endl;
    }
    
    int lineNumber = 1;
    while (getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;  
        
        stringstream ss(line);
        string cell;
        vector<string> row;
        
    
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        
    
        string closePrice;
        if (row.size() >= 2) {
            closePrice = row[1];
        } else if (row.size() >= 5) {
            closePrice = row[4];
        }
        
        if (!closePrice.empty()) {
            try {
                // Remove any whitespace
                closePrice.erase(0, closePrice.find_first_not_of(" \t"));
                closePrice.erase(closePrice.find_last_not_of(" \t") + 1);
                
                double price = stod(closePrice);
                prices.push_back(price);
            } catch (const invalid_argument& e) {
                cout << "Line " << lineNumber << ": Invalid price data '" << closePrice << "'" << endl;
            } catch (const out_of_range& e) {
                cout << "Line " << lineNumber << ": Price out of range '" << closePrice << "'" << endl;
            }
        } else {
            cout << "Line " << lineNumber << ": Empty close price" << endl;
        }
    }
    
    cout << "Successfully read " << prices.size() << " price records" << endl;
    if (prices.size() > 0) {
        cout << "First price: " << prices[0] << ", Last price: " << prices.back() << endl;
    }
    
    return prices;
}

// Calculate Exponential Moving Average
vector<double> EMA(const vector<double>& prices, int period) {
    if (prices.empty()) return {};
    
    vector<double> ema(prices.size());
    double multiplier = 2.0 / (period + 1);
    ema[0] = prices[0];
    
    for (size_t i = 1; i < prices.size(); ++i) {
        ema[i] = ((prices[i] - ema[i-1]) * multiplier) + ema[i-1];
    }
    return ema;
}

// Compute MACD and Signal Line
void computeMACD(const vector<double>& prices, vector<double>& macd, vector<double>& signal) {
    if (prices.empty()) {
        cout << "Error: No price data to compute MACD" << endl;
        return;
    }
    
    vector<double> ema12 = EMA(prices, 12);
    vector<double> ema26 = EMA(prices, 26);
    
    macd.resize(prices.size());
    for (size_t i = 0; i < prices.size(); ++i) {
        macd[i] = ema12[i] - ema26[i];
    }
    
    signal = EMA(macd, 9);
}

// Generate Buy/Sell signals based on MACD crossovers
vector<int> generateSignals(const vector<double>& macd, const vector<double>& signal) {
    vector<int> signals(macd.size(), 0);
    
    for (size_t i = 1; i < macd.size(); ++i) {
        if (macd[i-1] < signal[i-1] && macd[i] > signal[i]) {
            signals[i] = 1; // Buy signal
        } else if (macd[i-1] > signal[i-1] && macd[i] < signal[i]) {
            signals[i] = -1; // Sell signal
        }
    }
    return signals;
}

// Backtest using simple buy/sell logic
double backtest(const vector<double>& prices, const vector<int>& signals) {
    if (prices.empty() || signals.empty()) {
        cout << "Error: No data for backtesting" << endl;
        return 0.0;
    }
    
    double cash = 10000.0;
    double shares = 0.0;
    int trades = 0;

    for (size_t i = 0; i < prices.size(); ++i) {
        if (signals[i] == 1 && cash > 0) {
            // Buy signal
            shares = cash / prices[i];
            cash = 0;
            trades++;
            cout << "BUY at $" << fixed << setprecision(2) << prices[i] 
                 << " (Shares: " << setprecision(4) << shares << ")" << endl;
        } else if (signals[i] == -1 && shares > 0) {
            // Sell signal
            cash = shares * prices[i];
            cout << "SELL at $" << fixed << setprecision(2) << prices[i] 
                 << " (Cash: $" << cash << ")" << endl;
            shares = 0;
            trades++;
        }
    }

    cout << "Total trades executed: " << trades << endl;
    
    // Final portfolio value
    double finalValue = cash + shares * prices.back();
    return finalValue;
}

// Export signals to CSV file
void exportSignals(const vector<double>& prices, const vector<double>& macd, 
                  const vector<double>& signal, const vector<int>& signals) {
    ofstream file("signals.csv");
    if (!file.is_open()) {
        cout << "Error: Could not create signals.csv" << endl;
        return;
    }
    
    file << "Price,MACD,Signal,Trade\n";
    for (size_t i = 0; i < prices.size(); ++i) {
        file << fixed << setprecision(4) << prices[i] << "," 
             << macd[i] << "," << signal[i] << "," << signals[i] << "\n";
    }
    
    cout << "Signals exported to signals.csv" << endl;
}

int main() {
    cout << "=== MACD Trading Strategy Backtest ===" << endl;
    
    // Read price data
    vector<double> prices = readCSV("price_data.csv");
    
    if (prices.empty()) {
        cout << "Error: No price data found. Please check your CSV file." << endl;
        cout << "Expected format: Date,Close or Date,Open,High,Low,Close" << endl;
        return 1;
    }
    
    if (prices.size() < 26) {
        cout << "Error: Need at least 26 data points for MACD calculation" << endl;
        return 1;
    }

    // Compute MACD indicators
    vector<double> macd, signal;
    computeMACD(prices, macd, signal);
    
    // Generate trading signals
    vector<int> signals = generateSignals(macd, signal);
    
    // Run backtest
    cout << "\n=== Backtest Results ===" << endl;
    double finalValue = backtest(prices, signals);
    double initialValue = 10000.0;
    double returnPercent = ((finalValue - initialValue) / initialValue) * 100;
    
    cout << fixed << setprecision(2);
    cout << "Initial portfolio value: $" << initialValue << endl;
    cout << "Final portfolio value: $" << finalValue << endl;
    cout << "Total return: " << returnPercent << "%" << endl;
    
    // Export results
    exportSignals(prices, macd, signal, signals);
    
    return 0;
}
