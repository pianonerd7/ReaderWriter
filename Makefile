
all: 
	gcc -pthread -o hw5 ReaderWriter.c
	chmod 755 hw5
clean:
	-rm hw5
