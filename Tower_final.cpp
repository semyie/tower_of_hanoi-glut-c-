#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


GLfloat WIDTH = 1360;
GLfloat HEIGHT = 768;

GLint NUM_DISKS;

GLboolean motion = GL_FALSE;
GLboolean rotateScreen = GL_TRUE;
GLfloat xangle = 0, yangle = 0;
GLfloat xlangle = 0, ylangle = 0;

#define other(i,j) (6-(i+j))
#define DISK_HEIGHT 35 
#define CONE NUM_DISKS+1
#define HANOI_SOLVE 0
#define HANOI_QUIT 1

GLfloat lightTwoColor[] = { 1.0, 0.5, 1, 1.0 };
GLfloat lightZeroColor[] = { .3, .3, .3, .3 };

GLfloat diskColor[] = { 1.0, 0.0, 0.0 };
GLfloat baseCubeColor[] = { 0.5,0.5,0.5 };
GLfloat poleColor[] = { 0.0,0.0,0.0 };




//Structures for stack
typedef struct stack_node
{
    int size;
    struct stack_node* next;
} stack_node;


typedef struct stack
{
    struct stack_node* head;
    int depth;
} stack;


stack poles[4];

int push(int which, int size)
{
    stack_node* next = (stack_node*)malloc(sizeof(stack_node));
    if (!next)
    {
        fprintf(stderr, "out of memory!\n");
        exit(-1);
    }
    next->size = size;
    next->next = poles[which].head;
    poles[which].head = next;
    poles[which].depth++;
}


int pop(int which)
{
    int retval = poles[which].head->size;
    stack_node* temp = poles[which].head;
    poles[which].head = poles[which].head->next;
    poles[which].depth--;
    free(temp);
    return retval;
}

typedef struct move_node
{
    int t, f;
    struct move_node* next;
    struct move_node* prev;
} move_node;


typedef struct move_stack
{
    int depth;
    struct move_node* head, * tail;
} move_stack;


move_stack moves;


//Initialize towers and rings
void init(void)
{
    glClearColor(1, 1, 1, 1);
    glShadeModel(GL_SMOOTH);
    int i;
    for (i = 0; i < 4; i++)
    {
        poles[i].head = NULL;
        poles[i].depth = 0;
    }
    moves.head = NULL;
    moves.tail = NULL;
    moves.depth = 0;

    for (i = 1; i <= NUM_DISKS; i++)
    {
        glNewList(i, GL_COMPILE);
        {
            glutSolidTorus(DISK_HEIGHT / 2, 12 * i, 15, 15); //glutSolidTorus(innerRadius, outerRadius, nsides, rings)
        }
        glEndList();
    }

    glNewList(CONE, GL_COMPILE);
    {
        glutSolidCone(15, (NUM_DISKS + 1) * DISK_HEIGHT, 20, 20); //glutSolidCone(base, height, slices, stacks);
    }
    glEndList();
}


void mpop(void)
{
    move_node* temp = moves.head;
    moves.head = moves.head->next;
    free(temp);
    moves.depth--;
}


void mpush(int t, int f)
{
    move_node* new1 = (move_node*)malloc(sizeof(move_node));
    new1->t = t;
    new1->f = f;
    new1->next = NULL;
    new1->prev = moves.tail;
    if (moves.tail)
        moves.tail->next = new1;
    moves.tail = new1;
    if (!moves.head)
        moves.head = moves.tail;
    moves.depth++;
}


void update()
{
    while (motion == GL_TRUE && motion++)	//To solve it at once without mouse clicks, comment this while loop
    {
        glutPostRedisplay();
    }
}


void DrawPost(GLfloat xcenter)
{
    glPushMatrix();
    {
        glTranslatef(xcenter, 0, 0);
        glRotatef(90, -1, 0, 0);
        glCallList(CONE);
    }
    glPopMatrix();
}


void DrawPosts(void)
{
    glColor3fv(poleColor);
    glLineWidth(10);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, poleColor);
    DrawPost((int)(WIDTH / 4));
    DrawPost((int)(2 * WIDTH / 4));
    DrawPost((int)(3 * WIDTH / 4));
}


void DrawDisk(GLfloat xcenter, GLfloat ycenter, GLfloat size)
{
    glPushMatrix();
    {
        glTranslatef(xcenter, ycenter, 0);
        glRotatef(90, 1, 0, 0);
        glCallList(size);
    }
    glPopMatrix();
}


void DrawDisks(void)
{
    int i;
    stack_node* temp;
    int xcenter, ycenter;
    glColor3fv(diskColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diskColor);
    for (i = 1; i <= 3; i++)
    {
        xcenter = i * WIDTH / 4;
        for (temp = poles[i].head, ycenter = DISK_HEIGHT * poles[i].depth - DISK_HEIGHT / 2; temp; temp = temp->next, ycenter -= DISK_HEIGHT) {
            DrawDisk(xcenter, ycenter, temp->size);
        }
    }
}


#define MOVE(t,f) mpush((t),(f))


static void mov(int n, int f, int t)
{
    int o;
    if (n == 1)
    {
        MOVE(t, f);
        printf("\nDisk moves From Peg: %d -> Peg: %d", f, t);
        return;
    }
    o = other(f, t);
    mov(n - 1, f, o);
    mov(1, f, t);
    mov(n - 1, o, t);
}





