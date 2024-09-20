---
title: |
  Background
  - Information Theory
  - Entropy of Puzzles
level: 1
layout: figure-side
figureUrl: /img/shannon.jpg
figureCaption: Claude Elwood Shannon, 1950s
---
<!--suppress HtmlUnknownTag -->

# Information Theory

<br>

<v-click>

## Information Content

$$
I(x) = -\log_2P(x)
$$

</v-click>

<br>

<v-click>

## Information Entropy

$$
\begin{align*}
H(X) &= \mathbb{E}\left[I(X)\right] \\
     &= -\sum_{x\in\mathcal{X}}P(x)\log_2P(x)
\end{align*}
$$

</v-click>

<v-click>

This is the minimum number of bits<sup>1</sup>
required to encode a message from a source with a given probability distribution without data loss.

<Footnotes>
<Footnote :number=1>binary unit (a.k.a. shannon)</Footnote>
</Footnotes>

</v-click>

---
layout: figure-side
figureUrl: /img/d6.jpg
figureCaption: Standard D6
hideInToc: true
---

# Example

<v-click>

| outcome     |       1       |       2       |       3       |       4       |       5       |       6       |
|:------------|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|
| probability | $\frac{1}{6}$ | $\frac{1}{6}$ | $\frac{1}{6}$ | $\frac{1}{6}$ | $\frac{1}{6}$ | $\frac{1}{6}$ |
| information |  $\log_2 6$   |  $\log_2 6$   |  $\log_2 6$   |  $\log_2 6$   |  $\log_2 6$   |  $\log_2 6$   |

</v-click>

<v-click>

$$
\begin{align*}
H(X) &= \mathbb{E}\left[I(X)\right] \\
     &= -\sum_{x\in\mathcal{X}}P(x)\log_2P(x) \\
     &= -6\times\frac{1}{6}\log_2\frac{1}{6} \\
     &= \log_2 6
\end{align*}
$$

</v-click>

<v-click>

The minimum amount of information required to encode the whole event
is equal to the information of describing a single outcome
if the probability distribution is uniform.

</v-click>

---
layout: two-cols-title
hideInToc: true
---

# Example: MUSE

::left::

<v-click>

Suppose:

</v-click>

<v-clicks>

- $s_0$ is the initial state, $G$ is the goal state
- Arrows represent player actions
- Player's policy is uniformly random

</v-clicks>

<v-click>

The probability of reaching the goal state is $P(G)=\frac{1}{3}\times\frac{1}{3}=\frac{1}{9}$

</v-click>

<v-click>

