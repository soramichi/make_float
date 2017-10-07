all:
	g++ make_float.cpp -o make_float
	gcc -lm trick_exception_handler.c -o trick_exception_handler
