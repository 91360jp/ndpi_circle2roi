::set dname_in=D:\20230315\DrWu\ndpi56\ndpi56\
set dname_in=%1\
set dname_in_ndpa=%dname_in%
set gamma=1.8
set ratio=3.0
set width=200
set dname_out=%dname_in%output%width%\

mkdir %dname_out%

.\bin\Release\ndpi_circle2roi.exe %dname_in% %dname_in_ndpa% %dname_out% %gamma% %ratio% %width%

