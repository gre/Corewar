#include<X11/Xlib.h>
#include<X11/cursorfont.h>
#include<X11/Xatom.h>
#include<X11/Xutil.h>
#include<X11/keysym.h>
#include<X11/xpm.h>

#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include<time.h>


#include "graph.h"
#define N 50


void get_GC(Window win, GC *gc);

typedef struct
{
  int posx,posy;
  int tx,ty;
  int etat;
  Pixmap s;
  Pixmap smask;
  Pixmap sauvegarde;
}
sprite;

sprite S[N];                        /* tableau de sprite    */
int S_alloue[N];

/******************** variables globales ****************************/

Display *display;
int screen;
int display_width=0,display_height=0;
int X_fen,Y_fen;
int conecte=0;
int nbfen=0;
Window win;
XColor coul;
unsigned long couleur_dess;
XEvent report;                      /* structure des evenements */

GC gc;                              /* ID du contexte graphique */
unsigned int border_w=4; 
Cursor croix;
XSetWindowAttributes attribut;
Colormap pal;			    /* palette 			*/
Font f[3];			    /* font d'ecriture          */
XFontStruct *s[3];
XFontStruct *stemp;
Pixmap icone;
XImage * xi;
XPoint sommet[3];
int num_ecran=0;
Window ecran[11];
int app=0;
XWMHints  wm_hints;
XClassHint  class_hints;
XSizeHints  size_hints;

XTextProperty nom_fenetre,nom_icone;
char *nf="CoreWar par Gaetan Renaudeau"; // changer si vous voulez :-)
char *ni="mon_icone";          

int _X=0,_Y=0;

/******************************************************************/
/*                  icone de la fenetre en dur !!!                */
/******************************************************************/

#define icone_width 30
#define icone_height 30
static unsigned char icone_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 0x10, 0x00, 0x10, 0x10, 0x0c, 0x00,
   0x08, 0x20, 0x02, 0x00, 0x04, 0xc0, 0x01, 0x00, 0x04, 0x40, 0x00, 0x00,
   0x04, 0x70, 0xfc, 0x1f, 0x04, 0x48, 0x04, 0x10, 0x04, 0x44, 0x04, 0x10,
   0x04, 0x43, 0x04, 0x10, 0x84, 0x40, 0x04, 0x10, 0x68, 0x20, 0x04, 0x10,
   0x10, 0x10, 0x04, 0x10, 0xec, 0x0f, 0xfc, 0x1f, 0x02, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x02, 0x00, 0x81, 0x00, 0x01,
   0x80, 0x03, 0x01, 0x01, 0xc0, 0x03, 0x82, 0x00, 0xe0, 0x07, 0x44, 0x00,
   0xe0, 0x07, 0x48, 0x00, 0xf0, 0x0f, 0x30, 0x00, 0xf8, 0x1f, 0x50, 0x00,
   0xfc, 0x1f, 0x90, 0x00, 0xfe, 0x3f, 0x08, 0x01, 0xe0, 0x3f, 0x04, 0x02,
   0x00, 0x78, 0x04, 0x04, 0x00, 0x00, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00};

static char *iconet[]={"30 30 10 1",
".	c #760001",
"+	c #0017E6",
"@	c #FF281B",
"#	c #4296FF",
"$	c #FFAE00",
"%	c #00FFB1",
"&	c #00FAFF",
"*	c #41FF11",
"=	c #FBFD00",
"-	c #FFFDAB",
"------------------------------",
"-----##########---------------",
"----#############-------------",
"---##+++++++++++##------------",
"--###++++++++++++##-----------",
"-###++#########+++#-----------",
"-##++#---------#++#-----------",
"-##++#----------##------------",
"-##++##-----------------------",
"-##+++##----------------------",
"-##++++#----------------------",
"-##++++#-----..-----.-----..--",
"-##++++#----.@@.--..@@---..@@-",
"-##++++#---..@@.###.@@.--.@@@.",
"-##++++#---..@@####.@@.--.@@@.",
"--#++++#########++#.@@.--.@@@.",
"--#++++++######+++..@@.--.@@@.",
"---#+++++++++++++-..@@.--.@@@.",
"----#+++++++++++.-..@@.--.@@@.",
"-----------..@@@@...@@.--.@@@.",
"-----------..@@@@@..@@.--.@@@.",
"-----------..@@@@@..@@.--.@@@.",
"-----------..@@@@@..@@.--.@@@.",
"-----------..@@@@@..@@.--.@@@.",
"-----------..@@@@@.@@@.-..@@@.",
"------------.@@@@@@@@@@..@@@.-",
"-------------.@@@@@@@@@@@@@.--",
"--------------.@@@@@@@@@@@.---",
"---------------...........----",
"------------------------------"};


