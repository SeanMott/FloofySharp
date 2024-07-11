#pragma once

//manages loading DLLs

#include <FloofySharp/BTDSTD/Types/String.hpp>

#if defined(_WIN32)

//include a stripped windows.h
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

namespace BTD::IO
{
	//defines a loaded DLL
	struct DLL
	{
		//Windows
#if defined(_WIN32)
		std::wstring path = L"";
		HMODULE libraryPtr = nullptr;

	//Linux
#else
		std::string path = "";
		void* libraryPtr = nullptr;
#endif
	};

	//loads a DLL
	inline bool LoadDLL(DLL& dll, const char_t* path)
	{
		//Windows
#if defined(_WIN32)
		dll.libraryPtr = ::LoadLibraryW(path);

		//Linux
#else
		dll.libraryPtr = dlopen(path, RTLD_LAZY | RTLD_LOCAL);

#endif

		if (!dll.libraryPtr)
		{
			printf("BTD STD ERROR: DLL LOADING || func<boo> LoadDLL(DLL& dll, const char_t* path) || Failed to load DLL at Path \"%ws\"\n",
				path);
			return false;
		}

		dll.path = path;
		return true;
	}

	//unloads a DLL

	//gets a function from the DLL
	inline void* GetDLLExportedFunction(DLL& dll, const char_force_not_wide_t* name)
	{
		void* funcPtr = nullptr;

		//Windows
#if defined(_WIN32)
		funcPtr = ::GetProcAddress(dll.libraryPtr, name);

		//Linux
#else
		funcPtr = dlsym(dll.libraryPtr, name);

#endif

		if (!funcPtr)
		{
			printf("BTD STD ERROR: DLL LOADING || func<void*> GetDLLExportedFunction(DLL& dll, const char_force_not_wide_t* name) || Failed to load DLL function \"%s\"\n",
				name);
		}

		return funcPtr;
	}

	//gets a function from the DLL
	template<typename Func>
	inline Func GetDLLExportedFunction(DLL& dll, const char_force_not_wide_t* name)
	{
		return (Func)GetDLLExportedFunction(dll, name);
	}
}