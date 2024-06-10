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
sns.set_theme(rc={"figure.dpi": 300})
# noinspection SpellCheckingInspection
DATA_TYPES = {"id": str, "timestamp": np.int64, "upvote": np.int32, "muse": np.float64, "remuse": np.float64,
              "adv_entropy": np.float64, "tsi": np.float64, "solutions": np.int32, "solves": np.int32}


def ranking(date=DATE):
    with open(f"data/{date}.json") as f:
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


# noinspection SpellCheckingInspection
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

    muse = data[["muse"]]
    ax = sns.regplot(data, x="muse", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(muse) - 1.5, text_y, f"r = {r_regression(muse, upvote)[0]:.3f}")
    ax.set(xlabel="MUSE", ylabel="Upvote")
    plt.savefig("MUSE_vs_upvote.png")
    plt.clf()

    remuse = data[["remuse"]]
    ax = sns.regplot(data, x="remuse", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(remuse) - 1.5, text_y, f"r = {r_regression(remuse, upvote)[0]:.3f}")
    ax.set(xlabel="ReMUSE", ylabel="Upvote")
    plt.savefig("ReMUSE_vs_upvote.png")
    plt.clf()

    adv_entropy = data[["adv_entropy"]]
    ax = sns.regplot(data, x="adv_entropy", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(adv_entropy) - 1.5, text_y, f"r = {r_regression(adv_entropy, upvote)[0]:.3f}")
    ax.set(xlabel="Adv Entropy", ylabel="Upvote")
    plt.savefig("adv_entropy_vs_upvote.png")
    plt.clf()

    tsi = data[["tsi"]]
    text_x = np.max(tsi) - 1.5
    ax = sns.regplot(data, x="tsi", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(text_x, text_y, f"r = {r_regression(tsi, upvote)[0]:.3f}")
    ax.set(xlabel="TSI", ylabel="Upvote")
    plt.savefig("TSI_vs_upvote.png")
    plt.clf()

    ax = sns.regplot(data, x="tsi", y="solves", line_kws={"color": "black"})
    ax.text(text_x, np.max(solves) - 1, f"r = {r_regression(tsi, solves)[0]:.3f}")
    ax.set(xlabel="TSI", ylabel="Solves")
    plt.savefig("TSI_vs_solves.png")
    plt.clf()

    ax = sns.relplot(data, x="muse", y="adv_entropy", hue="upvote", size="upvote",
                     palette="plasma", alpha=0.95)
    ax.set(xlabel="MUSE", ylabel="Adv Entropy")
    ax.legend.set_title("Upvote")
    plt.savefig("MUSE_vs_adv_entropy.png")
    plt.clf()

    ax = sns.relplot(data, x="remuse", y="adv_entropy", hue="upvote", size="upvote",
                     palette="plasma", alpha=0.95)
    ax.set(xlabel="ReMUSE", ylabel="Adv Entropy")
    ax.legend.set_title("Upvote")
    plt.savefig("ReMUSE_vs_adv_entropy.png")
    plt.clf()

    ax = sns.relplot(data, x="tsi", y="adv_entropy", hue="upvote", size="upvote",
                     palette="plasma", alpha=0.95)
    ax.set(xlabel="TSI", ylabel="Adv Entropy")
    ax.legend.set_title("Upvote")
    plt.savefig("TSI_vs_adv_entropy.png")
    plt.clf()

    reg.fit(data[["muse", "remuse", "adv_entropy", "tsi"]], upvote)
    with open("measures.json", "w") as f:
        json.dump({
            "muse": reg.coef_[0],
            "remuse": reg.coef_[1],
            "adv_entropy": reg.coef_[2],
            "tsi": reg.coef_[3],
            "score": reg.score(data[["muse", "remuse", "adv_entropy", "tsi"]], upvote)
        }, f, indent=2)


if __name__ == '__main__':
    plot()
