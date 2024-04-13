#ifndef _UISDK_SRC_PYTHON_PYTYPES_H_
#define _UISDK_SRC_PYTHON_PYTYPES_H_

#include "include/interface/iwindow.h"
#include "include/interface/iuiapplication.h"
#include <Python.h>

//
// 用于将PyObject绑定到回调函数中(ui Slot)，并增加引用计数，维持生命周期。
//
class PyObjectPtr{
public:
  PyObjectPtr(PyObject* o) : ptr(o){
    Py_XINCREF(ptr);
  }
  PyObjectPtr(const PyObjectPtr& o) {
    this->ptr = o.ptr;
    Py_XINCREF(ptr);
  }
  PyObjectPtr(PyObjectPtr&& o) {
    this->ptr = o.ptr;
    o.ptr = nullptr;
  }
  ~PyObjectPtr() {
    Py_XDECREF(ptr);
  }
  PyObject* operator->() {
    return ptr;
  }
  PyObject* operator*() {
    return ptr;
  }
public:
  PyObject* ptr;
};

//
// app = uisdk.Application()
// app.run()
//

typedef struct {
  PyObject_HEAD;
  ui::IApplicationPtr app;
} PyApplication;


//
// win = uisdk.Window(app)
// win.show()
//

typedef struct {
  PyObject_HEAD;
  ui::IWindowPtr win;
} PyWindow;


#endif
