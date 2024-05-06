#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import csv
import json
from typing import Dict, List, Tuple

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
from sklearn.feature_selection import r_regression
from sklearn.linear_model import LinearRegression

DATE = "2024-02-20"
# retrieved from https://ideaowl.com/remuse
INTERCEPT = np.float64(67.33339206338432)
SLOPE = np.float64(-3.987906486478669e-11)
sns.set_theme(rc={"figure.dpi": 300})
DATA_TYPES = {"id": object, "timestamp": np.int64, "upvote": np.int32, "entropy": np.float64,
              "adv_entropy": np.float64, "tsi": np.float64, "solutions": np.int32, "solves": np.int32}


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


def plot():
    data = pd.read_csv("results.csv", dtype=DATA_TYPES)
    timestamp = data[["timestamp"]]
    upvote = data["upvote"]
    solves = data["solves"]

    reg = LinearRegression()
    reg.fit(timestamp, upvote)
    upvote -= reg.predict(timestamp)

    reg.fit(timestamp, solves)
    solves -= reg.predict(timestamp)

    data["upvote"] = upvote
    data["solves"] = solves

    text_y = np.max(upvote) - 1

    entropy = data[["entropy"]]
    ax = sns.regplot(data, x="entropy", y="upvote", ci=None, line_kws={"color": "black"})
    ax.text(np.max(entropy) - 1.5, text_y, f"r = {r_regression(entropy, upvote)[0]:.3f}")
    ax.set(xlabel="Entropy", ylabel="Upvote")
    plt.savefig("entropy_vs_upvote.png")
    plt.clf()

    adv_entropy = data[["adv_entropy"]]
    ax = sns.regplot(data, x="adv_entropy", y="upvote", ci=None, line_kws={"color": "black"})
    ax.text(np.max(adv_entropy) - 1.5, text_y, f"r = {r_regression(adv_entropy, upvote)[0]:.3f}")
    ax.set(xlabel="Adv Entropy", ylabel="Upvote")
    plt.savefig("adv_entropy_vs_upvote.png")
    plt.clf()

    tsi = data[["tsi"]]
    text_x = np.max(tsi) - 1.5
    ax = sns.regplot(data, x="tsi", y="upvote", ci=None, line_kws={"color": "black"})
    ax.text(text_x, text_y, f"r = {r_regression(tsi, upvote)[0]:.3f}")
    ax.set(xlabel="TSI", ylabel="Upvote")
    plt.savefig("tsi_vs_upvote.png")
    plt.clf()

    ax = sns.regplot(data, x="tsi", y="solves", ci=None, line_kws={"color": "black"})
    ax.text(text_x, np.max(solves) - 1, f"r = {r_regression(tsi, solves)[0]:.3f}")
    ax.set(xlabel="TSI", ylabel="Solves")
    plt.savefig("tsi_vs_solves.png")
    plt.clf()

    ax = sns.relplot(data, x="entropy", y="adv_entropy", hue="upvote", size="upvote",
                     palette="plasma", alpha=0.95)
    ax.set(xlabel="Entropy", ylabel="Adv Entropy")
    ax.legend.set_title("Upvote")
    plt.savefig("entropy_vs_adv_entropy.png")
    plt.clf()


if __name__ == '__main__':
    plot()
