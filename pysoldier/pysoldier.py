
import numpy as np
from cv2 import imshow, imread, waitKey, cvtColor, COLOR_RGB2BGR, COLOR_RGBA2BGRA, IMREAD_UNCHANGED 
from hashlib import md5,sha1
from zlib import adler32
from fileinput import input
from os import listdir

with open("checksums") as f:
  checksums = iter([ int(line.strip(),16) for line in f ])



images = {
  x[:-4] : cvtColor( imread(x, flags=IMREAD_UNCHANGED ), COLOR_RGBA2BGRA )
  for x in listdir('.') if x.endswith('png')
}


canvas = np.zeros( ( 650, 500, 3 ),dtype=np.uint8 )


def draw( id, x, y , offset, size ):
  if id not in images: return

  tile = images[ id ][ offset : offset+size, :, : ]

  image = tile[
    max(0,-y) : min( canvas.shape[0] - y, tile.shape[0]  ) ,
    max(0,-x) : min( canvas.shape[1] - x, tile.shape[1]  ) ,
    :
  ]

  h, w, _ = image.shape
  mask  = np.repeat( ( image[:,:,3]>0 ).reshape((h,w,1)), 3, axis=2 )
  data = image[:,:,:3]

  x,y = max(x,0), max(y,0)
  canvas_roi = canvas[ y : y + h, x : x + w ]
  if 0 in canvas_roi.shape: return
  canvas[ y : y + h, x : x + w ] = np.where( mask, data, canvas_roi )

def process(  ):
  assert(  adler32(  canvas ) == next( checksums ) )
  imshow( "a",  canvas )
  canvas[ :, :, : ] = 0
  assert ord('q')!=waitKey(1)

