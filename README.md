# libArrhenius

A C++ library for performing Arrhenius Integral calculations, including coefficient fittings.

# Description

`libArrhenius` provides a set of classes for performing common calculations related to the Arrhenius Integral, specifically when
used to model thermal damage to tissue. Thermal damage is commonly modeled as a simple chemical reaction where damage
accumulation is given by the Arrhenius Integral.

<a href="https://www.codecogs.com/eqnedit.php?latex=\Omega&space;=&space;A&space;\int&space;e^{-E_a&space;/&space;RT(t)}&space;dt" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\Omega&space;=&space;A&space;\int&space;e^{-E_a&space;/&space;RT(t)}&space;dt" title="\Omega = A \int e^{-E_a / RT(t)} dt" /></a>

When ![1](http://latex.codecogs.com/gif.latex?\Omega\ge%201), the tissue is damaged.

# Getting Started

`libArrenius` is a library that provides utilities for evaluating the Arrhenius damage integral, computing
damage threshold scaling factors for a given thermal profile, and even fitting the Arrhenius parameters to a
set of thermal profiles.

For convenience, a CLI is also provided. If all you want to do is evaluate the damage integral, or compute
the damage threshold scaling factor for a given thermal profile, the CLI is the easiest to use.

## Building and Using the CLI

To build the CLI, install its dependencies with conan (Boost and Eigen3), and then use CMake to configure and build.

```
$ mkdir build
$ cd build
$ conan install ..
$ source activate.sh
$ cmake ../applications
$ cmake --build .
```

This will create an executable named `Arrhenius-cli`. You can check that it runs with the `-h` option.
```
$ ./Arrhenius-cli -h
```

The CLI provides multiple sub-commands for performing certain actions. To evaluate the damage integral for
a thermal profile stored in a file named `Tvst.txt` (the file needs to be in gnuplot format), use the `calc-damage`
sub-command.
```
$ ./Arrhenius-cli calc-damage Tvst.txt 
```
By default, the Welch-Polhamus coefficients for Retinal damage are used. These can be overridden with
command line options.

## Library Examples

`libArrenius` is a header-only library, but it if you install it, it will provide 
a CMake config file that works with CMake's
`find_package(...)` command to provide a target named `libArrenius::Arrhenius`. To build and install the library,
you will need to have Boost and Eigen3. A conanfile is provided to automatically install the dependencies.

```
$ mkdir build
$ cd build
$ conan install ..
$ cmake ..
$ cmake --build . --target install
```

To use the library, add a `find_package(libArrenius)` call to your `CMakeLists.txt` and link against `libArrenius::Arrhenius` target.


### Evaluating the Arrhenius Integral

Let's say you just wanted to evaluate the Arrhenius integral for a given temperature profile.
To do this, first create an `ArrheniusIntegral<T>` instance initialized with the Arrhenius
coefficients (A and Ea), then pass it two pointers to the data to integrate.

```
#include <libArrhenius/Integration/ArrheniusIntegral.hpp>

// build a temperature profile...
...
// t and T are std::vector<double> instances
// (of the same size) that contian time and temperature pairs
// for a thermal profile

// the Welch-Polhamus coefficients for retinal thermal damage
ArrheniusIntegral<double> Arr(3.1e99,6.28e5);

double Omega = Arr(t.size(),t.data(),T.data());
```
The `integrate` functions takes a size and two pointers, which means you don't have
to use `std::vector` to store the temperature profile. Any type that stores its
data in a contiguous array of elements can be used. However, both pointers must point to the same type
(i.e. `float` vs `double`), and which must match the `ArrheniusIntegral` template parameter.