/**************************************************************************/

void  ChoisirModeDessin(int m)
{

switch(m)
{
case 0 : XSetFunction(display,gc,GXcopy);
        break;
case 1 : XSetFunction(display,gc,GXxor);
	break;
case 2 : XSetFunction(display,gc,GXinvert);
	break;
default : XSetFunction(display,gc,GXcopy); 

}  
}
void ChoisirCouleurDessin(unsigned long couleur)
{
  couleur_dess=couleur;
  XSetForeground(display,gc,couleur);
  XFlush(display);
}

void ChoisirEcran(int n)
{
  if (0<=n&&n<=NB_PIXMAP) num_ecran=n;
}

int InitialiserGraphique()
{


  if (!conecte)
    {
      if((display=XOpenDisplay(NULL))==NULL) {
	fprintf(stderr,"erreur : ne peut pas se connecter ");
	  fprintf(stderr,"au serveur %s\n",XDisplayName(NULL));
	  return (1);
      }
      if (conecte) return 0;  
      
      
      /* Recuperer les informations concernant l'ecran (root window) sur
	 lequel on va afficher notre application. Le numero de l'ecran
	 par defaut s'obtient a l'aide de la macro DefaultScreen(), la
	 taille a l'aide des macros DisplayWidth() et
	 DisplayHeight().*/
      
      screen = DefaultScreen(display);
      display_width = DisplayWidth(display,screen);
      display_height = DisplayHeight(display,screen);
      pal=DefaultColormap(display,screen);
      conecte=1;
      return 0;
    }
}

