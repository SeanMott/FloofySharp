# FloofySharp

Floofy Sharp a soft and comfty, straight forward C header only* library for using C# in C/C++ via the CLR.

# Examples

All the code needed to init the runtime, load a Assembly, execute a C# function, and then clean up.
> C++ Land
![CppCpde](/READMEAssets/ExampleImages_1/CppCode.png?raw=true "Optional Title")

> C# Land
![C#Code](/READMEAssets/ExampleImages_1/CSharpCode.png?raw=true "tional Title")

> Execute
![C#Code](/READMEAssets/ExampleImages_1/ProgramOutput.png?raw=true "tional Title")

> [!NOTE]
> Floofy Sharp depends on [.NET 8+ Runtime](https://dotnet.microsoft.com/en-us/download/dotnet). It is capable of running earlier revitions, but this is the one I reccomand.

# Out In The Wild

- Floofy Sharp is used in the [Smok]() and [Hologram]() game engines. And is provided bundled with the [Bytes The Dust SDK]().

# Using In Your Project

Floofy Sharp is implemented purly in header files. It does require a link against `nethost.lib` and the project using it to find or carry the `nethost.dll` for it to work.

# To C or Cpp, That Is The Question

FS has a almost identical API for C and C++. The main differance is in C++, it offers the ability to use templates when casting the returned Managed Function.

They can be found under `includes/FloofySharp/C` and `includes/FloofySharp/Cpp` respectivly.

It's reccomanded just make FloofySharp your included folder. As it depends on a few other headers inside.

# Resources
> [!TIP]
> You can view examples under [FS C UwU](), [FS Cpp UwU](), and [FS Executes A C# EXE]()