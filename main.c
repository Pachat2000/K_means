#include <unistd.h>     
#include <math.h>
#include <string.h>
#include "ima.h"


#include <math.h>



Image *image;

#define ESCAPE 27


void Display(void) {
  


  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(image->sizeX, image->sizeY, GL_RGB, GL_UNSIGNED_BYTE, 
	       image->data);

  glFlush();
}



Clut* ClutInit(int r, int g, int b){ // initialise un rgb dans la clut
  Clut* clut = malloc(sizeof(clut));
  assert(clut);
  clut->frequence = 1;
  clut->r = r;
  clut->g = g;
  clut->b = b;
  return clut;
}

void free_clut(Clut* clut){ // libère la mémoire d'un élément dans la clut
  free(clut);
}

void free_tabClut(Clut** clut, int nb){ // libère la mémoire du tableau de Clut
  
  for(int i =0; i < nb ; i++){
    free_clut(clut[i]);
    clut[i]=NULL;
  }
  free(clut);
  clut = NULL;
}

int carre(int x){
  return x*x;
}

int IsinClut(int r, int g, int b, Clut** tabclut, int nb){// vérifie si un rgb est dans la Clut ou éloigner 
  int distMax = 70;
  //printf("%d\n",nb);
  for(int i = 0; i < nb; i ++){
    // printf("%d\n",(int)sqrt(carre(r-tabclut[i]->r)+carre(g-tabclut[i]->g)+carre(b-tabclut[i]->b)) );    
    if((int)sqrt(carre(r-tabclut[i]->r)+carre(g-tabclut[i]->g)+carre(b-tabclut[i]->b)) <= distMax){
      return 0;      
    }    
  }
  //printf("okpaspris\n");
  return 1;
}

Tuple* tabClut(Image* img){  // initialise le tableau de CLut par rapport à l'image
  //Clut** tabclut = malloc(sizeof(Clut*)*img->sizeX*img->sizeY);
  Clut** tabclut = malloc(sizeof(Clut*));
  assert(tabclut);
  int nb =0;
  for(int i =0; i < img->sizeX*img->sizeY; i+=3){
    //if(IsinClut(img->data[i],img->data[i+1],img->data[i+2], tabclut, nb) == 0){
      tabclut[nb]  = ClutInit(img->data[i],img->data[i+1],img->data[i+2]);
      nb++;
      tabclut = realloc(tabclut, (nb+1)*sizeof(Clut*));
      //}

  }
  //quicksort(tabclut, nb);
  Tuple* t = malloc(sizeof(Tuple));
  t->nb = nb;
  t->tab= tabclut;
  return t;
}




Centroid* initCentr(int r, int g, int b){ // initialise un Centroïde
  Centroid* centr = malloc(sizeof(Centroid));
  assert(centr);
  centr->r = r;
  centr->g = g;
  centr->b = b;
  centr->tmp = -50000;
  centr->NbcA =0;
  centr->MoyCr = 0;
  centr->MoyCg = 0;
  centr->MoyCb = 0;
  return centr;
}

void free_centroid(Centroid *centr){ // libère un centroïde en mémoire
  free(centr);
}

void free_Tabcentr(Centroid** centr, int k){// libère tous les éléments du tableau
  for(int i = 0; i <k;i++){
    free_centroid(centr[i]);
    centr[i]= NULL;
  }
  free(centr);
  centr = NULL;
}

Centroid ** tabCentr(Clut** tabclut, int nb, int *k){ //crée le le tableau de centroïdes
  int tmp[nb];
  memset( tmp, 0, nb*sizeof(int) );
  if(nb < *k) *k = nb; 
  Centroid** tab = malloc(sizeof(Centroid*) * (*k));
  int nbtab = *k-1;
  int i = *k -1;
  while( i != -1){
    int max =-1;
    int pos = -1;
    for(int y =0; y < nb; y++){
      if(max < tabclut[y]->frequence && tmp[y] == 0){
	pos = y;
	max =  tabclut[y]->frequence;
      }
    }
    tmp[pos] = 1;
    tab[nbtab] = initCentr(tabclut[pos]->r, tabclut[pos]->g, tabclut[pos]->b);
    nbtab--;
    i--;
    max = -1;
  }
  return tab;
}

int bestcentroide(int r, int g, int b, Centroid** tabcentr, int nbcentr){// regarde quelle centroïde est le plus proche niveau couleur
  int dist = 999999999;
  int posClusterwin = 0;
  for(int i =0; i< nbcentr; i++){
    //float tmp = ( sqrt(carre(r-tabcentr[i]->r)+carre(g-tabcentr[i]->g)+carre(b-tabcentr[i]->b)) );
    int tmp =  abs(ColorToInt(r,g,b) - ColorToInt(tabcentr[i]->r,tabcentr[i]->g,tabcentr[i]->b));
    printf("color to int: %d %d %d\n",ColorToInt(r,g,b), ColorToInt(tabcentr[i]->r,tabcentr[i]->g,tabcentr[i]->b), tmp);
    //if(r == 138 && g == 174 && b ==201) printf("res : %f\n",tmp);    
    if( tmp < dist){
      dist = tmp;
      posClusterwin = i;
    }    
  }
  
  return posClusterwin;
}

