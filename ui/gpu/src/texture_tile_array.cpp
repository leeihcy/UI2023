#include "stdafx.h"
#include "texture_tile_array.h"
#include <assert.h>

TextureTile2DArray::TextureTile2DArray()
{
    m_ppArray = nullptr;
    m_row = 0;
    m_col = 0;
}
TextureTile2DArray::~TextureTile2DArray()
{
    Destroy();
}

void  TextureTile2DArray::Destroy()
{
    if (m_ppArray)
    {
        for (int y = 0; y < m_row; y++)    
        {
            delete[] m_ppArray[y];
        }
        delete[] m_ppArray;
    }

    m_row = 0;
    m_col = 0;
}

void  TextureTile2DArray::Create(int row, int col)
{
    if (row == m_row && col == m_col)
        return;

    assert(row > 0 && col > 0);

    Destroy();

    m_ppArray = new LPTEXTURETILE*[row];
    for (int y = 0; y < row; y++)
    {
        m_ppArray[y] = new LPTEXTURETILE[col];
    }
    m_row = row;
    m_col = col;
}

int  TextureTile2DArray::GetRow()
{
    return m_row;
}
int  TextureTile2DArray::GetCol()
{
    return m_col;
}