#pragma once
class TextureTile;
typedef TextureTile* LPTEXTURETILE;

class TextureTile2DArray
{
public:
    TextureTile2DArray();
    ~TextureTile2DArray();

    void  Create(ULONG row, ULONG col);
    void  Destroy();

    ULONG  GetRow();
    ULONG  GetCol();

    LPTEXTURETILE*& operator [](int row)
    {
        return m_ppArray[row];
    }

private:
    LPTEXTURETILE**  m_ppArray;
    ULONG  m_row;
    ULONG  m_col;
};