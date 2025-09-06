## AI-course-project

This repository contains foundational implementations from two main areas explored in our course:

### 1. Heuristic Search in Classical Planning
- The folder`src/downward/heuristics` includes several classical heuristic algorithms used in automated planning, such as Fast Forward (FF), Additive, and Landmark heuristics.
- These algorithms estimate goal distance and improve search efficiency by leveraging planning graphs, critical subgoals, and domain knowledge.


### 2. Reinforcement Learning (Q-Learning)
- The `src/q_learning` folder covers Q-learning algorithms, including:
  - Epsilon-greedy and random policies
  - Experience sampling (on-policy/off-policy buffer)
  - State encoding and simulation strategies
  - Q-value function approximation with tables and neural networks (Torch), covering both classical and deep Q-learning.
- The folder includes code and theory on Q-learning mechanisms, policy selection, experience handling, and neural network integration for value approximation.
