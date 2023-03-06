/*
Projekt lasu bambusowego

Nalezy zmienic lokalizacje pliku biblioteka.h oraz tekstur recznie w kodzie

Sterowanie:
w, s, a, d: poruszanie kamera

Sceny:
i: Scena lasu
o: Pojedynczy bambus
p: Pojedynczy segment bambusa

Las:
n: wygenerowanie nowego lasu
m: stworzenie lasu domyslnego

Testownie:
+ -: zmiana grubosci pedu bambusa
9, 6: zmiana podstawy graniastoslupa (segmentu)
8, 5: zmiana wysokosci segmentow
7, 4: zmiana ilosci domyslnej segmentow w bambusie
*/
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <LOKALIZACJA\biblioteka.h>
#include <ctime>
using namespace std;

//Zmienne globalne
static int IloscPoly = 6;
static double PromienPedu = 0.1;
static double WysokoscSegmentu = 2;
static int IloscSegmentow = 5;

//Tablice rand
static bool** tablicaRandPozycja = 0;
static int** tablicaRandWysokosc = 0;

//Wymiary pola bambusa
static int PoleBambX = 30;
static int PoleBambZ = 90;

//Wybor sceny
static int NrSceny = 0;

//Kamera
float angleGlob=0.0;
float lxGlob=0.0f,lzGlob=-1.0f,lyGlob=1.0f;
float xGlob=0.0f,zGlob=5.0f;

//Tablica z teksturami
#define LICZBA_OB_TEXTUR 4
GLuint obiektyTextur [LICZBA_OB_TEXTUR];
char *plikiTextur[] = {"LOKALIZACJA/Bambus.tga", "LOKALIZACJA/Ziemia.tga", "LOKALIZACJA/Kamienie.tga", "LOKALIZACJA/Scianka.tga"};

void zwalnianiePamieci() //Zwalnianie pamieci tablic rand
{
    for (int i = 0; i < PoleBambX; i++)
    {
        delete[] tablicaRandPozycja[i];
    }
    delete[] tablicaRandPozycja;
    tablicaRandPozycja = 0;
    for (int i = 0; i < PoleBambX; i++)
    {
        delete[] tablicaRandWysokosc[i];
    }
    delete[] tablicaRandWysokosc;
    tablicaRandWysokosc = 0;
}

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void rysujTrojkat(double podstawa)
{
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3d(0,0,0);
    glVertex3d(-podstawa, 0, PromienPedu);
    glVertex3d(podstawa, 0, PromienPedu);
    glEnd();
}

static void rysujSciane(double podstawa)
{
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,1.0);
    glVertex3d(-podstawa, WysokoscSegmentu, PromienPedu);
    glTexCoord2d(0.0,0.0);
    glVertex3d(-podstawa, 0, PromienPedu);
    glTexCoord2d(1.0,0.0);
    glVertex3d(podstawa, 0, PromienPedu);
    glTexCoord2d(1.0,1.0);
    glVertex3d(podstawa, WysokoscSegmentu, PromienPedu);
    glEnd();
}

static void rysujWycinek() //Rysowanie wycinka graniastoslupa
{
    double kat = tan((180.0/IloscPoly)*(M_PI/180.0));
    double podstawa = kat * PromienPedu;
    glPushMatrix();
    rysujTrojkat(podstawa);
    rysujSciane(podstawa);
    glTranslated(0,WysokoscSegmentu,0);
    rysujTrojkat(podstawa);
    glPopMatrix();
}

static void rysujSegment() //Rysowanie segmentu (graniastoslupa)
{
    for (int i = 0; i < IloscPoly; i++)
    {

        glRotated(360.0/IloscPoly, 0,1,0);
        rysujWycinek();
    }
}

static void rysujBambusa() //Rysowanie pojedynczego pedu
{
    glBindTexture (GL_TEXTURE_2D, obiektyTextur[0]);
    glPushMatrix();
    for(int i = 0; i < IloscSegmentow; i++)
    {
        rysujSegment();
        glTranslated(0, WysokoscSegmentu, 0);

    }
    glPopMatrix();
}

static void rysujBambusaRand(int x, int y) //Rysowanie pojedynczego pedu z uwzglednieniem losowosci
{
    glBindTexture (GL_TEXTURE_2D, obiektyTextur[0]);
    srand(time(NULL));
    glPushMatrix();
    for(int i = 0; i < (IloscSegmentow + tablicaRandWysokosc[x][y]); i++)
    {
        rysujSegment();
        glTranslated(0, WysokoscSegmentu, 0);

    }
    glPopMatrix();
}

static void randomLas() //Ustalenie w ktorych pozycjach znajda sie bambusy oraz ich wysokosc
{
    srand(time(NULL));
    tablicaRandPozycja = new bool*[PoleBambX];
    for (int i = 0; i < PoleBambX; i++)
    {
        tablicaRandPozycja[i] = new bool[PoleBambZ];
        for(int j = 0; j < PoleBambZ; j++)
            tablicaRandPozycja[i][j] = rand()%2;
    }

    tablicaRandWysokosc = new int*[PoleBambX];
    for (int i = 0; i < PoleBambX; i++)
    {
        tablicaRandWysokosc[i] = new int[PoleBambZ];
        for(int j = 0; j < PoleBambZ; j++)
            if (tablicaRandPozycja[i][j])
                tablicaRandWysokosc[i][j] = rand()%4;
    }
}

