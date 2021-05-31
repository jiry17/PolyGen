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

### Run experiments 

```$bash
$ cd build
# Run Polygen 
$ ./run INPUT OUTPUT DOMAIN
```

DOMAIN is `cegis` for oracle model $O_V$ and `random` for oracle model $O_R$.

Some examples are listed below:

```$bash
$ cd build
# Run original Polygen with cegis
$ ./run max5.sl ans.txt cegis
```
