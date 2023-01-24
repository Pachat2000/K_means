/* il s agit de prendre une image et de la modifier */

#include "ima.h"
#include <limits.h>

void gris_uniform(Image * i) {
  int j, size;
  GLubyte * im, val;
  val = 128;
  size = 3 * i->sizeY * i->sizeX;
  im = i->data;
  for (j = 0; j < size; j ++) {
	*im++ = val;
  }
}
void gris_uniforme (Image * i) {
  int j, size, v;
  GLubyte * imr, * img, *imb, val;

  size = i->sizeY * i->sizeX;
  imr = i->data;
  img = i->data + 1;
  imb = i->data + 2;
  for (j = 0; j < size; j ++) {
	v = *imr + (int) *imr;
	v += 3 * (int) *img;
	v += *imb;
	v /= 6;
	val = (GLubyte) v;
	*imr = val;
	*img = val;
	*imb = val;
	imr += 3;
	img += 3;
	imb += 3;
  }
}

