all:
	nasm -f elf64 -g data_modifications_xmm.asm -l data_modifications.lst -F dwarf
	gcc -shared -g data_modifications_xmm.o -o libdata_modifications_asm.so

clean:
	rm data_modifications.lst
	rm data_modifications_xmm.o
	rm libdata_modifications_asm.so