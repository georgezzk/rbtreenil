#!/usr/bin/env python3
import subprocess
import sys
import random
import itertools
from pathlib import Path

BIN = Path(r"D:\Skibidi Rizzlers\rbtreenil\out\build\x64-Debug\rbtreenil.exe")

def run_one(seq):
    inp = f"{len(seq)}\n" + " ".join(map(str, seq)) + "\n"
    try:
        r = subprocess.run(
            [str(BIN)],
            input=inp,
            capture_output=True,
            text=True,
            timeout=5,
        )
        out = r.stdout
    except subprocess.TimeoutExpired:
        return "TIMEOUT", None
    except Exception as e:
        return f"CRASH: {e}", None

    fail_parts = []
    if "rbtree invalid" in out:
        fail_parts.append("red-black property violated")
    if "black height invalid" in out:
        fail_parts.append("black-height violated")

    if fail_parts:
        tree_line = out.strip().splitlines()[-1]
        return ", ".join(fail_parts), tree_line
    return None, None

def run_batch(label, sequences):
    passed = failed = 0
    for seq in sequences:
        err, tree = run_one(seq)
        if err:
            print(f"FAIL [{label}] seq={seq}  {err}")
            if tree:
                print(f"  tree: {tree}")
            failed += 1
        else:
            passed += 1
    return passed, failed

def main():
    random.seed(42)
    total_pass = total_fail = 0

    print("=== 3-element permutations ===")
    seqs = list(itertools.permutations([1, 2, 3]))
    seqs = [list(s) for s in seqs]
    p, f = run_batch("perm3", seqs)
    total_pass += p; total_fail += f
    print(f"  passed={p} failed={f}\n")

    print("=== 5-element permutations ===")
    seqs = list(itertools.permutations([1, 2, 3, 4, 5]))
    seqs = [list(s) for s in seqs]
    p, f = run_batch("perm5", seqs)
    total_pass += p; total_fail += f
    print(f"  passed={p} failed={f}\n")

    print("=== Random sequences (1000 trials, n=3..50) ===")
    seqs = []
    for _ in range(1000):
        n = random.randint(3, 50)
        seq = [random.randint(0, 200) for _ in range(n)]
        seqs.append(seq)
    p, f = run_batch("rand", seqs)
    total_pass += p; total_fail += f
    print(f"  passed={p} failed={f}\n")

    print("=== Summary ===")
    print(f"Total passed: {total_pass}")
    print(f"Total failed: {total_fail}")
    return 1 if total_fail > 0 else 0

if __name__ == "__main__":
    sys.exit(main())
