#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import csv
import matplotlib.pyplot as plt
from typing import Dict, List
from scipy.stats import pearsonr


def plot():
    x_1: List[float] = []
    x_2: List[float] = []
    y: List[int] = []
    with open("results.csv", "r") as f:
        reader = csv.DictReader(f)
        row: Dict[str, str]
        for row in reader:
            if row["entropy"] == "inf":
                continue
            x_1.append(float(row["entropy"]))
            x_2.append(float(row["adv_entropy"]))
            y.append(int(row["up_votes"]))
    plt.scatter(x_1, y)
    plt.text(12, 39, "r = {:.2f}".format(pearsonr(x_1, y)[0]))
    plt.xlabel("Entropy")
    plt.ylabel("Up Votes")
    plt.savefig("entropy_vs_up_votes.png")
    plt.clf()
    plt.scatter(x_2, y)
    plt.text(14, 39, "r = {:.2f}".format(pearsonr(x_2, y)[0]))
    plt.xlabel("Adv Entropy")
    plt.ylabel("Up Votes")
    plt.savefig("adv_entropy_vs_up_votes.png")


if __name__ == '__main__':
    plot()
