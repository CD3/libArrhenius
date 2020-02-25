# libArrhenius

A C++ library for performing Arrhenius Integral calculations, including coefficient fittings.

# Description

`libArrhenius` provides a set of classes for performing common calculations related to the Arrhenius Integral, specifically when
used to model thermal damage to tissue. Thermal damage is commonly modeled as a simple chemical reaction where damage
accumulation is given by the Arrhenius Integral.

<a href="https://www.codecogs.com/eqnedit.php?latex=\Omega&space;=&space;A&space;\int&space;e^{-E_a&space;/&space;RT(t)}&space;dt" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\Omega&space;=&space;A&space;\int&space;e^{-E_a&space;/&space;RT(t)}&space;dt" title="\Omega = A \int e^{-E_a / RT(t)} dt" /></a>

When ![1](http://latex.codecogs.com/gif.latex?\Omega\ge%201), the tissue is damaged.

# Getting Started

## Examples

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

