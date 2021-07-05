#pragma once
#include "TypeList.h"

#ifdef REMOVE_BOOST
#else
#include <boost/shared_ptr.hpp>
#endif

template<typename R,class TList>
class FunctorImpl;

template<typename R>
class FunctorImpl<R,Empty>
{
public:
	virtual R operator() () = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<typename R,typename P1>
class FunctorImpl<R,TYPELIST_1(P1)>
{
public:
	virtual R operator() (P1) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<typename R,typename P1,typename P2>
class FunctorImpl<R,TYPELIST_2(P1,P2)>
{
public:
	virtual R operator() (P1,P2) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<typename R,typename P1,typename P2,typename P3>
class FunctorImpl<R,TYPELIST_3(P1,P2,P3)>
{
public:
	virtual R operator() (P1,P2,P3) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};

template<class ParentFunctor,typename Fun>
class FunctorHandler : public FunctorImpl
	<
	typename ParentFunctor::ResultType,
	typename ParentFunctor::ParmList
	>
{
public:
	typedef typename  ParentFunctor::ResultType ResultType;

	FunctorHandler(const Fun &fun) : _fun(fun) {}
	FunctorHandler* Clone() const { return new FunctorHandler(*this); }

	ResultType operator()()
	{
		return _fun();
	}

	ResultType operator()(typename ParentFunctor::Param1 p1)
	{
		return _fun(p1);
	}

	ResultType operator()(typename ParentFunctor::Param1 p1,typename ParentFunctor::Param2 p2)
	{
		return _fun(p1,p2);
	}


	ResultType operator()(typename ParentFunctor::Param1 p1,typename ParentFunctor::Param2 p2,typename ParentFunctor::Param3 p3)
	{
		return _fun(p1,p2,P3);
	}
public:
	Fun _fun;
};

template<class ParentFunctor,typename PointerToObj,typename PointerToMemFn>
class MemFunHandler : public FunctorImpl
	<
	typename ParentFunctor::ResultType,
	typename ParentFunctor::ParmList
	>
{
public:
	typedef typename  ParentFunctor::ResultType ResultType;

	MemFunHandler(const PointerToObj &pObj,PointerToMemFn pMemFn) : pObj_(pObj),pMemFn_(pMemFn) {}
	MemFunHandler* Clone() const { return new MemFunHandler(*this); }

	ResultType operator()()
	{
		return ((*pObj_).*pMemFn_)();
	}

	ResultType operator()(typename ParentFunctor::Param1 p1)
	{
		return  ((*pObj_).*pMemFn_)(p1);
	}

	ResultType operator()(typename ParentFunctor::Param1 p1,typename ParentFunctor::Param2 p2)
	{
		return ((*pObj_).*pMemFn_)(p1,p2);
	}

	ResultType operator()(typename ParentFunctor::Param1 p1,typename ParentFunctor::Param2 p2,typename ParentFunctor::Param3 p3)
	{
		return ((*pObj_).*pMemFn_)(p1,p2,p3);
	}
public:
	PointerToObj	pObj_;
	PointerToMemFn	pMemFn_;
};

template<typename R,class TList>
class Functor
{
public:
	typedef FunctorImpl<R,TList> Impl;

	typedef TList ParmList;
	typedef typename TypeAt<TList,0>::Result  Param1;
	typedef typename TypeAt<TList,1>::Result  Param2;
	typedef typename TypeAt<TList,2>::Result  Param3;
	typedef R ResultType;

public:
	Functor(){}
	Functor(const Functor&);
	Functor& operator-=(const Functor &);
	explicit Functor(Impl spImpl);

	template<typename Fun,typename ClassType>
	Functor(const Fun fun,const ClassType &classType);

	template<typename Fun>
	Functor(const Fun fun);

	R operator()()
	{
		return (*spImpl)();
	}

	R operator()(Param1 p1)
	{
		return (*spImpl)(p1);
	}

	R operator()(Param1 p1,Param2 p2)
	{
		return (*spImpl)(p1,p2);
	}

