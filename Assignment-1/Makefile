
FILES= report.pdf prob1_mandelbrot_threads/mandelbrot.cpp \
	prob2_vecintrin/functions.cpp \
	prob3_mandelbrot_ilp/mandelbrot.cpp \
	prob4_mandelbrot_ispc/mandelbrot.ispc \
	prob5_cuberoot/data.cpp \


handin.tar: $(FILES)
	tar cvf handin.tar $(FILES)

clean:
	(cd prob1_mandelbrot_threads; make clean)
	(cd prob2_vecintrin; make clean)
	(cd prob3_mandelbrot_ilp; make clean)
	(cd prob4_mandelbrot_ispc; make clean)
	(cd prob5_cuberoot; make clean)
	rm -f *~ handin.tar