void drawBase() {
    glBegin(GL_QUADS);
    // Base cuboid vertices
    glVertex3f(0, -20, -400);
    glVertex3f(1360, -20, -400);
    glVertex3f(1360, -20, 400);
    glVertex3f(0, -20, 400);

    glVertex3f(0, -20, -400);
    glVertex3f(1360, -20, -400);
    glVertex3f(1360, 0, 400);
    glVertex3f(0, 0, 400);

    glVertex3f(0, -20, -400);
    glVertex3f(0, -20, 400);
    glVertex3f(0, 0, 400);
    glVertex3f(0, 0, -400);

    glVertex3f(1360, -20, -400);
    glVertex3f(1360, -20, 400);
    glVertex3f(1360, 0, 400);
    glVertex3f(1360, 0, -400);

    glVertex3f(0, -20, -400);
    glVertex3f(1360, -20, -400);
    glVertex3f(1360, 0, -400);
    glVertex3f(0, 0, -400);

    glVertex3f(0, -20, 400);
    glVertex3f(1360, -20, 400);
    glVertex3f(1360, 0, 400);
    glVertex3f(0, 0, 400);
    glEnd();
}

void drawLegs() {
    glBegin(GL_QUADS);
    // Back leg vertices
    glVertex3f(0, -200, -400);
    glVertex3f(50, -200, -400);
    glVertex3f(50, -20, -400);
    glVertex3f(0, -20, -400);

    glVertex3f(1310, -200, -400);
    glVertex3f(1360, -200, -400);
    glVertex3f(1360, -20, -400);
    glVertex3f(1310, -20, -400);

    // Front leg vertices
    glVertex3f(0, -200, 400);
    glVertex3f(50, -200, 400);
    glVertex3f(50, -20, 400);
    glVertex3f(0, -20, 400);

    glVertex3f(1310, -200, 400);
    glVertex3f(1360, -200, 400);
    glVertex3f(1360, -20, 400);
    glVertex3f(1310, -20, 400);
    glEnd();
}




void draw(void)
{
    int t, f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    {
        glTranslatef(WIDTH / 2, HEIGHT / 1.1, 0);
        glRotatef(xangle, 0, 1, 0);
        glRotatef(yangle, 1, 0, 0);
        glTranslatef(-WIDTH / 2, -HEIGHT / 2, 0);

        // Set material properties for the base cube
        glMaterialfv(GL_FRONT, GL_DIFFUSE, baseCubeColor);

        DrawPosts();
        DrawDisks();
        drawBase();
        drawLegs();
    }
    glPopMatrix();

    if (motion && moves.depth)
    {
        t = moves.head->t;
        f = moves.head->f;
        push(t, pop(f));
        mpop();
    }
    glutSwapBuffers();
}


void hanoi_menu(int value)
{
    switch (value)
    {
    case HANOI_SOLVE: motion = GL_TRUE;
        if (motion)
        {
            update;

        }

        break;

    case HANOI_QUIT: exit(0);
        break;
    }
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
    case 'Q':
        exit(0);
        break;
    case 'r':
    case 'R':
        xangle = yangle = xlangle = ylangle = 0;
        glutPostRedisplay();
        break;
    case 'w':
        if (rotateScreen) {
            motion = GL_FALSE;
            yangle += 5;
            printf(" w clicked ");
            glutPostRedisplay();
        }
        break;
    case 's':
        if (rotateScreen) {
            motion = GL_FALSE;
            yangle -= 5;
            printf(" s clicked ");
            glutPostRedisplay();
        }
        break;
    case 'a':
        motion = 0;
        if (rotateScreen) {
            xangle += 5;
            printf(" a clicked ");
            glutPostRedisplay();
        }
        break;
    case 'd':
        motion = 0;
        if (rotateScreen) {
            xangle -= 5;
            printf(" d clicked ");
            glutPostRedisplay();
        }
        break;

    case 'n':

        if (rotateScreen) {
            motion = GL_TRUE;
            glutIdleFunc(update);
        }
        break;

    case 't': // Toggle screen rotation
    case 'T':
        rotateScreen = !rotateScreen;
        break;
    }
}


int main(int argc, char* argv[])
{
    int i;

    printf("Enter the number of Disks: ");
    scanf_s("%d", &NUM_DISKS);

    double DISK_MOVES = pow(2.0, NUM_DISKS) - 1;
    printf("\nTotal number of Disk movements for %d disks : %.0lf\n", NUM_DISKS, DISK_MOVES);

    if (NUM_DISKS > 0)
    {
        glutInit(&argc, argv);
        glutInitWindowSize(1000, 600);
        glutInitWindowPosition(100, 100);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
        glutCreateWindow("Tower of Hanoi");
        glutDisplayFunc(draw);

        glViewport(0, 0, (int)WIDTH, (int)HEIGHT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, WIDTH, -70.0, HEIGHT, -10000, 10000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0, 0, 0, 0);
        glClearDepth(1.0);
        glEnable(GL_DEPTH_TEST);

        glLightfv(GL_LIGHT2, GL_DIFFUSE, lightTwoColor);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 10);
        glEnable(GL_LIGHT2);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);

        glutKeyboardFunc(keyboard);

        glutCreateMenu(hanoi_menu);
        glutAddMenuEntry("Solve", HANOI_SOLVE);
        glutAddMenuEntry("Quit", HANOI_QUIT);
        glutAttachMenu(GLUT_RIGHT_BUTTON);
        init();



        for (i = 0; i < NUM_DISKS; i++)
            push(1, NUM_DISKS - i);
        mov(NUM_DISKS, 1, 3);

        glutMainLoop();
        return 0;
    }
    else
    {
        exit(0);
    }

    
}