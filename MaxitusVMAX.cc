#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME MaxitusVMAX


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  void moveFuryans(const vector<int>& F, vector<bool>& fur,set<Pos> visited) {
    int n = F.size();
    for(int i = 0; i < n; ++i) {
      if(not fur[i]) {
        int id = F[i];
        Unit u = unit(id);
        Pos pos = u.pos;
        Pos penemy;
        if(enemyToKill(pos,penemy)) {
          command(id,Dir(dirConverter(pos,penemy,visited,true)));
        }else if(hellhoundNear(pos,penemy)) {
          command(id,Dir(flee(pos,penemy,false,visited)));
        }else if(enemyWithMoreHp(pos,penemy)) {
          command(id,Dir(flee(pos,penemy,false,visited)));
        }else {
          goKill(pos,penemy);
          command(id,Dir(dirConverter(pos,penemy,visited,true)));
        }
      }
    }
  }

  void goKill(Pos p, Pos& penemy) {
    queue<Pos> cola;
    cola.push(p);
    set<Pos> visitats;
    visitats.insert(p);
    map<Pos, Pos> parent; // Para almacenar el camino recorrido
    bool enemyFound = false;
    while (not cola.empty() and not enemyFound) {
      Pos actual = cola.front();
      cola.pop();
      Cell c = cell(actual);
      if (c.id != -1) {
        Unit uenemy = unit(c.id);
        if (uenemy.player != -1 and uenemy.player != me() and (uenemy.type == Pioneer or (uenemy.type == Furyan and not moreHealth(p,actual)))) {
          penemy = uenemy.pos;
          enemyFound = true;
          // Reconstruir el camino recorrido
          stack<Pos> path;
          path.push(penemy); // Agregar la posici贸n del enemigo al camino
          Pos current = actual;
          while (parent.find(current) != parent.end()) {
            path.push(current);
            current = parent[current];
            if (current == p) {
                break; // Evitar bucles infinitos
            }
          }
          // Ir a la primera posici贸n del camino
          if (!path.empty()) {
            penemy = path.top();
          }
          return;
        }
      }

      for (int x = 0; x < 8; ++x) {
        Pos k = actual + Dir(x);
        set<Pos>::iterator it = visitats.find(k);
        if (pos_ok(k) and it == visitats.end()) {
          visitats.insert(k);
          if (isValid(k)) {
              cola.push(k);
              parent[k] = actual; // Almacenar el camino
          }
        }
      }
    }
  }

  CellType whatCell(Pos p) {
    Cell c = cell(p);
    return c.type;
  }

  bool enemyWithMoreHp(Pos p,Pos& penemy) {
    queue<Pos> cola;
    cola.push(p);
    set<Pos> visitats;
    visitats.insert(p);
    while(not cola.empty()) {
      Pos actual = cola.front();
      cola.pop();
      if(abs(p.i-actual.i) > 3 or abs(p.j-actual.j) > 3) return false;
      Cell c = cell(actual);
      if(c.id != -1) {
        Unit uenemy = unit(c.id);
        if(uenemy.player != me() and uenemy.type == Furyan and moreHealth(p,uenemy.pos)) {
          penemy = uenemy.pos;
          return true;
        }
      }
      vector<int> d = random_permutation(8);
      for(int x = 0; x < 8; ++x) {
        Pos k = actual + Dir(d[x]);
        if (pos_ok(k)) {
          set<Pos>::iterator it = visitats.find(k);
          if (it == visitats.end()) {
            visitats.insert(k);
            if(isValid(k)) cola.push(k);
          }
        }
      }
    }
    return false;
  }

  bool moreHealth(Pos p, Pos penemy) {
    Cell c1 = cell(p);
    Unit ume = unit(c1.id);
    Cell c2 = cell(penemy);
    Unit uenemy = unit(c2.id);
    if(ume.health < uenemy.health and uenemy.type == UnitType(1)) return true;
    else return false;
  }

  bool enemyToKill(Pos p, Pos& penemy) {
    for(int i = 0; i < 8; ++i) {
      Cell c = cell(p+Dir(i));
      if(c.id != -1) {
        Unit uenemy = unit(c.id);
        if(uenemy.player != me()) {
          penemy = uenemy.pos;
          return true;
        }
      }
    }
    return false;
  }

  bool enemyNear(Pos p, Pos& penemy) {
    for(int i = 0; i < 8; ++i) {
      Cell c = cell(p+Dir(i));
      if(c.id != -1) {
        Unit uenemy = unit(c.id);
        if(uenemy.type == UnitType(1) and uenemy.player != me()) {
          penemy = p+Dir(i);
          return true;
        }
      }
    }
    return false;
  }

  int flee(Pos p,Pos penemy,bool paint,set<Pos> visited) {
    Pos op = opuesta(p,penemy);
    bool useaux = false;
    Pos aux;
    Cell c1 = cell(op);
    set<Pos>::iterator it = visited.find(op);
    if(isValid(op) and c1.id == -1 and it == visited.end()) {
      if(c1.owner != me() or not paint) {
        visited.insert(op);
        return dirConverter(p,op,visited,true);
      }
      else {
        aux = op;
        useaux = true;
      }
    }
    vector<vector<int>> posibilities = {
        {3,4,5},
        {4,5,6,3,7},
        {5,6,7},
        {6,7,0,5,1},
        {7,0,1},
        {0,1,2,3,7},
        {1,2,3},
        {2,3,4,5,1}
    };
    int i = dirConverter(p,penemy,visited,false);
    Pos x; //no sirve para nada
    int n = posibilities[i].size();
    for(int j = 0; j < n; ++j) {
      Pos k = p + Dir(posibilities[i][j]);
      Cell c2 = cell(k);
      it = visited.find(k);
      if(isValid(k) and not enemyNear(k,x) and c2.id == -1 and it == visited.end()) {
        if(c2.owner != me() or not paint) {
          visited.insert(op);
          return dirConverter(p,k,visited,true);
        }else if(not useaux){
          aux = k;
          useaux = true;
        }
      }
    }
    if(useaux) return dirConverter(p,aux,visited,true);
    else return random(0,7);
  }

  Pos opuesta(Pos p,Pos penemy) {
    if(penemy.j < p.j) {
      if(penemy.i == p.i) return p + Right;
      else if(penemy.i < p.i) return p + BR;
      else if(penemy.i > p.i) return p + RT;
    }else if(penemy.j > p.j) {
      if(penemy.i == p.i) return p + Left;
      else if(penemy.i < p.i) return p + LB;
      else if(penemy.i > p.i) return p + TL;
    }else {//Misma columna
      if(penemy.i < p.i) {
        return p + Bottom;
      }else {
        return p + Top;
      }
    }
    return p + Bottom;
  }

  int dirConverter(Pos p1, Pos p2, set<Pos> visited,bool vis) { //Devuelve la direction que hay tomar para llegar de p1 a p2
    if(p1.j < 10 and p2.j > 70) {
      if(vis) visited.insert(p1+Dir(6));
      return 6;
    }
    else if(p1.j > 70 and p2.j < 10){
      if(vis) visited.insert(p1+Dir(2));
      return 2;
    }
    else if(p2.j < p1.j) {
      if(p2.i == p1.i) {
        if(vis) visited.insert(p1+Dir(6));
        return 6;
      }
      else if(p2.i < p1.i){
        if(vis) {
          visited.insert(p1+Dir(5));
        }
        return 5;
      }
      else if(p2.i > p1.i) {
        if(vis) visited.insert(p1+Dir(7));
        return 7;
      }
    }else if(p2.j > p1.j) {
      if(p2.i == p1.i) {
        if(vis) visited.insert(p1+Dir(2));
        return 2;
      }
      else if(p2.i < p1.i) {
        if(vis) visited.insert(p1+Dir(3));
        return 3;
      }
      else if(p2.i > p1.i) {
        if(vis) visited.insert(p1+Dir(1));
        return 1;
      }
    }else {//Misma columna
      if(p2.i < p1.i) {
        if(vis) visited.insert(p1+Dir(4));
        return 4;
      }else {
        if(vis) visited.insert(p1+Dir(0));
        return 0;
      }
    }
    visited.insert(p1+Dir(0));
    return 0;
  }

  bool isValid(Pos p) {
    if(whatCell(p) != Rock and pos_ok(p.i,p.j, 0)) return true;
    else return false;
  }

  bool hellhoundNear(Pos p, Pos& penemy) {
    queue<Pos> cola;
    cola.push(p);
    set<Pos> visitats;
    visitats.insert(p);
    while(not cola.empty()) {
      Pos actual = cola.front();
      cola.pop();
      if(abs(p.i-actual.i) > 6 or abs(p.j-actual.j) > 6) return false;
      Cell c = cell(actual);
      if(c.id != -1) {
        Unit uenemy = unit(c.id);
        if(uenemy.type == Hellhound) {
          penemy = uenemy.pos;
          return true;
        }
      }
      vector<int> d = random_permutation(8);
      for(int x = 0; x < 8; ++x) {
        Pos k = actual + Dir(d[x]);
        if (pos_ok(k)) {
          set<Pos>::iterator it = visitats.find(k);
          if (it == visitats.end()) {
            visitats.insert(k);
            if(isValid(k)) cola.push(k);
          }
        }
      }
    }
    return false;
  }


  void goPaint(Pos p, Pos& ppaint) {
    queue<Pos> cola;
    cola.push(p);
    set<Pos> visitats;
    visitats.insert(p);
    map<Pos, Pos> parent; // Para almacenar el camino recorrido
    bool paintFound = false;
    while (not cola.empty() and not paintFound) {
      Pos actual = cola.front();
      cola.pop();
      if (cell(actual).owner != me() and cell(actual).type != Elevator) {
        ppaint = actual;
        paintFound = true;
        // Reconstruir el camino recorrido
        stack<Pos> path;
        path.push(ppaint); // Agregar la posici贸n no pintada al camino
        Pos current = actual;
        while (parent.find(current) != parent.end() and current != p) {
          path.push(current);
          current = parent[current];
        }
        // Ir a la primera posici贸n del camino
        if (!path.empty()) {
          ppaint = path.top();
        }
        return;
      }
      vector<int> d = random_permutation(8);
      for (int x = 0; x < 8; ++x) {
        Pos k = actual + Dir(d[x]);
        if (pos_ok(k)) {
          set<Pos>::iterator it = visitats.find(k);
          if(it == visitats.end()) {
            visitats.insert(k);
            Cell c = cell(k);
            if (isValid(k) and (c.id == -1 or (c.id != -1 and abs(p.i - k.i) >= 2 and abs(p.j -p.j) >= 2 ) ) ) {
              cola.push(k);
              parent[k] = actual; // Almacenar el camino
            }
          }
        }
      }
    }
  }

  bool furyanNear(Pos p, Pos& penemy) {
    queue<Pos> cola;
    cola.push(p);
    set<Pos> visitats;
    visitats.insert(p);
    while(not cola.empty()) {
      Pos actual = cola.front();
      cola.pop();
      if(abs(p.i-actual.i) > 3 or abs(p.j-actual.j) > 3) return false;
      Cell c = cell(actual);
      if(c.id != -1) {
        Unit uenemy = unit(c.id);
        if(uenemy.player != me() and uenemy.type == Furyan) {
          penemy = uenemy.pos;
          return true;
        }
      }
      vector<int> d = random_permutation(8);
      for(int x = 0; x < 8; ++x) {
        Pos k = actual + Dir(d[x]);
        if (pos_ok(k)) {
          set<Pos>::iterator it = visitats.find(k);
          if (it == visitats.end()) {
            visitats.insert(k);
            if(isValid(k)) cola.push(k);
          }
        }
      }
    }
    return false;
  }

  void movePioneers(const vector<int>& P, vector<bool>& pio, set<Pos> visited) {
    int n = P.size();
    for(int i = 0; i < n; ++i) {
        if(not pio[i]) {
            int id = P[i];
            Unit u = unit(id);
            Pos pos = u.pos;
            Pos penemy;
            if(enemyNear(pos,penemy)) {
                command(id,Dir(flee(pos,penemy,true,visited))); //furyan enemigo en cell adyacente
            }else if(furyanNear(pos,penemy)){
                command(id,Dir(flee(pos,penemy,true,visited)));
            }else if(hellhoundNear(pos,penemy)) {
                command(id,Dir(flee(pos,penemy,true,visited)));
            }else {
                bool move = false;
                vector<int> d = random_permutation(8);

                for(int i = 0; i < 8 and not move; ++i) {
                if(pos_ok(pos+Dir(d[i]))) {
                    Cell c = cell(pos+Dir(d[i]));
                    set<Pos>::iterator it = visited.find(pos+Dir(d[i]));
                    if(c.type == Cave and c.owner != me() and c.id == -1 and it == visited.end()) {
                      move = true;
                      command(id,Dir(d[i]));
                      visited.insert(pos+Dir(d[i]));
                    }
                }
                }
                if(move == false) {
                Pos ppaint;
                goPaint(pos,ppaint);
                command(id,Dir(dirConverter(pos,ppaint,visited,true)));
                }
            }
        }
    }
  }

  void urgentePioneers(const vector<int>& P, vector<bool>& pio,set<Pos> visited) {
    int n = P.size();
    for(int i = 0; i < n;++i) {
      int id = P[i];
      Unit u = unit(id);
      Pos pos = u.pos;
      Pos penemy;
      if(enemyNear(pos,penemy)) {
        command(id,Dir(flee(pos,penemy,true,visited))); //furyan enemigo en cell adyacente
        pio[i] = true;
      }
    }
  }

  void urgenteFuryans(const vector<int>& F, vector<bool>& fur, set<Pos>& visited) {
    int n = F.size();
    for(int i = 0; i < n;++i) {
      int id = F[i];
      Unit u = unit(id);
      Pos pos = u.pos;
      Pos penemy;
      if(enemyToKill(pos,penemy)) {
        if(hellhoundNear(pos,penemy)) command(id,Dir(flee(pos,penemy,false,visited)));
        else command(id,Dir(dirConverter(pos,penemy,visited,true)));
        fur[i] = true;
      }
    }
  }

  virtual void play () {
    set<Pos> visited;
    vector<int> P = pioneers(me());
    vector<int> F = furyans(me());
    vector<bool> pio(P.size(),false);
    vector<bool> fur(F.size(),false);
    urgentePioneers(P,pio,visited);
    urgenteFuryans(F,fur,visited);
    movePioneers(P,pio,visited);
    moveFuryans(F,fur,visited);
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

