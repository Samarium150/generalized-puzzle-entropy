#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import csv
import json
import numpy as np
from datetime import datetime, timezone
import matplotlib.pyplot as plt
from scipy.optimize import minimize
from scipy.stats import pearsonr
from typing import Dict, List

D0 = "2020-02-27"
D1 = "2024-02-20"


def normalize() -> float:
    d0 = datetime.fromisoformat(D0).astimezone(timezone.utc)
    d1 = datetime.fromisoformat(D1).astimezone(timezone.utc)
    record = dict()
    old_up_votes = []
    new_up_votes = []
    old_ages = []
    new_ages = []
    with open(f"data/{D0}.json", "r") as f:
        old = json.load(f)
        for data in old:
            # noinspection SpellCheckingInspection
            record[data.get("id")] = (data.get("upvotes"),
                                      (d0 - datetime.utcfromtimestamp(data.get("createUtc") // 1000).astimezone()).days)
    with open(f"data/{D1}.json", "r") as f:
        new = json.load(f)
        for data in new:
            pid = data.get("id")
            if pid in record:
                old_up_votes.append(record.get(data.get("id"))[0])
                old_ages.append(record.get(data.get("id"))[1])
                # noinspection SpellCheckingInspection
                new_up_votes.append(data.get("upvotes"))
                new_ages.append((d1 - datetime.utcfromtimestamp(data.get("createUtc") // 1000).astimezone()).days)

    old_up_votes = np.array(old_up_votes, dtype=np.int16)
    new_up_votes = np.array(new_up_votes, dtype=np.int16)
    old_ages = np.array(old_ages, dtype=np.float64) / 7
    new_ages = np.array(new_ages, dtype=np.float64) / 7

    def loss(r):
        predicted = old_up_votes * (1 + r[0]) ** (new_ages - old_ages)
        error = new_up_votes - predicted
        return np.sum(error ** 2)

    result = minimize(loss, np.array([0.001]), bounds=[(0, None)])
    return result.x[0]


def ranking():
    with open(f"data/{D1}.json") as f:
        data = json.load(f)
    d: Dict[str, int] = dict()
    for record in data:
        name = record.get("creatorName")
        # noinspection SpellCheckingInspection
        d[name] = d.setdefault(name, 0) + int(record.get("upvotes"))
    print(sorted(d.items(), key=lambda x: x[1], reverse=True))


def plot():
    x_1: List[float] = []
    x_2: List[float] = []
    y: List[float] = []
    with open("results.csv", "r") as f:
        reader = csv.DictReader(f)
        row: Dict[str, str]
        for row in reader:
            if row["entropy"] == "inf":
                continue
            x_1.append(float(row["entropy"]))
            x_2.append(float(row["adv_entropy"]))
            y.append(float(row["up_votes"]))
    plt.scatter(x_1, y)
    plt.text(max(x_1) - 1.5, max(y) - 1, "r = {:.2f}".format(pearsonr(x_1, y)[0]))
    plt.xlabel("Entropy")
    plt.ylabel("Up Votes")
    plt.savefig("entropy_vs_up_votes.png")
    plt.clf()
    plt.scatter(x_2, y)
    plt.text(max(x_2) - 1.5, max(y) - 1, "r = {:.2f}".format(pearsonr(x_2, y)[0]))
    plt.xlabel("Adv Entropy")
    plt.ylabel("Up Votes")
    plt.savefig("adv_entropy_vs_up_votes.png")


if __name__ == '__main__':
    plot()
