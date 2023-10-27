from typing import List, Dict, Set
from queue import Queue


Adj = Set[str]


class Graph:
    """
    Directed, 
    adj list representation
    """
    def __init__(self) -> None:
        self.nodes: Dict[str, Adj] = {}

    def add_node(self, n: str):
        if n not in self.nodes:
            self.nodes[n] = set()
        else:
            raise ValueError("Node already exists")

    def add_edge(self, n1: str, n2: str):
        if n1 in self.nodes and n2 in self.nodes:
            self.nodes[n1].add(n2)
        else:
            raise ValueError("Node does not exist")

    def bfs(self, src: str) -> List[List[str]]:
        """
        Returns the nodes, grouped by level
        """

        if src not in self.nodes:
            raise ValueError("Source node does not exist")

        visited: Set[str] = set()
        q: Queue = Queue()

        q.put(src)
        visited.add(src)

        levels: List[List[str]] = []
        nodes_curr_lvl: int = 1
        nodes_next_lvl: int = 0
        curr_level: List[str] = []

        while not q.empty():
            c_node: str = q.get()
            curr_level.append(c_node)
            nodes_curr_lvl -= 1

            for elem in self.nodes[c_node]:
                if elem not in visited:
                    q.put(elem)
                    visited.add(elem)
                    nodes_next_lvl += 1

            if not nodes_curr_lvl:
                nodes_curr_lvl = nodes_next_lvl
                nodes_next_lvl = 0
                levels.append(curr_level)
                curr_level = []

        return levels


    def dfs(self, src: str) -> List[str]:
        res: List[str] = []
        visited: Set[str] = set()

        self.dfs_rec(src, visited, res)

        return res

    def dfs_rec(self,
                src: str,
                visited: Set[str],
                res: List[str]) -> None:

        visited.add(src)

        for adj in self.nodes[src]:
            if adj not in visited:
                self.dfs_rec(adj, visited, res)

        res.append(src)


if __name__ == "__main__":
    g: Graph = Graph()

    for letter in "abcdef":
        g.add_node(letter)

    g.add_edge("a", "b")
    g.add_edge("a", "c")
    g.add_edge("b", "d")
    g.add_edge("b", "e")
    g.add_edge("c", "f")
    g.add_edge("b", "f")
    g.add_edge("f", "a")
    g.add_edge("c", "b")

    print(g.bfs("a"))

    print(g.dfs('a'))
