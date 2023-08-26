#pragma  once
#include "Inc/Interface/renderlibrary.h"

namespace ui
{
class UICursor : public IUICursor
{
private:
    UICursor();
public:
    ~UICursor();

    static void CreateInstance2( const std::string strCurFilePath, UICursor** pOutRef );
    static void CreateInstance2( LPTSTR nCursorID, HINSTANCE hInstance, UICursor** ppOutRef );
    void     SetOutRef(UICursor** ppOutRef) { m_pOutRef = ppOutRef; }
    bool     Load(LPTSTR nCursorID, HINSTANCE hInstance);
    bool     Load(const wchar_t* szCursorFile);
    void     Attach(HCURSOR hCursor);
    HCURSOR  Detach();
    
    virtual HCURSOR  GetCursor();
    virtual long  AddRef();
    virtual long  Release();

private:
    long      m_dwRef;
    HCURSOR   m_hCursor;

    UICursor** m_pOutRef;
};

}