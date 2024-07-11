#pragma once

//defines a assembly we import

#include <FloofySharp/BTDSTD/Types/String.hpp>

#include <string>

namespace CSRuntime::Assembly
{
	//function pointer types for handling DLLs and their managed code
	typedef int (*LoadAssembly)(const char_t*, void*, void*); //loads a DLL
	typedef int (*GetManagedFunctionPointer)(const char_t*, const char_t*, const char_t*, void*, void*, void**); //gets a function from C# land
	
	typedef void(*ManagedEntrypoint)(); //calls a C# function
	typedef void(*ManagedEntrypoint_WithString)(const char*); //calls a C# function

	//defines a Assembly library we are using and will call functions from
	struct Assembly_DLL
	{
		Assembly::GetManagedFunctionPointer getManagedFunctionPtr = nullptr; //callback for getting function pointers

		std::wstring workingDir = L""; //the working directory for the DLL
		std::wstring assemblyName = L""; //the name of the assembly

		//sets the assembly name

		//sets the working directory

		//gets a function
		inline bool GetFunction(const char_t* className, const char_t* delagateSig, const char_t* funcName, void** func)
		{
			if (!getManagedFunctionPtr)
			{
				printf("Call the function \"LoadAssembly\" from a instance of the \"CSharpRuntime\" and pass in this Assembly_DLL. Before calling \"GetFunction\" as we don't have the needed data on this Assembly yet.\n");
				return false;
			}

			//gets function
			const std::wstring classStr = assemblyName + L"." + className + L", " + assemblyName;
			const std::wstring delagateSignetureStr = assemblyName + L"." + className + L"+" + delagateSig + L", " + assemblyName;

			getManagedFunctionPtr(classStr.c_str(), funcName, delagateSignetureStr.c_str(), NULL, NULL, func);

			if (!func) {
				printf("Failed to get Managed Function, from Assembly (\"%ws\") calling Class: \"%ws\", delagateSig (\"%ws\"), funcName: \"%ws\"\n",
					assemblyName.c_str(), className, delagateSig, funcName);

				auto rc = GetLastError();
				std::cout << rc << std::endl;
				return false;
			}

			return true;
		}

		//gets and executes a function
		inline bool GetAndExecuteManagedFunc(const char_t* className, const char_t* delagateSig, const char_t* funcName)
		{
			if (!getManagedFunctionPtr)
			{
				printf("Call the function \"LoadAssembly\" from a instance of the \"CSharpRuntime\" and pass in this Assembly_DLL. Before calling \"GetAndExecuteManagedFunc\" as we don't have the needed data on this Assembly yet.\n");
				return false;
			}

			//gets func
			Assembly::ManagedEntrypoint func = nullptr;
			if (!GetFunction(className, delagateSig, funcName, (void**)&func))
				return false;

			//calls func
			func();

			return true;
		}

		//gets and executes a function
		inline bool GetAndExecuteManagedFunc_String(const char_t* className, const char_t* delagateSig, const char_t* funcName)
		{
			if (!getManagedFunctionPtr)
			{
				printf("Call the function \"LoadAssembly\" from a instance of the \"CSharpRuntime\" and pass in this Assembly_DLL. Before calling \"GetAndExecuteManagedFunc\" as we don't have the needed data on this Assembly yet.\n");
				return false;
			}

			//gets func
			Assembly::ManagedEntrypoint_WithString func = nullptr;
			if (!GetFunction(className, delagateSig, funcName, (void**)&func))
				return false;

			//calls func
			func("This is a string we were given OWOWOWOOWOWOWOWOWOWWO");

			return true;
		}
	};

	//Defines a EXE Assembly we are wanting to run inside the runtime
	struct Assembly_EXE
	{
		std::vector<const char_t*> args; //defines the args to pass to the EXE when we run it
		/*
		index 0 will always be the path to the DLL we want to run
		*/

		Assembly_EXE()
		{
			args.resize(1);
		}

		//sets the DLL to execute || index 0 will always be the path to the DLL we want to run
		inline void SetDLLToExecute(const char_t* pathToDLLToExecute)
		{
			args[0] = pathToDLLToExecute;
		}

		//adds a argument

		//removes a argument
	};
}