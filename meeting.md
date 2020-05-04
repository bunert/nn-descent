# Week 6
## Project meeting
- Find k that it becomes compute bound
- Is the dimension dependent on the performance?
- Try out dataset, where the neighbouring datapoints are somehow in order (with Gaussian noise) -> Prior on data distribution
- Use brute force algorithm to improve performance
- Project presentation
   - 10 Min in total
   - 2 min introduction (2 slides)
   - Say, what is the main bottleneck
   - Discuss optimization techniques that are not straight forward
   - 3-4 slides: Baseline, Roofline plot, Plots
   - Backup slides
   - Present both approaches (Random/Brute force), focus on things, that are novel and not straight forward
   - Explain, why we get that speedup or why we don't
   - 
# Week 5
## Discussion
- Use collaborative tools for slides/latex
- Meeting questions:
   - Do we need to enlarge our project? e.g optimize Brute Force KNN or extend our existing algorithm (UMap)
   - What does count to flops because we don't have comparisons accounted
- Random number generator: a bit shady, but recall and performance is the same (1 mult and 1 modulo)
- Bottleneck: Memory
- Random number generator: We only have a 32 bit integer --> limited in the number of points
- Possible improvements: Vectorize Heap, blocking and sampling

## Tasks
- Sam: Create slides for project meeting
- Tobi: Update Project Systems
- Dan: Benchmark our starting point (nn_descent) implementation and MNIST Dataset
- Jonas: Max heapify eliminate recursion

# Week 3
## Discussion
- Git Tag for different versions
- Keep API clean when trying to increase performance
- What is the cost measure: Flops, similarity?
- Solve problem when using too many points (malloc)
- New main file, which executes two things: performance, runtime
- Do we need tests or just referring to recall? 

## Tasks
- Create presentation (5 slides max.) (3th/4th of May)
- Use faster random number generator (replace crand)
- Change structs
- Add const to integer

- Dan: GCC Profiler
- Samuel: Add some improvements (int to const, scalar replacment, make struct better)
- Tobias: Blocking/Random number generator (comment which parts are blocked)
- Jonas: small changes to improvment and measure it at the end (Standard C optimization) (scalar replacment, int to const)

# Week 2
## Review
## Discussion
- Cost analysis
    - cost measure? [distance metric, comparisons?]
    - runtime depends on input data, how to determine cost? [Instrumentation of code, ]

- Performance Plot dataset
    - need dataset which scales well in terms of cost [in whatever measure we are going to define] and runtime for plot 
    - could use generative model to scale dataset

- Validation
    - script to automate test(s)
    - end to end recall test 
    - run all unit tests
    - define correctness (same as in original paper, recall)

## Tasks
- Jonas: see how syntethic dataset scales...
- Sam: Comment code, explore optimization possibilities
- Dan: Validation, instrumentation/profiling of code
- Tobi: Cost analysis

# Week 1
## Review
## Discussion
### [3] C-Straightforward
- Algorithm2 NNDescentBasic implemented
-- Full 2nd step
- Unittests
- Evaluate Unittest framework
-- each component / datastructyure (e.g. heap extract max, heaplist reverse)
- Input (read from stdin), output result (KNN) to stdout [or file]
-- including K, number of iterations as parameters

### [4] Validation
- Trivial O(n^2) solution
- Modify "official" implementation to our input/output method
- Comparison of results
-- use recall (like paper) [for recall compare with trivial]
-- compare to recall of "official implementation"
- Simple parametrizable synthetic datasets
- datasets from paper (Coral etc.)

### [5] Timing
- Need: synthetic dataset, on which runtime has low variance and scales with input size
- Concept for for measuring T(n) multiple times (and taking median? smth like that)
- Infrastructure
-- measurement method from hw [spinning up, cycle counter etc] (cycle counter may not work anymore if runtime very long?)
-- put data in memory [synthic dataset]
-- call function
- maybe: think about benchmarking smaller parts of the code [critical parts]
- [8]: use infrastructure to benchmark reference C implementation
## Tasks
Tobi, Jonas, Dan: C reference implementation to get to know the algorithm
Tobi: Timing
Dan: Validation
