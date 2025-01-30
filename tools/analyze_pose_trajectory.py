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

    # Check standard deviations
    if any(np.std(df_diff[col]) > 0.1 for col in interp_columns):
        exit(-1)

    # Plot differences with error bars
    plt.figure(figsize=(10, 6))
    for col in interp_columns:
        plt.errorbar(df_diff['time'], df_diff[col], yerr=np.std(df_diff[col]), label=col, fmt='-o')
    plt.xlabel('Time')
    plt.ylabel('Difference')
    plt.title('Differences over Time with Error Bars')
    plt.legend()
    plt.show()

if __name__ == "__main__":
    main()