#!/usr/bin/python3

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

import sys

def main():
    if len(sys.argv) > 1:
        for csv_path in sys.argv[1:]:

            df = pd.read_csv(csv_path, header=0, names=["time", "x", "y", "z", "roll", "pitch", "yaw"])

            first_valid_index = df[df["x"] != 0].index[0]
            df = df.iloc[first_valid_index:]
            df["time"] -= df["time"][first_valid_index]
            df["time"] /= 1e9

            print(csv_path)
            print(df.describe())


            x_val = df["time"]

            fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))

            fig.suptitle(csv_path)


            ax1.set_title("Position x, y, z")
            ax1.scatter(x_val, df["x"], s=1, label="x")
            ax1.scatter(x_val, df["y"], s=1, label="y")
            ax1.scatter(x_val, df["z"], s=1, label="z")
            ax1.legend()
            ax1.text(0.5, -0.3, 'x stddev: {:.4e}\ny stddev: {:.4e}\nz stddev: {:.4e}'.format(df["x"].std(), df["y"].std(), df["z"].std()), transform=ax1.transAxes, ha='center')

            ax2.set_title("Orientation roll, pitch, yaw")
            ax2.scatter(x_val, df["roll"], s=1, label="roll")
            ax2.scatter(x_val, df["pitch"], s=1, label="pitch")
            ax2.scatter(x_val, df["yaw"], s=1, label="yaw")
            ax2.legend()
            ax2.text(0.5, -0.3, 'roll stddev: {:.4e}\npitch stddev: {:.4e}\nyaw stddev: {:.4e}' \
                     .format(df["roll"].std(), df["pitch"].std(), df["yaw"].std()), transform=ax2.transAxes, ha='center')

            plt.tight_layout()
            plt.savefig(csv_path + ".png")
            plt.show()


    else:
        print("Please specify a .csv file")

if __name__ == "__main__":
    main()