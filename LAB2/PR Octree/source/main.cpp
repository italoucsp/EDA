#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

#define LIMIT_PER_CHILD 10 //numero de puntos que los nodos hoja pueden tener como maximo

//CHILD = OCTANT

#define CURRENT_SIZE(pp) int(pp->content.size())
#define REMOVE_POINT(pp,p_p) pp->content.erase(pp->content.begin() + p_p)
#define PRINT(item) cout << item << endl
#define PRINTC(item) cout << item

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

struct OcTree{
  OcTree() {
    parent = NULL;
  }
  OcTree(Point offset, Point size, OcTree *parent_ = NULL){
    boundfp = offset;
    boundsp = boundfp + size;
    chSX = Point(size.x*0.5, 0.0, 0.0);
    chSY = Point(0.0, size.y*0.5, 0.0);
    chSZ = Point(0.0, 0.0, size.z*0.5);
    parent = parent_;
  }
  bool sfind(Point apoint){
    OcTree *pp = this;
    int p_p = 0;
    return find(pp,apoint,p_p);
  }
  bool insert(Point newpoint){
    OcTree *pp = this;
    int p_p = 0;
    if(!find(pp,newpoint,p_p)){
      Point *newpointptr = new Point(newpoint.x,newpoint.y,newpoint.z);
      pp->content.push_back(newpointptr);
      split(pp);
      return true;
    }
    return false;
  }
  bool remove(Point rempoint){
    OcTree *pp = this;
    int p_p = 0;
    if(find(pp,rempoint,p_p)){
      Point *temp = pp->content[p_p];
      REMOVE_POINT(pp, p_p);
      delete temp;
      temp = NULL;
      join(pp->parent);
      return true;
    }
    return false;
  }
  private:
    bool find(OcTree *&pp, Point apoint, int &p_p) {
      if (isInside(apoint)) {
        while (pp->children[0]) {
          for (auto &child : pp->children) {
            if (child->isInside(apoint)) {
              pp = child;
              break;
            }
          }
        }
        for (auto &pt : pp->content) {
          if (*pt == apoint) { return true; }
          ++p_p;
        }
      }
      return false;
    }
    bool isInside(Point apoint) {
      return (boundfp.x <= apoint.x && boundsp.x > apoint.x) &&
             (boundfp.y <= apoint.y && boundsp.y > apoint.y) &&
             (boundfp.z <= apoint.z && boundsp.z > apoint.z);
    }
    bool ready2merge(OcTree *&pp) {//pp no puede ser hoja
      for (auto &child : pp->children) {
        if (CURRENT_SIZE(child)) return false;
      }
      return true;
    }
    void split(OcTree *&pp) {
      if (CURRENT_SIZE(pp) > LIMIT_PER_CHILD) {
        pp->children[0] = new OcTree(pp->boundfp, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[1] = new OcTree(pp->boundfp + pp->chSY, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[2] = new OcTree(pp->boundfp + pp->chSZ, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[3] = new OcTree(pp->boundfp + pp->chSY + pp->chSZ, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[4] = new OcTree(pp->boundfp + pp->chSX, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[5] = new OcTree(pp->boundfp + pp->chSX + pp->chSY, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[6] = new OcTree(pp->boundfp + pp->chSX + pp->chSZ, pp->chSX + pp->chSY + pp->chSZ, pp);
        pp->children[7] = new OcTree(pp->boundfp + pp->chSX + pp->chSY + pp->chSZ, pp->chSX + pp->chSY + pp->chSZ, pp);
        for (auto &p_ : pp->content) {
          for (auto &child : pp->children) {
            if (child->isInside(*p_)) {
              child->content.push_back(p_);
              break;
            }
          }
        }
        pp->content.clear();
        for (auto &child : pp->children) {
          split(child);
        }
      }
    }
    void join(OcTree *&pp) {
      if (pp && ready2merge(pp)) {
        OcTree *temp = pp;
        for (auto &child : pp->children) {
          delete child;
          child = NULL;
        }
        join(temp->parent);
      }
    }

    OcTree* children[8] = {NULL};
    OcTree* parent;
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

void setPoints(string path, OcTree &o_tree, vector<Point> &data_set){
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
  }
  Point p111 = Point(1,1,1);
  minBound = minBound - p111;
  maxBound = maxBound + p111;
  PRINTC("=> MIN BOUND: "); minBound.print();
  PRINTC("=> MAX BOUND: "); maxBound.print();
  o_tree = OcTree(minBound, maxBound - minBound);
  for(auto &point : data_set){
    o_tree.insert(point);
  }
  PRINT("ALL_DATA  SAVED");
}

void destroyTree(OcTree &o_tree, vector<Point> &data_set){
  for(auto &p : data_set){
    o_tree.remove(p);
  }
  PRINT("ALL_DATA  REMOVED");
}

int main(){
  srand(time(NULL));
  vector<Point> data;
  OcTree oct;
  setPoints("teapot.obj",oct, data);//Lee archivos obj e inserta los puntos en el octree haciendo un rebound automatico de este.
  // rebound = Reposicionamiento de los puntos extremos (bounds) del nodo raiz del octree.

  int random_ind = rand() % int(data.size());

  //sfind = busqueda simple que solo necesita el punto a buscar en el octree.
  //find = busqueda exclusiva solo para realizar inserciones y borrados en el octree. Si nada más se quiere buscar un elemento, use sfind.

  //TEST
  PRINT(oct.sfind(data[random_ind]));//1 busqueda de un dato random que debe encontrarse en el octree.
  destroyTree(oct, data);//Mata todo el arbol 
  PRINT(oct.sfind(data[random_ind]));//0 busqueda del mismo dato random que debio haberse borrado del octree.
  return 0;
}
