#!/usr/bin/env python3
import subprocess, sys, random
from pathlib import Path
from collections import Counter

EXE = str(Path(r"D:\Skibidi Rizzlers\rbtreenil\out\build\x64-Debug\rbtreenil.exe"))

import re
TOK = re.compile(r'(-?\d+)([RB])')

def run_trial(ins, dele):
    inp = f"{len(ins)}\n" + " ".join(map(str, ins)) + "\n"
    inp += f"{len(dele)}\n" + " ".join(map(str, dele)) + "\n"
    try:
        r = subprocess.run([EXE], input=inp, capture_output=True, text=True, timeout=15)
    except subprocess.TimeoutExpired:
        return ["TIMEOUT (possible infinite recursion)"]
    out = r.stdout
    issues = []
    if r.returncode != 0:
        issues.append(f"nonzero exit={r.returncode} (crash/abort)")
    if "rbtree invalid" in out:
        issues.append("delete rbtree invalid (parent-link / red-red violation)")
    if "black height invalid" in out:
        issues.append("delete blackheight invalid (uneven black paths)")

    vals = [int(m[0]) for m in TOK.findall(out)]
    if len(vals) < len(ins):
        issues.append(f"truncated output: saw {len(vals)} inorder tokens, need >= {len(ins)}")
    else:
        ins_phase = vals[:len(ins)]
        if ins_phase != sorted(ins):
            issues.append(f"insert inorder wrong: got {ins_phase} want {sorted(ins)}")
        c = Counter(ins)
        removed = 0
        for v in dele:
            if c[v] > 0:
                c[v] -= 1
                removed += 1
        k = len(ins) - removed
        exp_del = sorted(c.elements())
        del_phase = vals[len(ins):len(ins)+k]
        if len(vals) - len(ins) < k:
            issues.append(f"missing delete inorder tokens: got {len(vals)-len(ins)} want {k}")
        elif del_phase != exp_del:
            issues.append(f"delete inorder wrong: got {del_phase} want {exp_del}")
    return issues

def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 42
    trials = int(sys.argv[2]) if len(sys.argv) > 2 else 400
    random.seed(seed)
    fails = 0
    # targeted: force leaf deletions (debt==NIL) to probe the pool-erase path
    targeted = [
        ([10,20,30,40,50],[20]),          # internal, two-children case
        ([10,20,30,40,50],[10]),          # leaf delete (debt==NIL)
        ([5,3,7,1,4,6,8],[1]),            # leaf
        ([1], [1]),                        # single node
        ([10,5,15,3,7,13,17],[5]),        # two-children with children
        ([10,20,30],[10,20,30]),          # delete all
        ([2,1,3,4],[2]),                   # two-children (root)
    ]
    seqs = []
    for ins, dele in targeted:
        seqs.append((ins, dele))
    for _ in range(trials):
        n = random.randint(1, 25)
        ins = [random.randint(0, 40) for _ in range(n)]
        m = random.randint(1, max(1, n))
        dele = [random.choice(ins) for _ in range(m)]
        seqs.append((ins, dele))
    # also deletions of values possibly NOT present
    for _ in range(trials//2):
        n = random.randint(1, 20)
        ins = [random.randint(0, 30) for _ in range(n)]
        dele = [random.randint(0, 40) for _ in range(random.randint(1, n))]
        seqs.append((ins, dele))
    for ins, dele in seqs:
        issues = run_trial(ins, dele)
        if issues:
            fails += 1
            print(f"FAIL ins={ins} del={dele}")
            for it in issues: print("   ", it)
            if fails >= 25:
                print("... (stopping after 25 failures)"); break
    total = len(seqs)
    print(f"\n=== {total-fails}/{total} trials OK, {fails} FAILED ===")
    return 1 if fails else 0

if __name__ == "__main__":
    sys.exit(main())
