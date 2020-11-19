
set Defines=-D ILSENGINE_INTERNAL_DEBUG -D ILSENGINE_DEBUG
set IncludePaths=-I ..\librairies\glfw\include\ -I ..\librairies\Glad\include\ -I ..\IllusionEngine\include
set Libs=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib ..\librairies\glfw\lib-vc2019\glfw3.lib
set Src=..\librairies\Glad\src\glad.c
pushd IllusionEngine\src
for /R %%f in (*.cpp) do echo %%f && call set Src= %%Src%% %%f
popd
echo %Src% 

IF NOT EXIST build mkdir build
pushd build
cl.exe -FC -Zi -EHsc -MD -FeIllusionEngine.exe %IncludePaths% %IncludePaths% %IncludePaths% %Defines% %Src% %Libs%
popd