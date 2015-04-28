# ysda-search-engine [![Build Status](https://travis-ci.org/yazevnul/ysda-search-engine.svg?branch=master)](https://travis-ci.org/yazevnul/ysda-search-engine)
Search engine we build during practice lessons on "Information Retrieval" at Yandex School of Data Analysis

## Requirements
* Clang 3.6+ or GCC-4.9+
* Google Protocol Buffers 2.6+
* CMake

Compilation on Ubuntu with Clang is a bit tricky because of [g3log][g3log] dependency. You will
either need to install libc++ and libc++abi packages (which are not available on Ubuntu Precise, but
you can steel build them yourself, see instruction [here][libcxx] and [here][libcxx-hacking]), or
you may try to patch [g3log][g3log] build scripts the way it is done in
[travis-ci config][self-travis-ci-config]. If you are going to patch [g3log][g3log] build scripts,
I'd like to warn you, that despite the fact that it will compile without errors, I haven't launch
any binary after applying this patch.

## Crawl [simple.wikipedia.org](http://simple.wikipedia.org)
```bash
cmake -DCMAKE_BUILD_TYPE=Release . && make -j
cd src/tools/simple_crawler
./prepare_for_crawling.sh
./simple_crawler new --config default_config.h
```

## Usage

Build tools
```bash
cmake -DCMAKE_BUILD_TYPE=Release . && make -j
```

Prepare data
```bash
cd src/tools/dictionary_parser
./dictionary_parser -i dictionary.txt -o dictionary.txt.bin

cd src/tools/index_parser
./index_parser -i index.txt -o index.txt.bin -s index.txt.statistics.bin
```

