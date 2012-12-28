@echo on
@rem
@rem by Allan Schytt    allan.schytt@gmail.com
@rem
@rem type sandcastle ? for help
@rem dont use spaces in output directory (because of config file)

@IF "%1"=="?" GOTO HELP
@IF "%1"=="" GOTO NOPARAMS
@SET SOLUTIONNAME=%1
@SET ASSEMBLY=%2
@SET OUTPUTDIR=%3
@SET XMLFILE=%4

@rem Setting defaults
@IF "%2"=="" SET ASSEMBLY=%SOLUTIONNAME%.DLL
@if not exist %ASSEMBLY% goto ASSEMBLY_NOT_FOUND
@IF "%4"=="" SET XMLFILE=%SOLUTIONNAME%.XML
@if not exist %XMLFILE% goto XML_NOT_FOUND
@set SANDCASTLE="%ProgramFiles%"\Sandcastle
@if not exist %SANDCASTLE% goto SANDCASTLE_NOT_INSTALLED
@if not exist "%ProgramFiles%\HTML Help Workshop" goto HTMLHELP_NOT_INSTALLED
@SET SANDTOOLS=%SANDCASTLE%\ProductionTools
@SET TRANSFORM=%SANDCASTLE%\ProductionTransforms
@if not exist %OUTPUTDIR% mkdir %OUTPUTDIR%

@echo *****Building %SOLUTIONNAME%.hhp***** >%OUTPUTDIR%\%SOLUTIONNAME%.log
@echo [OPTIONS] >%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo Compatibility=1.1 or later >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo Compiled file=%SOLUTIONNAME%.chm >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo Contents file=%SOLUTIONNAME%.hhc >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo Display compile progress=No >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo Language=0x409 English (United States) >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo.  >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo [FILES] >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo art\*.gif >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo html\*.htm >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo scripts\*.js >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo styles\*.css >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo.  >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo [INFOTYPES] >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp
@echo.  >>%OUTPUTDIR%\%SOLUTIONNAME%.hhp

