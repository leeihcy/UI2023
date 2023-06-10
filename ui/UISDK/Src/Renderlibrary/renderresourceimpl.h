#pragma  once

// 为了实现不导出IRenderResource类，只暴露纯接口，将IRenderResource的一些实现采用模板类来实现
namespace UI
{
interface IRenderResource;

template<class T>
class IRenderResourceImpl : public T
{
protected:
    IRenderResourceImpl()
    {
        m_dwRef = 0;
        m_ppOutRef = nullptr;
    }

public:
    virtual ~IRenderResourceImpl()
    {
        if (m_ppOutRef!=nullptr)
        {
            *m_ppOutRef = nullptr;
            m_ppOutRef = nullptr;
        }
    }
    //virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() = 0;

    void   SetOutRef(IRenderResource** ppOutRef) { m_ppOutRef = ppOutRef; }

    long   AddRef()
    {
        m_dwRef ++;
        return m_dwRef;
    }
    long   Release()
    {
        m_dwRef--;
        if (0 >= m_dwRef)
        {
            UIASSERT(m_dwRef==0);  // 有可能调用者忘记addref，直接release了

            delete this;
            return 0;
        }

        return m_dwRef;
    }

protected:
    long   m_dwRef;
    IRenderResource**   m_ppOutRef;  // 外部引用，用于创建单例和release自动释放
    // 为了解决需要在CImageItem中保存一份Image*指针，要外部Get时赋值，但这个保存的UIImage*又不能给它计算引用。
    // 当所有外部的Release结束后，CImageItem::Image*却无法置空，因此在这里添加一个m_pOutRef，指向这个
    // Image*，在析构函数中将中置空
};
}