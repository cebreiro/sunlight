"../sl/generator/api/protoc-29.0-win64/bin/protoc.exe" ^
--proto_path=../sl/generator/api/ ^
--cpp_out=../sl/generator/api/generated/ ^
dto.proto ^
request.proto ^
response.proto

"../sl/generator/api/protoc-29.0-win64/bin/protoc.exe" ^
--proto_path=../sl/generator/api/ ^
--csharp_out=../sl/tool/dotnet/ManagementStudio/Models/Controller/Message/ ^
dto.proto ^
request.proto ^
response.proto 


pause