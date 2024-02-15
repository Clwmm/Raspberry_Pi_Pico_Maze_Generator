#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#include "Cell.h"
Cell* grid[MAZE_W][MAZE_H];

#include "StackArray.h"
StackArray<Cell*> stack;

#include <Vector.h>
Vector<Cell*> openSet;
Cell* openSet_storage[MAZE_W*MAZE_H];
Vector<Cell*> closedSet;
Cell* closedSet_storage[MAZE_W*MAZE_H];
Cell* start;
Cell* end_;

Vector<Cell*> path;
Cell* path_storage[MAZE_W*MAZE_H];

Vector<Cell*> oldPath;
Cell* oldPath_storage[MAZE_W*MAZE_H];

bool solved = false;


void drawPath();
float heuristics(Cell* neighbor, Cell* e);
void removeFromSet(Vector<Cell*>& set, Cell* cell);
bool includedInSet(Vector<Cell*>& set, Cell* cell);
Cell* checkNeighborsAndReturnRandom(const Point& ite);
void clearScreen();


void setup(void) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);

  for (short i = 0; i < MAZE_W; ++i)
  {
    for (short j = 0; j < MAZE_H; ++j)
    {
      grid[i][j] = new Cell(i, j);
    }
  }
  stack.push(grid[0][0]);
  grid[0][0]->visited = true;

  start = grid[0][0];
  end_ = grid[MAZE_W - 1][MAZE_H - 1];

  openSet.setStorage(openSet_storage);
  closedSet.setStorage(closedSet_storage);

  openSet.push_back(start);

  path.setStorage(path_storage);
  oldPath.setStorage(oldPath_storage);

  randomSeed(analogRead(26));

  // Generating
  
  while (stack.count() > 0)
  {
    auto current = stack.pop();

    auto next = checkNeighborsAndReturnRandom(current->ite);
    if (next)
    {
      stack.push(current);
      current->addNeighbor(next);
      next->addNeighbor(current);
      next->visited = true;
      stack.push(next);
    }
  }

  // Solving
  while (openSet.size() > 0)
  {
    short winner = 0;
    for (size_t i  = 0; i < openSet.size(); ++i)
    {
      if (openSet[i]->f < openSet[winner]->f)
        winner = i;
    }

    auto current = openSet[winner];

    if (current == end_)
    {
      path.clear();
      auto temp = current;
      path.push_back(temp);
      while(temp->getPrevious())
      {
        path.push_back(temp->getPrevious());
        temp = temp->getPrevious();
      }
      break;
    }

    removeFromSet(openSet, current);
    closedSet.push_back(current);

    auto neighbours = current->getNeighbors();
    for (auto x : neighbours)
    {
      if (!includedInSet(closedSet, x))
      {
        auto tempG = current->g + 1;

        if (includedInSet(openSet, x))
        {
          if (tempG < x->g)
          {
            x->g = tempG;
          }
        }
        else
        {
          x->g = tempG;
          openSet.push_back(x);
        }

        x->h = heuristics(x, end_);
        x->f = x->g + x->h;
        x->setPrevious(current);
      }
    }
   
  }
  
}

void loop() {

  for (short i = 0; i < MAZE_W; ++i)
  {
    for (short j = 0; j < MAZE_H; ++j)
    {
      grid[i][j]->draw(tft);
    }
  }

  drawPath();
  
  delay(1000);
}

void drawPath()
{
  Point p0;
  Point p1;
  bool first = true;

  // OLD PATH - ERASING THE OLD LINES
  for (size_t i = 0; i < oldPath.size(); ++i)
  {
    if (first)
    {
      p0.x = oldPath[i]->getPos().x + (CELL_SIZE / 2);
      p0.y = oldPath[i]->getPos().y + (CELL_SIZE / 2);
      first = false;
    }
    else
    {
      p1.x = oldPath[i]->getPos().x + (CELL_SIZE / 2);
      p1.y = oldPath[i]->getPos().y + (CELL_SIZE / 2);
    
      tft.drawLine(p0.x, p0.y, p1.x, p1.y, TFT_WHITE);

      p0.x = oldPath[i]->getPos().x + (CELL_SIZE / 2);
      p0.y = oldPath[i]->getPos().y + (CELL_SIZE / 2);
    }
  }

  first = true;
  // ACTUAL PATH - DRAWING THE ACTUAL LINES
  delay(100);
  for (size_t i = 0; i < path.size(); ++i)
  {
    if (first)
    {
      p0.x = path[i]->getPos().x + (CELL_SIZE / 2);
      p0.y = path[i]->getPos().y + (CELL_SIZE / 2);
      first = false;
    }
    else
    {
      p1.x = path[i]->getPos().x + (CELL_SIZE / 2);
      p1.y = path[i]->getPos().y + (CELL_SIZE / 2);
    
      tft.drawLine(p0.x, p0.y, p1.x, p1.y, TFT_GREEN);

      p0.x = path[i]->getPos().x + (CELL_SIZE / 2);
      p0.y = path[i]->getPos().y + (CELL_SIZE / 2);
    }
  }

  oldPath = path;
  for (size_t i = 0; i < MAZE_W*MAZE_H; ++i)
  {
    oldPath_storage[i] = path_storage[i];
  }
}

float heuristics(Cell* neighbor, Cell* e)
{
  float x_dif = abs(neighbor->ite.x - e->ite.x);
  float y_dif = abs(neighbor->ite.y - e->ite.y);
    
  return sqrt(pow(x_dif, 2) + pow(y_dif, 2));
}

void removeFromSet(Vector<Cell*>& set, Cell* cell)
{
  for (size_t i = 0; i < set.size(); ++i)
  {
    if (set[i] == cell)
    {
      set.remove(i);
    }
  }
}

bool includedInSet(Vector<Cell*>& set, Cell* cell)
{
  for (size_t i = 0; i < set.size(); ++i)
  {
    if (set[i] == cell)
    {
      return true;
    }
  }
  return false;
}

Cell* checkNeighborsAndReturnRandom(const Point& ite)
{
  byte number = 0;
  Cell* temp[4];

  if (ite.y - 1 >= 0)
  {
    if (grid[ite.x][ite.y - 1]->visited == false)
    {
      temp[number] = grid[ite.x][ite.y - 1];
      ++number;
    }
  }

  if (ite.y + 1 < MAZE_H)
  {
    if (grid[ite.x][ite.y + 1]->visited == false)
    {
      temp[number] = grid[ite.x][ite.y + 1];
      ++number;
    }
  }

  if (ite.x - 1 >= 0)
  {
    if (grid[ite.x - 1][ite.y]->visited == false)
    {
      temp[number] = grid[ite.x - 1][ite.y];
      ++number;
    }
  }

  if (ite.x + 1 < MAZE_W)
  {
    if (grid[ite.x + 1][ite.y]->visited == false)
    {
      temp[number] = grid[ite.x + 1][ite.y];
      ++number;
    }
  }
  if (number == 0)
    return nullptr;

  byte a = random(200) % number;
  return temp[a];   
}

void clearScreen()
{
  for (byte i = 1; i < MAZE_W; ++i)
    tft.drawLine(i*CELL_SIZE, 0, i*CELL_SIZE, WINDOW_SIZE_Y, TFT_BLACK);

  for (byte i = 1; i < MAZE_H; ++i)
    tft.drawLine(0, i*CELL_SIZE, WINDOW_SIZE_X, i*CELL_SIZE, TFT_BLACK);
}