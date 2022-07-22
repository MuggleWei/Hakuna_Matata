#ifndef FOO_H_
#define FOO_H_

#if WIN32 && defined(foo_USE_DLL)
	#ifdef foo_EXPORTS
		#define FOO_EXPORT __declspec(dllexport)
	#else
		#define FOO_EXPORT __declspec(dllimport)
	#endif
#else
	#define FOO_EXPORT
#endif

enum eVarType
{
	VAR_TYPE_UNKNOWN      = 0, // unknown variable type
	VAR_TYPE_CLASS_STATIC = 1, // class static variable
	VAR_TYPE_FUNC_STATIC  = 2, // function static variable
	VAR_TYPE_GLOBAL       = 3, // global variable
};

class Foo
{
public:
	FOO_EXPORT
	Foo(eVarType var_type);
	FOO_EXPORT
	virtual ~Foo();

	FOO_EXPORT
	static Foo* InstanceClassStatic();
	FOO_EXPORT
	static Foo* InstanceExternGlobal();
	FOO_EXPORT
	static Foo* InstanceFuncStatic();

public:
	static Foo s_class_static_var;

public:
	eVarType var_type;
	int num;
};

extern Foo g_foo;

#endif /* ifndef FOO_H_ */
