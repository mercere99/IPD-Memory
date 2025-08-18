import pandas as pd
import os

TFT_ID = 5
MR_ID= 69 # Majority Response
AD_ID = 0

tft_count = 0
mr_count = 0

ad_count_tft = 0
ad_count_mr = 0

# Count the number of runs that end with TFT
for filename in os.listdir('./data/baseline-tft-ad'):
    if filename.endswith('count.csv'):
        try:
            df = pd.read_csv(os.path.join('./data/baseline-tft-ad', filename))
            if not df.empty and df.shape[1] == 5: 
                last_row = df.iloc[-1]
                if last_row.iloc[2] == TFT_ID:
                    tft_count += 1
                elif last_row.iloc[2] == AD_ID:
                    ad_count_tft += 1
        except Exception as e:
            print(f"Error reading {filename}.")
print(f"TFT became the most common strategy {tft_count} times (4 TFT vs. 496 AD).")
print(f"AD became the most common strategy {ad_count_tft} times (4 TFT vs 496 AD).")

# Count the number of runs that end with MR
for filename in os.listdir('./data/baseline-mr-ad'):
    if filename.endswith('count.csv'):
        try:
            df = pd.read_csv(os.path.join('./data/baseline-mr-ad', filename))
            if not df.empty and df.shape[1] == 5: 
                last_row = df.iloc[-1]
                if last_row.iloc[2] == MR_ID:
                    mr_count += 1
                elif last_row.iloc[2] == AD_ID:
                    ad_count_mr += 1
        except Exception as e:
            print(f"Error reading {filename}.")
print(f"MR became the most common strategy {mr_count} times (4 MR vs. 496 AD).")
print(f"AD became the most common strategy {ad_count_mr} times (4 MR vs 496 AD).")