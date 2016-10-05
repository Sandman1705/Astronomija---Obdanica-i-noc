/*
Astronimija: Smena dana i noci
Autor: Mirko Brkusanin, Matematicki fakultet, Beograd 2014. godine
*/

#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

const double RADIAN=0.01745329251994329576923690768489;
const double MAX_NAGIB=23.5;
const double PROLECE = 0.0;
const double LETO = 91.25; // broj dana u godisnjem dobu
const double JESEN = 182.5;
const double ZIMA = 273.75;

double camera_ori_x=1.0, camera_ori_y=0.0, camera_ori_z=1.0;
double camera_pos_x=3.0, camera_pos_y=0.0, camera_pos_z=0.0;
double sun_x=1.0,sun_y=0.0,sun_z=0.0;
double suns_x=0.0,suns_y=1.0;
double sun_angle=0.0; // Ugao deklinacije
double camera_angle = 0.0; // Pocetni ugao kamere (od 0 do 360)
double camera_move = 1.0; // Pomeraj ugla kamere
double latitude = 0.0; //Pocetna geografska sirina
double latitude_move = 1.0; // Pomeraj ugla geografske sirine
double latitude_move2 = 0.5;
double earth_radius = 1.0; //Poluprecnik Zemlje
int earth_size = 0;
double sun_radius = 0.15; // Poluprecnik Sunca
int sun_size = 0;
double udaljenost = 1.6; // Udaljenost Sunca od Zemlje
int udaljenost_plus = 0;
double day = 0.0; //Dan u godini
char* mesec=(char*)"mart      ";
int dan=21;
bool display_sun=true;
bool display_sunbeam=true;
bool display_net=true;
bool display_greenlines=true;
bool display_redline=true;
bool display_sunreflection=true;
bool display_textsunangle=true;
GLfloat light_position[] = {(GLfloat)sun_x,(GLfloat)sun_y,(GLfloat)sun_z,0.0}; // Pozicija sunca
GLfloat mat_specular[] = {1.0,1.0,1.0,1.0};
GLfloat mat_specular_off[] = {0.0,0.0,0.0,1.0};
GLfloat mat_shininess[] = {100.0};

void init();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void izracunaj_dan();
void print (double x, double y, double z, char *s, void*font);

int main(int argc, char**argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600,600);
    glutInitWindowPosition(75,75);
    glutCreateWindow("Astronomija: Obdanica i noc");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

