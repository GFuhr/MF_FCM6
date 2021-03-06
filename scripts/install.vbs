'Objects
Set oFso = CreateObject("Scripting.FileSystemObject")
Set oShell = WScript.CreateObject ("WScript.Shell")
dim xHttp: Set xHttp = createobject("Microsoft.XMLHTTP")
dim bStrm: Set bStrm = createobject("Adodb.Stream")
Set environmentVars = WScript.CreateObject("WScript.Shell").Environment("Process")

homepath = environmentVars("HOMEDRIVE")&environmentVars("HOMEPATH")
tempFolder = environmentVars("TEMP")
installfolder = homepath &"\Miniconda3\"

condacmd = installfolder & "Scripts\conda"
packagesinstall = " install -y numpy matplotlib spyder jupyter cython"

'check if Miniconda already exist
exists = oFso.FolderExists(installfolder)
if NOT (exists) then 
    ' download miniconda
    outputfolder = tempFolder&"\miniconda3-install.exe"
    xHttp.Open "GET", "https://repo.continuum.io/miniconda/Miniconda3-latest-Windows-x86.exe", False
    xHttp.Send

    with bStrm
        .type = 1 '//binary
        .open
        .write xHttp.responseBody
        .savetofile outputfolder, 2 '//overwrite
    end with

    ' install miniconda for python 3
    finalcmd = outputfolder &" /S /AddToPath=1 /NoRegistry=1"

    oShell.run finalcmd, 1, True
    oShell.run condacmd&packagesinstall, 1, True
end if


' install python 3.6.5
pythonupdate = installfolder & "Scripts\" & "conda install -y python=3.6.5"
oShell.run pythonupdate, 1, True

' freeze python version to version 3.6
pinnedfile = installfolder &"\conda-meta\pinned"
Set objFile = oFso.CreateTextFile(pinnedfile,True)
objFile.Write "python 3.6.*" & vbCrLf
objFile.Close


' ipython default imports
ipythonstartup = homepath &"\.ipython\profile_default\startup"
outFile = ipythonstartup&"00-imports.py"
Set objFile = oFso.CreateTextFile(outFile,True)
objFile.Write "import numpy as np; import matplotlib.pyplot as plt; from os import getcwd, chdir; import cython" & vbCrLf
objFile.Close

' launch jupyter
pythoncmd = installfolder & "python.exe"
jupyterpath = installfolder & "pkgs\qtconsole-4.2.1-py35_2\Scripts\"
jupytercmd = jupyterpath & "jupyter-qtconsole-script.py"
'oShell.run pythoncmd & " " & jupytercmd, 1, False

' create spyder desktop shortcut
pythonscript = installfolder & "Scripts\"
spydercmd = pythonscript & "spyder.exe"
strDesktop = oShell.SpecialFolders("Desktop")
set oMyShortcut = oShell.CreateShortcut(strDesktop + "\Spyder.lnk")
oMyShortcut.TargetPath = spydercmd
oMyShortCut.Save

' run spyder afterwards
oShell.run spydercmd, 1, False
