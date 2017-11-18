# Python benchmark performance

The goal of this document is to better understand the performance, potential bottlenecks and reasonable expectations of Python-based programs on the BeagleBone Black platform.

## pyperformance
This tool is maintaned by the CPython developers and is used to evaluate new features and optimizations in the interpreter. We will use this tool to compare the perfomance of the CPython interpreter on different platforms.

For details, see the GitHub repository: [https://github.com/python/performance](https://github.com/python/performance)

The following platforms were evaluated:

|  | BBB | VM | Desktop | Odroid-XU4
|--|--|--|--|--|
| Description  | BeagleBone Black (bbb-btrfs-16GB-101017) | VMWare hosted on Windows | Native desktop environment | Odroid-XU4 |
| Architecture | ARM Cortex-A8 | X86-64 | X86-64 | ARM Cortex A15 (+ A7) |
| CPU(s) | AM335x @1GHz (1 core) | Core i7-3770K @3.5 GHz (2 cores) |  Core i7-3770S @3.10GHz (8 cores) | Samsung Exynos5422 @2GHz (4 cores)|
| Memory | 512MB | 4GB | 16GB| 2GB |
| OS | Ubuntu 16.04.2 LTS | Ubuntu 16.04.3 LTS | Ubuntu 16.04.3 LTS | Ubuntu 16.04.3 LTS |
| Python | Python 3.5.2 | Python 3.5.2 | Python 3.5.2 | Python 3.5.2 |

### Performance results

All numbers are in milliseconds (ms). Lower number means better perfomance.

| Benchmark  | BBB | VM | Desktop | Odroid
|--|--|--|--|--|
| 2to3 | 16100 | 734 | 756 | 3030 |
| chameleon | 432 | 19.4 | 19.1 | 82.1 |
| chaos | 6300 | 252 | 241 | 1020 |
| crypto_pyaes | 4970 | 193 | 186 | 819 |
| deltablue | 394 | 12.5 | 12.1 | 58.3 | 
| django_template | 6410 | 315 | 292 | 1180 | 
| dulwich_log | 3150 | 140 | 122 | 468 | 
| fannkuch | 20300 | 873 | 849 | 4270 | 
| float | 6620 | 223 | 215 | 947 | 
| genshi_text | 1490 | 64.6 | 62.2 | 296 | 
| genshi_xml | 2890 | 143 | 137 | 615 | 
| go | 11700 | 436 | 423 | 1850 | 
| hexiom | 544 | 17.1 | 16.6 | 83.1 | 
| html5lib | 4040 | 186 | 173 | 742 | 
| json_dumps | 497 | 22.6 | 21.6 | 100 | 
| json_loads | 0.789 | 0.0435 | 0.0425 | 0.186 | 
| logging_format | 0.497 | 0.0247 | 0.0229 | 0.0927 | 
| logging_silent | 0.0184 | 0.000586 | 0.000574 | 0.00326 | 
| logging_simple | 0.422 | 0.0202 | 0.0193 | 0.0798 | 
| mako | 798 | 29.3 | 28.0 | 134 | 
| meteor_contest | 2930 | 158 | 152 | 658 | 
| nbody | 6530 | 238 | 230 | 1020 | 
| nqueens | 5050 | 209 | 203 | 919 | 
| pathlib | 741 | 38.4 | 33.3 | 161 | 
| pickle | 0.237 | 0.0143 | 0.0136 | 0.0642 | 
| pickle_dict | 0.501 | 0.0343 | 0.0332 | 0.129 | 
| pickle_list | 0.0747 | 0.005 | 0.00486 | 0.0204 | 
| pickle_pure_python | 23.8 | 0.902 | 0.873 | 4.0 | 
| pidigits | 4610 | 218 | 212 | 1080 | 
| python_startup | 310 | 21.8 | 27.9 | 57.8 | 
| python_startup_no_site | 155 | 11.5 | 12.5 | 31.8 | 
| raytrace | 30000 | 1120 | 1090 | 5060 | 
| regex_compile | 6910 | 265 | 255 | 1140 |  
| regex_dna | 2520 | 228 | 222 | 671 | 
| regex_effbot | 56.3 | 4 | 3.91 | 12.7 | 
| regex_v8 | 587 | 35.7 | 34.7 | 160 | 
| richards | 3530 | 130 | 126 | 575 | 
| scimark_fft | 17200 | 602 | 590 | 2460 | 
| scimark_lu | 13700 | 364 | 350 | 2050 | 
| scimark_monte_carlo | 5910 | 194 | 189 | 871 | 
| scimark_sor | 12100 | 404 | 394 | 1930 | 
| scimark_sparse_mat_mult | 216 | 8.29 | 7.94 | 32.3 | 
| spectral_norm | 7350 | 260 | 241 | 1160 | 
| sqlalchemy_declarative | 6000 | 277 | 236 | 950 | 
| sqlalchemy_imperative | 1210 | 54.2 | 46.1 |167 |
| sqlite_synth | 0.143 | 0.00827 | 0.00726 | 0.0381 |
| sympy_expand | 21600 | 967 | 841 | 3780 | 
| sympy_integrate | 899 | 37.6 | 34.7 | 146 |
| sympy_sum | 4400 | 192 | 171 | 711 |
| sympy_str | 9220 | 427 | 363 | 1590 | 
| telco | 371 | 18.3 | 17.2 | 63.1 |
| tornado_http | 8390 | 344 | 290 | 1210 | 
| unpack_sequence | 0.00108 | 0.0001 | 0.0001 | 0.000003 |
| unpickle | 0.350 | 0.0231 | 0.0225 | 0.106 | 
| unpickle_list | 0.139 | 0.00765 | 0.00741| 0.0525 |
| unpickle_pure_python | 18.5 | 0.630 | 0.604 | 2.77 |
| xml_etree_parse | 5990 | 220 | 209 | 1170 |
| xml_etree_iterparse | 9710 | 366 | 350 | 1650 |
| xml_etree_generate | 5810 | 268 | 255 | 944 |
| xml_etree_process | 4750 | 179 | 172 | 790 |


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

 ### Addition: Odroid-XU4
 
A stock Odroid XU-4 was evaluate using the same benchmark. **It seems to be about 5x faster that the BBB platform. Note it is running 4, 2GHz cores.** 
 

