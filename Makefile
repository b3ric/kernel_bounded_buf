buffer: buffer_user.c buffer.h utils.c utils.h
	gcc buffer_user.c buffer.h utils.c utils.h -pthread -Wall -o buffer
