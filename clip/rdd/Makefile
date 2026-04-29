CFLAGS += -I.. -I../include -fPIC
LIB = librdd.a
OBJS = rddclip.o rdd.o dbf.o ntx.o cdx.o dbt.o fpt.o rushmore.o btree.o six.o hiper.o

all:	$(LIB)

clean:
	rm -f *.o *.a *.b *.bak *.BAK *.tmp *.ex *.nm \
	test_ref.c test.c test.log \
	*.dbf *.ntx *.cdx *.dbt *.fpt \
	$(LIB) test

$(LIB): $(OBJS)
	rm -f $(LIB)
	../clip_makelib $(LIB) $(OBJS) && ranlib $(LIB)

rddclip.o:	rddclip.c ../../include/rdd.h
rdd.o:		rdd.c ../../include/rdd.h
dbf.o:		dbf.c ../../include/rdd.h
ntx.o:		ntx.c ../../include/rdd.h
cdx.o:		cdx.c ../../include/rdd.h
dbt.o:		dbt.c ../../include/rdd.h
fpt.o:		fpt.c ../../include/rdd.h
rushmore.o:	rushmore.c ../../include/rdd.h
btree.o:	btree.c btree.h
six.o:		six.c ../../include/rdd.h ../include/six.ch
hiper.o:	hiper.c ../../include/rdd.h btree.h

test: test.prg rddclip.o rdd.o dbf.o ntx.o cdx.o dbt.o fpt.o rushmore.o btree.o six.o hiper.o ../clipbase.h ../clipbase.c ../libclip.a
	clip -e -R test.prg
