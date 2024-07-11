#pragma once

//defines the runtime for managing C# Assemblys in C++

#if defined(_WIN32)

//include a stripped windows.h
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

//assert and output
#include <assert.h>
#include <iostream>

//Host CLR
#include <FloofySharp/RuntimeDeps/nethost.h>
#include <FloofySharp/RuntimeDeps/coreclr_delegates.h>
#include <FloofySharp/RuntimeDeps/hostfxr.h>

//BTDSTD Path and String includes
#include <FloofySharp/BTDSTD/IO/DLLoad.hpp>

//since GetLastError doesn't exit on Linux we define it
#if defined(__linux__)
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int GetLastError() { return 0; }
#endif

#include <FloofySharp/Cpp/Assembly.hpp>

namespace CSRuntime
{
	//defines the runtime
	struct CSharpRuntime
	{
		//CLR Context
		bool inited = false; //is the CLR inited
		hostfxr_handle CLRContext = nullptr; //the context for the CLR Host
		BTD::IO::DLL hostfxr_dll;

		//---FUNCTION POINTERS---//

		//CLR initalizing and deinitalizing
		hostfxr_initialize_for_runtime_config_fn hostfxr_funcPtr_initConfig = nullptr; //initalizes the runtime and it's config data
		hostfxr_initialize_for_dotnet_command_line_fn hostfxr_funcPtr_initCLI = nullptr; //initalizes the context for CLI
		hostfxr_close_fn hostfxr_funcPtr_Close = nullptr; //closes the CLR, we don't use it as far as this comment knows (dated 01-29-24)

		//CLR properties
		hostfxr_set_runtime_property_value_fn hostfxr_funcPtr_SetRuntimePropery = nullptr; //sets the runtime property
		hostfxr_get_runtime_properties_fn hostfxr_funcPtr_GetRuntimeProps = nullptr; //gets the runtime properties

		//loading assemblies

		//mic
		hostfxr_get_runtime_delegate_fn hostfxr_funcPtr_GetRuntimeDelegate = nullptr; //gets a function from managed C# land
		hostfxr_run_app_fn hostfxr_funcPtr_RunApp = nullptr; //runs the app	

		//----FUNCTIONS----//

		//initalizes the runtime
		inline bool Init()
		{
			//gets the hostfxr path automatically
			get_hostfxr_parameters params{ sizeof(get_hostfxr_parameters), nullptr, nullptr };
			char_t hostfxrPath[999];
			size_t buffer_size = sizeof(hostfxrPath) / sizeof(char_t);
			if (get_hostfxr_path(hostfxrPath, &buffer_size, &params) != 0)
			{
				std::cout << "Failed to find HostFXR on your system! Please make sure you have the Dot Net SDK or Runtime installed. Thank you.\n";
				return false;
			}

			//loads hostfxr
			if(!BTD::IO::LoadDLL(hostfxr_dll, hostfxrPath))
			{
				std::cout << "Failed to load HostFXR DLL! Please make sure you have the Dot Net SDK or Runtime installed. Thank you.\n";
				return false;
			}

			inited = true;
			return true;
		}

		//desinitalize the runtime
		inline void Shutdown()
		{
			if (inited)
			{
				//quits the runtime
				hostfxr_funcPtr_Close = BTD::IO::GetDLLExportedFunction<hostfxr_close_fn>(hostfxr_dll, "hostfxr_close");
				hostfxr_funcPtr_Close(CLRContext);

				//clear the libs

				inited = false;
			}
		}

		//sets a property
		inline bool SetCLRProperty(const char_t* propertyName, const char_t* value)
		{
			if (!inited)
			{
				std::cout << "CLR ERROR: Call \"Init\" first before calling any other functions with the runtime!" << std::endl;
				return false;
			}

			if (!hostfxr_funcPtr_SetRuntimePropery)
			{
				hostfxr_funcPtr_SetRuntimePropery = BTD::IO::GetDLLExportedFunction<hostfxr_set_runtime_property_value_fn>(hostfxr_dll, "hostfxr_set_runtime_property_value");
			}

			printf("OwO Property Setting Changed: \"%ws\" || \"%ws\"\n", propertyName, value);
			hostfxr_funcPtr_SetRuntimePropery(CLRContext, propertyName, value);

			return true;
		}

		//gets all the properties
		inline bool GetAllProperties(std::vector<const char_t*>& propKeys, std::vector<const char_t*>& propValues, size_t& propCount)
		{
			if (!inited)
			{
				std::cout << "CLR ERROR: Call \"Init\" first before calling any other functions with the runtime!" << std::endl;
				return false;
			}

			hostfxr_funcPtr_GetRuntimeProps(CLRContext, &propCount, nullptr, nullptr);
			propKeys.resize(propCount); propValues.resize(propCount);
			hostfxr_funcPtr_GetRuntimeProps(CLRContext, &propCount, propKeys.data(), propValues.data());

			return true;
		}

