:: ------------------------------------------------------------------
:: _________                                    
:: \_   ___ \_______  ____   ____  __ __  ______
:: /    \  \/\_  __ \/    \ /    \|  |  \/  ___/
:: \     \____|  | \(  ( ) )   |  \  |  /\___ \ 
::  \______  /|__|   \____/|___|  /____//____  >
::         \/                   \/           \/  
:: ----- Descrição: -------------------------------------------------
::
:: Batch Script para compilar o projeto em ambiente Windows.
:: Feito por: Diego da Silva Pinheiro (diegopinheiro.sjc@hotmail.com)
::---------------------------------------------------------------------

CLS
@ECHO OFF
color 0B

SETLOCAL EnableExtensions
SETLOCAL EnableDelayedExpansion
::Manter Duas linhas de espaço da variável LF
set LF=^


:: ----- Compilador: -------------------------------------
set flags= -march=native -O2 -pipe -std=c99 -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-switch -Wno-sign-compare -Wno-gnu-zero-variadic-macro-arguments -Wno-empty-translation-unit -fno-strict-aliasing -Wshadow -Wcast-qual -Werror-implicit-function-declaration -Wno-format-non-iso -Wshorten-64-to-32 -Winline -Wstrict-prototypes -DFD_SETSIZE=1024
:: Para testes com GCC... (Não toque)
::set flags= -march=native -O3 -pipe -std=c99 -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-switch -Wno-sign-compare -fno-strict-aliasing -Wshadow -Werror-implicit-function-declaration -DFD_SETSIZE=1024
:: ----- Nomes: ------------------------------------------
:: Você pode alterar o nome de todo mundo aqui (Caso modifique, altere o arquivo start.bat)
set CharServer=char-server
set LoginServer=login-server
set MapServer=map-server

:: ----- Pastas: ------------------------------------------
set charF=%~dp0src\char\
set commonF=%~dp0src\common\
set configF=%~dp0src\config\
set loginF=%~dp0src\login\
set mapF=%~dp0src\map\
set myF=%~dp0src\3rdparty\mysql\include\
set pcreF=%~dp0src\3rdparty\pcre\include\
set zlibF=%~dp0src\3rdparty\zlib\include\
set myLib=%~dp0src\3rdparty\mysql\lib\libmysql.a
set pcreLib=%~dp0src\3rdparty\pcre\lib\pcre.a
set zlibLib=%~dp0src\3rdparty\zlib\lib\zlib.a
set mtF=%~dp0src\3rdparty\mt19937ar\
set lcF=%~dp0src\3rdparty\libconfig\

:: ----- Quem precisa de quem...: ------------------------
:: OBS: Os elementos a seguir precisam ser delimitados apenas por espaço (SEM QUEBRA DE LINHA)

:: mt19937ar
set mtS=mt19937ar
:: LibConfig
set DepS=libconfig grammar scanctx strbuf scanner
::Obj_all
set sharedS=sql conf db ers mapindex md5calc mutex random showmsg strlib thread timer utils console core malloc socket
:: Char-Server
set CharS=char int_auction int_elemental int_guild int_homun int_mail int_mercenary int_party int_pet int_quest int_storage inter pincode
:: Login-Server
set LoginS=account login loginlog
:: Map-Server
set MapS=atcommand battle battleground buyingstore chat chrif clif date duel elemental guild intif itemdb log mail map mapreg homunculus instance mercenary mob npc npc_chat party path pc pc_groups pet quest script searchstore skill status storage trade unit vending

:: ----- Main-----------------------------------------------
echo -------------------------------------------------------
echo _________                                    
echo \_   ___ \_______  ____   ____  __ __  ______
echo /    \  \/\_  __ \/    \ /    \^|  ^|  ^\/  ___/
echo \     \____^|  ^| ^\(  ( ) )   ^|  ^\  ^|  /\___ \ 
echo  \______  /^|__^|   ^\____/^|___^|  /____//____  ^>
echo         \/                   \/           \/  
echo --------- We can do it with Batch Scripts--------------!LF!


call:DepExist
call:AdminRights
call:ProcessRunning
call:CreateObjects
call:LinkObjects
call:End

::------------------------- Principais----------------------------------

:DepExist
:: Se o compilador não estiver registrado como variável de ambiente... Não há nada pra fazer.
call clang -cc1 -version
if ERRORLEVEL 1 goto End
goto:eof

