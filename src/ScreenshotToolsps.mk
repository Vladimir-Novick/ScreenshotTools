
ScreenshotToolsps.dll: dlldata.obj ScreenshotTools_p.obj ScreenshotTools_i.obj
	link /dll /out:ScreenshotToolsps.dll /def:ScreenshotToolsps.def /entry:DllMain dlldata.obj ScreenshotTools_p.obj ScreenshotTools_i.obj \
		kernel32.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DREGISTER_PROXY_DLL \
		$<

clean:
	@del ScreenshotToolsps.dll
	@del ScreenshotToolsps.lib
	@del ScreenshotToolsps.exp
	@del dlldata.obj
	@del ScreenshotTools_p.obj
	@del ScreenshotTools_i.obj
