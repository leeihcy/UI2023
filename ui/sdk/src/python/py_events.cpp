#include "include/event.h"
#include "py_types.h"

#define self_event ((PyEvent *)self)->event

static PyTypeObject typeinfo = {PyVarObject_HEAD_INIT(NULL, 0)};
static PyMethodDef Methods[] = {
    {NULL, NULL, 0, NULL}
};

PyObjectPtr MakePyEvent(ui::Event* e) {
  PyEvent* obj = PyObject_New(PyEvent, &typeinfo);
  obj->event = e;
  return PyObjectPtr((PyObject*)obj, false);
}

bool InitPyEvent(PyObject *module) {
  PyObject *pModule = nullptr;

  typeinfo.tp_name = "uisdk.Event";
  typeinfo.tp_basicsize = sizeof(PyEvent);
  typeinfo.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  typeinfo.tp_methods = Methods;

  typeinfo.tp_new = PyType_GenericNew;
  
  if (PyType_Ready(&typeinfo) < 0)
    return false;

  if (-1 == PyModule_AddObject(module, "Event", (PyObject *)&typeinfo)) {
    PyErr_Print();
    return false;
  }
  return true;
}