#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

#define COLOR int
#define FOR_EACH_CHILD(child,pp) for(auto &child : pp->children)
#define REMOVE_POINT(pp,p_p) pp->content.erase(pp->content.begin() + p_p)
#define PRINT(item) cout << item << endl
#define PRINTC(item) cout << item

enum {WHITE = 0, BLACK = 1, GRAY = 2};//GRAY SERIA UTIL SI SE VISUALIZARA EL ARBOL COMO UN GRAFO

struct Point{
  double x,y,z;
  Point(){x=y=z=0.0;}
  Point(double X, double Y, double Z){
    x = X; y = Y; z = Z;
  }
  bool operator==(Point &p){
    return x == p.x && y == p.y && z == p.z;
  }
  Point operator+(Point &p2){
    Point p;
    p.x = x + p2.x; p.y = y + p2.y; p.z = z + p2.z;
    return p;
  }
  Point operator-(Point &p2) {
    Point p;
    p.x = x - p2.x; p.y = y - p2.y; p.z = z - p2.z;
    return p;
  }
  Point& operator=(const Point &p){
    this->x = p.x; this->y = p.y; this->z = p.z;
    return *this;
  }
  void print(){
    cout << '<' << x << ',' << y << ',' << z << '>' << endl;
  }
};

template<unsigned int RESOLUTION>//PROFUNDIDAD DE ARBOL TOLERADA
struct RocTree{
  RocTree(){
    parent = NULL;
    color = WHITE;
  }
  RocTree(Point offset, Point size, RocTree *parent_ = NULL){
    boundfp = offset;
    boundsp = boundfp + size;
    chSX = Point(size.x*0.5, 0.0, 0.0);
    chSY = Point(0.0, size.y*0.5, 0.0);
    chSZ = Point(0.0, 0.0, size.z*0.5);
    parent = parent_;
    color = WHITE;
  }
  bool sfind(Point srchp){
    RocTree *pp = this;
    int p_p = 0;
    return find(pp,srchp,p_p);
  }
  bool insert(Point newpoint){
    RocTree *pp = this;
    int p_p = 0, OCTANT_LEVEL = 1;
    if(!find(pp,newpoint,p_p)){
      while(pp->color != BLACK && OCTANT_LEVEL != RESOLUTION){
        pp->children[0] = new RocTree(pp->boundfp, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[1] = new RocTree(pp->boundfp + pp->chSY, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[2] = new RocTree(pp->boundfp + pp->chSZ, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[3] = new RocTree(pp->boundfp + pp->chSY + pp->chSZ, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[4] = new RocTree(pp->boundfp + pp->chSX, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[5] = new RocTree(pp->boundfp + pp->chSX + pp->chSY, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[6] = new RocTree(pp->boundfp + pp->chSX + pp->chSZ, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[7] = new RocTree(pp->boundfp + pp->chSX + pp->chSY + pp->chSZ, pp->chSX + pp->chSY + pp->chSZ, pp);
        FOR_EACH_CHILD(child,pp){
          if(child->isInside(newpoint)){
            ++OCTANT_LEVEL;
            pp = child;
            break;
          }
        }
      }
      Point *newpointptr = new Point(newpoint.x,newpoint.y,newpoint.z);
      pp->content.push_back(newpointptr);
      pp->color = BLACK;
      recall(pp->parent);
      return true;
    }
    return false;
  }
  bool remove(Point rempoint){
    RocTree *pp = this;
    int p_p = 0;
    if(find(pp,rempoint,p_p)){
      Point *temp = pp->content[p_p];
      REMOVE_POINT(pp, p_p);
      delete temp;
      temp = NULL;
      recall(pp->parent, WHITE);
      return true;
    }
    return false;
  }
private:
  bool find(RocTree *&pp, Point apoint, int &p_p){
    if (isInside(apoint)) {
      while (pp->children[0]) {
        FOR_EACH_CHILD(child,pp) {
          if (child->isInside(apoint)) {
            pp = child;
            break;
          }
        }
      }
      if (!pp->content.empty()) {
        for (auto &pt : pp->content) {
          if (*pt == apoint) { return true; }
          ++p_p;
        }
      }
    }
    return false;
  }
  bool isInside(Point apoint){
    return (boundfp.x <= apoint.x && boundsp.x > apoint.x) &&
            (boundfp.y <= apoint.y && boundsp.y > apoint.y) &&
            (boundfp.z <= apoint.z && boundsp.z > apoint.z);
  }
  void recall(RocTree *&pp, COLOR col2ver = BLACK){
    if(pp){
      FOR_EACH_CHILD(child,pp){
        if(child->color != col2ver) return;
      }
      FOR_EACH_CHILD(child,pp){
        if(!child->content.empty()){
          for(auto &point : child->content){
            pp->content.push_back(point);
          }
        }
        delete child;
        child = NULL;
      }
      pp->color = col2ver;
      recall(pp->parent,col2ver);
    }
  }
  RocTree* children[8] = {NULL};
  RocTree* parent;
  COLOR color;
  vector<Point*> content;
  Point boundfp, boundsp;
  Point chSX, chSY, chSZ;
};

//String to Point
Point StoPt(string line){
  string s_dat[3] = {"","",""};
  int xyz = 0;
  for(int i = 2; i < line.length(); i++){
    if(line[i] == ' '){
      ++xyz;
      continue;
    }
    if(line[i] == '\n')break;
    s_dat[xyz].push_back(line[i]);
  }
  Point p(stod(s_dat[0]),stod(s_dat[1]),stod(s_dat[2]));
  return p;
}
//Util para la busqueda de los bounds del octree (maxPoint y minPoint)
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

template<unsigned int R>
void setPoints(string path, RocTree<R> &o_tree, vector<Point> &data_set){
  ifstream obj_file(path);
  string line;
  Point maxBound, minBound;
  if(obj_file.is_open()){
    PRINT("file " + path.substr(0,path.length() - 4) + " openned");
    while(!obj_file.eof()){
      getline(obj_file,line);
      if(line[0] == 'v' && line[1] == ' ') {
        Point temp = StoPt(line);
        maxPoint(maxBound, temp);
        minPoint(minBound, temp);
        data_set.push_back(temp);
      }
      else if(line[0] == 'f') break;
    }
    obj_file.close();
  }
  else {
    PRINT("ERROR: file can not be openned");
    return;
  }
  minBound = minBound - Point(1, 1, 1);
  maxBound = maxBound + Point(1, 1, 1);
  PRINTC("=> MIN BOUND: "); minBound.print();
  PRINTC("=> MAX BOUND: "); maxBound.print();
  o_tree = RocTree<R>(minBound, maxBound - minBound);
  for(auto &point : data_set){
    o_tree.insert(point);
  }
  PRINT("ALL_DATA  SAVED");
}

template<unsigned int R>
void destroyTree(RocTree<R> &o_tree, vector<Point> &data_set){
  if(!data_set.empty()){
    for(auto &p : data_set){
      o_tree.remove(p);
    }
    PRINT("ALL_DATA  REMOVED");
  }
  else{
    PRINT("WARNING: DATA_SET IS EMPTY!");
  }
}

int main(){
  srand(time(NULL));
  vector<Point> data;
  RocTree<10> oct;//Profundidad de arbol 10 para el ejemplo.
  setPoints<10>("teapot.obj",oct, data);//Lee archivos obj e inserta los puntos en el octree haciendo un rebound automatico de este.
  // rebound = Reposicionamiento de los puntos extremos (bounds) del nodo raiz del octree.

  int random_ind = rand() % int(data.size());

  //sfind = busqueda simple que solo necesita el punto a buscar en el octree.
  //find = busqueda exclusiva solo para realizar inserciones y borrados en el octree. Si nada más se quiere buscar un elemento, use sfind.

  //TEST
  PRINT(oct.sfind(data[random_ind]));//1 busqueda de un dato random que debe encontrarse en el octree.
  destroyTree<10>(oct, data);//Mata todo el arbol
  PRINT(oct.sfind(data[random_ind]));//0 busqueda del mismo dato random que debio haberse borrado del octree.
  return 0;
}
