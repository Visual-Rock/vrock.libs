clang++ -std=c++2b --precompile -x c++-module ../utils/src/ByteArray.ixx -o vrock.utils.ByteArray.pcm
clang++ -std=c++2b -c vrock.utils.ByteArray.pcm -o vrock.utils.ByteArray.o
clang++ -std=c++2b --precompile -x c++-module ../utils/src/List.ixx -o vrock.utils.List.pcm
clang++ -std=c++2b -c vrock.utils.List.pcm -o vrock.utils.List.o
clang++ -std=c++2b -stdlib=libstdc++ -fprebuilt-module-path=. vrock.utils.ByteArray.o vrock.utils.List.o ByteArray.o ../utils/examples/example_ByteArray.cpp -o example_ByteArray