windres icon.rc -O coff -o icon.res
gcc RefreshTaskbarThumbnails.cpp -lstdc++ -ldwmapi -mwindows -DUNICODE -o ../bin/RefreshTaskbarThumbnails.exe icon.res