# Python benchmark performance

The goal of this document is to better understand the performance, potential bottlenecks and reasonable expectations of Python-based programs on the BeagleBone Black platform.

## pyperformance
This tool is maintaned by the CPython developers and is used to evaluate new features and optimizations in the interpreter. We will use this tool to compare the perfomance of the CPython interpreter on different platforms.

For details, see the GitHub repository: [https://github.com/python/performance](https://github.com/python/performance)

The following platforms were evaluated:

|  | BBB | VM | Desktop |
|--|--|--|--|
| Description  | BeagleBone Black (bbb-btrfs-16GB-101017) | VMWare hosted on Windows | Native desktop environment |
| Architecture | ARM Cortex-A8 | X86-64 | X86-64 |
| CPU(s) | AM335x @1GHz (1 core) | Core i7-3770K @3.5 GHz (2 cores) |  Core i7-3770S @3.10GHz (8 cores)
| Memory | 512MB | 4GB | 16GB|
| OS | Ubuntu 16.04.2 LTS | Ubuntu 16.04.3 LTS | Ubuntu 16.04.3 LTS |
| Python | Python 3.5.2 | Python 3.5.2 | Python 3.5.2 |

### Performance results

All numbers are in milliseconds (ms). Lower number means better perfomance.

| Benchmark  | BBB | VM | Desktop |
|--|--|--|--|
| 2to3 | 16100 | 734 | 756 |
| chameleon | 432 | 19.4 | 19.1 |
| chaos | 6300 | 252 | 241 |
| crypto_pyaes | 4970 | 193 | 186 |
| deltablue | 394 | 12.5 | 12.1 |
| django_template | 6410 | 315 | 292 |
| dulwich_log | 3150 | 140 | 122 |
| fannkuch | 20300 | 873 | 849 |
| float | 6620 | 223 | 215 |
| genshi_text | 1490 | 64.6 | 62.2 |
| genshi_xml | 2890 | 143 | 137 |
| go | 11700 | 436 | 423 |
| hexiom | 544 | 17.1 | 16.6 |
| html5lib | 4040 | 186 | 173 |
| json_dumps | 497 | 22.6 | 21.6 |
| json_loads | 0.789 | 0.0435 | 0.0425 |
| logging_format | 0.497 | 0.0247 | 0.0229 |
| logging_silent | 0.0184 | 0.000586 | 0.000574 |
| logging_simple | 0.422 | 0.0202 | 0.0193 |
| mako | 798 | 29.3 | 28.0 |
| meteor_contest | 2930 | 158 | 152 |
| nbody | 6530 | 238 | 230 |
| nqueens | 5050 | 209 | 203 |
| pathlib | 741 | 38.4 | 33.3 |
| pickle | 0.237 | 0.0143 | 0.0136 |
| pickle_dict | 0.501 | 0.0343 | 0.0332 |
| pickle_list | 0.0747 | 0.005 | 0.00486 |
| pickle_pure_python | 23.8 | 0.902 | 0.873 |
| pidigits | 4610 | 218 | 212 |
| python_startup | 310 | 21.8 | 27.9 |
| python_startup_no_site | 155 | 11.5 | 12.5 |
| raytrace | 30000 | 1120 | 1090 |
| regex_compile | 6910 | 265 | 255 |
| regex_dna | 2520 | 228 | 222 |
| regex_effbot | 56.3 | 4 | 3.91 |
| regex_v8 | 587 | 35.7 | 34.7 |
| richards | 3530 | 130 | 126 |
| scimark_fft | 17200 | 602 | 590 |
| scimark_lu | 13700 | 364 | 350 |
| scimark_monte_carlo | 5910 | 194 | 189 |
| scimark_sor | 12100 | 404 | 394 |
| scimark_sparse_mat_mult | 216 | 8.29 | 7.94 |
| spectral_norm | 7350 | 260 | 241 |
| sqlalchemy_declarative | 6000 | 277 | 236 |
| sqlalchemy_imperative | 1210 | 54.2 | 46.1 |
| sqlite_synth | 0.143 | 0.00827 | 0.00726 |
| sympy_expand | 21600 | 967 | 841 |
| sympy_integrate | 899 | 37.6 | 34.7 |
| sympy_sum | 4400 | 192 | 171 |
| sympy_str | 9220 | 427 | 363 |
| telco | 371 | 18.3 | 17.2 |
| tornado_http | 8390 | 344 | 290 |
| unpack_sequence | 0.00108 | 0.0001 | 0.0001 |
| unpickle | 0.350 | 0.0231 | 0.0225 |
| unpickle_list | 0.139 | 0.00765 | 0.00741|
| unpickle_pure_python | 18.5 | 0.630 | 0.604 |
| xml_etree_parse | 5990 | 220 | 209 |
| xml_etree_iterparse | 9710 | 366 | 350 |
| xml_etree_generate | 5810 | 268 | 255 |
| xml_etree_process | 4750 | 179 | 172 |


### Summary

Both x84-64 platforms showed similar performance on all benchmark suites. **Apparently, the memory size (with a reasonable lower limit), number of cores and x86 virtualization does not make a significant difference for typical Python programs**.

**The BeagleBone Black (Cortex-A8) platform demonstrated a significantly worse performance, ranging from 11x to 40x increase in time (average x24 across all benchmark suites)**. The worse results in order:

- scimark_lu
- hexiom
- deltablue
- logging_silent
- scimark_monte_carlo
- float
- unpickle_pure_python
- scimark_sor
- spectral_norm
- scimark_fft

Most of these benchmarks target floating-point and/or numerical computation performance.

 

