qmake -tp vc -r build.pro

set choice=n
set /p choice=Do you want to open DetectAndMatch.sln(y/n,default to n)?
IF %choice% == y (DetectAndMatch.sln) ELSE (ECHO Do nothing!)
