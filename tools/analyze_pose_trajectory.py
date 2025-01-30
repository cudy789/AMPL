#!/usr/bin/python3

import pandas as pd
import numpy as np

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

    print("Test x stddev: {:.4e}".format(df_test["x"].std()))
    print("GT x stddev: {:.4e}".format(df_gt["x"].std()))

if __name__ == "__main__":
    main()