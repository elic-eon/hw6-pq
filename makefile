all: pq.func.demo

pq.func.demo: pq.o pq.func.tester utility.o
	gcc -o pq.func.demo pq.o pq.func.tester utility.o -g

pq.perf.demo: pq.o pq.perf.tester
	gcc -o pq.perf.demo pq.o pq.perf.tester -g

pq.o: pq.c
	gcc -c -o pq.o pq.c -g

pq.func.tester: pq.func.tester.c
	gcc -c -o pq.func.tester pq.func.tester.c -g

utility.o: utility.c
	gcc -c -o utility.o utility.c -g

pq.perf.tester: pq.perf.tester.c
	gcc -c -o pq.perf.tester pq.perf.tester.c -g

clean:
	rm -f *.demo *.tester *.o
