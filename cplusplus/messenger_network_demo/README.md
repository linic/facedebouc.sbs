# Requirements
Install libsodium on your dev environment. Either by following
https://doc.libsodium.org/installation
or by `sudo apt update` and maybe 
`sudo apt install libsodium-dev build-essential g++ autoconf autotools-dev`

Also install fltk. [Here's how I did.](https://facedebouc.sbs/cplusplus/fltk.html)

Also install yaml-cpp by `sudo apt update` and `sudo apt install libyaml-cpp-dev`
If you encounter any issues with finding the library or headers, you might need to specify the include and library paths explicitly. The default locations are usually:

Headers: /usr/include/yaml-cpp
Library: /usr/lib/x86_64-linux-gnu/libyaml-cpp.so (for 64-bit systems)

You can specify these paths in your compilation command if needed:
`g++ your_program.cpp -o your_program -I/usr/include/yaml-cpp -L/usr/lib/x86_64-linux-gnu -lyaml-cpp`

This should install the yaml-cpp library and the necessary header files.

When compiling your program, you'll need to link against the yaml-cpp library. If you're using g++ directly, you can do this by adding -lyaml-cpp to your compilation command. For example:
`g++ your_program.cpp -o your_program -lyaml-cpp`

If you're using CMake, you can add the following to your CMakeLists.txt:
```
find_package(yaml-cpp REQUIRED)
target_link_libraries(your_target yaml-cpp)
```
Make sure to include the yaml-cpp header in your C++ files:
`#include <yaml-cpp/yaml.h>`

# Usage
`make`
You'll get an executable which works with a configuration.yaml file.
If you want to test locally, copy the executable and the yaml file
to another folder, invert the sending and receiving ports and run 2
instances.

You could also have a friend run the other executable on the same network
and after adjusting the destination IP to the one of your friend, you'll
be able to communicate.

If your friend is at home behind a router and you too, you could configure
port forwarding on each side and share your public IPs, then adjust your
configuration.yaml file before running the programs.

You'll have debug information printed to the terminal by running from the 
terminal. You'll be able to follow the logic.
Enter some words in the text field next to the send button and click on
the send button to send them.

