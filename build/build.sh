clang++ -std=c++2b --precompile -x c++-module ../utils/src/vrock.utils.ByteArray.ixx
clang++ -std=c++2b -c vrock.utils.ByteArray.pcm 
clang++ -std=c++2b -stdlib=libstdc++ -fprebuilt-module-path=. vrock.utils.ByteArray.o ../utils/examples/example_ByteArray.cpp -o example_ByteArray