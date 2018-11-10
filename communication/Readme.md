
In the communication directory, compile the main file by running, 

g++ -std=gnu++11 main.cpp ClientSocket.cpp -o harness

Next, compile the test workload file as 
	gcc write-cacheline.c -o workload


Now simply run the main file by supplying the IP address of the host machine as
	./harness -i <host IP> -p <host port>
