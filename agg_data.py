import pandas as pd
import os

TFT_ID = 5
MR_ID= 69 # Majority Response

tft_count = 0
mr_count = 0

# Count the number of runs that end with TFT
for filename in os.listdir('./data/baseline-tft-ad'):
    if filename.endswith('count.csv'):
        try:
            df = pd.read_csv(os.path.join('./data/baseline-tft-ad', filename))
            if not df.empty and df.shape[1] == 5: 
                last_row = df.iloc[-1]
                if last_row.iloc[2] == TFT_ID:
                    tft_count += 1
        except Exception as e:
            print(f"Error reading {filename}.")
print(f"TFT became the most common strategy {tft_count} times.")


# Count the number of runs that end with MR
for filename in os.listdir('./data/baseline-mr-ad'):
    if filename.endswith('count.csv'):
        try:
            df = pd.read_csv(os.path.join('./data/baseline-mr-ad', filename))
            if not df.empty and df.shape[1] == 5: 
                last_row = df.iloc[-1]
                if last_row.iloc[2] == MR_ID:
                    mr_count += 1
        except Exception as e:
            print(f"Error reading {filename}.")
print(f"MR became the most common strategy {mr_count} times.")