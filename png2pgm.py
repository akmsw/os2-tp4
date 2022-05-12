from wand.image import Image

import sys

main = Image(filename = 'img/' + sys.argv[1] + '.png')
target = Image(filename = 'img/' + sys.argv[2] + '.png')

main_pgm = main.convert('pgm')
target_pgm = target.convert('pgm')

main_pgm.save(filename = 'img/' + sys.argv[1] + '.pgm')
target_pgm.save(filename = 'img/' + sys.argv[2] + '.pgm')