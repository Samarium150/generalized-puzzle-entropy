---
title: Experiments
level: 1
layout: figure
figureUrl: /img/the_windmill.png
---
<!--suppress HtmlUnknownTag, HtmlUnknownTarget -->

# The Windmill

---
layout: default
hideInToc: true
---

# The Windmill

<br>
<Transform :scale="1.1">
<v-clicks>

- Users can design and publish Witness puzzles
- Users can upvote or downvote puzzles based on their enjoyment

</v-clicks>
</Transform>
<Transform :scale="1.1">
<br>
<v-clicks>

Assumption:
- Users are advanced players of _The Witness_
- Users generally upvote challenging puzzles and downvote trivial ones

</v-clicks>
</Transform>

---
layout: default
hideInToc: true
---

# Experiment Design

<v-clicks>

- Data Collection: all $4\times 4$ puzzles prior to Nov 29, 2021
    - Each experiment will use a subset of it
- Metric: the Pearson Correlation Coefficients (r-values) from simple linear regression of puzzle difficulty and the number of upvotes
- Measurements:
    - Comparisons between different algorithms (104 puzzles)
    - Comparisons between inference rules (179 puzzles and 226 puzzles)
    - Comparisons between the ML model and inference rules (24 puzzles)

</v-clicks>

---
layout: default
hideInToc: true
---

# Different Algorithms

Question: How do different algorithms perform on the same puzzle datasets?<br>
Dataset: $104$ puzzles that only contain separation and path constraints

<div class="w-4/5 mx-auto">

| **Approach**           |  **Correlation**  | **p-value**            |
|:-----------------------|:-----------------:|:-----------------------|
| MUSE (Reported)        |      $0.410$      | $1.10\times 10^{-5}$   |
| MUSE (Reproduction)    |      $0.412$      | $1.53\times 10^{-5}$   |
| ReMUSE (Reported)      |      $0.570$      | $1.57\times 10^{-10}$  |
| ReMUSE (Reproduction)  |      $0.583$      | $1.02\times 10^{-10}$  |
| TSI                    |      $0.563$      | $5.78\times 10^{-10}$  |

</div>

---
layout: default
hideInToc: true
---

# Different Algorithms

<br>

<div class="grid grid-cols-2">

<img src="/img/ReMUSE_repr.png" alt="ReMUSE" class="w-full">

<img src="/img/TSI_repr.png" alt="TSI" class="w-full">

</div>

---
layout: default
hideInToc: true
---

# Additional Inference Rules

Question: How do different inference rules affect the correlation?<br>
Dataset: $179$ puzzles that contain only, but at least one, triangle constraint and possibly path constraints

<div class="w-4/5 mx-auto">

| **Inference Rule**  |  **Correlation**  | **p-value**          |
|:--------------------|:-----------------:|:---------------------|
| Baseline            |      $0.366$      | $4.67\times 10^{-7}$ |
| Along-the-path      |      $0.433$      | $1.40\times 10^{-9}$ |
| Region-completion   |      $0.252$      | $6.58\times 10^{-4}$ |
| Both                |      $0.322$      | $1.09\times 10^{-5}$ |

</div>

---
layout: default
hideInToc: true
---

# Additional Inference Rules

Question: How do different inference rules affect the correlation?<br>
Dataset: $226$ puzzles that do not contain triangle constraints but contain other types of constraints

<div class="w-4/5 mx-auto">

| **Inference Rule**  |  **Correlation**  | **p-value**           |
|:--------------------|:-----------------:|:----------------------|
| Baseline            |      $0.515$      | $1.03\times 10^{-16}$ |
| Along-the-path      |      $0.494$      | $2.62\times 10^{-15}$ |
| Region-completion   |      $0.497$      | $1.71\times 10^{-15}$ |
| Both                |      $0.479$      | $2.16\times 10^{-14}$ |

</div>

---
layout: default
hideInToc: true
---

# Results from ML Policy

