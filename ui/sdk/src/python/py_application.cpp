#include "py_types.h"

#define self_app ((PyApplication *)self)->app

static int __init__(PyObject *self, PyObject *args, PyObject *kwds) {
  self_app = ui::IApplication::create();
  return 0;
}
static void __dealloc__(PyObject *self) {
  self_app.reset();
}
static PyObject *__str__(PyObject *self) {
  return Py_BuildValue("s", "Application");
}
static PyObject *__repr__(PyObject *self) {
  return Py_BuildValue("s", "uisdk.Application()");
}

static PyObject *PyRun(PyObject *self) {
  self_app->Run();
  Py_RETURN_NONE;
}
static PyObject *PyQuit(PyObject *self, PyObject* args) {
  self_app->Quit();
  Py_RETURN_NONE;
}

static PyTypeObject typeinfo = {PyVarObject_HEAD_INIT(NULL, 0)};
static PyMethodDef Methods[] = {
    // 用METH_NOARGS标志来指示该方法不需要除self之外的任何参数）
    {"run", (PyCFunction)&PyRun, METH_NOARGS, nullptr},
    // 使用METH_VARARGS，兼容win.connect("destroy", app.quit)场景
    {"quit", (PyCFunction)&PyQuit, METH_VARARGS, nullptr},
    {NULL, NULL, 0, NULL}
};

bool InitPyApplication(PyObject *module) {
  PyObject *pModule = nullptr;

  typeinfo.tp_name = "uisdk.Application";
  typeinfo.tp_basicsize = sizeof(PyApplication);
  typeinfo.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  typeinfo.tp_methods = Methods;

  typeinfo.tp_init = (initproc)&__init__;
  typeinfo.tp_dealloc = (destructor)&__dealloc__;
  typeinfo.tp_repr = (reprfunc)&__repr__;
  typeinfo.tp_str = (reprfunc)&__str__;
  typeinfo.tp_new = PyType_GenericNew;
  
  if (PyType_Ready(&typeinfo) < 0)
    return false;

  if (-1 == PyModule_AddObject(module, "Application", (PyObject *)&typeinfo)) {
    PyErr_Print();
    return false;
  }
  return true;
}