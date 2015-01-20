
--------------------------------------------------
--------------------------------------------------
BEFORE YOU START:

    You must *install* the WiX toolset!

    You must locate the WiX binary executables
(heat.exe, candle.exe, and light.exe) and make
sure you add that location to your PATH variable!
--------------------------------------------------
--------------------------------------------------


1. Create a sub-folder under the 'Presentation XID Extension Installer' directory.  This new sub-folder must be named 'manual_install'.  Then you should have this:

	Presentation XID Extension Installer\manual_install

2. Build the XID Device Driver dll using the SCons script in SCons_build\

3. If you built the release version, copy these files to manual_install:
	..\Release\CedrusXidDriver.dll
	..\SCons_build\scons-out\opt\staging\xid_device_driver.dll
	..\SCons_build\scons-out\opt\staging\boost_system-vc100-mt-1_49.dll
	..\SCons_build\scons-out\opt\staging\boost_filesystem-vc100-mt-1_49.dll
	..\SCons_build\scons-out\opt\staging\boost_date_time-vc100-mt-1_49.dll
	
   If you built debug, you want these:
	..\Debug\CedrusXidDriver.dll
	..\SCons_build\scons-out\dbg\staging\xid_device_driver.dll
	..\SCons_build\scons-out\dbg\staging\boost_system-vc100-mt-gd-1_49.dll
	..\SCons_build\scons-out\dbg\staging\boost_filesystem-vc100-mt-gd-1_49.dll
	..\SCons_build\scons-out\dbg\staging\boost_date_time-vc100-mt-gd-1_49.dll
						
4. Using a windows command prompt, make sure the working directory is 'Presentation XID Extension Installer', then execute 'run_heat.bat' 	

	(the run_heat.bat script should produce 'dlls_to_include.wxs'. If a previous file 'dlls_to_include.wxs' is present, then it will be overwritten, which is fine -- this is what we WANT.)
	
5. Using the same command prompt, make sure the working directory is still 'Presentation XID Extension Installer', then execute 'build_wix.bat' 	

6. If step 5 succeeds, then you should now have 'CedrusPresentationXIDExtension.msi'.  If you run/install that msi, it should install the extension correctly to 'C:\Program Files (x86)\Cedrus XID Response Extension for Presentation\'.

7. [OPTIONAL] If you're a Cedrus developer, codesign the msi.  The codesigning command will look roughly like:

G:\path_to_tools\codesigning_win\signtool sign /t http://timestamp.digicert.com /f "G:\path_to_tools\codesigning_win\digicert_6_27_win.pfx" /p xxxxxxxxxx "G:\some_path\PresentationXIDExtensionInstaller\CedrusPresentationXIDExtension.msi"