int CreerFenetre(int posx,int posy,int X,int Y)
{
  int i;
  Visual *v;
  if (!conecte) 
    {
      printf("initialisez d'abord le mode graphique\n");
      return (1);
    }
  
  if (nbfen==1) 
    {
      printf("une seule fentre on a dit !!!!\n");
      return(1);
    }
  
  
  ecran[0] = XCreateSimpleWindow(display, RootWindow(display, screen), posx, posy,
				 X, Y, border_w,
				 BlackPixel(display,screen),
				 WhitePixel(display,screen));
  
/* selection des evenements que l'on desirera traiter pour cette fenetre */
  
  XSelectInput(display, ecran[0], ExposureMask | KeyPressMask |
	       ButtonPressMask  );


  /* creation d'un contexte graphique pour cette fenetre,
     indispensable pour dessiner ou afficher du texte */
  
  
  get_GC(ecran[0], &gc);
  
  /* creation d'une fonte curseur */

  croix=XCreateFontCursor(display,XC_crosshair);
  XDefineCursor(display,ecran[0],croix);
	
 /* le contenu de la fenetre est persistant : le serveur le supporte-t-il ? */       
  
  attribut.backing_store=Always;
  XChangeWindowAttributes(display,ecran[0],CWBackingStore,&attribut);

  
  /* chargement d'une font par defaut pour l'ecriture de texte */

	// f[0]=XLoadFont(display,"-adobe-courier-medium-r-normal--12-120-75-75-m-70-iso8859-1");
	// f[1]=XLoadFont(display,"-adobe-courier-medium-r-normal--20-140-100-100-m-110-iso8859-1");
	//f[2]=XLoadFont(display,"-adobe-courier-medium-r-normal--34-240-100-100-m-200-iso8859-1");

	stemp=XLoadQueryFont(display,"-adobe-times-medium-r-*-*-12-*-*-*-*-*-*-*");
	if (stemp==NULL) stemp=XLoadQueryFont(display,"fixed");
	f[0]=stemp->fid;
	s[0]=stemp;

	stemp=XLoadQueryFont(display,"-adobe-times-medium-r-*-*-20-*-*-*-*-*-*-*");
	if (stemp==NULL) stemp=XLoadQueryFont(display,"fixed");
	f[1]=stemp->fid;
	s[1]=stemp;

	stemp=XLoadQueryFont(display,"-adobe-times-medium-r-*-*-30-*-*-*-*-*-*-*");
	if (stemp==NULL) stemp=XLoadQueryFont(display,"fixed");
	f[2]=stemp->fid;
	s[2]=stemp;

	
     XSetFont(display,gc,f[1]);


  
  /* creation des pixmaps pour la sauvegarde d'images */
for(i=1;i<=NB_PIXMAP;i++)
{ 
	ecran[i]=XCreatePixmap( display , DefaultRootWindow(display), X , Y , DefaultDepth(display,screen) );
} 
 /* variables globales pour la taille de la fenetre */
  
  X_fen=X;
  Y_fen=Y;

  //  icone=XCreateBitmapFromData(display,ecran[0],icone_bits,icone_width,icone_height);
  
  XCreatePixmapFromData(display,ecran[0],iconet,&icone,&icone,NULL);

  //wm_hints=XAllocWMHints();
  //class_hints=XAllocClassHint();
  //size_hints=XAllocSizeHints();
  
  XStringListToTextProperty(&nf,1,&nom_fenetre);
  XStringListToTextProperty(&ni,1,&nom_icone);
  wm_hints.initial_state=NormalState;
  wm_hints.input=True;
  wm_hints.icon_pixmap=icone;
  wm_hints.flags=StateHint|IconPixmapHint|InputHint;
  size_hints.x=posx;
  size_hints.y=posy;
  size_hints.flags=USPosition;

  XSetWMProperties(display,ecran[0],&nom_fenetre,&nom_icone,NULL,0,NULL,&wm_hints,NULL);
  XSetWMNormalHints(display,ecran[0],&size_hints);
  XSetIconName(display,ecran[0],"Super");
  //  XStoreName(display,ecran[0],"D.M Entertainment");
  XMapWindow(display,ecran[0]);
  //  v=DefaultVisual(display,screen);
  //printf("classe = %d\n",v->class);
  //printf("nb cellule : %d\n",DisplayCells(display,screen));
  //printf("%ld\n",couleur_nom("red"));
  //printf("%ld\n",couleur_composante(31,0,0));
  do 
    {
    XWindowEvent(display,ecran[0],ExposureMask,&report);
    }
  while(report.xexpose.count);
  nbfen=1;

  
  return 0;
}

void ChoisirTitreFenetre(char *s)
{
XStoreName(display,ecran[0],s );
}
void ChoisirCurseur(unsigned int forme)
{
XFreeCursor(display,croix);
croix=XCreateFontCursor(display,forme);
XDefineCursor(display,ecran[0],croix);

}


/*----------------------*/

void get_GC(Window win, GC *gc)

{
  unsigned long valuemask = 0;                /* Ignore XGCvalues et prend les
					         valeurs par defaut */
  XGCValues values;

  /* creation d'un contexte graphique par defaut */
  
  *gc = XCreateGC(display, ecran[0], valuemask, &values);


  /* specification d'un background noir, puisque par defaut on fait du
     blanc sur du blanc */
  //XSetBackground(display,*gc,154);
 
  XSetForeground(display, *gc, BlackPixel(display, screen));
  XSetGraphicsExposures(display,*gc,False);	 
	/* inutile d'enfler la file d'evenements avec les evenements
	Noexpose et GraphicsExpose */
  
//  XSetForeground(display, *gc, 150);
  //values.line_width=10;
  //XChangeGC(display,*gc,GCLineWidth,&values);
  //XSetForeground(display, *gc,1000);
}

int Maxx()
{
return display_width;
}

int Maxy()
{
return display_height;
}

void DessinerPixelC(int x,int y,unsigned long couleur)
{
  XSetForeground(display,gc,couleur);
  XDrawPoint(display,ecran[num_ecran],gc,x,y);
  XSetForeground(display,gc,couleur_dess);
  XFlush(display);
}

void DessinerPixel(int x,int y)
{
  XDrawPoint(display,ecran[num_ecran],gc,x,y);
  XFlush(display);
}

