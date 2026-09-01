#!/usr/bin/env python3
import sys
from stress_del import run_trial

def failing(ins, dele):
    if not ins:  # need at least the deletes to find a tree
        return False
    return bool(run_trial(ins, dele))

def minimize(ins, dele):
    # greedy: try dropping each insert (skip if it makes a needed node absent trivially)
    changed = True
    while changed:
        changed = False
        for i in range(len(ins)):
            cand = ins[:i] + ins[i+1:]
            if failing(cand, dele):
                ins = cand; changed = True; break
        if changed: continue
        for i in range(len(dele)):
            cand = dele[:i] + dele[i+1:]
            if failing(ins, cand):
                dele = cand; changed = True; break
    return ins, dele

if __name__ == "__main__":
    seed_cases = [
        ([36, 27, 36, 25, 40, 26, 18, 7, 25, 1, 20], [1, 7, 26]),
        ([7, 29, 7, 9, 31, 18, 32, 17], [17, 17, 9, 17, 7, 32, 9]),
        ([1, 11, 16, 3, 8, 26, 33, 7, 4, 30, 28], [4, 30, 11, 7, 4, 3]),
    ]
    for ins, dele in seed_cases:
        if not failing(ins, dele):
            print(f"(not failing) {ins} {dele}"); continue
        mi, md = minimize(list(ins), list(dele))
        print(f"MIN ins={mi} del={md}")
        print("   detail:", run_trial(mi, md))
