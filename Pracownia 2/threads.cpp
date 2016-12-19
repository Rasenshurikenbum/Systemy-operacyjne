#include <iostream>
#include <functional>
#include "threads.h"
using namespace std;
template <typename func_type, typename args_type> class thread
{
	public:
	function<func_type (args_type...)> func;
	va_list arg_list;
	thread(function <func_type (args_type...)> f_in, args_type Args... )
		{
		func = f_in;
		arg_list = Args;
		}
};


