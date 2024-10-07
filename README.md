# anything2obj

Compile a text or binary file into a .o file referenced by a global
variable.

This is useful when there is a text file (maybe JSON format) or a binary
file like a GIF that is needed to be compiled into the main executable.

An example of this is in the example/ directory of the repo. There
is a text file called info.json which has:

    {
      "name": "mike",
      "phone": "555-5555"
    }

Compiling it with:

    ./anything2obj -v json -i test/info.json out.o

creates out.o which is equivalent to a C file that has:

    const char *json = "{\n  \"name\": \"mike\",\n  \"phone\": \"555-5555\"\n}\n";

The out.o file can be linked with a C, C++, etc program. The program
references the global name with:

    extern const char *json;

The example/testing.c shows how this is done. To test this type:

    make
    make test
    ./testing

Note that the type: const char * could really be changed to any pointer.
Could make it const uint8_t * or even const uint32_t *. The linker really
doesn't care.

In C++, it's possible the variable name needs to be name mangled or
it might require extern "C" to stop it:

    extern "C"
    {
      extern const char *json;
    }

Oddly, it doesn't seem that C++ is name mangling global variable symbols
so that might not be required.

Currently only x86-64 is supported, but it would be easy to make it
support other platforms.

