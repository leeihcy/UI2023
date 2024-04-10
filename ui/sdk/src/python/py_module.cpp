#include <iostream>
#include <Python.h>

static PyObject* py_ping(PyObject* self, PyObject* args) {
    std::cout << "ping" << std::endl;
    return PyUnicode_FromString("pong");
}

// 定义模块方法表
static PyMethodDef UISdkMethods[] = {
    { "ping", py_ping, METH_NOARGS, "ping pong." },
    { nullptr, nullptr, 0, nullptr }
};

// 定义模块
static struct PyModuleDef nes_module = {
    PyModuleDef_HEAD_INIT,
    "uisdk",            // 模块名
    nullptr,             // 文档
    -1,                  // 变量大小
    UISdkMethods         // 方法表
};

// import时会触发这个函数
// 不定义这个函数import会报错：
// dynamic module does not define module export function (PyInit_nesc)
PyMODINIT_FUNC PyInit_uisdk(void) {
    return PyModule_Create(&nes_module);
}