void init()
{
    //GLfloat mat_specular[] = {1.0,1.0,1.0,1.0};
    //GLfloat mat_shininess[] = {100.0};
    //GLfloat light_position[] = {x,y,z,0.0};

    glClearColor (0.0,0.0,0.0,0.0);
    glShadeModel (GL_SMOOTH);

    glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv (GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
}

void display()
{
    // Promeni pogled (rotacija kamere)
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt (camera_pos_x, camera_pos_y, camera_pos_z, 0.0, 0.0,0.0,camera_ori_x,camera_ori_y,camera_ori_z);

    glMatrixMode(GL_PROJECTION);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Odsjaj sunca
    if (display_sunreflection)
        glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular);
    else
        glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular_off);

    //Pozicija sunca / izvor svetlosti
    light_position[0] = sun_x;
    light_position[1] = sun_y;
    light_position[2] = sun_z;
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);

    // Ucitavanje teksta
    char* text_credits="Autor: Mirko Brkusanin, Matematicki fakultet, Beograd 2014. godine\0";
    char text1[50];
    sprintf(text1,"Geografska sirina: %3.1f",latitude);
    char text2[50];
    char text3[50];
    sprintf(text3,"Datum: %d. %s",dan,mesec);
    char text4[50];
    sprintf(text4,"Ugao deklinacije: %f",sun_angle);
    char* text5="Kontrole: 1=kamera levo; 2=kamera desno; 3=geo.sir +1; 4=geo.sir -1; 5=dan +1; 6=dan -1; E=geo.sir +0.5; R=geo.sir -0.5\0";
    char* text6="Kontrole (prikaz): Z=Sunce; X=Suncev zrak; C=Mreza; V=Ekv.,Pol.,Pov.; B=Trenutna geo.sir; N=Odsjaj Sunca\0";
    char* text7="Kontrole (nebeska tela): A=veci poluprecnik; S=manji poluprecnik; D=odalji Sunce; F=priblizi Sunce\0";

    // Crtanje: Linija geografske sirine (crvena linija)
    if (display_redline)
    {
    glPushMatrix();
        double latitude_radius = cos(abs(latitude)*RADIAN) * earth_radius;
        double translate_y = sin(latitude*RADIAN) * earth_radius;
        glColor3ub(255,0,0);
        glTranslatef(0.0,translate_y,0.0);
        glutWireTorus(0.015,latitude_radius+0.0005,40,50);
    glPopMatrix();
    }

    // Crtanje: Zelene linije (ekvator, polarnici i povratnici)
    if (display_greenlines)
    {
    double lat2;
    double translate_y2;
    glColor3ub(0,150,0);
    glPushMatrix();
        lat2 = cos(0.0*RADIAN) * earth_radius;
        translate_y2 = sin(0.0*RADIAN) * earth_radius;
        glTranslatef(0.0,translate_y2,0.0);
        glutWireTorus(0.0075,lat2+0.0005,40,50);
    glPopMatrix();
    glPushMatrix();
        lat2 = cos(23.5*RADIAN) * earth_radius;
        translate_y2 = sin(23.5*RADIAN) * earth_radius;
        glTranslatef(0.0,translate_y2,0.0);
        glutWireTorus(0.005,lat2+0.0005,40,50);
    glPopMatrix();
    glPushMatrix();
        lat2 = cos(-23.5*RADIAN) * earth_radius;
        translate_y2 = sin(-23.5*RADIAN) * earth_radius;
        glTranslatef(0.0,translate_y2,0.0);
        glutWireTorus(0.005,lat2+0.0005,40,50);
    glPopMatrix();
    glPushMatrix();
        lat2 = cos(66.5*RADIAN) * earth_radius;
        translate_y2 = sin(66.5*RADIAN) * earth_radius;
        glTranslatef(0.0,translate_y2,0.0);
        glutWireTorus(0.005,lat2+0.0009,40,50);
    glPopMatrix();
    glPushMatrix();
        lat2 = cos(-66.5*RADIAN) * earth_radius;
        translate_y2 = sin(-66.5*RADIAN) * earth_radius;
        glTranslatef(0.0,translate_y2,0.0);
        glutWireTorus(0.005,lat2+0.0009,40,50);
    glPopMatrix();
    }

    //Racunje duzine dana
    double day_length = 2*acos((-1)*tan(sun_angle*RADIAN)*tan(latitude*RADIAN))/RADIAN/15.0;
    if (day_length > 0.0 && day_length <24)
        sprintf(text2,"Duzina obdanice: %f",day_length);
        //printf("Duzina dana: %f %f %f\n",day_length,sun_angle,latitude);
    else if (((day < JESEN) && (latitude > 0)) || ((day > JESEN) && (latitude < 0) ))
    {
        day_length = 24.0;
        sprintf(text2,"Duzina obdanice: %f",day_length);
        //printf("Duzina dana: 24.00000 %f %f\n",sun_angle,latitude);
    }
    else
    {
        day_length = 0.0;
        sprintf(text2,"Duzina obdanice: %f",day_length);
        //printf("Duzina dana: 0.00000 %f %f\n",sun_angle,latitude);
    }
    // Crtanje: Zemlja
    glPushMatrix();
        glColor3ub(17,200,200);
        glutSolidSphere(earth_radius,100,100);
    glPopMatrix();

    // Crtanje: Mreza
    if (display_net)
    {
    glPushMatrix();
        glColor3ub(200,0,200);
        //glLoadIdentity();
        //glTranslatef(0.0,0.0,earth_radius);
        //glRotated(camera_angle,0.0,1.0,0.0);
        glutWireSphere(earth_radius+0.009,24,18);
    glPopMatrix();
    }

    // Crtanje: Sunce;
    if (display_sun)
    {
    glPushMatrix();
        glColor3ub(255,225,0);
        glTranslatef(suns_x*udaljenost,sun_z*udaljenost,suns_y*udaljenost);
        //glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular_off);
        glDisable(GL_LIGHTING);
        glutSolidSphere(sun_radius,24,24);
        glEnable(GL_LIGHTING);
        //glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular);
    glPopMatrix();
    }

    if (display_sunbeam && display_sun)
    {
    //Crtanje: Zrak sunca;
    glPushMatrix();
        glColor3ub(255,0,0);
        double add = sun_radius * (sun_angle/MAX_NAGIB);
        if (add<0.0)
            add*=-1;
        glDisable(GL_LIGHTING);
        glBegin(GL_POLYGON);
            glVertex3f(sun_x*udaljenost+add,sun_y*udaljenost,sun_z*udaljenost+sun_radius);
            glVertex3f(sun_x*udaljenost+add,sun_y*udaljenost,sun_z*udaljenost-sun_radius);
            glVertex3f(sun_x*earth_radius,sun_y*earth_radius,sun_z*earth_radius);
        glEnd();
        glEnable(GL_LIGHTING);
    glPopMatrix();
    }

    // Racunanje pozicije za ispisivanje teksta
    double text_pos_x, text_pos_y, text_pos_z, angle90;
    text_pos_z=1.60;
    angle90 = camera_angle - 90;
    if (angle90 < 0)
        angle90 += 360;
    text_pos_x = cos(angle90*RADIAN) * 1.60;
    text_pos_y = sin(angle90*RADIAN) * 1.60;

    // Ispisivanje teksta
    glColor3ub(200,200,200);
    glDisable(GL_LIGHTING);
    print (text_pos_x,text_pos_y,text_pos_z,text1,GLUT_BITMAP_HELVETICA_18);
    print (text_pos_x,text_pos_y,text_pos_z-0.16,text2,GLUT_BITMAP_HELVETICA_18);
    print (text_pos_x,text_pos_y,text_pos_z-0.32,text3,GLUT_BITMAP_HELVETICA_18);
    if (display_textsunangle)
        print (text_pos_x,text_pos_y,text_pos_z-0.48,text4,GLUT_BITMAP_HELVETICA_18);
    print (text_pos_x,text_pos_y,-text_pos_z+0.24,text5,GLUT_BITMAP_HELVETICA_10);
    print (text_pos_x,text_pos_y,-text_pos_z+0.16,text7,GLUT_BITMAP_HELVETICA_10);
    print (text_pos_x,text_pos_y,-text_pos_z+0.08,text6,GLUT_BITMAP_HELVETICA_10);
    print (text_pos_x,text_pos_y,-text_pos_z,text_credits,GLUT_BITMAP_HELVETICA_10);
    glEnable(GL_LIGHTING);


    glFlush();
}

