rmdir /Q /S .vs
rmdir /Q /S x64
rmdir /Q /S bin

cd Framework
call _clean.bat
cd ..
