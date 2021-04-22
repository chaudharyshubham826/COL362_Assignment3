sampleobjects = buffer_manager.o file_manager.o sample_run.o
linearobjects= buffer_manager.o file_manager.o linear_search.o
join1objects= buffer_manager.o file_manager.o join1.o
join2objects= buffer_manager.o file_manager.o join2.o
sample_run : $(sampleobjects)
	     g++ -std=c++11 -o sample_run $(sampleobjects)

linearsearch : $(linearobjects)
	g++ -std=c++11 -o linearsearch $(linearobjects)

join1: $(join1objects)
	g++ -std=c++11 -o join1 $(join1objects)

join2: $(join2objects)
	g++ -std=c++11 -o join2 $(join2objects)

linear_search.o : linear_search.cpp
	g++ -std=c++11 -c linear_search.cpp

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
	rm -f sample_run
