# Execution:
#   python visualisation.py

from matplotlib import pyplot
from numpy import loadtxt

# draw the big cities (94)
cities = loadtxt("resuBigCities.dat", dtype=float, delimiter=" ")
pyplot.scatter(cities[:,1], cities[:,2], s=cities[:,0]/1000, c=cities[:,0], alpha=0.5)

# draw the graph big cities
graph = loadtxt("resuGraph_bigcities.dat", dtype=int)
print(graph.shape[0])
for x in range(graph.shape[0]):
  edge = [graph[x,0], graph[x,1]]
  pyplot.plot(cities[edge,1], cities[edge,2], 'b')


# draw the citie per dpt
cities = loadtxt("resuCities.dat", dtype=float, delimiter=" ")
pyplot.scatter(cities[:,1], cities[:,2], s=cities[:,0]/1000, c=cities[:,0], alpha=0.5)


# draw the graph per dpt
graph = loadtxt("resuGraph.dat", dtype=int)
for x in range(graph.shape[0]):
  edge = [graph[x,0], graph[x,1]]
  pyplot.plot(cities[edge,1], cities[edge,2], 'b')


pyplot.xlabel('Longitude', size=16)
pyplot.ylabel('Latitude', size=16)

# save in a PNG file
pyplot.savefig('result.png')

# show the result
pyplot.show()