void reshape(int w, int h)
{
    glViewport (0,0,(GLsizei)w,(GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    double view=1.75;
    if (w<=h)
        glOrtho(-view,view,-view*(GLfloat)h/(GLfloat)w,view*(GLfloat)h/(GLfloat)w,-100.0,100.0);
    else
        glOrtho(-view*(GLfloat)w/(GLfloat)h,view*(GLfloat)w/(GLfloat)h,-view,view,-100.0,100.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}

void keyboard (unsigned char key, int x, int y)
{
    double sun_c;
    switch (key)
    {
    case '1':
        camera_angle += camera_move;
        if (camera_angle > 360.0)
            camera_angle -= 360.0;
        camera_ori_y = camera_pos_y = sin(camera_angle*RADIAN) * 3.0;
        camera_ori_x = camera_pos_x = cos(camera_angle*RADIAN) * 3.0;
        sun_c = camera_angle + 90.0;
        if (sun_c > 360.0)
            sun_c -= 360.0;
        suns_x=cos(sun_c*RADIAN);
        suns_y=sin(sun_c*RADIAN);
        glutPostRedisplay();
        break;
    case '2':
        camera_angle -= camera_move;
        if (camera_angle < 360.0)
            camera_angle += 360.0;
        camera_ori_y = camera_pos_y = sin(camera_angle*RADIAN) * 3.0;
        camera_ori_x = camera_pos_x = cos(camera_angle*RADIAN) * 3.0;
        sun_c = camera_angle + 90.0;
        if (sun_c > 360.0)
            sun_c -= 360.0;
        suns_x=cos(sun_c*RADIAN);
        suns_y=sin(sun_c*RADIAN);
        glutPostRedisplay();
        break;
    case '3':
        latitude += latitude_move;
        if (latitude > 90.0)
            latitude = 90.0;
        glutPostRedisplay();
        break;
    case 'e':
    case 'E':
        latitude += latitude_move2;
        if (latitude > 90.0)
            latitude = 90.0;
        glutPostRedisplay();
        break;
    case '4':
        latitude -= latitude_move;
        if (latitude < -90.0)
            latitude = -90.0;
        glutPostRedisplay();
        break;
    case 'r':
    case 'R':
        latitude -= latitude_move2;
        if (latitude < -90.0)
            latitude = -90.0;
        glutPostRedisplay();
        break;
    case '5':
        day += 1.0;
        if (day >= 365)
            day -= 365;
        izracunaj_dan();
        glutPostRedisplay();
        break;
    case '6':
        day -= 1.0;
        if (day < 0)
            day += 365;
        izracunaj_dan();
        glutPostRedisplay();
        break;
    case 'a':
    case 'A':
        earth_size++;
        if(earth_size>4)
            earth_size=4;
        earth_radius=1.0+0.05*earth_size;
        glutPostRedisplay();
        break;
    case 's':
    case 'S':
        earth_size--;
        if(earth_size<-18)
            earth_size=-18;
        earth_radius=1.0+0.05*earth_size;
        glutPostRedisplay();
        break;
    case 'd':
    case 'D':
        udaljenost_plus++;
        if(udaljenost_plus>4)
            udaljenost_plus=4;
        udaljenost=1.6+0.1*udaljenost_plus;
        glutPostRedisplay();
        break;
    case 'f':
    case 'F':
        udaljenost_plus--;
        if(udaljenost_plus<-6)
            udaljenost_plus=-6;
        udaljenost=1.6+0.1*udaljenost_plus;
        glutPostRedisplay();
        break;
    /*
    case 'g':
    case 'G':
        sun_size++;
        if(sun_size>15)
            sun_size=15;
        sun_radius=0.15+0.01*sun_size;
        glutPostRedisplay();
        break;
    case 'h':
    case 'H':
        sun_size--;
        if(sun_size<-14)
            sun_size=-14;
        sun_radius=0.15+0.01*sun_size;
        glutPostRedisplay();
        break;
    */
    case 'z':
    case 'Z':
        if (display_sun)
            display_sun=false;
        else
            display_sun=true;
        glutPostRedisplay();
        break;
    case 'x':
    case 'X':
        if (display_sunbeam)
            display_sunbeam=false;
        else
            display_sunbeam=true;
        glutPostRedisplay();
        break;
    case 'c':
    case 'C':
        if (display_net)
            display_net=false;
        else
            display_net=true;
        glutPostRedisplay();
        break;
    case 'v':
    case 'V':
        if (display_greenlines)
            display_greenlines=false;
        else
            display_greenlines=true;
        glutPostRedisplay();
        break;
    case 'b':
    case 'B':
        if (display_redline)
            display_redline=false;
        else
            display_redline=true;
        glutPostRedisplay();
        break;
    case 'n':
    case 'N':
        if (display_sunreflection)
            display_sunreflection=false;
        else
            display_sunreflection=true;
        glutPostRedisplay();
        break;
    /*
    case 'm':
    case 'M':
        if (display_textsunangle)
            display_textsunangle=false;
        else
            display_textsunangle=true;
        glutPostRedisplay();
        break;
    */
    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

void izracunaj_dan()
{
    if (day<LETO) //onda je prolece na severnoj hemisferi
    {
        sun_angle= day / LETO * MAX_NAGIB;
        sun_x = cos(sun_angle*RADIAN);
        sun_z = sin(sun_angle*RADIAN);
        //printf("PROLECE %f %f\n",day,sun_angle);
    }
    else if (day<JESEN) // leto
    {
        sun_angle= (1-(day-LETO)) / LETO * MAX_NAGIB;
        if (sun_angle > 0.0)
            sun_angle=0.0;
        sun_angle+=MAX_NAGIB;
        sun_x = cos(sun_angle*RADIAN);
        sun_z = sin(sun_angle*RADIAN);
        //printf("LETO %f %f\n",day,sun_angle);
    }
    else if (day<ZIMA) // jesen
    {
        sun_angle= (day-JESEN) / LETO * MAX_NAGIB;
        sun_x = cos(sun_angle*RADIAN);
        sun_z = -sin(sun_angle*RADIAN);
        sun_angle*=-1;
        //printf("JESEN %f %f\n",day,sun_angle);
    }
    else // zima
    {
        sun_angle= (1-(day-ZIMA)) / LETO * MAX_NAGIB;
        if (sun_angle > 0.0)
            sun_angle=0.0;
        sun_angle+=MAX_NAGIB;
        sun_x = cos(sun_angle*RADIAN);
        sun_z = -sin(sun_angle*RADIAN);
        sun_angle*=-1;
        //printf("ZIMA %f %f\n",day,sun_angle);
    }
   // printf("%f %lf\n",sun_angle,sun_angle);
    int day_g=(int)day +80;
    if (day_g > 365)
        day_g -= 365;

    if (day_g <= 31)
    {
        dan=day_g;
        mesec=(char*)"januar";
    }
    else if (day_g <= 59)
    {
        dan=day_g-31;
        mesec=(char*)"februar";
    }
    else if (day_g <= 90)
    {
        dan=day_g-59;
        mesec=(char*)"mart";
    }
    else if (day_g <= 120)
    {
        dan=day_g-90;
        mesec=(char*)"april";
    }
    else if (day_g <= 151)
    {
        dan=day_g-120;
        mesec=(char*)"maj";
    }
    else if (day_g <= 181)
    {
        dan=day_g-151;
        mesec=(char*)"jun";
    }
    else if (day_g <= 212)
    {
        dan=day_g-181;
        mesec=(char*)"jul";
    }
    else if (day_g <= 243)
    {
        dan=day_g-212;
        mesec=(char*)"avgust";
    }
    else if (day_g <= 273)
    {
        dan=day_g-243;
        mesec=(char*)"septembar";
    }
    else if (day_g <= 304)
    {
        dan=day_g-273;
        mesec=(char*)"oktobar";
    }
    else if (day_g <= 334)
    {
        dan=day_g-304;
        mesec=(char*)"novembar";
    }
    else
    {
        dan=day_g-334;
        mesec=(char*)"decembar";
    }
    //printf("%d %s \n",dan,mesec);
}

void print (double x, double y, double z, char *s, void*font)
{
    int len,i;
    glRasterPos3f(x,y,z);
    len = (int) strlen(s);
    for (i=0; i<len; i++)
    {
        glutBitmapCharacter(font,s[i]);
    }
}