:AdminRights
NET SESSION >nul 2>&1
IF NOT ERRORLEVEL 0 (
    ECHO [Cronus]: Script exige conta de administrador... Encerrando
	goto End
)
goto:eof

:ProcessRunning
:: Checa a existência de processos existentes com os mesmos que serão gerados pelo processo
:: Caso exista, os mesmos são finalizados para o processo (Forçadamente)
echo !LF![Cronus]: Checando Processos ativos...
for %%s in (%CharServer% %LoginServer% %MapServer%) do (
tasklist /FI "IMAGENAME eq %%s.exe" |find ":" >nul 2>&1
If ERRORLEVEL 1 (
echo [Cronus]: Finalizando %%s ...
taskkill /s localhost /f /im %%s.exe >nul 2>&1
)
del /q %%s.exe >nul 2>&1
)
goto:eof

:CreateObjects
echo [Cronus]: Iniciando...

::Mt19939ar
cd %mtF%
clang %flags% -c %mtS%.c
call:FileExists %mtS%.o %mtF%%mtS%.o
call:Replicate %mtF%%mtS%.o

cd %LcF%
for %%s in (%DepS%) do (
clang -I%commonF% %flags% -c %%s.c
call:FileExists %%s.o %LcF%%%s.o
set ArgDepS=!ArgDepS! %%s.o 
call:Replicate %%s.o
)

::Object_CAll
cd %commonF%
for %%s in (%sharedS%) do (
clang -I%myF% -I%zlibF% -I%mtF% -I%LcF% %flags% -c %%s.c
call:FileExists %%s.o %commonF%%%s.o
call:Replicate %commonF%%%s.o
set ArgsharedS=!ArgsharedS! %%s.o 
call:Replicate %%s.o
)

::Char-Server
cd %charF%
for %%s in (%CharS%) do (
clang %flags% -c %%s.c
call:FileExists %%s.o %charF%%%s.o
set ArgCharS=!ArgCharS! %%s.o 
)


::Login-Server
cd %LoginF%
for %%s in (%LoginS%) do (
clang %flags% -c %%s.c
call:FileExists %%s.o %LoginF%%%s.o
set ArgLoginS=!ArgLoginS! %%s.o
)


::Map-Server
cd %MapF%
for %%s in (%MapS%) do (
clang -I%configF% -I%pcreF% %flags% -c %%s.c
call:FileExists %%s.o %MapF%%%s.o
set ArgMapS=!ArgMapS! %%s.o 
)
goto:eof


:LinkObjects
:: DO NOT TOUCH...
cd %CharF%
echo [Cronus]: Gerando emulador... Aguarde.
clang -s -o %CharServer%.exe %ArgCharS% %ArgsharedS% %ArgDepS% %mtS%.o %myLib% -lws2_32
move %CharServer%.exe %~dp0
cd %LoginF%
clang -s -o %LoginServer%.exe %ArgLoginS% %ArgsharedS% %ArgDepS% %mtS%.o %myLib% -lws2_32
move %LoginServer%.exe %~dp0
cd %MapF%
clang -s -o %MapServer%.exe %ArgMapS% %ArgsharedS% %ArgDepS% %mtS%.o %myLib% %zlibLib% %pcreLib% -lws2_32
move %MapServer%.exe %~dp0
goto:eof


:End
::Finaliza o script.
call:RemoveObject
echo [Cronus]: Processo Finalizado.
ENDLOCAL EnableExtensions
ENDLOCAL EnableDelayedExpansion
pause
exit

::------------------------- Auxiliares --------------------------------------

:FileExists
:: Não Existe? Acabou....Finaliza o script caso o arquivo falte.
if exist "%2" (
echo [Cronus]: Objeto gerado - %1
) else (
echo [Cronus]: Falha no arquivo %1
goto End
)
goto:eof

:RemoveObject
:: Remove todos os arquivos Objeto e outros gerados pelo compilador
for %%s in (%charF% %commonF% %loginF% %mapF% %mtF% %lcF%) do (
cd %%s
del /q *.o >nul 2>&1
del /q *.gch >nul 2>&1
)
echo [Cronus]: Arquivos-Objeto removidos com sucesso.
goto:eof


:Replicate
:: Replicação para não precisar ficar navegando entre pastas, linkando caminhos... facilita minha vida.
for %%s in (%charF% %loginF% %mapF%) do copy %1 %%s >nul 2>&1
goto:eof