@echo *****Building %OUTPUTDIR%\%SOLUTIONNAME%.config***** >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@echo  ^<configuration^> >%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo    ^<dduetools^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo      ^<builder^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo        ^<components^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo          ^<component type="Microsoft.Ddue.Tools.CopyFromFileComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo 	   ^<data file="%ProgramFiles%\SandCastle\Presentation\transforms\skeleton.xml" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo 	   ^<copy source="/*" target="/" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo 	 ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^<component type="Microsoft.Ddue.Tools.CopyFromIndexComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<index name="reflection" value="/reflection/apis/api" key="@id" cache="10"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo             ^<data files="%OUTPUTDIR%\reflection.xml" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo             ^<^data files="%ProgramFiles%\SandCastle\Examples\cpref_reflection\*.xml" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<^/index^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<^copy name="reflection" source="*" target="/document/reference" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^<^/component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^<^component type="Microsoft.Ddue.Tools.CopyFromIndexComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<^copy name="reflection" key="string(/document/reference/containers/namespace/@api)" source="*[not(local-name()='elements')]" target="/document/reference/containers/namespace" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^<^/component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^<component type="Microsoft.Ddue.Tools.CopyFromIndexComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo          ^<copy name="reflection" key="string(/document/reference/containers/type/@api)" source="*[not(local-name()='elements')]" target="/document/reference/containers/type" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^<component type="Microsoft.Ddue.Tools.IfThenComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	   ^<if condition="not(starts-with($key,'Overload:') or starts-with($key,'R:'))" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<then^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	   ^<component type="Microsoft.Ddue.Tools.SyntaxComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	     ^<syntax input="/document/reference" output="/document/syntax" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	     ^<generators^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo               ^<generator type="Microsoft.Ddue.Tools.CSharpDeclarationSyntaxGenerator" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\SyntaxGenerators.dll" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo               ^<generator type="Microsoft.Ddue.Tools.VisualBasicDeclarationSyntaxGenerator" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\SyntaxGenerators.dll" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo               ^<generator type="Microsoft.Ddue.Tools.CPlusPlusDeclarationSyntaxGenerator" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\SyntaxGenerators.dll" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo             ^</generators^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^</then^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^<component type="Microsoft.Ddue.Tools.CopyFromIndexComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<index name="comments" value="/doc/members/member" key="@name" cache="100"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo             ^<data files="%XMLFILE%" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo             ^<data files="%SystemRoot%\Microsoft.NET\Framework\v2.0.50727\*.xml" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^</index^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<copy name="comments" source="*" target="/document/comments" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo         ^<component type="Microsoft.Ddue.Tools.ForEachComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<variable expression="/document/reference/elements/element/@api" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<components^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo             ^<component type="Microsoft.Ddue.Tools.CopyFromIndexComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	       ^<copy name="reflection" source="*[not(local-name()='elements')]" target="/document/reference/elements/element[@api=$key]" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	     ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo             ^<component type="Microsoft.Ddue.Tools.CopyFromIndexComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo               ^<copy name="comments" source="summary" target="/document/reference/elements/element[@api=$key]" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo             ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^</components^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo         ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo         ^<component type="Microsoft.Ddue.Tools.TransformComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<transform file="%ProgramFiles%\SandCastle\Presentation\transforms\main_sandcastle.xsl" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo         ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo         ^<component type="Microsoft.Ddue.Tools.SharedContentComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<content file="%ProgramFiles%\SandCastle\Presentation\content\shared_content.xml" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<content file="%ProgramFiles%\SandCastle\Presentation\content\reference_content.xml" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^<component type="Microsoft.Ddue.Tools.ResolveReferenceLinksComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	   ^<targets files="%OUTPUTDIR%\reflection.xml" type="local" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	   ^<targets files="%ProgramFiles%\SandCastle\Examples\cpref_reflection\*.xml" type="none" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo	 ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo         ^<component type="Microsoft.Ddue.Tools.SaveComponent" assembly="%ProgramFiles%\SandCastle\ProductionTools\BuildComponents\BuildComponents.dll"^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo           ^<save path="concat('%OUTPUTDIR%\html\',/html/head/meta[@name='guid']/@content,'.htm')" indent="false" omit-xml-declaration="true" /^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo         ^</component^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo       ^</components^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo     ^</builder^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo   ^</dduetools^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config
@echo ^</configuration^> >>%OUTPUTDIR%\%SOLUTIONNAME%.config

@echo *****Building the SandCastle documentation***** >>%OUTPUTDIR%\%SOLUTIONNAME%.log

@echo *****Begin the generation process***** >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@%SANDTOOLS%\MRefBuilder %2 /out:%OUTPUTDIR%\reflection.org >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if  errorlevel 1 goto error

@%SANDTOOLS%\XslTransform %TRANSFORM%\AddOverloads.xsl %OUTPUTDIR%\reflection.org | %SANDTOOLS%\XslTransform %TRANSFORM%\AddGuidFilenames.xsl /out:%OUTPUTDIR%\reflection.xml >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if  errorlevel 1 goto error

@echo *****generate a topic manifest***** >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@%SANDTOOLS%\XslTransform %TRANSFORM%\ReflectionToManifest.xsl  %OUTPUTDIR%\reflection.xml /out:%OUTPUTDIR%\manifest.xml  >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if  errorlevel 1 goto error

@echo *****build the output path***** >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if not exist %OUTPUTDIR%\html mkdir %OUTPUTDIR%\html >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if not exist %OUTPUTDIR%\art mkdir %OUTPUTDIR%\art >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@copy %SANDCASTLE%\Presentation\art\* %OUTPUTDIR%\art >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if not exist %OUTPUTDIR%\scripts mkdir %OUTPUTDIR%\scripts >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@copy %SANDCASTLE%\Presentation\scripts\* %OUTPUTDIR%\scripts >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if not exist %OUTPUTDIR%\styles mkdir %OUTPUTDIR%\styles >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@copy %SANDCASTLE%\Presentation\styles\* %OUTPUTDIR%\styles >>%OUTPUTDIR%\%SOLUTIONNAME%.log

@echo *****build the manifest***** >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@%SANDTOOLS%\BuildAssembler /config:%OUTPUTDIR%\%SOLUTIONNAME%.config %OUTPUTDIR%\manifest.xml  >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if errorlevel 1 goto error

@echo *****change reflection to chm***** >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@%SANDTOOLS%\XslTransform %TRANSFORM%\ReflectionToChmContents.xsl %OUTPUTDIR%\reflection.xml /arg:html=html /out:%OUTPUTDIR%\%SOLUTIONNAME%.hhc  >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@if errorlevel 1 goto error

@echo *****Build Html help file*****  >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@"%ProgramFiles%\HTML Help Workshop\hhc" %OUTPUTDIR%\%SOLUTIONNAME%.hhp >>%OUTPUTDIR%\%SOLUTIONNAME%.log

@if not exist %OUTPUTDIR%\%SOLUTIONNAME%.chm goto error

@echo Done building the documentation for %SOLUTIONNAME% (%ASSEMBLY%) to %OUTPUTDIR% >>%OUTPUTDIR%\%SOLUTIONNAME%.log
@echo Done building the documentation for %SOLUTIONNAME% (%ASSEMBLY%) to %OUTPUTDIR%
@goto end

:error
@echo Failure in generating help file
@echo Check log file %OUTPUTDIR%\%SOLUTIONNAME%.config for details
@goto end

:NOPARAMS
@echo Command failure ! Name must be provided as first parameter
@goto help

:XML_NOT_FOUND
@echo File Failure ! %XMLFILE% is not found
@goto help

:ASSEMBLY_NOT_FOUND
@echo File Failure ! %ASSEMBLY% is not found
@goto help

:HTMLHELP_NOT_INSTALLED
@echo Failure ! HTML Help compiler is not installed on this computer
@echo HTML Workshop must be installed in %ProgramFiles%\HTML Help Workshop
@goto end

:SANDCASTLE_NOT_INSTALLED
@echo Failure ! Sandcastle is not installed on this computer
@echo Sandcastle must be installed in %ProgramFiles%\Sandcastle
@goto end

:HELP
@echo.
@echo sandcastle.bat ^%%1 ^%%2 ^%%3 ^%%4
@echo where
@echo       ^%%1 (required) is desired name of the helpfile without extension
@echo       ^%%2 (optional) is the assembly file (default ^%%1 + .dll)
@echo       ^%%3 (optional) is the project directory  (default .)
@echo       ^%%4 (optional) is xmlcomments file name (default ^%%1 + .xml)
@echo and
@echo     Sandcastle are installed in %ProgramFiles%\Sandcastle
@echo     HTML Workshop are installed in %ProgramFiles%\HTML Help Workshop
@echo.

:end