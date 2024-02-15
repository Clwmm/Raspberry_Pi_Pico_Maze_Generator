#include "Cell.h"

Cell::Cell(const short& x, const short& y)
{
  this->ite.x = x;
  this->ite.y = y;
  this->pos.x = x * CELL_SIZE;
  this->pos.y = y * CELL_SIZE;

  neighbors.setStorage(neighbors_storage);

  topLine.p0.x = pos.x;
  topLine.p0.y = pos.y;
  topLine.p1.x = pos.x + CELL_SIZE;
  topLine.p1.y = pos.y;

  botLine.p0.x = pos.x;
  botLine.p0.y = pos.y + CELL_SIZE;
  botLine.p1.x = pos.x + CELL_SIZE;
  botLine.p1.y = pos.y + CELL_SIZE;

  rightLine.p0.x = pos.x + CELL_SIZE;
  rightLine.p0.y = pos.y;
  rightLine.p1.x = pos.x + CELL_SIZE;
  rightLine.p1.y = pos.y + CELL_SIZE;

  leftLine.p0.x = pos.x;
  leftLine.p0.y = pos.y;
  leftLine.p1.x = pos.x;
  leftLine.p1.y = pos.y + CELL_SIZE;
}

void Cell::draw(TFT_eSPI& tft)
{
  if (!top)
    tft.drawLine(topLine.p0.x, topLine.p0.y, topLine.p1.x, topLine.p1.y, TFT_BLACK);
  else
    tft.drawLine(topLine.p0.x+1, topLine.p0.y, topLine.p1.x-1, topLine.p1.y, TFT_WHITE);
  if (!bot)
    tft.drawLine(botLine.p0.x, botLine.p0.y, botLine.p1.x, botLine.p1.y, TFT_BLACK);
  else
    tft.drawLine(botLine.p0.x+1, botLine.p0.y, botLine.p1.x-1, botLine.p1.y, TFT_WHITE);
  if (!right)
    tft.drawLine(rightLine.p0.x, rightLine.p0.y, rightLine.p1.x, rightLine.p1.y, TFT_BLACK);
  else
    tft.drawLine(rightLine.p0.x, rightLine.p0.y+1, rightLine.p1.x, rightLine.p1.y-1, TFT_WHITE);
  if (!left)
    tft.drawLine(leftLine.p0.x, leftLine.p0.y, leftLine.p1.x, leftLine.p1.y, TFT_BLACK);
  else
    tft.drawLine(leftLine.p0.x, leftLine.p0.y+1, leftLine.p1.x, leftLine.p1.y-1, TFT_WHITE);
}

Vector<Cell*> Cell::getNeighbors()
{
  return neighbors;
}

void Cell::setPrevious(Cell* cell)
{
  this->previous = cell;
}

Cell* Cell::getPrevious()
{
  return this->previous;
}

Point Cell::getPos()
{
  return pos;
}

void Cell::addNeighbor(Cell* cell)
{
  neighbors.push_back(cell);

  if (cell->pos.x > this->pos.x && cell->pos.y == this->pos.y)
      right = true;
  else if (cell->pos.x < this->pos.x && cell->pos.y == this->pos.y)
      left = true;
  else if (cell->pos.x == this->pos.x && cell->pos.y > this->pos.y)
      bot = true;
  else if (cell->pos.x == this->pos.x && cell->pos.y < this->pos.y)
      top = true;
  
  //this->neighbors[this->nOneighbors] = cell;
  //++this->nOneighbors;
}