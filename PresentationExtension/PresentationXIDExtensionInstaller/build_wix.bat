erase *wixobj *msi *wixpdb

candle -dSourceDir=".\manual_install" -pedantic -v -wx CedrusPresentationXIDExtension.wxs dlls_to_include.wxs

light -sw1076 -ext WixUIExtension CedrusPresentationXIDExtension.wixobj dlls_to_include.wixobj -o CedrusPresentationXIDExtension.msi

erase *wixobj *wixpdb