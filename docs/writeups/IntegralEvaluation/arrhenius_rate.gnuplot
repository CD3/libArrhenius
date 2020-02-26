R = 8.314

Rate(Ea,A,T) = A*exp(-Ea/R/T)

ConstantTemp(t) = t > 0.1 ? 10 : (t < 0.8 ? 10 : 0)

plot ConstantTemp(x)