void DessinerSegmentC(int x,int y,int xx,int yy,unsigned long couleur)
{
  XSetForeground(display,gc,couleur);
  XDrawLine(display,ecran[num_ecran],gc,x,y,xx,yy);

  XSetForeground(display,gc,couleur_dess);
  XFlush(display);
}
void DessinerSegment(int x,int y,int xx,int yy)
{
  XDrawLine(display,ecran[num_ecran],gc,x,y,xx,yy);
  XFlush(display);
}

void FermerGraphique()
{ 
  int i;
  for (i=0;i<3;i++) XUnloadFont(display,f[i]);
  XFreePixmap(display,ecran[1]);
  XFreePixmap(display,ecran[2]);
  XFreeGC(display,gc);
  XCloseDisplay(display);
  nbfen=0;
  conecte=0;
  app=0;
}

unsigned long CouleurParNom(char * nom)
{
  XParseColor(display,pal,nom,&coul);
  XAllocColor(display,pal,&coul);
  return coul.pixel;
}

void DessinerRectangleC(int x,int y,int l,int h,unsigned long c)
{
  XSetForeground(display,gc,c);
  XDrawRectangle(display,ecran[num_ecran],gc,x,y,l,h);
  
  XSetForeground(display,gc,couleur_dess);
  XFlush(display);
}
void DessinerRectangle(int x,int y,int l,int h)
{
  
  XDrawRectangle(display,ecran[num_ecran],gc,x,y,l,h);
  XFlush(display);
  
}
void RemplirRectangle(int x,int y,int l,int h)
{
  XFillRectangle(display,ecran[num_ecran],gc,x,y,l,h);
  XFlush(display);
}

void DessinerArcC(int x,int y,int l,int h,int angle1,int angle2,unsigned long c)
{
  XSetForeground(display,gc,c);
  XDrawArc(display,ecran[num_ecran],gc,x,y,l,h,angle1*64,angle2*64);
  XSetForeground(display,gc,couleur_dess);
 XFlush(display);
}

void DessinerArc(int x,int y,int l,int h,int angle1,int angle2)
{

  XDrawArc(display,ecran[num_ecran],gc,x,y,l,h,angle1*64,angle2*64);
  XFlush(display);

}

void RemplirArc(int x,int y,int l,int h,int angle1,int angle2)
{
  XFillArc(display,ecran[num_ecran],gc,x,y,l,h,angle1*64,angle2*64);
  XFlush(display);
}


void DessinerSegments(Point * sommets,int nb_sommet)
{
XDrawLines(display,ecran[num_ecran],gc,sommets, 
              nb_sommet, CoordModeOrigin);
}

void RemplirPolygone(Point * sommets,int nb_sommet)
{
XFillPolygon(display, ecran[num_ecran],gc, sommets,
              nb_sommet, Complex,CoordModeOrigin);
}
void RemplirTriangle(int x,int y,int xx,int yy,int xxx,int yyy)
{
sommet[0].x=(short)x;
sommet[0].y=(short)y;
sommet[1].x=(short)xx;
sommet[1].y=(short)yy;
sommet[2].x=(short)xxx;
sommet[2].y=(short)yyy;
XFillPolygon(display,ecran[num_ecran],gc,sommet,3,Convex,CoordModeOrigin);
XFlush(display);
}

int ToucheEnAttente()
{
  if (app==1) return 1;
  if (XCheckWindowEvent(display,ecran[0],KeyPressMask,&report)) 
    {
      app=1;
      return 1;
    }
  //app=0;
  return 0;
}


KeySym Touche()
{
/* fonction bloquante */
int  buffer;
int c;
KeySym t;
 if (app==0) XWindowEvent(display,ecran[0],KeyPressMask,&report);
 //printf("%d\n",report.xkey.keycode);
 c=XLookupString(&(report.xkey), NULL, sizeof(KeySym), &t,NULL);

//printf("%s\n",XKeysymToString(XKeycodeToKeysym(report.xkey.keycode)));
//printf("%d:%s\n",c,buffer);
//if (t==XK_Up) printf("oui\n");
//if (t==XK_Up) buffer=0x
 app=0;
 return t;
}

void EffacerEcran(unsigned long c)
{
  if (num_ecran==0){
    XSetWindowBackground(display,ecran[num_ecran],c);
    XClearWindow(display,ecran[num_ecran]);
    
  }
  else 
    { 
      XSetForeground(display,gc,c);
      XFillRectangle(display,ecran[num_ecran],gc,0,0,X_fen,Y_fen);
      XSetForeground(display,gc,couleur_dess);
      
  }
  XFlush(display);
  
}

