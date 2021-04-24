compiled = buffer_manager.o file_manager.o
sampleobjects = $(compiled) sample_run.o
linearobjects= $(compiled) linear_search.o
join1objects= $(compiled) join1.o
binaryobjects= $(compiled) binary_search.o
deleteobjects= $(compiled) deletion.o
join2objects= $(compiled) join2.o

sample_run : $(sampleobjects)
	     g++ -std=c++11 -o sample_run $(sampleobjects)

linearsearch : $(linearobjects)
	g++ -std=c++11 -o linearsearch $(linearobjects)

join1: $(join1objects)
	g++ -std=c++11 -o join1 $(join1objects)

binarysearch : $(binaryobjects)
	g++ -std=c++11 -o binarysearch $(binaryobjects)

deletion : $(deleteobjects)
	g++ -std=c++11 -o deletion $(deleteobjects)
	
join2: $(join2objects)
	g++ -std=c++11 -o join2 $(join2objects)

linear_search.o : linear_search.cpp
	g++ -std=c++11 -c linear_search.cpp

binary_search.o : binary_search.cpp
	g++ -std=c++11 -c binary_search.cpp

deletion.o : deletion.cpp
	g++ -std=c++11 -c deletion.cpp

join1.o : join1.cpp
	g++ -std=c++11 -c join1.cpp

join2.o : join2.cpp
	g++ -std=c++11 -c join2.cpp

sample_run.o : sample_run.cpp
	g++ -std=c++11 -c sample_run.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f *.exe
	rm -f deletion
	rm -f binarysearch
	rm -f sample_run
	rm -f linearsearch
	rm -f join1
	rm -f join2
