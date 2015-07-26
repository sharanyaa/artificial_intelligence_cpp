1. I have used the C++ STL list for all 3 search strategies.
2. As per Prof. Itti's suggestion, I have not pushed the nodes in reverse alphabetical order and instead pop the nodes alphabetically if they have the same path cost.
3. Input verification:
- File not found.
- If source and destination node are the same, "no path found" is wriiten into the file.
- Validity of search choice 1: BFS, 2: DFS or 3: UCS
- If initial state or goal state is not found in state space, error message is written into the file.
- The number of states in the input is checked against the actual number of states listed in the file.