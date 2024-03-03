#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
converted from
https://github.com/ideaowl-sandbox/entropy-as-a-measure-of-puzzle-difficulty
"""
import json
from datetime import datetime
from random import uniform
from time import sleep
from typing import Dict, List, Optional

import numpy as np
import requests
from scipy.stats import linregress

BASE_URL = "https://windmill.thefifthmatt.com/_/things?sort=new"
DATE = datetime.today().strftime("%Y-%m-%d")


def get(start_id: Optional[str] = None) -> dict:
    url = BASE_URL
    if start_id:
        url += f"&start={start_id}"
    response = requests.get(url)
    if response.status_code != 200:
        raise requests.RequestException(f"Failed to download data: {response.status_code}")
    return response.json()


def download():
    has_more = True
    start_id: Optional[str] = None
    data: List[Dict[str, str]] = []
    count = 0
    fd = open(f"data/{DATE}.json", "w")
    while has_more:
        try:
            response = get(start_id)
        except requests.RequestException as e:
            fd.close()
            print(e)
            return
        has_more = response.get("hasMore")
        data.extend(response.get("things"))
        start_id = data[-1]["id"]
        count += 1
        if count % 10 == 0:
            fd.seek(0)
            json.dump(data, fd, indent=2)
            print(f"# of puzzles downloaded: {len(data)}")
        sleep(uniform(0, 0.5))
    fd.seek(0)
    json.dump(data, fd, indent=2)
    fd.close()
    print(f"# of puzzles downloaded: {len(data)}")


def normalize(file: str = f"data/{DATE}.json"):
    upvote = np.array([], dtype=np.int32)
    timestamp = np.array([], dtype=np.int64)
    with open(file) as f:
        data = json.load(f)
    for record in data:
        # noinspection SpellCheckingInspection
        upvote = np.append(upvote, np.int32(record.get("upvotes")))
        timestamp = np.append(timestamp, np.int64(record.get("createUtc")))
    return linregress(timestamp, upvote)


if __name__ == '__main__':
    download()
    print(normalize())
