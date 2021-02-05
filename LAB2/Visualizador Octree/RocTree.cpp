#include <GL/freeglut.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <time.h>

#define CCOLORG 0.5,0.5,0.5
#define CCOLORN 0.0,0.0,0.0

using namespace std;

double rotX = 0.0, rotY = 0.0, rotZ = 0.0;
double zoom = -40.0;
bool viewMode = 1;

#define COLOR int
#define FOR_EACH_CHILD(child,poct) for(auto &child : (*poct)->children)
#define FOR_EACH_CHILD2(child,poct) for(auto &child : poct->children)
#define REMOVE_POINT(poct,pind) (*poct)->content.erase((*poct)->content.begin() + pind)
#define PRINT(item) cout << item << endl
#define PRINTC(item) cout << item

enum { WHITE = 0, BLACK = 1, GRAY = 2 };

struct Point {
  double x, y, z;
  Point() { x = y = z = 0.0; }
  Point(double X, double Y, double Z) {
    x = X; y = Y; z = Z;
  }
  bool operator==(Point &p) {
    return x == p.x && y == p.y && z == p.z;
  }
  Point operator+(Point &p2) {
    Point p;
    p.x = x + p2.x; p.y = y + p2.y; p.z = z + p2.z;
    return p;
  }
  Point operator-(Point &p2) {
    Point p;
    p.x = x - p2.x; p.y = y - p2.y; p.z = z - p2.z;
    return p;
  }
  Point operator*(Point &p2) {
    Point p;
    p.x = x * p2.x; p.y = y * p2.y; p.z = z * p2.z;
    return p;
  }
  Point& operator=(const Point &p) {
    this->x = p.x; this->y = p.y; this->z = p.z;
    return *this;
  }

  void print() {
    cout << '<' << x << ',' << y << ',' << z << '>' << endl;
  }
};

void drawFilledCube(Point pos0, Point dim) {
  glBegin(GL_QUADS);

  glColor3d(CCOLORG);
  glVertex3f(pos0.x, pos0.y, pos0.z);
  glVertex3f(pos0.x + dim.z, pos0.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z);

  glVertex3f(pos0.x, pos0.y, pos0.z + dim.z);
  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z + dim.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z + dim.z);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z + dim.z);

  glVertex3f(pos0.x, pos0.y, pos0.z);
  glVertex3f(pos0.x, pos0.y, pos0.z + dim.z);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z + dim.z);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z);

  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z + dim.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z + dim.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z);

  glVertex3f(pos0.x, pos0.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z + dim.z);
  glVertex3f(pos0.x, pos0.y, pos0.z + dim.z);

  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z + dim.z);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z + dim.z);
  glEnd();
}

void drawSkelethonCube(Point pos0, Point dim) {
  glBegin(GL_LINE_LOOP);
  glColor3d(CCOLORN);
  glVertex3f(pos0.x, pos0.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glColor3d(CCOLORN);
  glVertex3f(pos0.x, pos0.y, pos0.z + dim.z);
  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z + dim.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z + dim.z);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z + dim.z);
  glEnd();

  glBegin(GL_LINES);
  glColor3d(CCOLORN);
  glVertex3f(pos0.x, pos0.y, pos0.z);
  glVertex3f(pos0.x, pos0.y, pos0.z + dim.z);
  glEnd();

  glBegin(GL_LINES);
  glColor3d(CCOLORN);
  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y, pos0.z + dim.z);
  glEnd();

  glBegin(GL_LINES);
  glColor3d(CCOLORN);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z);
  glVertex3f(pos0.x + dim.x, pos0.y + dim.y, pos0.z + dim.z);
  glEnd();

  glBegin(GL_LINES);
  glColor3d(CCOLORN);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z);
  glVertex3f(pos0.x, pos0.y + dim.y, pos0.z + dim.z);
  glEnd();
}

struct DrawableOctant {
  Point startP, dim;
  DrawableOctant(Point boundMin, Point boundMax) {
    startP = boundMin;
    dim = boundMax - boundMin;
    dim = Point(dim.x * 0.5, dim.y * 0.5, dim.z * 0.5);
  }
  void show() {
    if (viewMode) {
      drawFilledCube(startP, dim);
      drawSkelethonCube(startP, dim);
    }
    else {
      drawSkelethonCube(startP, dim);
    }
  }
};

