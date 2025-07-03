import yfinance as yf
import pandas as pd

data = yf.download('TSLA', start='2024-06-23', end='2025-06-22', auto_adjust=False)

data = data[['Open', 'High', 'Low', 'Close']]
data.to_csv("tsla_data.csv")
print("Saved Tesla data to tsla_data.csv")