void EcrireTexteC(int x,int y,char *texte,int mode , unsigned long c)
{
  if (mode<0||mode>2) return;
  XSetFont(display,gc,f[mode]);
  XSetForeground(display,gc,c);
  XDrawString(display,ecran[num_ecran],gc,x,y,texte,strlen(texte));
  XSetForeground(display,gc,couleur_dess);
  XFlush(display);
}

void EcrireTexte(int x,int y,char *texte,int mode)
{
  if (mode<0||mode>2) return;
  XSetFont(display,gc,f[mode]);
  XDrawString(display,ecran[num_ecran],gc,x,y,texte,strlen(texte));
  XFlush(display);

}

void SauverImage(char * file,int x,int y,int l,int h)
{
  XEvent report;
  Pixmap p1=0;
  p1=XCreatePixmap( display , DefaultRootWindow(display), X_fen , Y_fen , DefaultDepth(display,screen) );
  XCopyArea(display,ecran[num_ecran],p1,gc,x,y,l,h,0,0);
  XpmWriteFileFromPixmap(display,file,p1,0,NULL);
  if (p1) XFreePixmap(display,p1);
  //XFlush(display);
  while(XCheckWindowEvent(display,ecran[0],ExposureMask,&report));
  while(XCheckTypedEvent(display,GraphicsExpose,&report));
  while(XCheckTypedEvent(display,NoExpose,&report));
}

void ChargerImage(char *file,int x,int y,int xx,int yy,int l,int h)
{
  XEvent report;
  Pixmap p1=0,p1mask=0;
  XpmReadFileToPixmap(display,ecran[num_ecran],file,&p1,&p1mask,NULL);
  XSetClipMask(display,gc,p1mask);
  XSetClipOrigin(display,gc,x,y);
  XCopyArea(display,p1,ecran[num_ecran],gc,xx,yy,l,h,x,y);
  
  XSetClipMask(display,gc,None);
  if (p1) XFreePixmap(display,p1);
  if (p1mask) XFreePixmap(display,p1mask);
  
//XFlush(display);
  
  
  while(XCheckWindowEvent(display,ecran[0],ExposureMask,&report));
  while(XCheckTypedEvent(display,GraphicsExpose,&report));
  while(XCheckTypedEvent(display,NoExpose,&report));    
  
}

void ChargerImageFond(char *file)
{ 
  XSetWindowAttributes att;
  Pixmap pbackground=0,pmask=0;
  int i=num_ecran;
  if (i<0 || i>NB_PIXMAP) return ;
  
  switch(i)
  {
  case 0 :
  XpmReadFileToPixmap(display,ecran[i],file,&pbackground,&pmask,NULL);
  
  att.background_pixmap=pbackground;
  XChangeWindowAttributes(display,ecran[i],CWBackPixmap,&att);
  XClearWindow(display,ecran[i]);
  
  if (pbackground) XFreePixmap(display,pbackground);
  if (pmask) XFreePixmap(display,pmask);
  XFlush(display);
  break;
  default:
  ChargerImage(file,0,0,0,0,X_fen,Y_fen);
 } 
}

int TailleChaineEcran(char *t,int mode)
{
  if (mode<0||mode>2) return -1;
  else return XTextWidth(s[mode],t,strlen(t));
}
int TailleSupPolice(int mode)
{
  if (mode<0||mode>2) return -1;
  return s[mode]->ascent;
}

int TailleInfPolice(int mode)
{
  if (mode<0||mode>2) return -1;
  return s[mode]->descent;
}


void CopierZone(int src,int dst,int ox,int oy,int L,int H,int dx,int dy)
{
  XEvent report;
  XCopyArea(display,ecran[src],ecran[dst],gc,ox,oy,L,H,dx,dy);
  XFlush(display);
  while(XCheckWindowEvent(display,ecran[0],ExposureMask,&report));
  while(XCheckTypedEvent(display,GraphicsExpose,&report));
  while(XCheckTypedEvent(display,NoExpose,&report));
}


void CacherFenetre()
{
  XUnmapWindow(display,ecran[0]);
  XFlush(display);
};

void AfficherFenetre(void)
{
  XMapWindow(display,ecran[0]);
  XFlush(display);
}


