from PIL import Image
import numpy

s=128
ms=(s + 1) / 2

data = numpy.zeros([s, s, 4], dtype = numpy.uint8)
for i in range(s):
	for j in range(s):
		dist = ((i - ms) ** 2 + (j - ms) ** 2) ** 0.5
		data[i][j] = numpy.array([0, 0, 0, max(0, 1 - dist / ms) ** 1 * 255])

im = Image.fromarray(data, 'RGBA')
im.save("pb.png")