	R operator()(Param1 p1,Param2 p2,Param2 p3)
	{
		return (*spImpl)(p1,p2,p3);
	}

public:
#ifdef REMOVE_BOOST
	std::shared_ptr<Impl> spImpl;
#else
	boost::shared_ptr<Impl> spImpl;
#endif
	
};


template<typename R,class TList>
template<typename Fun,typename ClassType>
Functor<R,TList>::Functor(const Fun fun,const ClassType &classType)
	:spImpl(new MemFunHandler<Functor,ClassType,Fun>(classType,fun))
{

}

template<typename R,class TList>
template<typename Fun>
Functor<R,TList>::Functor(const Fun fun)
	:spImpl(new FunctorHandler<Functor,Fun>(fun))
{

}

/*//// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
//#include <string>
//#include <stdio.h>
//#include <Windows.h>
//#define PY_SSIZE_T_CLEAN
//#include "Python3.8/Python.h"
//#include "pybind11/pybind11.h"
//
//namespace py = pybind11;
//
//class Pet
//{
//public:
//    Pet(const std::string& name, int hunger) : name(name), hunger(hunger) {}
//    ~Pet() {}
//
//    void go_for_a_walk() { hunger++; }
//    const std::string& get_name() const { return name; }
//    int get_hunger() const { return hunger; }
//
//private:
//    std::string name;
//    int hunger;
//};
//
//int add(int i, int j) {
//    return i + j;
//}
//
//PYBIND11_MODULE(example, m) {
//    // optional module docstring
//    m.doc() = "pybind11 example plugin";
//
//    // define add function
//    m.def("add", &add, "A function which adds two numbers");
//
//    // bindings to Pet class
//    py::class_<Pet>(m, "Pet")
//        .def(py::init<const std::string&, int>())
//        .def("go_for_a_walk", &Pet::go_for_a_walk)
//        .def("get_hunger", &Pet::get_hunger)
//        .def("get_name", &Pet::get_name);
//}
//
//int
//main(int argc, char* argv[])
//{
//    PyObject* pName, * pModule, * pFunc;
//    PyObject* pArgs, * pValue;
//    int i;
//
//    if (argc < 3) {
//        fprintf(stderr, "Usage: call pythonfile funcname [args]\n");
//        return 1;
//    }
//
//    Py_Initialize();
//    PyRun_SimpleString("import sys");
//    PyRun_SimpleString("sys.path.append(\".\")");
//    pName = PyUnicode_DecodeFSDefault(argv[1]);
//    /* Error checking of pName left out */
//
//    pModule = PyImport_Import(pName);
//    Py_DECREF(pName);
//
//    if (pModule != NULL) {
//        pFunc = PyObject_GetAttrString(pModule, argv[2]);
//        /* pFunc is a new reference */
//
//        if (pFunc && PyCallable_Check(pFunc)) {
//            pArgs = PyTuple_New(argc - 3);
//            for (i = 0; i < argc - 3; ++i) {
//                pValue = PyLong_FromLong(atoi(argv[i + 3]));
//                if (!pValue) {
//                    Py_DECREF(pArgs);
//                    Py_DECREF(pModule);
//                    fprintf(stderr, "Cannot convert argument\n");
//                    return 1;
//                }
//                /* pValue reference stolen here: */
//                PyTuple_SetItem(pArgs, i, pValue);
//            }
//            pValue = PyObject_CallObject(pFunc, pArgs);
//            Py_DECREF(pArgs);
//            if (pValue != NULL) {
//                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
//                Py_DECREF(pValue);
//            }
//            else {
//                Py_DECREF(pFunc);
//                Py_DECREF(pModule);
//                PyErr_Print();
//                fprintf(stderr, "Call failed\n");
//                return 1;
//            }
//        }
//        else {
//            if (PyErr_Occurred())
//                PyErr_Print();
//            fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
//        }
//        Py_XDECREF(pFunc);
//        Py_DECREF(pModule);
//    }
//    else {
//        PyErr_Print();
//        fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
//        return 1;
//    }
//    if (Py_FinalizeEx() < 0) {
//        return 120;
//    }
//    return 0;
//}
//
//#include "cpp/CLI.GS.pb.h"
//#include "cpp/GS.CLI.pb.h"
//#include "cpp/Structure.pb.h"
//
//#include <stdio.h>
//#include <Python3.8/Python.h>
//
//class CPyInstance
//{
//public:
//	CPyInstance()
//	{
//		Py_Initialize();
//	}
//
//	~CPyInstance()
//	{
//		Py_Finalize();
//	}
//};
//
//
//static PyObject* arnav_foo(PyObject* self, PyObject* args)
//{
//	printf_s("... in C++...: foo() method\n");
//	return PyLong_FromLong(51);
//}
//
//static PyObject* arnav_show(PyObject* self, PyObject* args)
//{
//	PyObject* obj;
//
//	if (!PyArg_ParseTuple(args, "O", &obj)) {
//		// error
//	}
//
//	PyObject* iter = PyObject_GetIter(obj);
//	if (!iter) {
//		// error not iterator
//	}
//
//	int pos = 0;
//	char foo[256];
//	memset(foo, 0, sizeof(foo));
//	while (true) {
//		PyObject* next = PyIter_Next(iter);
//		if (!next) {
//			// nothing left in the iterator
//			break;
//		}
//
//		if (!PyFloat_Check(next)) {
//			// error, we were expecting a floating point value
//		}
//
//		foo[pos++] = PyLong_AsLong(next);
//		// do something with foo
//	}
//	VERSION_RES res;
//
//	bool _result_ = res.ParseFromArray(foo, pos);
//
//	return PyFloat_FromDouble(0);
//
//	//if (PyArg_UnpackTuple(args, "", 1, 5, &a))
//	//{
//	//	//a->
//	//	//bool _result_ = VERSION_RES.ParseFromArray(a->ob_type,5);
//	//	
//
//	//	printf_s("C++: show(%ld)\n", PyLong_AsLong(a));
//	//}
//
//	return PyLong_FromLong(0);
//}
//
//static struct PyMethodDef methods[] = {
//	{ "foo", arnav_foo, METH_VARARGS, "Returns the number"},
//	{ "show", arnav_show, METH_VARARGS, "Show a number" },
//	{ NULL, NULL, 0, NULL }
//};
//
//static struct PyModuleDef modDef = {
//	PyModuleDef_HEAD_INIT, "arnav", NULL, -1, methods,
//	NULL, NULL, NULL, NULL
//};
//
//static PyObject* PyInit_arnav(void)
//{
//	return PyModule_Create(&modDef);
//}
//
//int main()
//{
//	PyImport_AppendInittab("arnav", &PyInit_arnav);
//
//	CPyInstance hInstance;
//
//	const char pFile[] = "pyemb6.py";
//	FILE* fp = _Py_fopen(pFile, "r");
//	PyRun_AnyFile(fp, pFile);
//
//	return 0;
//}/*
//
// #define PY_SSIZE_T_CLEAN
//#include <Python3.7/Python.h>
//#include <thread>
//#include <chrono>
//#include <windows.h>
//
//PyThreadState *threadState;
//void longRunningFunction() {
//	PyThreadState *t = PyEval_SaveThread();
//	std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	PyEval_RestoreThread(t);
//}
//void run() {
//	// Prepare
//	PyEval_RestoreThread(threadState);
//	PyThreadState *t = Py_NewInterpreter();
//	// Execute Python code
//	// For simplicity of the example, this is mixed with C++ code.
//	// Usually you would have the for loop and the call
//	// to longRunningFunction() in Python code.
//
//	while (true)
//	{
//		PyRun_SimpleString("x = 0");
//		for (int i = 0; i < 5; i++) {
//			PyRun_SimpleString("x += 1; print(x, flush=True)");
//		}
//
//	//	longRunningFunction();
//
//	}
//
//	// Clean up
//	Py_EndInterpreter(t);
//	PyThreadState_Swap(threadState);
//	PyEval_SaveThread();
//}
//#define THREAD_COUNT 10
//int main() {
//	// Initialize Python
//	Py_Initialize();
//	threadState = PyEval_SaveThread();
//	// Start threads
//	std::thread thread[THREAD_COUNT];
//	for (int i = 0; i < THREAD_COUNT; i++) {
//		thread[i] = std::thread(run);
//	}
//	// Wait for threads to finish
//	for (int i = 0; i < THREAD_COUNT; i++) {
//		thread[i].join();
//	}
//	// Finalize Python
//	Py_Finalize();
//}
