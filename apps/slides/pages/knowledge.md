---
title: Player Modeling (Inference Rules)
level: 1
---
<!--suppress HtmlUnknownTag, HtmlUnknownTarget -->

# Player Modeling

<br>

$$
\psi(X)=-\log_2\sum_{g\in G}\pi(g)
$$

<v-click>
<Transform :scale="1.1" origin="top left">

The same puzzle may reflect different difficulties for different players.

</Transform>
</v-click>
<v-click>
<Transform :scale="1.1" origin="top left">

Therefore, we also need to model player's knowledge.

</Transform>
</v-click>

<br>

<v-clicks>

- Inference rules: determine whether a specific action should be taken at a given state
- Machine learning model: outputs a probability distribution over possible actions at a given state

</v-clicks>

---
layout: default
hideInToc: true
---

# Inference Rules

<br>
<Transform :scale="1.5" origin="top left">
<v-clicks>

- help players simplify or solve the puzzle
- are functions that determine whether <br> a specific action should be taken at a given state
- can be directly incorporated into policies

</v-clicks>
</Transform>

---
layout: default
hideInToc: true
---

# Inference Rules

<br>
<Transform :scale="1.3" origin="top left">

In _The Witness_, the actions can be classified as:

<v-clicks>

- **UNKNOWN**: cannot be determined
- **CANNOT\_TAKE**: directly violate the puzzle constraints or <br> lead to situations where constraints cannot be satisfied later
- **MUST\_TAKE**: necessary actions to satisfy constraints

</v-clicks>
</Transform>

---
layout: default
hideInToc: true
---

# Incorporating Inference Rules with Policies

````md magic-move
```python
def uniform_policy(action, state):
    return 1 / get_possible_actions(state)
```
```python
def uniform_policy(action, state):
    actions = get_possible_actions(state)
    m = {a: UNKNOWN for a in actions}
    return 1 / get_possible_actions(state)
```
```python
def uniform_policy(action, state):
    actions = get_possible_actions(state)
    m = {a: UNKNOWN for a in actions}
    for rule in inference_rules:
        result = rule(action, state)
        if result == CANNOT_TAKE:
            return 0
        else if result == MUST_TAKE:
            return 1
    return 1 / get_possible_actions(state)
```
```python
def uniform_policy(action, state):
    actions = get_possible_actions(state)
    m = {a: UNKNOWN for a in actions}
    for rule in inference_rules:
        result = rule(action, state)
        if result == CANNOT_TAKE:
            return 0
        else if result == MUST_TAKE:
            return 1
        for a in actions:
            if m[a] != UNKNOWN and m[a] != result:
                return 0  # conflicts between rules
            m[a] = result
    return 1 / get_possible_actions(state)
```
```python
def uniform_policy(action, state):
    actions = get_possible_actions(state)
    m = {a: UNKNOWN for a in actions}
    for rule in inference_rules:
        result = rule(action, state)
        if result == CANNOT_TAKE:
            return 0
        else if result == MUST_TAKE:
            return 1
        for a in actions:
            if m[a] != UNKNOWN and m[a] != result:
                return 0  # conflicts between rules
            m[a] = result
    filtered_actions = []
    for a in actions:
        if m[a] == MUST_TAKE and a != action:
            return 0  # another action is MUST_TAKE
        else if m[a] == CANNOT_TAKE:
            continue
        filtered_actions.append(a)
    return 1 / filtered_actions
```
````

---
layout: default
hideInToc: true
---

# Existing Inference Rules [[Chen et al.](https://ojs.aaai.org/index.php/AIIDE/article/view/27499/27272)]

<img src="/img/existing.png" alt="existing" class="w-6/10 mx-auto">

---
layout: two-cols-center
hideInToc: true
---

# Along-the-path Rule

<div class="w-90/100 mx-auto my-8">

Region constraints adjacent to the same side of the path should not be violated

</div>

::left::

<v-click>

<img src="/img/aps.png" alt="aps" class="w-3/4">

</v-click>

::right::

<v-click>

<img src="/img/rapf.png" alt="rapf" class="w-3/4">

</v-click>

---
layout: two-cols-center
hideInToc: true
---

# Region-completion Rule

<div class="w-90/100 mx-auto my-8">

Every constraint within a closed region must be satisfied; <br>
otherwise, these constraints cannot be satisfied later

</div>

::left::

<v-click>

<img src="/img/2crs.png" alt="2crs" class="w-3/4">

</v-click>

::right::

<v-click>

<img src="/img/1crf.png" alt="1crf" class="w-3/4">

</v-click>
