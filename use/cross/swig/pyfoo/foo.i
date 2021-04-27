%module(directors="1", "threads"=1) pyfoo
%{
#include "foo.h"

#define SWIG_PYTHON_STRICT_BYTE_CHAR
%}

%feature("director");

%include "../foo/include/foo_macro.h"
%include "../foo/include/foo_struct.h"
%include "../foo/include/foo.h"
