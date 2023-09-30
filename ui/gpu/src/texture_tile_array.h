#ifndef _UI_GPU_SRC_TEXTURE_TILE_ARRAY_H_
#define _UI_GPU_SRC_TEXTURE_TILE_ARRAY_H_

namespace ui {
class TextureTile;
typedef TextureTile *LPTEXTURETILE;

class TextureTile2DArray {
public:
  TextureTile2DArray();
  ~TextureTile2DArray();

  void Create(int row, int col);
  void Destroy();

  int GetRow();
  int GetCol();

  LPTEXTURETILE *&operator[](int row) { return m_ppArray[row]; }

protected:
  LPTEXTURETILE ** doCreate(int row, int col);

private:
  LPTEXTURETILE **m_ppArray;
  int m_row;
  int m_col;
};
}
#endif