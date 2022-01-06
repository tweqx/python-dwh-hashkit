#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>

#include "dwh_hashkit/hashbox.h"

// Wrapper around a hashbox object. It is used to prevent a double free() on the hashbox.
typedef struct {
  hashbox_t* hashbox;
  bool is_freed;
} hashbox_wrapper_t;

const char* capsule_name = "hashbox";

static void capsule_destructor(PyObject* self) {
  hashbox_wrapper_t* wrapper = (hashbox_wrapper_t*)PyCapsule_GetPointer(self, capsule_name);
  if (wrapper) {
    // Finalizes the hashbox if that wasn't already done. This frees the hashbox
    if (!wrapper->is_freed) {
      hashbox_final(wrapper->hashbox);
      wrapper->is_freed = true;
    }

    // Free the object associated with the hashbox
    free(wrapper);
  }
}

static PyObject*
dwh_hashkit_hashbox_new(PyObject *self, PyObject *args) {
  hashbox_t* hashbox = hashbox_new();
  if (hashbox == NULL)
    return PyErr_NoMemory();

  hashbox_wrapper_t* wrapper = (hashbox_wrapper_t*)malloc(sizeof(hashbox_wrapper_t));
  if (wrapper == NULL) {
    free(hashbox);

    return PyErr_NoMemory();
  }

  wrapper->hashbox = hashbox;
  wrapper->is_freed = false;

  return PyCapsule_New(wrapper, capsule_name, capsule_destructor);
}

