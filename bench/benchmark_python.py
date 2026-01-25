import time
import sys

DATA_SIZE = 10_000_000

base = "A" * DATA_SIZE
base = "".join(" " if i % 10 == 0 else c for i, c in enumerate(base))

csv = list(base)
for i in range(0, DATA_SIZE, 15):
    csv[i] = ","
csv = "".join(csv)

# ---------------- Timing ----------------
def timed_median(func, runs=30):
    times = []
    for _ in range(runs):
        start = time.perf_counter()
        r = func()
        end = time.perf_counter()

        # force materialization
        if isinstance(r, str):
            _ = len(r)
        elif isinstance(r, list):
            _ = sum(len(x) for x in r)

        times.append((end - start) * 1000)
    times.sort()
    return times[len(times) // 2]

# ---------------- Benchmarks ----------------
results = {}

results["splitChar"] = timed_median(lambda: csv.split(","))
results["splitWS"]   = timed_median(lambda: base.split())
results["join"]      = timed_median(lambda: "-".join(base.split()))
results["replace"]   = timed_median(lambda: base.replace("A", "B"))
results["lower"]     = timed_median(lambda: base.lower())
results["upper"]     = timed_median(lambda: base.upper())
results["strip"]     = timed_median(lambda: base.strip())
results["count"]     = timed_median(lambda: base.count("A"))
results["contains"]  = timed_median(lambda: "XYZ" in base)
results["repeat"]    = timed_median(lambda: "abc" * 100_000)

# ---------------- Output ----------------
for k, v in results.items():
    print(f"{k}:{v:.3f}")