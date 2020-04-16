## [3] C-Straightforward
- Algorithm2 NNDescentBasic implemented
-- Full 2nd step
- Unittests
- Evaluate Unittest framework
-- each component / datastructyure (e.g. heap extract max, heaplist reverse)
- Input (read from stdin), output result (KNN) to stdout [or file]
-- including K, number of iterations as parameters

## [4] Validation
- Trivial O(n^2) solution
- Modify "official" implementation to our input/output method
- Comparison of results
-- use recall (like paper) [for recall compare with trivial]
-- compare to recall of "official implementation"
- Simple parametrizable synthetic datasets
- datasets from paper (Coral etc.)

## [5] Timing
- Need: synthetic dataset, on which runtime has low variance and scales with input size
- Concept for for measuring T(n) multiple times (and taking median? smth like that)
- Infrastructure
-- measurement method from hw [spinning up, cycle counter etc] (cycle counter may not work anymore if runtime very long?)
-- put data in memory [synthic dataset]
-- call function
- maybe: think about benchmarking smaller parts of the code [critical parts]
- [8]: use infrastructure to benchmark reference C implementation

