
import numpy as np
from cv2 import imshow, waitKey, cvtColor, COLOR_RGB2BGR
from hashlib import md5,sha1
from zlib import adler32
from fileinput import input

with open("checksums") as f:
  checksums = iter([ int(line.strip(),16) for line in f ])

def process(buffer):
  assert  adler32( buffer ) == next(checksums)
  image = buffer.reshape( ( -1, 500, 3 ) )
  imshow("",cvtColor(image,  COLOR_RGB2BGR))
  assert ord('q')!=waitKey(1)

