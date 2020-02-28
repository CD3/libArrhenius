R = 8.314

Rate(Ea,A,T) = A*exp(-Ea/R/T)

Offset(T0,T) = T0 + T
ConstantTemp(t) = t < 0.1 ? 0 : (t < 0.9 ? 10 : 0)
TriangleTemp(t) = t < 0.1 ? 0 : (t < 0.5 ? 20*(t-0.1) : ( t < 0.9 ? -20*(t-0.9): 0))
InfPlaneSourceTemp(k,t) = t > 0 ? sqrt(t/pi/k) : 0
PulsePlaneSourceTemp(k,tau,t) = InfPlaneSourceTemp(k,t) - InfPlaneSourceTemp(k,t-tau)
Temp(t) = 20*PulsePlaneSourceTemp(1.,1.,t)

set samples 1000
set xrange[-1:2]
set yrange[-1:12]
set y2range[-0.0005:0.0035]
# set y2tics
set xlabel "time (s)"
set ylabel "temperature rise (C)"
plot Temp(x) title "T(t)", Rate(6.28e5,3.1e99,Offset(310, Temp(x))) axis x1y2 title "damage rate"



