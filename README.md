# Polygen

The artifact for OOPSLA 21 submission #542 "Generalizable Synthesis Through Unification".

### Install 

#### Build from source (Test on Ubuntu 18.04) 

1. Install dependencies

```bash
$ apt-get install cmake libgoogle-glog-dev python3-pip libboost-all-dev libjsoncpp-dev libboost-all-dev libgoogle-perftools-dev
$ pip3 install pyparsing z3-solver matplotlib 
```
2. Install *gurobi* from https://www.gurobi.com/
3. Clone *Polygen* 

```bash
$ git clone --recursive https://github.com/jiry17/PolyGen
```

4. Build the whole project under the root directory of the project. The installation requires the path of *gurobi*. 

```bash
$ ./install
```

#### Run tests

1. Test whether *Eusolver* is successfully installed :

   ```bash
   $ cd recommend/my-euphony
   $ . bin/setenv  
   $ ./bin/run_int ../../benchmark/cross ../../benchmark/CLIA_benchmark/max3.sl
   ```

   The expected output is a program that returns the maximum of three:

   ```
   (define-fun max3 ((x Int) (y Int) (z Int)) Int (ite (<= z x) (ite (<= y x) x y) (ite (<= z y) y z)))
   ```

2. Test whether *Eusolver* is successfully installed :


   ```bash
   $ cd recommend/my-euphony
   $ . bin/setenv  
   $ ./bin/run_int_eusolver ../../benchmark/CLIA_benchmark/max3.sl
   ```
   The  expected output is the same as 1


3.   Test whether *Esolver* is successfully installed :

   ```bash
   $ cd recommend/esolver
   $ ./eusolver benchmarks/max/max_2.sl
   ```
   The expected output is a program that takes two integers as the input and returns the larger one:

   ```
   (define-fun max2 ((a0 Int) (a1 Int)) Int
        (ite (<= a1 a0) a0 a1))
   ```

4. Test whether the project is successfully built:

   ```bash
   $ cd build
   $ ./run ../benchmark/CLIA_benchmark/sum.sl result.sl cegis
   ```

   The expected output is

   ```
   1
   (+ Param0 Param1)
   ```

    Number 1 indicates the synthesis process takes 1 example in total.

### Run synthesizers

#### Run synthesizers on a single SyGuS file 

```$bash
$ cd build
# Run Polygen 
$ ./run INPUT OUTPUT DOMAIN
# Run Polygen for ablation test
$ ./run INPUT OUTPUT DOMAIN ABLATION
```

1. DOMAIN is `cegis` for oracle model $O_V$ and `random` for oracle model $O_R$.
2. ABLATION is 1 for $ \text{ PolyGen }_{-T}$  and 2 for $\text { PolyGen }_{-U}$

Some examples are listed below:

```$bash
$ cd build
# Run original Polygen with cegis
$ ./run max5.sl ans.sl cegis
```

#### Run experiments 

```bash
$ cd run
$ ./run_exp  [-exp {1,2}]  [-r {R <Restart>,C <Clear>}] [-s {0,1}]
# For example, to reproduce all results:
$ ./run_exp -c R
```

1. `-exp`: the id of the experiment you want to run. All experiments will be executed by default.
2. `-c`: whether to clear the cache: `R` represents yes while `C` represents no, and the default value is `C`. 
3. `-s`: whether to skip random test when the same benchmark fails in cegis test:  `1` represents yes while  `0`  represents no. The default value is `1`. 

Some parameters can be set in config.py

1. `KMemoryLimit`: the memory limit. The default value is 8 GB
2. `KTimeLimit`: the time limit. The default value is 120 seconds.
3. `KIntMin `: lower bound of the input. The default value is -50.
4. `KIntMax `: upper bound of the input. The default value is 50.
5. `KExampleLimit `: The limit of examples involves in the synthesizing process. The default value is 10000.
6. `KRepeatNum `: the number of repetitions of each execution. The default value is 1 for efficiency. Note that all the algorithms are random, the smaller this value is, the more volatile the result will be. Our experiment set this value as 5.

The result of each single execution is cached in `exp/result_cache` . 

The figure of each experiment will be stored in `exp/figure`.

These results are expected to be consistent with results presented in `run`.

### Reproduce results in the paper 

#### Reproduce results of experiment 1

```bash
$ cd run
$ ./run_exp -exp 1
```

For (a) ~ (d) of Figure 2, the script will redraw them respectively.

For data listed in Table 2, `run_exp` will recalculate them and print them to the standard output.

#### Reproduce results of experiment 2

````bash
$ cd run
$ ./run_exp -exp 2
````

For (e) ~ (h) of Figure 2, the script will redraw them respectively.

For data listed in Table 3, `run_exp` will recalculate them and print them to the standard output.

**Note**: There may be some small differences between the results listed in our paper and the reproduced ones because there exists randomness.