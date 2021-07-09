# Polygen

### Install 

#### Build from source (Test on Ubuntu 18.04) 

1. Install dependencies

```bash
$ apt-get install cmake libgoogle-glog-dev python3-pip libboost-all-dev libjsoncpp-dev
$ pip3 install pyparsing z3-solver matplotlib 
```

1. Clone Polygen 

```bash
$ git clone https://github.com/jiry17/PolyGen
```

2. Build the whole project. Under the root directory of the project:

```bash
$ ./install
```

#### Run tests

1. Test whether *Euphony* is successfully installed:

   ```bash
   $ cd recommend/euphony
   $ . bin/setenv  
   $ ./bin/run_string benchmarks/string/train/firstname.sl
   ```

   The expected output is a program which splits an input string by a space and returns the first substring:

   ```
   (define-fun f ((name String)) String (str.substr name 0 (str.indexof name " " 0)))
   ```

2. Test whether *Eusolver* is successfully installed:

   ```bash
   $ cd recommend/eusolver
   $ ./eusolver benchmarks/max/max_2.sl
   ```

   The expected output is a program which takes two integers as the input and returns the larger one:

   ```
   (define-fun max2 ((a0 Int) (a1 Int)) Int
        (ite (<= a1 a0) a0 a1))
   ```

3. Test whether the project is successfully built:

   ```bash
   $ cd build
   $ ./run ../benchmark/CLIA_benchmark/sum.sl result.sl cegis
   ```

   The expected output is

   ```
   1
   (+ Param0 Param1)
   ```
   
    Number 1 indicates the synthesize process takes 1 example in total.

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
$ ./run max5.sl ans.txt cegis
```

#### Run experiments 

```bash
$ cd run
$ ./run_exp  [-exp {1,2}]  [-r {R <Restart>,C <Clear>}]
# For example, to reproduce all results:
$ ./run_exp -c R
```

1. `-exp`: the id of the experiment you want to run. All experiments will be executed by default.

2. `-c`: whether to clear the cache: `R` represents yes while `C` represents no, and the default value is `C`. 

Some parameters can be set in config.py

1. `KMemoryLimit`: the memory limit. The default value is 8 GB
2. `KTimeLimit`: the time limit. The default value is 120 seconds.
3. `KIntMin `: lower bound of the input. The default value is -50.
4. `KIntMax `: upper bound of the input. The default value is 50.
5. `KExampleLimit `: The limit of examples involves in the synthesizing process. The default value is 10000.
6. `KRepeatNum `: the number of repetitions of each execution. The default value is 5. Note that all the algorithms are random, the smaller this value is, the more volatile the result will be.

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
