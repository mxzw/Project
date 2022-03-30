BIN=gym 
LDPATH = -L /usr/lib64/mysql 
LDFLAG=-lpthread -ljsoncpp -lmysqlclient -lcrypto -g -w -std=c++11

$(BIN):gym.cpp 
	g++ $^ -o $@  $(LDPATH) $(LDFLAG)

clean:
	rm gym

