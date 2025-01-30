#!/usr/bin/python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys

def create_df(csv_path):
    df = pd.read_csv(csv_path, header=0, names=["time", "x", "y", "z", "roll", "pitch", "yaw"])
    first_valid_index = df[df["x"] != 0].index[0]
    df = df.iloc[first_valid_index:]
    df["time"] -= df["time"][first_valid_index]
    df["time"] /= 1e9

    print(csv_path)
    print(df.describe())

    return df

def main():
    if len(sys.argv) < 2:
        print("Usage: analyze_pose_trajectory.py test_trajectory.csv ground_truth_trajectory.csv")
        exit(1)
    csv_path_test = sys.argv[1]
    csv_path_gt = sys.argv[2]

    df_test = create_df(csv_path_test)
    df_gt = create_df(csv_path_gt)

    # Ignore the first 1 second of data
    df_test = df_test[df_test['time'] >= df_test['time'].min() + 1].reset_index(drop=True)
    df_gt = df_gt[df_gt['time'] >= df_gt['time'].min() + 1].reset_index(drop=True)


    # Interpolate df_test to match df_gt timestamps
    interp_columns = ["x", "y", "z", "roll", "pitch", "yaw"]
    df_test_interpolated = pd.DataFrame({"time": df_gt["time"]})
    for col in interp_columns:
        df_test_interpolated[col] = np.interp(df_gt["time"], df_test["time"], df_test[col])

    # Compute the difference
    df_diff = df_gt.copy()
    for col in interp_columns:
        df_diff[col] = df_gt[col] - df_test_interpolated[col]

    print("Difference between test and ground truth data: {}".format(df_diff.describe()))

    # Plot differences with error bars
    plt.figure(figsize=(10, 6))
    for col in interp_columns[:3]:
        plt.errorbar(df_diff['time'], df_diff[col], yerr=np.std(df_diff[col]), label=col, fmt='-o', alpha=0.2, capsize=3)
    plt.xlabel('Time')
    plt.ylabel('Difference')
    plt.title('Differences over Time with Error Bars')
    plt.legend()
    plt.tight_layout()
    plt.savefig(csv_path_gt + "_position.png")

    # Plot differences with error bars
    plt.figure(figsize=(10, 6))
    for col in interp_columns[3:]:
        plt.errorbar(df_diff['time'], df_diff[col], yerr=np.std(df_diff[col]), label=col, fmt='-o', alpha=0.2, capsize=3)
    plt.xlabel('Time')
    plt.ylabel('Difference')
    plt.title('Differences over Time with Error Bars')
    plt.legend()
    plt.tight_layout()
    plt.savefig(csv_path_gt + "_orientation.png")

    # Check standard deviations
    if any(np.std(df_diff[col]) > 0.1 for col in interp_columns[:3]):
        print("ERROR: a position stddev is >0.1")
        exit(-1)

    if any(np.std(df_diff[col]) > 3.0 for col in interp_columns[3:]):
        print("ERROR: an orientation stddev is >3.0")
        exit(-1)

if __name__ == "__main__":
    main()