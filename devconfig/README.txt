Proto- Super Rough Draft Devconfig "Documentation" With Bad Formatting

1) Product and Model ID are ASCII values, because StimTracker 1G returns 'S' and 'C' for those.

NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! 
LUMINA and SV-1 product Model IDs returned by the _d3 command are to be ignored!

2) The UseableAsResponse field in the Port section has been appropriated for use in config-parsing logic. It was previously not being used, but now if you want a port to accept keypresses or something, that better be set to Yes.

3) DeviceID is only used in SuperLab and has no bearing on anything in the library.

4) PortName is only used in SuperLab and has no bearing on anything in the library.
