all:
	gcc -g *.c -Wall -Wextra -o client `mysql_config --cflags --include --libs` 
clean:
	-rm client
all2:
	gcc -g *.c -Wall -Wextra -o client -I/usr/include/mysql -lmysqlclient
