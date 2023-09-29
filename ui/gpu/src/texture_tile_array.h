#pragma once
class TextureTile;
typedef TextureTile* LPTEXTURETILE;

class TextureTile2DArray
{
public:
    TextureTile2DArray();
    ~TextureTile2DArray();

    void  Create(int row, int col);
    void  Destroy();

    int  GetRow();
    int  GetCol();

    LPTEXTURETILE*& operator [](int row)
    {
        return m_ppArray[row];
    }

private:
    LPTEXTURETILE**  m_ppArray;
    int  m_row;
    int  m_col;
};