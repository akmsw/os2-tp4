import cv2
import sys

f = open('match_coords.txt', 'r')
line = f.readline()

coords = line.split(',')

x = int(coords[0])
y = int(coords[1])
limx = int(coords[2])
limy = int(coords[3])

main = cv2.imread("img/" + sys.argv[1] + ".png")

cv2.rectangle(main, (x, y), ((x + limx), (y + limy)), (0, 0, 255), 5)

resize = cv2.resize(main, dsize = (int(main.shape[1]/4), int(main.shape[0]/4)), interpolation = cv2.INTER_CUBIC)

cv2.imshow("match", resize)
cv2.waitKey(0)
cv2.destroyAllWindows()