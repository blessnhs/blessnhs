protoc.exe -I=./ --csharp_out=./CSharp ./Structure.proto
protoc.exe -I=./ --csharp_out=./CSharp ./Enum.proto
protoc.exe -I=./ --csharp_out=./CSharp ./CLI.GS.proto
protoc.exe -I=./ --csharp_out=./CSharp ./GS.CLI.proto

protoc.exe -I=./ --cpp_out=./Cpp ./Structure.proto
protoc.exe -I=./ --cpp_out=./Cpp ./Enum.proto
protoc.exe -I=./ --cpp_out=./Cpp ./CLI.GS.proto
protoc.exe -I=./ --cpp_out=./Cpp ./GS.CLI.proto

protoc.exe -I=./ --python_out=./Python ./Structure.proto
protoc.exe -I=./ --python_out=./Python ./Enum.proto
protoc.exe -I=./ --python_out=./Python ./CLI.GS.proto
protoc.exe -I=./ --python_out=./Python ./GS.CLI.proto
