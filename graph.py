import matplotlib.pyplot as plt

s = open('out.txt', 'r').read()
d = eval(s)

epochs = len(d)
num_weights = len(d[0]['weights'])
weights = [[e['weights'][i] for e in d] for i in range(num_weights)]
population = len(d[0]['lines'])
lines = [[sorted(e['lines'])[i] for e in d] for i in range(population)]

fig, (ax1, ax2) = plt.subplots(2, 1, sharex = True)
# fig.subplots_adjust(hspace = 0)

colours = 'red', 'orange', 'gold', 'brown'
ax1.set_ylabel('weights')
for i in range(num_weights): ax1.plot(weights[i], color=colours[i], label='weight' + str(i))
ax1.legend()

colour = 'blue'
colours = [(0, 0, 1, 1 - i / population) for i in range(population)]
ax2.set_xlabel('epoch')
ax2.set_ylabel('lines')  # we already handled the x-label with ax1
for i in range(population): ax2.plot(lines[i], color=colours[i])

fig.tight_layout()  # otherwise the right y-label is slightly clipped
plt.show()
