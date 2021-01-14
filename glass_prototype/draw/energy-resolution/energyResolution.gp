# Modify default canvas size
set terminal qt 0 font "Sans,9" size 400,450

set xlabel "Electron energy, GeV"
set ylabel "Resolution, %"
set title "Resolution vs Incident Electron Energy"
set grid

plot [:][:35] './output-farm-3x3-20x20x200mm-BaSi2O5/resolution-BaSi2O5-3x3-20x20x20-gaus.txt' using 1:2:3 with errorbars pt 6 title "BaSi2O5 3x3 20x20x200 mm (gauss)", \
              './output-farm-3x3-20x20x200mm-BaSi2O5/resolution-BaSi2O5-3x3-20x20x20-cball.txt' using 1:2:3 with errorbars pt 6 title "BaSi2O5 3x3 20x20x200 mm (cball)", \
              './output-farm-3x3-20x20x200mm-BaGdSiO/resolution-BaGdSiO-3x3-20x20x20.txt' using 1:2:3 with errorbars pt 6 title "BaGdSiO 3x3 20x20x200 mm", \
              './output-farm-1x1-40x40x400mm-BaGdSiO/resolution-BaGdSiO-1x1-40x40x40.txt' using 1:2:3 with errorbars pt 8 title "BaGdSiO 1x1 40x40x400 mm", \
              './output-farm-5x5-20x20x400mm-BaGdSiO/resolution-BaGdSiO-5x5-20x20x20.txt' using 1:2:3 with errorbars pt 4 title "BaGdSiO 5x5 20x20x200 mm", \
              './output-farm-3x3-20x20x200mm-PbWO4/resolution-PbWO4-3x3-20x20x20.txt'     using 1:2:3 with errorbars pt 8 title "PbWO4 3x3 20x20x200 mm"

