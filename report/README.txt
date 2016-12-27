Compile as:
latex report
bibtex report
latex report
latex report

Now you have a dvi file. Get a ps file as:
dvips -t letter -o report.ps -Ppdf -G0 report.dvi

Now you have a ps file. Get a pdf as:
ps2pdf report.ps