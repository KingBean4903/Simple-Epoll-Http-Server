# CMake generated Testfile for 
# Source directory: /home/baimungi/Documents/C_STUDIES/Simple_Epoll_Server
# Build directory: /home/baimungi/Documents/C_STUDIES/Simple_Epoll_Server/include
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(http_server "/home/baimungi/Documents/C_STUDIES/Simple_Epoll_Server/include/test_server")
set_tests_properties(http_server PROPERTIES  _BACKTRACE_TRIPLES "/home/baimungi/Documents/C_STUDIES/Simple_Epoll_Server/CMakeLists.txt;52;add_test;/home/baimungi/Documents/C_STUDIES/Simple_Epoll_Server/CMakeLists.txt;0;")
subdirs("_deps/json-build")
subdirs("_deps/catch2-build")