static vector<DrawableOctant> octants;

struct Octant {
  Octant() {
    parent = NULL;
    color = WHITE;
    volume = 0;
  }
  Octant(int octant_vol, Point offset, Point size, Octant *parent_ = NULL) {
    boundfp = offset;
    boundsp = boundfp + size;
    chSize = Point(size.x*0.5, size.y*0.5, size.z*0.5);
    parent = parent_;
    color = WHITE;
    volume = octant_vol;
  }
private:
  bool isInside(Point apoint) {
    return (boundfp.x <= apoint.x && boundsp.x > apoint.x) &&
      (boundfp.y <= apoint.y && boundsp.y > apoint.y) &&
      (boundfp.z <= apoint.z && boundsp.z > apoint.z);
  }
  bool isInStopCriterion(double fullCapacity) {
    return int(volume * fullCapacity) <= int(content.size());
  }
  Octant* children[8] = { NULL };
  Octant* parent;
  COLOR color;
  vector<Point*> content;
  Point boundfp, boundsp;
  Point chSize;
  int volume;
  friend struct ROctree;
};

struct ROctree {
  ROctree(string PATH, double fullCapacity) {
    ifstream obj_file(PATH);
    string line;
    Point maxBound, minBound;
    if (obj_file.is_open()) {
      PRINT("file " + PATH.substr(0, int(PATH.length()) - 4) + " openned");
      while (!obj_file.eof()) {
        getline(obj_file, line);
        if (line[0] == 'v' && line[1] == ' ') {
          Point temp = StoPt(line);
          maxPoint(maxBound, temp);
          minPoint(minBound, temp);
          data_set.push_back(temp);
        }
        else if (line[0] == 'f') break;
      }
      obj_file.close();
      Point p111 = Point(1, 1, 1);
      minBound = minBound - p111;
      maxBound = maxBound + p111;
      PRINTC("=> Region Octree MIN BOUND: "); minBound.print();
      PRINTC("=> Region Octree MAX BOUND: "); maxBound.print();
      root = new Octant(int(data_set.size()), minBound, maxBound - minBound);
      this->fullCapacity = fullCapacity;
      PRINTC("Data Set size: "); PRINT(int(data_set.size()));
      fill();
      PRINT("ALL_DATA  SAVED");
    }
    else {
      PRINT("ERROR: file can not be openned");
      return;
    }
  }
  ~ROctree() {
    if (root) {
      killTree();
      delete root;
      root = NULL;
    }
  }
  void killTree() {
    if (!data_set.empty()) {
      for (auto &p : data_set) {
        remove(p);
      }
      PRINT("ALL_DATA  REMOVED");
    }
    else {
      PRINT("WARNING: DATA_SET IS EMPTY!");
    }
  }
  bool sfind(Point a_point) {
    Octant **poct;
    int pind = 0;
    return find(a_point, poct, pind);
  }
  bool insert(Point a_point) {
    Octant **poct;
    int pind = 0;
    if (!find(a_point, poct, pind)) {
      while (!(*poct)->isInStopCriterion(fullCapacity)) {
        split(poct);
        FOR_EACH_CHILD(child, poct) {
          if (child->isInside(a_point)) {
            poct = &child;
            break;
          }
        }
      }
      (*poct)->content.push_back(new Point(a_point.x, a_point.y, a_point.z));
      if ((*poct)->color != BLACK) {
        (*poct)->color = BLACK;
        poct = &(*poct)->parent;
        recall(*poct);
      }
      return true;
    }
    return false;
  }
  bool remove(Point a_point) {
    Octant **poct;
    int pind = 0;
    if (find(a_point, poct, pind)) {
      Point *temp = (*poct)->content[pind];
      REMOVE_POINT(poct, pind);
      delete temp;
      temp = NULL;
      if ((*poct)->content.empty()) {
        (*poct)->color = WHITE;
        poct = &(*poct)->parent;
        recall(*poct);
      }
      return true;
    }
    return false;
  }
  void glread() {
    Octant *reader = root;
    DrawableOctant oct(root->boundfp, root->boundsp);
    octants.push_back(oct);
    read(reader);
  }
  Point getMaxBound() {
    return root->boundsp;
  }
  Point getMinBound() {
    return root->boundfp;
  }
private:
  void fill() {
    for (auto &point : data_set) {
      insert(point);
    }
  }
  bool find(Point a_point, Octant **&poct, int &pind) {
    poct = &root;
    if (*poct && !(*poct)->isInside(a_point)) return false;
    while ((*poct)->color == GRAY) {
      FOR_EACH_CHILD(child, poct) {
        if (child && child->isInside(a_point)) {
          poct = &child;
          break;
        }
      }
    }
    if ((*poct)->color == BLACK) {
      for (auto &p : (*poct)->content) {
        if (*p == a_point)
          return true;
        ++pind;
      }
    }
    return false;
  }
  void split(Octant **&poct) {
    (*poct)->color = GRAY;
    int childVolume = (*poct)->volume / 8, i = 0;
    for (int x = 0; x < 2; x++) {
      for (int y = 0; y < 2; y++) {
        for (int z = 0; z < 2; z++) {
          Point pgen = Point(double(x), double(y), double(z));
          pgen = (*poct)->chSize * pgen;
          (*poct)->children[i++] = new Octant(childVolume, (*poct)->boundfp + pgen, (*poct)->chSize, *poct);
        }
      }
    }
  }
  void recall(Octant *parnt, COLOR status = BLACK) {
    while (parnt) {
      FOR_EACH_CHILD2(child, parnt) if (child->color != status) return;
      FOR_EACH_CHILD2(child, parnt) {
        parnt->content.insert(parnt->content.end(), child->content.begin(), child->content.end());
        child->content.clear();
        delete child;
        child = NULL;
      }
      parnt->color = status;
      parnt = parnt->parent;
    }
  }
  void read(Octant *reader) {
    FOR_EACH_CHILD2(child, reader) {
      if (child && child->color != WHITE) {
        DrawableOctant oct(reader->boundfp, reader->boundsp);
        octants.push_back(oct);
        read(child);
      }
    }
  }
  void maxPoint(Point &M, Point&X) {
    M.x = (M.x < X.x) ? X.x : M.x;
    M.y = (M.y < X.y) ? X.y : M.y;
    M.z = (M.z < X.z) ? X.z : M.z;
  }
  void minPoint(Point &M, Point&X) {
    M.x = (M.x > X.x) ? X.x : M.x;
    M.y = (M.y > X.y) ? X.y : M.y;
    M.z = (M.z > X.z) ? X.z : M.z;
  }
  Point StoPt(string line) {
    string s_dat[3] = { "","","" };
    int xyz = 0;
    for (int i = 2; i < line.length(); i++) {
      if (line[i] == ' ') {
        ++xyz;
        continue;
      }
      if (line[i] == '\n')break;
      s_dat[xyz].push_back(line[i]);
    }
    Point p(stod(s_dat[0]), stod(s_dat[1]), stod(s_dat[2]));
    return p;
  }
  vector<Point> data_set;
  Octant *root = NULL;
  double fullCapacity;
};

