from PIL import Image
import numpy

s = 1024
m = 256
res = numpy.zeros([s, s, 3], dtype = numpy.uint8)
for i in range(s):
	for j in range(s):
		res[i, j, 0] = m * j / s
		res[i, j, 1] = m * i / s
		res[i, j, 2] = 255
img = Image.fromarray(res, 'HSV')
img.convert('RGB').save("colorsel1.png")
