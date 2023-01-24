#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>

#include <GL/glut.h>

#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glut.h>

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte *data;
};
typedef struct Image Image;
typedef unsigned short utab [3][3][3];

typedef struct Centre {  
  int r,g,b;
  int tmp;
  int NbcA, MoyCr, MoyCg, MoyCb;  
} Centroid;

typedef struct CLUT {
  int frequence;
  int r,g,b;
} Clut;


typedef struct tuple{
  int nb;
  Clut** tab;
}Tuple;

int ImageLoad_PPM(char *filename, Image *image);
void imagesave_PPM(char *filename, Image *image);
void upsidedown(Image *);
void gris_uniforme (Image *);
void gris (Image *);
void sobel (Image *);
Clut* ClutInit(int r, int g, int b);
void free_clut(Clut* clut);
void free_tabClut(Clut** clut, int nb);
int carre(int x);
int IsinClut(int r, int g, int b, Clut** tabclut, int nb);
Tuple* tabClut(Image* img);
Centroid* initCentr(int r, int g, int b);
void free_centroid(Centroid *centr);
void free_Tabcentr(Centroid** centr, int k);
Centroid ** tabCentr(Clut** tabclut, int nb, int *k);
int bestcentroide(int r, int g, int b, Centroid** tabcentr, int nbcentr);
int finished(Image*img,Image*svg,Centroid ** clust, int nbCentr);
void MoyCent(Image* img,Centroid ** clust, int nbCentr);
int ColorToInt(int r, int g, int b);
int compare (const void * first, const void * second );
Tuple* reduce(Clut** tab, int nb);
void K_means(int k, Image* img);
