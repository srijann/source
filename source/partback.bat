d:
cd \spsprograms
attrib -a *.exe /s
attrib -a *.obj /s
attrib -a *.csm /s
attrib -a *.bak /s
attrib -a *.lib /s
attrib -a *.dll /s
attrib -a *.apx /s
attrib -a *.~* /s
attrib -a *.mbt /s
attrib -a *.mrt /s
attrib -a *.tds /s
attrib -a *.il* /s
attrib -a *.cgl /s
xcopy *.* g:\sps\source /m/s
