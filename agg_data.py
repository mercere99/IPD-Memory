import numpy as np
from scipy.stats import fisher_exact
import pandas as pd
import os

TFT_ID = 5
MR_ID= 69 # Majority Response
AD_ID = 0

tft_count = 0
ad_count_tft = 0
other_count1 = 0

mr_count = 0
ad_count_mr = 0
other_count2 = 0

baseline_dir = './data/baseline-'
memcost01_dir = './data/memcost0.1-'
mut001_dir = './data/mut0.01-'
mut01_dir = './data/mut0.1-'
hd32_dir = './data/hd32-'


current_dir = baseline_dir

# Count the number of runs that end with TFT
for filename in os.listdir(f'{current_dir}tft-ad'):
    if filename.endswith('count.csv'):
        try:
            df = pd.read_csv(os.path.join(f'{current_dir}tft-ad', filename))
            if not df.empty and df.shape[1] == 5: 
                last_row = df.iloc[-1]
                if last_row.iloc[2] == TFT_ID:
                    tft_count += 1
                elif last_row.iloc[2] == AD_ID:
                    ad_count_tft += 1
                else:
                    other_count1 += 1
        except Exception as e:
            print(f"Error reading {filename}.")

# Count the number of runs that end with MR
for filename in os.listdir(os.path.join(f'{current_dir}mr-ad')):
    if filename.endswith('count.csv'):
        try:
            df = pd.read_csv(os.path.join(f'{current_dir}mr-ad', filename))
            if not df.empty and df.shape[1] == 5: 
                last_row = df.iloc[-1]
                if last_row.iloc[2] == MR_ID:
                    mr_count += 1
                elif last_row.iloc[2] == AD_ID:
                    ad_count_mr += 1
                else:
                    other_count2 += 1
        except Exception as e:
            print(f"Error reading {filename}.")


# Calculate win rates
tft_wins = tft_count
tft_losses = ad_count_tft + other_count1 # total runs minus tft wins

mr_wins = mr_count
mr_losses = ad_count_mr + other_count2

tft_rate = tft_wins / 100
mr_rate = mr_wins / 100

print(f"TFT win rate: {tft_rate:.2f}")
print(f"MR win rate: {mr_rate:.2f}")


# Fisher's exact test!
# Build contingency table
table = np.array([
    [tft_wins, tft_losses],
    [mr_wins, mr_losses]
])

oddsratio, p_value = fisher_exact(table)
print(f"Fisher exact test: odds ratio = {oddsratio:.2f}, p-value = {p_value:.4f}")