static PyObject*
dwh_hashkit_hashbox_update(PyObject *self, PyObject *args) {
  PyObject* capsule;
  /*const*/ char* data;
  Py_ssize_t len;

  if (!PyArg_ParseTuple(args, "Os#", &capsule, &data, &len))
    return NULL;

  // Consistency checks
  if (!PyCapsule_IsValid(capsule, capsule_name)) {
    PyErr_Format(PyExc_TypeError, "Argument \"hashbox\" should be an object returned by \"new\" not a \"%s\"", Py_TYPE(capsule)->tp_name);
    return NULL;
  }

  hashbox_wrapper_t* wrapper = PyCapsule_GetPointer(capsule, capsule_name);

  if (wrapper->is_freed) {
    PyErr_SetString(PyExc_RuntimeError, "\"hashbox\" is already finalized");
    return NULL;
  }

  // Updating
  hashbox_update(wrapper->hashbox, (uint8_t*)data, len);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject*
dwh_hashkit_hashbox_final(PyObject *self, PyObject *args) {
  PyObject* capsule;

  if (!PyArg_ParseTuple(args, "O", &capsule))
    return NULL;

  // Consistency checks
  if (!PyCapsule_IsValid(capsule, capsule_name)) {
    PyErr_Format(PyExc_TypeError, "Argument \"hashbox\" should be an object returned by \"new\" not a \"%s\"", Py_TYPE(capsule)->tp_name);
    return NULL;
  }

  hashbox_wrapper_t* wrapper = PyCapsule_GetPointer(capsule, capsule_name);

  if (wrapper->is_freed) {
    PyErr_SetString(PyExc_RuntimeError, "\"hashbox\" is already finalized");
    return NULL;
  }

  // Finalization
  int match = hashbox_final(wrapper->hashbox);
  wrapper->is_freed = true;

  return Py_BuildValue("i", match);
}

static PyObject*
dwh_hashkit_hashbox_final_hashes(PyObject *self, PyObject *args) {
  PyObject* capsule;

  if (!PyArg_ParseTuple(args, "O", &capsule))
    return NULL;

  // Consistency checks
  if (!PyCapsule_IsValid(capsule, capsule_name)) {
    PyErr_Format(PyExc_TypeError, "Argument \"hashbox\" should be an object returned by \"new\" not a \"%s\"", Py_TYPE(capsule)->tp_name);
    return NULL;
  }

  hashbox_wrapper_t* wrapper = PyCapsule_GetPointer(capsule, capsule_name);

  if (wrapper->is_freed) {
    PyErr_SetString(PyExc_RuntimeError, "\"hashbox\" is already finalized");
    return NULL;
  }

  // Finalization
  hashes_list_t* hashes = hashbox_final_hashes(wrapper->hashbox);

  if (hashes == NULL) {
    wrapper->is_freed = true; // even if 'hashes' failed to be allocated, the hashbox will still be freed.
    return PyErr_NoMemory();
  }

  PyObject* dict_hashes = PyDict_New();
  if (dict_hashes == NULL) {
    wrapper->is_freed = true;
    return PyErr_NoMemory();
  }

  // Building the returned dictionnary
  PyDict_SetItemString(dict_hashes, "sha512",     PyBytes_FromStringAndSize((char*)hashes->sha512,     64));
  PyDict_SetItemString(dict_hashes, "blake2b",    PyBytes_FromStringAndSize((char*)hashes->blake2b,    64));
  PyDict_SetItemString(dict_hashes, "streebog",   PyBytes_FromStringAndSize((char*)hashes->streebog,   64));
  PyDict_SetItemString(dict_hashes, "sha3",       PyBytes_FromStringAndSize((char*)hashes->sha3,       64));
  PyDict_SetItemString(dict_hashes, "fnv0",       PyBytes_FromStringAndSize((char*)hashes->fnv0,       64));
  PyDict_SetItemString(dict_hashes, "fnv1",       PyBytes_FromStringAndSize((char*)hashes->fnv1,       64));
  PyDict_SetItemString(dict_hashes, "fnv1a",      PyBytes_FromStringAndSize((char*)hashes->fnv1a,      64));
  PyDict_SetItemString(dict_hashes, "grostl",     PyBytes_FromStringAndSize((char*)hashes->grostl,     64));
  PyDict_SetItemString(dict_hashes, "md6",        PyBytes_FromStringAndSize((char*)hashes->md6,        64));
  PyDict_SetItemString(dict_hashes, "jh",         PyBytes_FromStringAndSize((char*)hashes->jh,         64));
  PyDict_SetItemString(dict_hashes, "blake512",   PyBytes_FromStringAndSize((char*)hashes->blake512,   64));
  PyDict_SetItemString(dict_hashes, "lsh",        PyBytes_FromStringAndSize((char*)hashes->lsh,        64));
  PyDict_SetItemString(dict_hashes, "skein",      PyBytes_FromStringAndSize((char*)hashes->skein,      64));
  PyDict_SetItemString(dict_hashes, "keccak3",    PyBytes_FromStringAndSize((char*)hashes->keccak3,    64));
  PyDict_SetItemString(dict_hashes, "cubehash",   PyBytes_FromStringAndSize((char*)hashes->cubehash,   64));
  PyDict_SetItemString(dict_hashes, "whirlpool0", PyBytes_FromStringAndSize((char*)hashes->whirlpool0, 64));
  PyDict_SetItemString(dict_hashes, "whirlpoolT", PyBytes_FromStringAndSize((char*)hashes->whirlpoolT, 64));
  PyDict_SetItemString(dict_hashes, "whirlpool",  PyBytes_FromStringAndSize((char*)hashes->whirlpool,  64));

  wrapper->is_freed = true;
  free(hashes);

  return dict_hashes;
}

static PyMethodDef dwh_hashkit_methods[] = {
     {"new", dwh_hashkit_hashbox_new, METH_VARARGS, "Returns a hashbox object"},
     {"update", dwh_hashkit_hashbox_update, METH_VARARGS, "Updates the hashbox with data to be hashed"},
     {"final", dwh_hashkit_hashbox_final, METH_VARARGS, "Finalizes the hashbox and returns a constant indicating whether or not the deep web hash was found"},
     {"final_hashes", dwh_hashkit_hashbox_final_hashes, METH_VARARGS, "Finalizes the hashbox and returns a dictionnary of algorithm/hash pairs"},
     {NULL, NULL, 0, NULL}
};

static struct PyModuleDef dwh_hashkit_module =
{
    PyModuleDef_HEAD_INIT,
    "dwh_hashkit_c",
    NULL,
    -1,
    dwh_hashkit_methods
};

PyMODINIT_FUNC
PyInit_dwh_hashkit_c(void) {
     return PyModule_Create(&dwh_hashkit_module);
}
