mk: init user
init:	filesystem.h filesystem_init.cpp
	g++ -w -o filesystem_init filesystem.h filesystem_init.cpp
user:	filesystem.h filesystem.cpp filesystem_user.cpp
	g++ -w -o filesystem_user filesystem.h filesystem.cpp filesystem_user.cpp
clean:	rm -f fs