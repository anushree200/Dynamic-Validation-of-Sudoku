final:
	@echo "Build Version 1.0.1"
	@sleep 3
	@echo "Entering Assignment - 2"
	@echo "DYNAMIC_SUDOKU_VALIDATION"
	@sleep 4

	@echo "Compiling validation using TAS sync tool"
	g++ -pthread Assgn2Src1_CO23BTECH11003.cpp -o tas_out
	@echo "Completing the compilation..."
	@./tas_out
	@echo "output written in outputtas.txt"
	@sleep 3

	@echo "Compiling validation using CAS sync tool"
	g++ -pthread Assgn2Src2_CO23BTECH11003.cpp -o cas_out
	@echo "Completing the compilation..."
	@./cas_out
	@echo "output written in outputcas.txt"
	@sleep 3

	@echo "Compiling validation using Bound CAS sync tool"
	g++ -pthread Assgn2Src3_CO23BTECH11003.cpp -o bound_out
	@echo "Completing the compilation..."
	@./bound_out
	@echo "output written in outputboundcas.txt"
	@sleep 3

	@echo "Compiling validation using sequential (no threading)"
	g++ Assgn2Src4_CO23BTECH11003.cpp -o seq_out
	@echo "Completing the compilation..."
	@./seq_out
	@echo "output written in outputseq.txt"
	@sleep 3

	@echo "Compiling validation using early termination in TAS"
	g++ -pthread Assgn2Src5_CO23BTECH11003.cpp -o early_out
	@echo "Completing the compilation..."
	@./early_out
	@echo "output written in outputtasearly.txt"
	@sleep 3

	@echo "All output files have been created."
	@echo "Makefile Success!!"