void init(int w, int h) {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClearDepth(80.0);
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glEnable(GL_DEPTH_TEST);
  gluPerspective(45, (float)w / h, .01, 100);
  glMatrixMode(GL_MODELVIEW);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glTranslatef(0, 0, zoom);

  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  glRotatef(rotZ, 0.0, 0.0, 1.0);

  for (auto &cube : octants) {
    cube.show();
  }

  glutSwapBuffers();
  glFlush();
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case '2':
    rotX--;
    return;
  case '8':
    rotX++;
    return;
  case '4':
    rotY--;
    return;
  case '6':
    rotY++;
    return;
  case '9':
    rotZ--;
    return;
  case '7':
    rotZ++;
    return;
  case ' ':
    rotX = 0;
    rotY = 0;
    rotZ = 0;
    zoom = -40;
    return;
  case '+':
    zoom += 1.0;
    return;
  case '-':
    zoom -= 1.0;
    return;
  case '.':
    viewMode = !viewMode;
    return;
  }
  glutPostRedisplay();
}


void runOPENGL(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(640, 480);
  glutInitWindowPosition(0, 0);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Range Octree Visualization");
  init(1024, 768);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
}

int main(int argc, char **argv)
{
  ROctree tree("teapot.obj", 0.6);
  tree.glread();
  tree.killTree();
  PRINTC("Nro. Octants to print: ");
  PRINT(octants.size());
  runOPENGL(argc, argv);
  return 0;
}