void BestclusterForC(Image * img, Centroid ** clust, int nbCentr){ //On explore toute l'image et on donnes les valeurs de chaque pixels au centroïdes pour leurs moyennes
  for(int i = 0; i < nbCentr; i++){
    clust[i]->tmp = clust[i]->NbcA;
    clust[i]->NbcA = 0;
    clust[i]->MoyCb =0;
    clust[i]->MoyCr = 0;
    clust[i]->MoyCg = 0;
  }
  GLubyte *svg = img->data;
  GLubyte *crt = img->data;
  int val1,val2,val3;
  int nb_pixel;
  nb_pixel =  img->sizeX * img->sizeY;
  for(int i =0; i < nb_pixel; i++){
    val1 = *crt++;
    val2 = *crt++;
    val3 = *crt++;
    //if(i > img->sizeX * (img->sizeY -1)) printf("Colors %d %d %d\n",val1,val2,val3);
    int bestcentroid = bestcentroide(val1,val2,val3,clust,nbCentr);
    //if(i > nb_pixel -1024 && i  < nb_pixel -900) printf("RGB %d %d %d, best %d\n",val1,val2,val3,bestcentroid);
    clust[bestcentroid]->NbcA+= 1;
    clust[bestcentroid]->MoyCr += val1;    
    clust[bestcentroid]->MoyCg += val2;
    clust[bestcentroid]->MoyCb += val3;
  }
  *crt = *svg;
}

int finished(Image*img,Image*svg,Centroid ** clust, int nbCentr){ // vérifie si les clusters sont stables
  int changement =0;
  for(int i =0; i < nbCentr; i++){
    changement += abs(clust[i]->tmp - clust[i]->NbcA);
  }
  //printf("%d\n",changement);
  if(changement  <= 20) return 1;
  int size = svg->sizeX * svg->sizeY;
  for(int i = 0,t = 0; i < size; i++,t+=3){
    img->data[t] = svg->data[t];
    img->data[t+1] =  svg->data[t+1];
    img->data[t+2] = svg->data[t+2];
  }
  return 0;
}

void MoyCent(Image* img,Centroid ** clust, int nbCentr){ // applique la couleur moyenne au pixel associé et l'applique aussi aux centroïdes
  
  GLubyte *crt = img->data;
  int nb_pixel;
  int val1,val2,val3;
  nb_pixel =  img->sizeX * img->sizeY;
  printf("%lu\n",img->sizeX);
  for(int i =0; i < nb_pixel; i++){
    val1 = *crt;
    val2 = *crt+1;
    val3 = *crt+2;
    int bestcentroid = bestcentroide(val1,val2,val3,clust,nbCentr);
    *crt++ = clust[bestcentroid]->MoyCr / clust[bestcentroid]->NbcA;
    *crt++ = clust[bestcentroid]->MoyCg / clust[bestcentroid]->NbcA;
    *crt++ = clust[bestcentroid]->MoyCb / clust[bestcentroid]->NbcA;
  }
  for(int i =0; i< nbCentr; i++){
    if( clust[i]->NbcA != 0){
      printf("%d %d | %d %d %d -> %d %d %d\n",i, clust[i]->NbcA, clust[i]->r, clust[i]->g, clust[i]->b,clust[i]->MoyCr/  clust[i]->NbcA, clust[i]->MoyCg/  clust[i]->NbcA, clust[i]->MoyCb/  clust[i]->NbcA);
      clust[i]->r = (int) clust[i]->MoyCr /  clust[i]->NbcA;
      clust[i]->g =  (int) clust[i]->MoyCg /  clust[i]->NbcA;
      clust[i]->b =  (int) clust[i]->MoyCb /  clust[i]->NbcA;
    }    
  }
  
}

int ColorToInt(int r, int g, int b){ // convertie un RGB en un int 
  int bits = 8;
  int rgb = 0;
 
  int nb = 0;
  for(int i = bits ; i > 0; i --){
    rgb = rgb  | ((1&g)<<nb);
    g = g >> 1;
    nb++;
    
    rgb = rgb |((1&r)<<nb) ;
    r = r >> 1;
    nb++;
    
    rgb = rgb | ((1&b)<<nb) ;
    b = b >> 1;
    nb++;
  }
  return rgb;
}

int compare (const void * first, const void * second ){// permet de comparer deux éléments Clut pour le trier
  Clut* tmp = (Clut*)first;
  Clut* tmp2 = (Clut*)second;
  
  int a =ColorToInt(tmp->r,tmp->g,tmp->b);
  int b = ColorToInt(tmp2->r,tmp2->g,tmp2->b);
  return (int) (a -b);
}

