#pragma once

//manages loading DLLs

#include <BTDSTD/Types/Chars.h>
#include <cstdio>

#if defined(_WIN32)

//include a stripped windows.h
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

	//defines a loaded DLL
	struct BTD_IO_DLL
	{
		const char_t* path = NULL;

		//Windows
#if defined(_WIN32)
		HMODULE libraryPtr = nullptr;

		//Linux
#else
		void* libraryPtr = nullptr;
#endif
	} typedef BTD_IO_DLL;

	//loads a DLL
	inline int LoadDLL(BTD_IO_DLL* dll, const char_t* path)
	{
		//Windows
#if defined(_WIN32)
		dll->libraryPtr = ::LoadLibraryW(path);

		//Linux
#else
		dll->libraryPtr = dlopen(path, RTLD_LAZY | RTLD_LOCAL);

#endif

		if (!dll->libraryPtr)
		{
			printf("BTD STD ERROR: DLL LOADING || func<boo> LoadDLL(DLL& dll, const char_t* path) || Failed to load DLL at Path \"%ws\"\n",
				path);
			return 0;
		}

		dll->path = path;
		return 1;
	}

	//unloads a DLL

	//gets a function from the DLL
	inline void* BTD_IO_GetDLLExportedFunction(BTD_IO_DLL* dll, const char_force_not_wide_t* name)
	{
		void* funcPtr = nullptr;

		//Windows
#if defined(_WIN32)
		funcPtr = ::GetProcAddress(dll->libraryPtr, name);

		//Linux
#else
		funcPtr = dlsym(dll->libraryPtr, name);

#endif

		if (!funcPtr)
		{
			printf("BTD STD ERROR: DLL LOADING || func<void*> GetDLLExportedFunction(DLL& dll, const char_force_not_wide_t* name) || Failed to load DLL function \"%s\"\n",
				name);
		}

		return funcPtr;
	}
}