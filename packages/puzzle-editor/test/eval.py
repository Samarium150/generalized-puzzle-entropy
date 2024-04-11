#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import csv
import json
from typing import Dict, List, Tuple

from matplotlib import rcParams, pyplot as plt
import numpy as np
from scipy.stats import linregress

DATE = "2024-02-20"
# retrieved from https://ideaowl.com/remuse
INTERCEPT = np.float64(67.33339206338432)
SLOPE = np.float64(-3.987906486478669e-11)
rcParams['figure.dpi'] = 300
# noinspection SpellCheckingInspection
MARKER_SIZE = rcParams["lines.markersize"]


def ranking():
    with open(f"data/{DATE}.json") as f:
        data = json.load(f)
    records: Dict[str, List[int]] = dict()
    for record in data:
        name: str = record.get("creatorName")
        # noinspection SpellCheckingInspection
        records.setdefault(name, []).append(int(record.get("upvotes")))
    d: Dict[str, Tuple[int, float]] = dict()
    for k, v in records.items():
        s = sum(v)
        avg = s / len(v)
        d[k] = (s, avg)
    with open("data/leaderboard.csv", "w") as f:
        writer = csv.DictWriter(f, fieldnames=["name", "total_up_votes", "average_up_votes"])
        writer.writeheader()
        writer.writerows(
            [{"name": k, "total_up_votes": v[0], "average_up_votes": v[1]} for k, v in
             sorted(d.items(), key=lambda item: item[1][0], reverse=True)]
        )


def get_marker_size(arr: np.ndarray) -> np.ndarray:
    t = (arr - np.min(arr)) ** 2
    return t / np.max(t) * MARKER_SIZE * 20 + MARKER_SIZE * 0.1


def plot():
    x1 = np.array([], dtype=np.float64)
    x2 = np.array([], dtype=np.float64)
    x3 = np.array([], dtype=np.float64)
    s = np.array([], dtype=np.float64)
    t = np.array([], dtype=np.int64)
    y = np.array([], dtype=np.float64)
    with open("results.csv", "r") as f:
        reader = csv.DictReader(f)
        row: Dict[str, str]
        for row in reader:
            if row["entropy"] == "inf":
                continue
            timestamp = np.int64(row["timestamp"])
            upvote = np.int32(row["upvote"])
            entropy = np.float64(row["entropy"])
            adv_entropy = np.float64(row["adv_entropy"])
            csi = np.float64(row["csi"])
            solutions = np.int32(row["solutions"])
            t = np.append(t, timestamp)
            y = np.append(y, upvote)
            x1 = np.append(x1, entropy)
            x2 = np.append(x2, adv_entropy)
            x3 = np.append(x3, csi)
            s = np.append(s, solutions)

    norm = linregress(t, y)
    y = y - (norm.slope * t + norm.intercept)

    result = linregress(x1, y)
    print(result)
    plt.scatter(x1, y)
    plt.plot(x1, result.slope * x1 + result.intercept, color="black")
    plt.text(np.max(x1) - 1.5, np.max(y) - 1, "r = {:.3f}".format(result.rvalue))
    plt.xlabel("Entropy")
    plt.ylabel("Upvote")
    plt.savefig("entropy_vs_upvote.png")
    plt.clf()

    result = linregress(x2, y)
    print(result)
    plt.scatter(x2, y)
    plt.plot(x2, result.slope * x2 + result.intercept, color="black")
    plt.text(np.max(x2) - 1.5, np.max(y) - 1, "r = {:.3f}".format(result.rvalue))
    plt.xlabel("Adv Entropy")
    plt.ylabel("Upvote")
    plt.savefig("adv_entropy_vs_upvote.png")
    plt.clf()

    result = linregress(x3, y)
    print(result)
    plt.scatter(x3, y)
    plt.plot(x3, result.slope * x3 + result.intercept, color="black")
    plt.text(np.max(x3) - 1.5, np.max(y) - 1, "r = {:.3f}".format(result.rvalue))
    plt.xlabel("CSI")
    plt.ylabel("Upvote")
    plt.savefig("csi_vs_upvote.png")
    plt.clf()

    plt.scatter(x1, x2, get_marker_size(y), y, cmap="plasma", alpha=0.95,
                linewidths=0.2, edgecolors="black")
    plt.xlabel("Entropy")
    plt.ylabel("Adv Entropy")
    bar = plt.colorbar()
    bar.set_label("Upvote")
    ticks = np.linspace(np.min(y), np.max(y), 9)
    bar.set_ticks(ticks)
    bar.set_ticklabels([f"{int(i)}" for i in ticks])
    plt.savefig("entropy_vs_adv_entropy.png")
    plt.clf()


if __name__ == '__main__':
    plot()