static void rysujLas(int przesuniecieX, int przesuniesieZ) //Rysowanie lasu
{
    glTranslated(przesuniecieX, 0, -przesuniesieZ);
    glTranslated(-(PoleBambX/2),0,0);

    if(tablicaRandPozycja)
    {
        for (int i = 0; i < PoleBambZ; i++)
        {
            for (int j = 0; j < PoleBambX; j++)
            {
                if( tablicaRandPozycja[j][i] )
                    rysujBambusaRand(j,i);
                glTranslated(0.5,0,0);
            }
            glTranslated(-(PoleBambX/2),0,0.5);
        }
    } else
    {
    for (int i = 0; i < PoleBambZ; i++)
        {
            for (int j = 0; j < PoleBambX; j++)
            {
                rysujBambusa();
                glTranslated(0.5,0,0);
            }
            glTranslated(-(PoleBambX/2),0,0.5);
        }
    }
    glBindTexture (GL_TEXTURE_2D, 0);
}

static void rysujZiemie(int wymiarX, int wymiarZ) //Rysowanie ziemi pod bambusami
{
    glBindTexture (GL_TEXTURE_2D, obiektyTextur[1]);
    glTranslated(-(wymiarX/2),0,-(wymiarZ/2));
    for (int i = 0; i < wymiarX; i++)
    {
        for (int j = 0; j < wymiarZ; j++)
        {
            glBegin(GL_QUADS);
            glTexCoord2d(0.0,0.0);
            glVertex3d(0,0,0);
            glTexCoord2d(0.0,1.0);
            glVertex3d(0,0,1);
            glTexCoord2d(1.0,1.0);
            glVertex3d(1,0,1);
            glTexCoord2d(1.0,0.0);
            glVertex3d(1,0,0);
            glEnd();
            glTranslated(1,0,0);
        }
        glTranslated(-wymiarX, 0, 1);
    }
}

void rysujKamienie(double z) //Rysowanie kamieni na drodze pomiedzy bambusami
{
    glPushMatrix();
    glBindTexture (GL_TEXTURE_2D, obiektyTextur[2]);
    for(double i = 0; i < z; i += 1)
    {
        glBegin(GL_QUADS);
        glTexCoord2d(0.0,1.0);
        glVertex3d(-1.5, 0.0, -i-1.5);
        glTexCoord2d(0.0,0.0);
        glVertex3d(-1.5, 0.0, -i);
        glTexCoord2d(1.0,0.0);
        glVertex3d(1.5, 0.0, -i);
        glTexCoord2d(1.0,1.0);
        glVertex3d(1.5, 0.0, -i-1.5);
        glEnd();
    }
    glPopMatrix();
}

void rysujBarierki(double z, double wysBar) //Rysowanie barierki oddzialajaej chodnik od lasu
{
    glPushMatrix();
    glBindTexture (GL_TEXTURE_2D, obiektyTextur[3]);
    for(double i = 0; i < z; i += 1)
    {
        glBegin(GL_QUADS);
        glTexCoord2d(1.0,1.0);
        glVertex3d(0, wysBar, -i);
        glTexCoord2d(0.0,1.0);
        glVertex3d(0, wysBar, -i-1.5);
        glTexCoord2d(0.0,0.0);
        glVertex3d(0, 0.0, -i-1.5);
        glTexCoord2d(1.0,0.0);
        glVertex3d(0, 0.0, -i);
        glEnd();
    }
    glPopMatrix();
}
void rysujSciezke(double z, double wysBar) //Rysowanie sciezki z drogi i barierek
{
    rysujKamienie(z);
    glTranslated(2,0,0);
    rysujBarierki(z, wysBar);
    glTranslated(-4,0,0);
    glTranslated(0,wysBar,0);
    glRotated(180, 0,0,1);
    rysujBarierki(z, wysBar);
    glBindTexture (GL_TEXTURE_2D, 0);
}