If the player's policy is uniform, 
the minimum uniform entropy required to describe a single solution to the puzzle a.k.a. 
Minimum Uniform Solution Entropy
(MUSE) [[Chen et al., 2023](https://ojs.aaai.org/index.php/AIIDE/article/view/27499/27272)] 
is<br>
$\mu(X)=\log_2(3)+\log_2(3)=\log_2 9$

</v-click>

::right::

```mermaid
flowchart TD
    style S0 fill:#FF6D00
    style S2 fill:#FFD600
    style G fill:#00C853
    S0(("S0")) -- 1/3 --> S1["S1"]
    S1 -- 1/2 --> S4((("S4"))) & S5((("S5")))
    S0 == 1/3 ==> S2["S2"]
    S2 -- 1/3 --> S6((("S6"))) & S7((("S7")))
    S2 == 1/3 ==> G((("G")))
    S0 -- 1/3 -->S3["S3"]
    S3 -- 1/2 --> S8((("S8"))) & S9((("S9")))
```

<v-click>

Base cases:
- Information for goal state: $0$
- Information for non-goal terminal states: $\infty$

</v-click>

---
layout: two-cols-title
hideInToc: true
---

# Example: MUSE

::left::

If we set<br>
- $A(s)$ as a function that returns all possible actions at state $s$, <br>
- $\tau_g$ (the solution path) as the sequence of state-action pairs from $s_0$ to $g$ <br>

<v-click>

Then, we have:
$$
\mu(X)=\sum_{s_i\in\tau_g}\log_2|A(s_i)|
$$

</v-click>

::right::

```mermaid
flowchart TD
    style S0 fill:#FF6D00
    style S2 fill:#FFD600
    style G fill:#00C853
    S0(("S0")) -- 1/3 --> S1["S1"]
    S1 -- 1/2 --> S4((("S4"))) & S5((("S5")))
    S0 == 1/3 ==> S2["S2"]
    S2 -- 1/3 --> S6((("S6"))) & S7((("S7")))
    S2 == 1/3 ==> G((("G")))
    S0 -- 1/3 -->S3["S3"]
    S3 -- 1/2 --> S8((("S8"))) & S9((("S9")))
```

---
layout: figure-side
figureUrl: /img/d6.jpg
figureCaption: Standard D6
hideInToc: true
---

# Example

<br>

## What if we want to describe multiple outcomes?

<br>

<v-click>

### $I(\text{the roll is greater than 3})$

<br>

### v.s.

<br>

### $I(\text{the roll is 1})$

</v-click>

---
layout: figure-side
figureUrl: /img/d6.jpg
figureCaption: Standard D6
hideInToc: true
---

# Example

<br>

## We have:

$$
I(x > 3) = \log_2 2 < I(x = 1) = \log_2 6
$$

## Equivalently:

<Transform :scale="1.4" origin="top left">

"The roll is greater than 3" <br/>
is **less informative** than <br />
"The roll is 1"

</Transform>

---
layout: two-cols-title
hideInToc: true
---

# Example: MUSE

::left::

In MUSE, if we have multiple goal states, we choose the one with minimum uniform entropy:
$$
\mu(X)=\min_{g\in G}\sum_{s_i\in\tau_g}\log_2|A(s_i)|
$$

<v-clicks>

- $G$: the set of goal states
- $\tau_g$: the sequence of state-action pairs from $s_0$ to $g$ a.k.a. the solution path
- $A(s_i)$: the function that returns the set of possible actions at state $s_i$

</v-clicks>

::right::

```mermaid
flowchart TD
    style S0 fill:#FF6D00
    style S1 fill:#FFD600
    style S2 fill:#FFD600
    style G0 fill:#00C853
    style G1 fill:#00C853
    S0(("S0")) ==1/3==> S1["S1"]
    S1 --1/2--> S4((("S4")))
    S1 ==1/2==> G0((("G0")))
    S0 ==1/3==> S2["S2"]
    S2 --1/3--> S5((("S5"))) & S6((("S6")))
    S2 ==1/3==> G1((("G1")))
    S0 --1/3--> S3["S3"]
    S3 --1/2--> S7((("S7"))) & S8((("S8")))
```

<br>

<v-click>

$$
\begin{align*}
\mu(X)&=\min(\log_2 2+\log_2 3,\log_2 3+\log_2 3)\\
      &=\log_2 6
\end{align*}
$$

</v-click>

---
layout: default
hideInToc: true
---

# Background: MUSE

Using the properties of logarithms, we can show that:

<v-click>

$$
\begin{align*}
\mu(X) &=\min_{g\in G}\sum_{s_i\in\tau_g}\log_2|A(s_i)| \\
       &=\log_2\min_{g\in G}\prod_{s_i\in\tau_g}|A(s_i)| \\
       &=-\log_2\max_{g\in G}\prod_{(s_i,a_i)\in\tau_g}\pi_\mu(s_i,a_i) \\
       &=-\log_2\max_{g\in G}\pi_\mu(g)
\end{align*}
$$

</v-click>

<v-click>

## Conclusion

MUSE is proportional to the probability of the most likely solution path, <br>
with a uniform probability on all actions

</v-click>

---
layout: default
hideInToc: true
---

# Background: ReMUSE

Relative minimum uniform solution entropy 
(ReMUSE) [[Chen et al., 2023](https://ojs.aaai.org/index.php/AIIDE/article/view/27499/27272)] 
provides a way to aggregate multiple goal states using relative entropy (KL-divergence):

$$
D_\text{KL}(P\ \|\ Q) = \sum_{x\in\mathcal{X}}P(x)\log_2\frac{P(x)}{Q(x)}
$$

KL-divergence measures the amount of information required 
for using the probability distribution $P$ to represent the distribution $Q$.

<v-click>

For ReMUSE, at each state, we compute the relative entropy
between the softmin of successors' relative entropy and the uniform distribution plus the minimum suceessors' entropy.

$$
\rho(s)=D_\text{KL}\left[\underset{s_i\in\sigma(s)}{\text{softmin}}(\rho(s_i))\ \|\ \text{Unif}(|A(s)|)\right]+\min_{s_i\in\sigma(s)}\rho(s_i)
$$

</v-click>

<v-click>

where $\sigma(s)$ is the function that returns the set of successors of state $s$

</v-click>

<v-click>

For direct comparison, the recursive definition of MUSE is:
$\mu(s)=\log_2|A(s)|+\underset{s_i\in\sigma(s)}{\min}\mu(s_i)$

</v-click>

---
layout: two-cols-title
hideInToc: true
---

# Example: ReMUSE

::left::

<v-click>

$$
\begin{align*}
\rho(s_0)&=\min_{s_i\in\sigma(s_0)}\rho(s_i)+D_\text{KL}\left[\underset{s_i\in\sigma(s_0)}{\text{softmin}}(\rho(s_i))\ \|\ \text{Unif}(|A(s_0)|)\right]\\
         &=-\log_2 \frac{1}{2}+\frac{1}{2}\log_2\frac{\frac{1}{2}}{\frac{1}{3}}+\frac{1}{2}\log_2\frac{\frac{1}{2}}{\frac{1}{3}}\\
         &=\log_2 3
\end{align*}
$$

</v-click>

<v-click>

Note:<br>
If $\rho(s_1)\neq \rho(s_2)$, then softmin scales the probabilities, as
$$
\frac{e^{-\rho(s_1)}}{e^{-\rho(s_1)}+e^{-\rho(s_2)}}=\frac{e^{\log_2 P(g_0)}}{e^{\log_2 P(g_0)}+e^{\log_2 P(g_1)}}=\frac{P(g_0)^{\log_2 e}}{P(g_0)^{\log_2 e}+P(g_1)^{\log_2 e}}
$$

</v-click>

::right::

<div class="h-14"></div>

```mermaid
flowchart TD
    style S0 fill:#FF6D00
    style S1 fill:#FFD600
    style S3 fill:#FFD600
    style G0 fill:#00C853
    style G1 fill:#00C853
    S0(("S0")) ==1/3==> S1["S1"]
    S1 --1/2--> S4((("S4")))
    S1 ==1/2==> G0((("G0")))
    S0 --1/3--> S2["S2"]
    S2 -->|1/3| S5((("S5"))) & S6((("S6"))) & S7((("S7")))
    S0 ==1/3==> S3["S3"]
    S3 ==1/2==> G1((("G1")))
    S3 --1/2--> S8((("S8")))
```
