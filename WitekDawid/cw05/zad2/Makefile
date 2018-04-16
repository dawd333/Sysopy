MASTER = master.o
MASTEREXE = master
SLAVE = slave.o
SLAVEEXE = slave
MAIN = main.o
MAINEXE = main

all: $(MASTER) $(MASTEREXE) $(SLAVE) $(SLAVEEXE) $(MAIN) $(MAINEXE)

clean:
	rm -r $(MASTER) $(MASTEREXE) $(SLAVE) $(SLAVEEXE) $(MAIN) $(MAINEXE) 

$(MASTER): master.c
	gcc -c master.c

$(MASTEREXE): $(MASTER)
	gcc $(MASTER) -o $(MASTEREXE)

$(SLAVE): slave.c
	gcc -c slave.c

$(SLAVEEXE): $(SLAVE)
	gcc $(SLAVE) -o $(SLAVEEXE)

$(MAIN): main.c
	gcc -c main.c

$(MAINEXE): $(MAIN)
	gcc $(MAIN) -o $(MAINEXE)