Tuple* reduce(Clut** tab, int nb){
  Clut** ntab = malloc(sizeof(Clut*));
  assert(ntab);
  int newnb = 1;
  ntab[0] = ClutInit(tab[0]->r, tab[0]->g, tab[0]->b);

  for(int i= 1; i < nb; i++){
    if(tab[i]->r == ntab[newnb -1]->r && tab[i]->g == ntab[newnb -1]->g && tab[i]->b == ntab[newnb -1]->b){
      ntab[newnb -1]->frequence = ntab[newnb -1]->frequence  +1;
    }
    else{
      ntab = realloc(ntab, (newnb+1)*sizeof(Clut*));
      ntab[newnb] = ClutInit(tab[i]->r, tab[i]->g, tab[i]->b);
      newnb++;
    }
  }
  free_tabClut(tab,nb);
  Clut** ftab = malloc(sizeof(Clut*));
  assert(ftab !=NULL);
  int fnb = 0;
  for(int i= 1; i < newnb; i++){
    
    if(IsinClut(ntab[i]->r,ntab[i]->g,ntab[i]->b,ftab, fnb) ==1){
      //      printf("before %d %d %d\n",ntab[i]->r,ntab[i]->g,ntab[i]->b);    
      ftab[fnb] =  ClutInit(ntab[i]->r,ntab[i]->g,ntab[i]->b);
      //printf("%d\n",ftab[fnb]->r);    
      fnb+=1;
      ftab = realloc(ftab, sizeof(Clut*)*(fnb+1));
      assert(ftab != NULL);
      
    }
  }
  //printf("ok\n");
  free_tabClut(ntab,newnb);

  Tuple* t = malloc(sizeof(Tuple));
  t->nb = fnb;
  t->tab= ftab;
  return t;
}

void K_means(int k, Image* img){ // fonction rassamblant toutes les autres fonctions vue juste avant

  Tuple* t = tabClut(img);
  Clut** clut=  t->tab;// tableau non trier
  int nb = t->nb;

  qsort(clut, nb, sizeof(Clut*), compare);

  free(t);

  Tuple* t2 = reduce(clut,nb);

  Clut** clut2 = t2->tab; //tableau trie reduit par distance de couleur et de frequance;
  nb = t2->nb;

  
  free(t2);

  Centroid ** centr = tabCentr(clut2,nb,&k);

  free_tabClut(clut2,nb);

  Image* svg = malloc(sizeof(Image));  
  svg->sizeX = img->sizeX;
  svg->sizeY = img->sizeY;
  
  int size = img->sizeX * img->sizeY;
  svg->data = malloc((size*3)*sizeof(GLubyte));


  for(int i = 0,t = 0; i < size; i++,t+=3){  
    svg->data[t] = img->data[t];
    svg->data[t+1] = img->data[t+1];
    svg->data[t+2] = img->data[t+2];
  }


  while(finished(img,svg,centr,k) == 0){ // commenter le while pour faire seullement une iteration   
    BestclusterForC(img,centr,k);
    MoyCent(img,centr,k);
    Display();
    sleep(1);
  }
  free_Tabcentr(centr,k);
}



void Keyboard(unsigned char key, int x, int y)  {
  switch(key){
  case ESCAPE :
    exit(0);                   
    break;
  default:
    fprintf(stderr, "Unused key\n");
  }
}

void Mouse(int button, int state, int x, int y) {

  switch(button){
  case GLUT_LEFT_BUTTON:
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;    
  }
  glutPostRedisplay();
}

int Init(char *s) {

  image = (Image *) malloc(sizeof(Image));
  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    return(-1);
  }
  if (ImageLoad_PPM(s, image)==-1) 
	return(-1);
  printf("tailles %d %d\n",(int) image->sizeX, (int) image->sizeY);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}
int ReInit() {
  /*
  if (ImageLoad_PPM(s, image)==-1) 
	return(-1);
  */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}



void Reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble)h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void menuFunc(int item) {
  char s[256];
  int pl;
  switch(item){
  case 0:
    free(image);
    exit(0);
    break;
    case 2:
    printf("Entrer le nombre de centroïde que vous voulez.\n");
    scanf("%d", &pl);
    K_means(pl,image);
    Display();
    break;
  case 5:
    printf("Entrer le nom pour l'image dans cette taille\n");
    scanf("%s", &s[0]);
    imagesave_PPM(s, image);
    break;
  case 6:
    printf("Taille de l image : %d %d\n", (int) image->sizeX, (int) image->sizeY);
    break;
  default:
    break;
  }
}




int main(int argc, char **argv) {  

  if (argc<2) {
    fprintf(stderr, "Usage : palette nom_de_fichier\n");
    exit(0);
  }
  
  glutInit(&argc, argv); 
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowSize(640,480);  
  glutInitWindowPosition(100, 100);  
  glutCreateWindow("VPUP8");  
  
  Init(argv[1]);
  glutCreateMenu(menuFunc);
  glutAddMenuEntry("Quit", 0);
  glutAddMenuEntry("K-means", 2);
  glutAddMenuEntry("Sauver", 5);
  glutAddMenuEntry("Informations", 6);
  glutAttachMenu(GLUT_LEFT_BUTTON);

  glutDisplayFunc(Display);  
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Keyboard);
  
  glutMouseFunc(Mouse);
  
  glutMainLoop();  

  return 1;
}
