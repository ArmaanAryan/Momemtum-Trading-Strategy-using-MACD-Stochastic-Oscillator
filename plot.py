import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("signals.csv")

plt.figure(figsize=(12,6))
plt.plot(df['Price'], label='Close Price')
plt.plot(df['MACD'], label='MACD')
plt.plot(df['Signal'], label='Signal Line')
plt.legend()

# Plot buy/sell signals
for i in range(1, len(df)):
    if df['Trade'][i] == 1:
        plt.scatter(i, df['Price'][i], color='green', label='Buy' if i == 1 else "", marker='^')
    elif df['Trade'][i] == -1:
        plt.scatter(i, df['Price'][i], color='red', label='Sell' if i == 1 else "", marker='v')

plt.title("MACD Momemtum Strategy")
plt.xlabel("Days")
plt.ylabel("Price")
plt.grid(True)
plt.legend()
plt.show()