Ranker interactive session
```bash
./ranker dictionary.txt.bin index.txt.bin. index.txt.statistics.bin
$ ./ranker -d dictionary.txt.bin -i index.txt.bin -s index.txt.statistics.bin
Loading data...
Loading data...DONE in 4.75734
Dictionary size is 2711920, index size is 2711920, number of documents is 26351, average document length is 1378.05
hello there
Splitted query: [ hello, there ]
Bag of words: [ (63594, 1) ]
Documents (fuzzy boolean search) [90]:[ 27933, 26900, 26071, 24493, 23507, 22607, 21851, 23252, 21312, 20790, 20735, 21293, 20129, 27667, 18840, 18451, 17002, 16408, 26010, 25428, 15922, 23987, 15354, 14495, 14204, 13260, 27629, 18414, 3670, 8229, 13079, 11447, 11136, 16832, 10624, 2045, 9611, 24746, 9517, 21180, 9055, 8676, 26758, 16476, 471, 2217, 11335, 8425, 7646, 16090, 15217, 8214, 27441, 20360, 7556, 7471, 25581, 12971, 7237, 27718, 4729, 4234, 17343, 12784, 1241, 27480, 7304, 3384, 26021, 2741, 2249, 1883, 2562, 8510, 4630, 10783, 15847, 2558, 16857, 4344, 2811, 17718, 743, 11740, 16077, 460, 789, 21696, 744, 3633 ]
Documents (exact boolean search) [90]:[ 27933, 26900, 26071, 24493, 23507, 22607, 21851, 23252, 21312, 20790, 20735, 21293, 20129, 27667, 18840, 18451, 17002, 16408, 26010, 25428, 15922, 23987, 15354, 14495, 14204, 13260, 27629, 18414, 3670, 8229, 13079, 11447, 11136, 16832, 10624, 2045, 9611, 24746, 9517, 21180, 9055, 8676, 26758, 16476, 471, 2217, 11335, 8425, 7646, 16090, 15217, 8214, 27441, 20360, 7556, 7471, 25581, 12971, 7237, 27718, 4729, 4234, 17343, 12784, 1241, 27480, 7304, 3384, 26021, 2741, 2249, 1883, 2562, 8510, 4630, 10783, 15847, 2558, 16857, 4344, 2811, 17718, 743, 11740, 16077, 460, 789, 21696, 744, 3633 ]
Documents BM25(fuzzy boolean search) [90]:[ (23987, 10.2706), (7471, 9.98626), (15217, 9.97647), (27718, 9.76666), (12971, 9.17373), (4234, 8.7368), (3633, 8.54207), (27629, 8.51916), (13260, 8.50772), (27667, 8.34157), (789, 8.24389), (471, 8.23318), (15354, 8.18355), (27933, 7.76477), (18840, 7.54965), (8425, 7.49463), (15847, 7.44473), (1883, 7.24192), (18414, 7.03939), (16077, 6.80025), (10783, 6.70668), (22607, 6.65502), (2558, 6.64106), (23252, 6.63965), (4729, 6.55405), (1241, 6.45931), (21696, 6.43424), (8510, 6.39097), (20129, 6.36742), (16832, 6.26665), (13079, 6.25833), (7304, 6.1871), (20735, 6.12924), (25428, 6.04919), (15922, 6.02625), (4630, 6.0091), (17718, 5.99766), (18451, 5.97886), (11136, 5.95637), (12784, 5.86992), (2741, 5.83031), (21312, 5.81248), (17002, 5.8036), (26071, 5.74131), (26900, 5.69749), (24746, 5.68046), (21180, 5.66185), (23507, 5.56237), (16408, 5.53328), (7646, 5.52203), (3670, 5.50923), (26021, 5.4586), (20360, 5.45185), (2249, 5.4166), (27480, 5.38743), (10624, 5.36462), (2217, 5.342), (24493, 5.31066), (9517, 5.24182), (17343, 5.21374), (9611, 5.13699), (11740, 5.02429), (20790, 4.91769), (16090, 4.76006), (11447, 4.76006), (2562, 4.76006), (8676, 4.74816), (27441, 4.73633), (21851, 4.69422), (9055, 4.64036), (16476, 4.57117), (26758, 4.56897), (2045, 4.53085), (7237, 4.46696), (4344, 4.07525), (16857, 3.88768), (460, 3.88373), (2811, 3.8228), (25581, 3.39974), (8214, 3.05236), (26010, 2.57286), (7556, 2.1943), (21293, 2.09468), (14495, 1.85662), (743, 1.80693), (3384, 1.65136), (744, 1.30642), (11335, 0.905472), (14204, 0.882991), (8229, 0.841104) ]
Documents BM25(exact boolean search) [90]:[ (23987, 10.2706), (7471, 9.98626), (15217, 9.97647), (27718, 9.76666), (12971, 9.17373), (4234, 8.7368), (3633, 8.54207), (27629, 8.51916), (13260, 8.50772), (27667, 8.34157), (789, 8.24389), (471, 8.23318), (15354, 8.18355), (27933, 7.76477), (18840, 7.54965), (8425, 7.49463), (15847, 7.44473), (1883, 7.24192), (18414, 7.03939), (16077, 6.80025), (10783, 6.70668), (22607, 6.65502), (2558, 6.64106), (23252, 6.63965), (4729, 6.55405), (1241, 6.45931), (21696, 6.43424), (8510, 6.39097), (20129, 6.36742), (16832, 6.26665), (13079, 6.25833), (7304, 6.1871), (20735, 6.12924), (25428, 6.04919), (15922, 6.02625), (4630, 6.0091), (17718, 5.99766), (18451, 5.97886), (11136, 5.95637), (12784, 5.86992), (2741, 5.83031), (21312, 5.81248), (17002, 5.8036), (26071, 5.74131), (26900, 5.69749), (24746, 5.68046), (21180, 5.66185), (23507, 5.56237), (16408, 5.53328), (7646, 5.52203), (3670, 5.50923), (26021, 5.4586), (20360, 5.45185), (2249, 5.4166), (27480, 5.38743), (10624, 5.36462), (2217, 5.342), (24493, 5.31066), (9517, 5.24182), (17343, 5.21374), (9611, 5.13699), (11740, 5.02429), (20790, 4.91769), (16090, 4.76006), (11447, 4.76006), (2562, 4.76006), (8676, 4.74816), (27441, 4.73633), (21851, 4.69422), (9055, 4.64036), (16476, 4.57117), (26758, 4.56897), (2045, 4.53085), (7237, 4.46696), (4344, 4.07525), (16857, 3.88768), (460, 3.88373), (2811, 3.8228), (25581, 3.39974), (8214, 3.05236), (26010, 2.57286), (7556, 2.1943), (21293, 2.09468), (14495, 1.85662), (743, 1.80693), (3384, 1.65136), (744, 1.30642), (11335, 0.905472), (14204, 0.882991), (8229, 0.841104) ]
Ay caramba
Splitted query: [ ay, caramba ]
Bag of words: [ (1946619, 1), (42264, 1) ]
Documents (fuzzy boolean search) [2]:[ 19621, 16788 ]
Documents (exact boolean search) [0]:[  ]
Documents BM25(fuzzy boolean search) [2]:[ (19621, 10.3874), (16788, 4.81148) ]
Documents BM25(exact boolean search) [0]:[  ]
karamba mamba
Splitted query: [ karamba, mamba ]
Bag of words: [ (43286, 1), (224357, 1) ]
Documents (fuzzy boolean search) [15]:[ 27229, 15747, 231, 3474, 13870, 24902, 12597, 11574, 3404, 9292, 5123, 5046, 22506, 1024, 3556 ]
Documents (exact boolean search) [0]:[  ]
Documents BM25(fuzzy boolean search) [15]:[ (27229, 13.7935), (5123, 13.5138), (12597, 12.2625), (15747, 8.89745), (3474, 8.59382), (3404, 8.55548), (3556, 8.01631), (231, 7.60316), (1024, 7.22213), (5046, 6.68208), (22506, 5.5575), (11574, 5.52548), (13870, 5.49746), (9292, 2.86138), (24902, 1.09459) ]
Documents BM25(exact boolean search) [0]:[  ]
```

[json11]: https://github.com/dropbox/json11.git
[cxxopts]: https://github.com/jarro2783/cxxopts.git
[thread-pool]: https://github.com/progschj/ThreadPool.git
[g3log]: https://bitbucket.org/KjellKod/g3log
[libcxx]: http://libcxx.llvm.org
[libcxx-hacking]: https://github.com/maidsafe/MaidSafe/wiki/Hacking-with-Clang-llvm-abi-and-llvm-libc
[self-travis-ci-config]: https://github.com/yazevnul/ysda-search-engine/blob/master/.travis.yml