		//executes a DLL Assembly
		inline bool LoadAssembly(const Assembly::Assembly_DLL& assembly)
		{
			if (!inited)
			{
				std::cout << "CLR ERROR: Call \"Init\" first before calling any other functions with the runtime!" << std::endl;
				return false;
			}

			//gets function ptrs
			hostfxr_funcPtr_initConfig = BTD::IO::GetDLLExportedFunction<hostfxr_initialize_for_runtime_config_fn>(hostfxr_dll, "hostfxr_initialize_for_runtime_config");
			hostfxr_funcPtr_GetRuntimeProps = BTD::IO::GetDLLExportedFunction<hostfxr_get_runtime_properties_fn>(hostfxr_dll, "hostfxr_get_runtime_properties");
			hostfxr_funcPtr_SetRuntimePropery = BTD::IO::GetDLLExportedFunction<hostfxr_set_runtime_property_value_fn>(hostfxr_dll, "hostfxr_set_runtime_property_value");
			if(!hostfxr_funcPtr_GetRuntimeDelegate)
				hostfxr_funcPtr_GetRuntimeDelegate = BTD::IO::GetDLLExportedFunction<hostfxr_get_runtime_delegate_fn>(hostfxr_dll, "hostfxr_get_runtime_delegate");

			//initalizes the config data
			int rc = hostfxr_funcPtr_initConfig(std::wstring(assembly.workingDir + L"/" + assembly.assemblyName + L".runtimeconfig.json").c_str(), nullptr, &CLRContext);
			if (CLRContext == nullptr || rc != 0)
			{
				std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
				printf("Failed to load JSON Assembly runtime data for Assembly: \"%ws\" in Directory: \"%ws\". Please verify DLL name and directory.\n",
					assembly.assemblyName.c_str(), assembly.workingDir.c_str());
				return false;
			}

			//gets all the properties and render them
			printf("CS Runtime: Loading Config data from \"%ws\"\n\n", std::wstring(assembly.workingDir + L"/" + assembly.assemblyName + L".runtimeconfig.json").c_str());
			
			std::vector<const char_t*> propKeys; std::vector<const char_t*> propValues; size_t propCount = 0;
			GetAllProperties(propKeys, propValues, propCount);
			for (size_t i = 0; i < propCount; ++i)
			{
				printf("Key: %ws, Value: %ws\n\n", propKeys[i], propValues[i]);
			}
			propCount = 0; propKeys.clear(); propValues.clear();

			//sets the working directory
			SetCLRProperty(L"APP_CONTEXT_BASE_DIRECTORY", assembly.workingDir.c_str());

			//loads Assembly into memory
			Assembly::LoadAssembly loadAssemblyData = nullptr;
			hostfxr_funcPtr_GetRuntimeDelegate(CLRContext, hdt_load_assembly, (void**)&loadAssemblyData);
			if (!loadAssemblyData) {
				std::cout << "Load Assembly not found" << std::endl;

				auto rc = GetLastError();
				std::cout << rc << std::endl;
				return false;
			}
			std::cout << "Load Assembly found" << std::endl;

			//gets the C# function
			if (!assembly.getManagedFunctionPtr)
			{
				hostfxr_funcPtr_GetRuntimeDelegate(CLRContext, hdt_get_function_pointer, (void**)&assembly.getManagedFunctionPtr);
				if (!assembly.getManagedFunctionPtr) {
					std::cout << "managedEntryPoint not found" << std::endl;

					auto rc = GetLastError();
					std::cout << rc << std::endl;
					return false;
				}
				std::cout << "managedEntryPoint found" << std::endl;
				loadAssemblyData(std::wstring(assembly.workingDir + L"/" + assembly.assemblyName + L".dll").c_str(), NULL, NULL);
			}

			return true;
		}

		//executes a EXE Assembly
		inline bool ExecuteAssembly_EXE(Assembly::Assembly_EXE& assembly)
		{
			if (!inited)
			{
				std::cout << "CLR ERROR: Call \"Init\" first before calling any other functions with the runtime!" << std::endl;
				return false;
			}

			//get the function ptrs
			hostfxr_funcPtr_RunApp = BTD::IO::GetDLLExportedFunction<hostfxr_run_app_fn>(hostfxr_dll, "hostfxr_run_app");
			hostfxr_funcPtr_initCLI = BTD::IO::GetDLLExportedFunction<hostfxr_initialize_for_dotnet_command_line_fn>(hostfxr_dll, "hostfxr_initialize_for_dotnet_command_line");

			//initalize for CLI
			std::cout << "initalizing CLR Context for Dot Net" << std::endl;
			int rc = hostfxr_funcPtr_initCLI(assembly.args.size(), assembly.args.data(), nullptr, &CLRContext);
			if (CLRContext == nullptr || rc != 0)
			{
				std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
				return -1;
			}

			//run the app
			if (!hostfxr_funcPtr_RunApp)
			{
				std::cout << "hostfxr_run_app was null" << std::endl;
				return -1;
			}
			hostfxr_funcPtr_RunApp(CLRContext);
		}
	};
}