unsigned long CouleurParComposante(unsigned char r, unsigned char v,unsigned char b)
{
  XColor c;
  c.red=r;
  c.red<<=8;
  c.green=v;
  c.green<<=8;
  c.blue=b;
  c.blue<<=8;
  XAllocColor(display,DefaultColormap(display,screen),&c);
  return c.pixel;
}


unsigned long Microsecondes()
{
        struct timeval t;
        gettimeofday(&t, NULL);
        return t.tv_usec+t.tv_sec*1000000;
}


/*
int souris(int *X,int *Y)
{
  XEvent r;
  if (XCheckWindowEvent(display,ecran[0],PointerMotionMask,&r))
    {*X=r.xmotion.x;*Y=r.xmotion.y;return 1;}
  return 0;
}
*/



void SourisPosition()
{
  int x,y,rx,ry;
  unsigned int etat;
  Window a,b;
  XQueryPointer(display, ecran[0], &a, &b, &rx, &ry, &x, &y, &etat);
  _X=x;_Y=y;
}

int longueur()
{
  return XQLength(display);
}

int SourisCliquee()
{
  XEvent r;
  if (XCheckWindowEvent(display,ecran[0],ButtonPressMask,&r))
    {_X=r.xbutton.x;_Y=r.xbutton.y;return 1;}
  return 0;
}

//int souris_cliquee(void)
//{
//  XEvent r;
//  return (XCheckWindowEvent(display, ecran[0], ButtonPressMask, &r)==True);
//}


int SpriteLibre()
{
  int i;
  for(i=0;i<N;i++) if (S_alloue[i]==0) return i;
  return -1;
}

int ChargerSprite(char *file)
{
  Pixmap p1=0,p1mask=0;
  int nu;
  XpmAttributes a;
  a.valuemask=XpmSize;
  nu=SpriteLibre();
if (nu==-1) return -1;
 S_alloue[nu]=1;
 XpmReadFileToPixmap(display,ecran[num_ecran],file,&p1,&p1mask,&a);
 S[nu].posx=-1;
 S[nu].posy=-1;
 S[nu].etat=0;
 S[nu].tx=a.width;
 S[nu].ty=a.height;
 S[nu].s=p1;
 S[nu].smask=p1mask;
S[nu].sauvegarde=0;
 S[nu].etat=0;
 
 return nu+1;
}

void DeplacerSprite(int n,int x,int y)
{
  Pixmap p;
  n--;
  if ((S[n].etat)==0)
      {
	p=XCreatePixmap( display , DefaultRootWindow(display), S[n].tx ,S[n].ty  , DefaultDepth(display,screen) );
	XCopyArea(display,ecran[num_ecran],p,gc,x,y,S[n].tx,S[n].ty,0,0);
	S[n].sauvegarde=p;
	
	
      }
  
    /* restitue la zone */
  
  XSetClipMask(display,gc,None);
  if (S[n].etat!=0) XCopyArea(display,S[n].sauvegarde,ecran[num_ecran],gc,0,0,S[n].tx,S[n].ty,S[n].posx,S[n].posy);
  XCopyArea(display,ecran[num_ecran],S[n].sauvegarde,gc,x,y,S[n].tx,S[n].ty,0,0);
  if (S[n].smask!=0) {
    XSetClipMask(display,gc,S[n].smask);
    XSetClipOrigin(display,gc,x,y);
  }
  
  XCopyArea(display,S[n].s,ecran[num_ecran],gc,0,0,S[n].tx,S[n].ty,x,y);
  S[n].posx=x;
  S[n].posy=y;
  S[n].etat=1;
  XFlush(display);
  XSetClipMask(display,gc,None);

}

void EffacerSprite(int n)
{
  n--;
  if (S[n].etat!=0) XCopyArea(display,S[n].sauvegarde,ecran[num_ecran],gc,0,0,S[n].tx,S[n].ty,S[n].posx,S[n].posy);
  S[n].etat=0;
}

void LibererSprite(int n)
{
  n--;
  if (S[n].s>0) XFreePixmap(display,S[n].s);
  if (S[n].smask>0) XFreePixmap(display,S[n].smask);
  if (S[n].sauvegarde>0) XFreePixmap(display,S[n].sauvegarde);
  S_alloue[n]=0;
}

