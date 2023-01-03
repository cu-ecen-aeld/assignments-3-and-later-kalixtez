#include "systemcalls.h"

int main()
{
	bool retv = do_exec(2, "echo", "Hello");
	printf(retv ? "true" : "false");

	return 0;
}