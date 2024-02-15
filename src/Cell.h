#ifndef CELL_H
#define CELL_H

#include "Arduino.h"
#include "settings.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Vector.h>

struct Point
{
  short x;
  short y;
};

struct Line
{
  Point p0;
  Point p1;
};

class Cell
{
public:
  bool visited = false;
  float f = 0;
  float h = 0;
  float g = 0;
  Cell(const short& x, const short& y);
  void draw(TFT_eSPI& tft);
  void addNeighbor(Cell* cell);
  Vector<Cell*> getNeighbors();

  void setPrevious(Cell* cell);
  Cell* getPrevious();

  Point getPos();

  Point ite;
  
private:
  Point pos;
  Cell* previous = nullptr;

  //byte nOneighbors = 0;
  Vector<Cell*> neighbors;
  Cell* neighbors_storage[4];
  //Cell* neighbors[4] = {nullptr, nullptr, nullptr, nullptr};


  bool top = false, bot = false, right = false, left = false;
  // Boreder lines
  Line topLine;
  Line botLine;
  Line rightLine;
  Line leftLine;
};


#endif