void lasBambusowy() //Rysowanie lasu i sciezki
{
    glPushMatrix();
        glTranslated(0,-4,-12);

        gluLookAt(	xGlob, 1.0, zGlob, //Przemieszczanie kamery
                xGlob+lxGlob, 1.0,  zGlob+lzGlob,
                0.0f, 1.0f,  0.0f);

        glColor3d(0.5,1,0.83); //Rosowanie podlogi pod ziemia
        glPushMatrix();
            glBegin(GL_TRIANGLE_FAN);
            glVertex3d(-100,-0.1,100);
            glVertex3d(100,-0.1,100);
            glVertex3d(100,-0.1,-100);
            glVertex3d(-100,-0.1,-100);
            glEnd();
        glPopMatrix();

        glColor3d(1,1,1);
            glPushMatrix();
            rysujZiemie(100,100);
        glPopMatrix();

        glPushMatrix();
            glTranslated(7.5,0,0);
            glColor3d(1,1,1);
            rysujLas(10, 20);
        glPopMatrix();

        glPushMatrix();
            glTranslated(7.5,0,0);
            glColor3d(1,1,1);
            rysujLas(-10, 20);
        glPopMatrix();

        glPushMatrix();
            glColor3d(1,1,0);
            glTranslated(0,5,70);
            glutSolidCube(10);
        glPopMatrix();

        glPushMatrix();
            glTranslated(0,0.1,17);
            rysujSciezke(100.0, 2.0);
        glPopMatrix();
    glPopMatrix();
}

void przykladBambus(double a) //Przykladowy pojedynczy bambus
{
    glPushMatrix();
    glBindTexture (GL_TEXTURE_2D, 0);
    glColor3d(1,0.5,0.5);
    gluLookAt(	xGlob, 1.0, zGlob, //Przemieszczanie kamery
                xGlob+lxGlob, 1.0,  zGlob+lzGlob,
                0.0f, 1.0f,  0.0f);
    glTranslated(0,-5,-4);
    glRotated(10,1,0,0);
    glRotated(a,0,1,0);
    rysujBambusa();
    glPopMatrix();
}

void przykladSegment(double a) //Przykladowy segment
{
    glPushMatrix();
    glBindTexture (GL_TEXTURE_2D, 0);
    glColor3d(1,0.5,0.5);
    gluLookAt(	xGlob, 1.0, zGlob, //Przemieszczanie kamery
                xGlob+lxGlob, 1.0,  zGlob+lzGlob,
                0.0f, 1.0f,  0.0f);
    glTranslated(0,0,3);
    glRotated(10,1,0,0);
    glRotated(a,0,1,0);
    rysujSegment();
    glPopMatrix();
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch(NrSceny)
    {
        case 0:
            lasBambusowy();
            break;

        case 1:
            przykladBambus(a);
            break;

        case 2:
            przykladSegment(a);
            break;
    }
    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    switch (key)
    {

        case '+':
            PromienPedu += 0.1;
            break;

        case '-':
            PromienPedu -= 0.1;
            break;

        case '9':
            IloscPoly++;
            cout << IloscPoly << endl;
            break;

        case '6':
            if(IloscPoly > 4)
            IloscPoly--;
            cout << IloscPoly << endl;
            break;

        case '8':
            WysokoscSegmentu += 0.2;
            cout << WysokoscSegmentu << endl;
            break;

        case '5':
            if (WysokoscSegmentu > 0.3)
            WysokoscSegmentu -= 0.2;
            cout << WysokoscSegmentu << endl;
            break;

        case '7':
            IloscSegmentow++;
            break;

        case '4':
            if(IloscSegmentow > 1)
                IloscSegmentow--;
            break;

        case 'n':
            randomLas();
            break;

        case 'm':
            zwalnianiePamieci();
            break;

        case 'i':
            NrSceny = 0;
            break;

        case 'o':
            NrSceny = 1;
            break;

        case 'p':
            NrSceny = 2;
            break;
        case 'a' :
			angleGlob -= 0.01f;
			lxGlob = sin(angleGlob*3.14);
			lzGlob = -cos(angleGlob*3.14);
			break;

		case 'd' :
			angleGlob += 0.01f;
			lxGlob = sin(angleGlob*3.14);
			lzGlob = -cos(angleGlob*3.14);
			break;

		case 'w' :
			xGlob += lxGlob;
			zGlob += lzGlob;
			break;

		case 's' :
			xGlob -= lxGlob;
			zGlob -= lzGlob;
			break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 5.0f, 50.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */

int main(int argc, char *argv[])
{
    randomLas();
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Las Bambusowy");

    glEnable(GL_TEXTURE_2D); //Wczytywanie tekstur
    glGenTextures(LICZBA_OB_TEXTUR, obiektyTextur);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    for (int i = 0; i < LICZBA_OB_TEXTUR; i++)
    {
        GLubyte *pBytes;
        GLint iWidth, iHeight, iComponents;
        GLenum eFormat;
        glBindTexture(GL_TEXTURE_2D, obiektyTextur[i]);
        pBytes = glploadtga(plikiTextur[i], &iWidth, &iHeight, &iComponents, &eFormat);
        gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes); // zwolnienie pamiêci
        cout << iWidth << ", " << iHeight << endl;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(0.9,0.9,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLfloat fogColor[4] = {0.8f,1.0f,0.8f,1.0f};
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, 0.0f);
    glFogf(GL_FOG_END, 20.0f);

    glEnable(GL_FOG);



    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);



    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glutMainLoop();
    zwalnianiePamieci();
    glDeleteTextures (LICZBA_OB_TEXTUR, obiektyTextur);
    return EXIT_SUCCESS;
}
