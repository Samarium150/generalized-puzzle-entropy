#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import csv
import json
from typing import Dict, List, Tuple

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
import statsmodels.api as sm

DATE = "2024-02-20"
# noinspection SpellCheckingInspection
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

    model = sm.OLS(upvote, sm.add_constant(timestamp))
    results = model.fit()
    upvote -= model.predict(results.params)
    data["upvote"] = upvote
    text_y = np.max(upvote) - 1

    muse = data[["muse"]]
    model = sm.OLS(upvote, sm.add_constant(muse))
    results = model.fit()
    with open("MUSE_vs_upvote.txt", "w") as f:
        f.write(results.summary().as_text())
    ax = sns.regplot(data, x="muse", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(muse) - 1.5, text_y, f"r = {np.sqrt(results.rsquared):.3f}")
    ax.set(xlabel="MUSE", ylabel="Upvote")
    plt.tight_layout()
    plt.savefig("MUSE_vs_upvote.png")
    plt.clf()

    remuse = data[["remuse"]]
    model = sm.OLS(upvote, sm.add_constant(remuse))
    results = model.fit()
    with open("ReMUSE_vs_upvote.txt", "w") as f:
        f.write(results.summary().as_text())
    ax = sns.regplot(data, x="remuse", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(remuse) - 1.5, text_y, f"r = {np.sqrt(results.rsquared):.3f}")
    ax.set(xlabel="ReMUSE (reproduction)", ylabel="Upvote")
    plt.savefig("ReMUSE_vs_upvote.png")
    plt.clf()

    tsi = data[["tsi"]]
    model = sm.OLS(upvote, sm.add_constant(tsi))
    results = model.fit()
    with open("TSI_vs_upvote.txt", "w") as f:
        f.write(results.summary().as_text())
    ax = sns.regplot(data, x="tsi", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(tsi) - 1.5, text_y, f"r = {np.sqrt(results.rsquared):.3f}")
    ax.set(xlabel="TSI", ylabel="Upvote")
    plt.savefig("TSI_vs_upvote.png")
    plt.clf()


def plot2():
    data = pd.read_csv("output/curricula/temp.csv")
    timestamp = data[["timestamp"]]
    upvote = data["upvote"]

    model = sm.OLS(upvote, sm.add_constant(timestamp))
    results = model.fit()
    upvote -= model.predict(results.params)
    data["upvote"] = upvote
    text_y = np.max(upvote) - 1

    policy_msi = data[["policy_msi"]]
    model = sm.OLS(upvote, sm.add_constant(policy_msi))
    results = model.fit()
    with open("policy_msi_vs_upvote.txt", "w") as f:
        f.write(results.summary().as_text())
    ax = sns.regplot(data, x="policy_msi", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(policy_msi) - 1.5, text_y, f"r = {np.sqrt(results.rsquared):.3f}")
    ax.set(xlabel="MSI (ML Policy)", ylabel="Upvote")
    plt.tight_layout()
    plt.savefig("policy_msi_vs_upvote.png")
    plt.clf()

    policy_tsi = data[["policy_tsi"]]
    model = sm.OLS(upvote, sm.add_constant(policy_tsi))
    results = model.fit()
    with open("policy_tsi_vs_upvote.txt", "w") as f:
        f.write(results.summary().as_text())
    ax = sns.regplot(data, x="policy_tsi", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(policy_tsi) - 1.5, text_y, f"r = {np.sqrt(results.rsquared):.3f}")
    ax.set(xlabel="TSI (ML Policy)", ylabel="Upvote")
    ax.set_ylim(ymin=-20, ymax=35)
    plt.tight_layout()
    plt.savefig("policy_tsi_vs_upvote.png")
    plt.clf()

    difficulty = data[["difficulty"]]
    model = sm.OLS(upvote, sm.add_constant(difficulty))
    results = model.fit()
    with open("difficulty_vs_upvote.txt", "w") as f:
        f.write(results.summary().as_text())
    ax = sns.regplot(data, x="difficulty", y="upvote", line_kws={"color": "black"}, robust=True)
    ax.text(np.max(policy_tsi) - 1.5, text_y, f"r = {np.sqrt(results.rsquared):.3f}")
    ax.set(xlabel="Difficulty (ML Policy)", ylabel="Upvote")
    ax.set_ylim(ymin=-20, ymax=35)
    plt.tight_layout()
    plt.savefig("difficulty_vs_upvote.png")
    plt.clf()


if __name__ == '__main__':
    plot()
