#include "include/event.h"
#include "py_types.h"
#include <iostream>

#define self_win ((PyWindow *)self)->win

static int __init__(PyObject *self, PyObject *args, PyObject *kwds) {
  PyObject *app = nullptr;
  const char *title = nullptr;

  // 注意：需要在kwlist中给所有的position args用""占位，否则会报参数个数不对。
  // 这什么鬼逻辑。
  const char *kwlist[] = {"", "title", nullptr};

  int ret = PyArg_ParseTupleAndKeywords(
      args, kwds, "O|s", const_cast<char **>(kwlist), &app, &title);
  if (!ret) {
    PyErr_Print();
    return 0;
  }

  self_win = ui::IWindow::create(((PyApplication *)app)->app->RootBundle());
  ui::Rect rc = {0, 0, 400, 400};
  self_win->Create(nullptr, &rc);

  if (title) {
    self_win->SetTitle(title);
  }
  return 0;
}

static void __dealloc__(PyObject *self) {
  self_win.reset();
  // std::cout << "PyWindow __dealloc__" << std::endl;
}
static PyObject *__str__(PyObject *self) {
  return Py_BuildValue("s", "Window");
}
static PyObject *__repr__(PyObject *self) {
  return Py_BuildValue("s", "uisdk.Window()");
}

static PyObject *PyShow(PyObject *self) {
  self_win->Show();
  Py_RETURN_NONE;
}

static void OnEvent(PyObjectPtr callable, ui::Event *event) {
  // std::cout << "callable=" << callable.ptr << ", ref=" <<  callable->ob_refcnt << std::endl;
  if (!PyCallable_Check(callable.ptr)) {
    PyErr_BadInternalCall();
    return;
  }

  PyObjectPtr eo = MakePyEvent(event);
  PyObjectPtr args(Py_BuildValue("(O)", *eo));

  PyObject *result = PyObject_CallObject(callable.ptr, *args);
  if (!result) {
    PyErr_Print();
  }
}
static PyObject *PyConnect(PyObject *self, PyObject *args) {
  const char* event = nullptr;
  PyObject* callback = nullptr;

  PyArg_ParseTuple(args, "sO", &event, &callback);

  // 要增加callback的引用计数，否则可能会销毁了。
  self_win->connect(event, ui::Slot(OnEvent, PyObjectPtr(callback)));
  Py_RETURN_NONE;
}


static PyTypeObject typeinfo = {PyVarObject_HEAD_INIT(NULL, 0)};
static PyMethodDef Methods[] = {
    {"show", (PyCFunction)&PyShow, METH_VARARGS, nullptr},
    {"connect", (PyCFunction)&PyConnect, METH_VARARGS, nullptr},
    {NULL, NULL, 0, NULL}
};

bool InitPyWindow(PyObject *module) {
  PyObject *pModule = nullptr;

  typeinfo.tp_name = "uisdk.Window";
  typeinfo.tp_basicsize = sizeof(PyWindow);
  typeinfo.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  typeinfo.tp_methods = Methods;

  typeinfo.tp_init = (initproc)&__init__;
  typeinfo.tp_dealloc = (destructor)&__dealloc__;
  typeinfo.tp_repr = (reprfunc)&__repr__;
  typeinfo.tp_str = (reprfunc)&__str__;
  typeinfo.tp_new = PyType_GenericNew;
  
  if (PyType_Ready(&typeinfo) < 0)
    return false;

  if (-1 == PyModule_AddObject(module, "Window", (PyObject *)&typeinfo)) {
    PyErr_Print();
    return false;
  }
  return true;
}