Question: How does the ML policy compare to inference rules?<br>
Model: a modified Bootstrap model proposed by
[Lelis et al., 2022](https://www.ijcai.org/proceedings/2022/0538.pdf) <br>
Test set: $24$ puzzles that only contain separation of two different colors

<Transform :scale="0.78" origin="center top" class="w-4/5 mx-auto -my-4">

| **Player Model**              | **Correlation** | **p-value**          |
|:------------------------------|:---------------:|:---------------------|
| Separation Rule (SR/baseline) |     $0.844$     | $2.23\times 10^{-7}$ |
| Along-the-path Rule (APR)     |     $0.720$     | $7.20\times 10^{-5}$ |
| Region-completion Rule (RCR)  |     $0.449$     | $2.77\times 10^{-2}$ |
| SR and APR                    |     $0.769$     | $1.15\times 10^{-5}$ |
| SR and RCR                    |     $0.827$     | $6.26\times 10^{-7}$ |
| APR and RCR                   |     $0.739$     | $3.69\times 10^{-5}$ |
| All Inference Rules           |     $0.771$     | $1.02\times 10^{-5}$ |
| ML Policy                     |     $0.787$     | $5.16\times 10^{-6}$ |

</Transform>

---
layout: default
hideInToc: true
---

Question: Can the ML policy learn inference rules?<br>

<div class="w-4/5 h-full grid grid-cols-2 grid-rows-2 mx-auto gap-y-4 -my-4">
<div class="grid grid-cols-subgrid col-span-2 gap-x-4">
<Transform :scale="0.78" origin="center top">

| **Method**  | **Probability** |
|:------------|:---------------:|
| uniform     |     $0.497$     |
| policy      |     $0.002$     |
| inference   |     $0.000$     |

<div class="text-center">Expected probabilities of taking CANNOT_TAKE actions pruned by the separation rule in 11050 states</div>
</Transform>
<Transform :scale="0.78" origin="center top">

| **Method**  | **Probability** |
|:------------|:---------------:|
| uniform     |     $0.497$     |
| policy      |     $0.994$     |
| inference   |     $1.000$     |

<div class="text-center">Expected probabilities of taking MUST_TAKE actions selected by the separation rule in 11050 states</div>
</Transform>
</div>
<div class="grid grid-cols-subgrid col-span-2 gap-x-4">
<Transform :scale="0.78" origin="center top">

| **Method**  | **Probability** |
|:------------|:---------------:|
| uniform     |     $0.465$     |
| policy      |     $0.091$     |
| inference   |     $0.000$     |

<div class="text-center">Expected probabilities of taking CANNOT_TAKE actions pruned by the along-the-path rule in 4518 states</div>
</Transform>
<Transform :scale="0.78" origin="center top">

| **Method**  | **Probability** |
|:------------|:---------------:|
| uniform     |     $0.418$     |
| policy      |     $0.331$     |
| inference   |     $0.000$     |

<div class="text-center">Expected probabilities of taking CANNOT_TAKE actions pruned by the region-completion rule in 2998 states</div>
</Transform>
</div>
</div>

---
layout: default
hideInToc: true
---

# EPCG

<v-click>

Exhaustive procedural content generation (EPCG):

</v-click>
<v-clicks>

- Explore all possible permutations and variations within a set of parameters
- Generator:
    - $\text{unrank}$: convert the input rank (index) into a puzzle
    - $\text{max\_rank}$: the total number of ranks that can be generated
- Evaluator: evaluated the generated puzzle

</v-clicks>

<v-click>

```python
def EPCG(generator, evaluator):
  best = generator.unrank(0)
  for i in range(1, generator.max_rank):
    puzzle = generator.unrank(i)
    if evaluator(puzzle) > evaluator(best):
      best = puzzle
  return best
```

</v-click>

---
layout: figure
figureUrl: /img/editor.png
hideInToc: true
---

# The Witness Puzzle Editor

https://samarium150.github.io/generalized-puzzle-entropy/

---
layout: figure
figureUrl: /img/editor_solution.png
hideInToc: true
---

# The Witness Puzzle Editor

https://samarium150.github.io/generalized-puzzle-entropy/

---
layout: two-cols-title
hideInToc: true
---

# EPCG Query

::left::

<img src="/img/placeholder.svg" alt="placeholder" class="size-95/100">

::right::

<img src="/img/epcg.svg" alt="epcg" class="size-95/100">

---
layout: default
hideInToc: true
---

# EPCG Query

<br>
<div class="w-4/5 mx-auto grid grid-cols-3 gap-x-10">
<div>
<v-click>

<img src="/img/original.png" alt="original">
<div>
  <a href="https://windmill.thefifthmatt.com/zmxfp88" target="_blank">Original puzzle</a><br>
  (207 solves / 4 upvotes)
</div>

</v-click>
</div>
<div>
<v-click>

<img src="/img/replaced.png" alt="replaced">
<div>
Replaced with placeholders
</div>

</v-click>
</div>
<div>
<v-click>

<img src="/img/generated.png" alt="generated">
<div>
  <a href="https://windmill.thefifthmatt.com/7nw1t3r" target="_blank">Generated puzzle</a><br> 
  (113 solves / 5 upvotes)
</div>

</v-click>
</div>
</div>
