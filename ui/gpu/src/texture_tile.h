#ifndef _UI_GPU_SRC_TEXTURE_TILE_H_
#define _UI_GPU_SRC_TEXTURE_TILE_H_
#include "sdk/include/common/math/rect.h"
#include "gpu/include/api.h"

namespace ui {
class TextureTile {
public:
  virtual ~TextureTile() {}

  void SetIndex(int xIndex, int yIndex);

  virtual void Upload(ui::Rect &dirty_of_tile, ui::Rect &dirty_of_layer,
                      ui::GpuUploadBitmap &source) = 0;

protected:
  long m_width = 0;
  long m_height = 0;
  long m_xIndex = 0;
  long m_yIndex = 0;
};
typedef TextureTile *TextureTilePtr;

class TextureTile2DArray {
public:
  TextureTile2DArray();
  ~TextureTile2DArray();

  void Create(int row, int col);
  void Destroy();

  int GetRow();
  int GetCol();
  int GetCount();

  TextureTilePtr *&operator[](int row) { return m_ppArray[row]; }

protected:
  TextureTilePtr ** doCreate(int row, int col);

private:
  TextureTilePtr **m_ppArray;
  int m_row;
  int m_col;
};
}
#endif